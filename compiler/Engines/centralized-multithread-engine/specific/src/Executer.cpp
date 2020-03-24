/**
 * Copyright Verimag laboratory.
 * 
 * contributors:
 *  Jacques Combaz (jacques.combaz@univ-grenoble-alpes.fr)
 * 
 * This software is a computer program whose purpose is to generate
 * executable code from BIP models.
 * 
 * This software is governed by the CeCILL-B license under French law and
 * abiding by the rules of distribution of free software.  You can  use, 
 * modify and/ or redistribute the software under the terms of the CeCILL-B
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info".
 * 
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or 
 * data to be ensured and,  more generally, to use and operate it in the 
 * same conditions as regards security.
 * 
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */

#include "Executer.hpp"
#include <Compound.hpp>
#include <InteractionValue.hpp>
#include <Atom.hpp>
#include <AtomInternalPort.hpp>
#include <AtomExportPort.hpp>
#include <BipError.hpp>
#include <NonDeterministicPetriNetError.hpp>
#include <NonOneSafePetriNetError.hpp>
#include <CycleInPrioritiesError.hpp>
#include <CycleInAtomPrioritiesError.hpp>
#include <AtomInvariantViolationError.hpp>
#include <StringTree.hpp>
#include <Logger.hpp>
#include <Thread.hpp>
#include <Job.hpp>
#include <ExecuteAtomJob.hpp>
#include <GlobalClock.hpp>
#include <BipCallBack.hpp>
#include <Resetable.hpp>
#include <Scheduler.hpp>

#include <stdlib.h>
#include "engine-version.hpp"

#include <string>
#include <iostream>
#include <sstream>

Executer::Executer(Compound &top, bool isRealTime, bool asap, bool first_enabled, bool relaxed, bool disable_time_safety, bool profiling, Logger &logger, unsigned int nbThreads, bool randomSeed, unsigned int seed) :
  mTop(top),
  mIsRealTime(isRealTime),
  mAsap(asap),
  mFirstEnabled(first_enabled),
  mRelaxed(relaxed),
  mDisabledTimeSafety(disable_time_safety),
  mProfiling(profiling),
  mLogger(logger),
  mNbThreads(nbThreads),
  mRandomSeed(randomSeed),
  mSeed(seed),
  mInitializeCallBack(NULL),
  mInitializeThreadCallBack(NULL) {
}

Executer::~Executer() {
}


/**
 * \brief Initialize the system.
 */
BipError &Executer::initialize() { 
  // initialize clocks
  if (isRealTime()) {
    setPlatformClock(mRealTimeClock);
  }
  else {
    setPlatformClock(mSimulationClock);
  }
  
  if (hasInitializeCallBack()) {
    initializeCallBack().callBackMethod(initializeCallBack().argc(), initializeCallBack().argv());
  }
  
  logger().log("BIP Engine (version " ENGINE_VERSION " )");
  logger().log("");
  logger().log("initialize components...");
  
  // initialize the seed for random
  ostringstream displayRandomSeed;
  displayRandomSeed << "random scheduling based on seed=";

  if (randomSeed()) {
    int random_seed = time(NULL);
    srand (random_seed);
    
    displayRandomSeed << random_seed;
  }
  else {
    srand(seed());

    displayRandomSeed << seed();
  }
  
  logger().log(displayRandomSeed.str());

  // initialize threads for handling jobs
  initializeThreads();

  // configure jobs parameters
  top().configureJobs(mLogger, isRealTime(), asap(), firstEnabled(), relaxed(), disabledTimeSafety(), profiling());
  
  // intialize precomputed values
  top().initialize();
  top().initializeValidators(mLogger);
  top().initialize2();
  top().initializeTimeSafetyValidators(isRealTime());

  // management of groups related to resetable
  top().initializeResetable();
  Group<ResetableItf>::initializeAll();
  
  // management of groups related to resources
  resetAllPriorities(top());
  
  // initialize reserver
  // initializeReserver(top());
  // // mReserver.addResource(Job::simulationClockResource());
  // mReserver.initialize();
  
  // restart all atoms
  top().restartAllAtoms();

  // Job::simulationClockResource().free();
  
  // restart all atoms
  //top().restartAllAtoms();

  return BipError::NoError;
}

/**
 * \brief Execute a single execution sequence randomly.
 *
 * Interactions and internal ports are chosen randomly according to
 * a uniform repartition of the probability. It is based on rand() of
 * the standard library for computing random values. Notice that the
 * seed for rand() is currently not fixed.
 */
BipError &Executer::run() {
  // initialize threads for handling jobs
  startThreads();
  
  // update model clock
  updateTime();

  while (!logger().reachedLimit() && !jobs.isStopped()) {    
    // take a snapshot of the treated notifications
    platformClock().resetNotifications();
    
    // /!\ order is critical: must be placed after updateTreatedNotifications()!
    // update current status of external ports w.r.t. incoming events
    BipError &errorInExternals = checkExternals();
    
    if (errorInExternals.type() != NO_ERROR) {
      logger().log(errorInExternals);
      
      return errorInExternals;
    }

    // check time-safety
    BipError &errorInTimeSafety = checkTimeSafety();

    if (errorInTimeSafety.type() != NO_ERROR) {      
      logger().log(errorInTimeSafety);
      
      return errorInTimeSafety;
    }

    // choose an interaction / internal port / external port
    TimeValue wakeUpTime = tryToExecute();

    if (logger().reachedLimit()) {
      break;
    }

    bool hasNotifications = false;
    
    if (wakeUpTime == TimeValue::MIN) {
      hasNotifications = platformClock().isNotified();
    }
    else {
      hasNotifications = modelClock().wait(wakeUpTime);
    }
    
    if (!hasNotifications) {
      if (wakeUpTime == TimeValue::MIN) {
        // deadlock or time-lock
        break;
      }
    }
    else {      
      // update model clock
      updateTime();
    }
  }

  BipError &error = Executer::jobs.waitForNoActivity();

  debug();

  // force threads to complete
  jobs.stop(BipError::NoError);

  // stop threads
  stopThreads();

  // log deadlock if the stop was not caused by an error
  if (error.type() == NO_ERROR) {
    logger().logDeadlock(top());
  }

  return error;
}

void Executer::updateTime() {  
  // update model-time w.r.t. platform clock
  modelClock().update();

  // update model-time w.r.t. resume time
  if (!isRealTime()) {
    for (vector<Atom *>::const_iterator atomIt = top().allAtoms().value().begin() ;
         atomIt != top().allAtoms().value().end() ;
         ++atomIt) {
      Atom &atom = **atomIt;
      
      if (atom.isReady() &&
          atom.hadResume()) {
        modelClock().wait(atom.time());
      }
    }
  }
}

BipError &Executer::checkExternals() {
  for (vector<Atom *>::const_iterator atomIt = top().allAtoms().value().begin() ;
       atomIt != top().allAtoms().value().end() ;
       ++atomIt) {
    Atom &atom = **atomIt;

    if (atom.isReady()) {
      BipError &error = atom.checkExternals();

      if (error.type() != NO_ERROR) {
        return error;
      }
    }
  }

  return BipError::NoError;
}

BipError &Executer::checkTimeSafety() const {
  if (!relaxed()) {
    for (vector<Connector *>::const_iterator connectorIt = top().allNonExportedConnectors().value().begin() ;
         connectorIt != top().allNonExportedConnectors().value().end() ;
         ++connectorIt) {
      Connector &connector = **connectorIt;

      if (connector.isReady()) {
        // check urgencies of interactions
        const vector<InteractionValue *> &interactions = connector.maximalInteractions();
  
        for (vector<InteractionValue *>::const_iterator interactionIt = interactions.begin() ;
             interactionIt != interactions.end() ;
             ++interactionIt) {
          const InteractionValue &interaction = **interactionIt;
          const TimingConstraint &timingConstraint = interaction.timingConstraintAfterPriorities();
          
          // check for resume-eager specific case
          if (modelClock().time() >= interaction.time() &&
              !hadResumeEager(interaction) &&
              !timingConstraint.wait(interaction.time()).in(modelClock().time())) {
            TimeSafetyViolationError &error = *new TimeSafetyViolationError(modelClock().time(), connector);

            if (relaxed()) {            
              logger().logWarning(error);
            }
            else {
              return error;
            }
          }
        }
      }
    }

    for (vector<Atom *>::const_iterator atomIt = top().allAtoms().value().begin() ;
         atomIt != top().allAtoms().value().end() ;
         ++atomIt) {
      Atom &atom = **atomIt;

      if (atom.isReady()) {
        // check invariant
        if (atom.hasInvariant()) {
          if (!atom.invariant().in(modelClock().time())) {
            TimeSafetyViolationError &error = *new TimeSafetyViolationError(modelClock().time(), atom);

            if (relaxed()) {            
              logger().logWarning(error);
            }
            else {
              return error;
            }
          }
        }
        
        // check urgencies of internal ports
        const vector<AtomInternalPort *> &enabledInternals = atom.internals();
        
        for (vector<AtomInternalPort *>::const_iterator portIt = enabledInternals.begin() ;
             portIt != enabledInternals.end() ;
             ++portIt) {
          const AtomInternalPort &port = **portIt;
          const TimingConstraint &timingConstraint = port.timingConstraint();

          // check for resume-eager specific case
          if (modelClock().time() >= atom.time() &&
              !hadResumeEager(port) &&
              !timingConstraint.wait(atom.time()).in(modelClock().time())) {            
            TimeSafetyViolationError &error = *new TimeSafetyViolationError(modelClock().time(), atom);

            if (relaxed()) {            
              logger().logWarning(error);
            }
            else {
              return error;
            }
          }
        }

        // check urgencies of external ports
        for (vector<AtomExternalPort *>::const_iterator portIt = atom.externals().begin() ;
             portIt != atom.externals().end() ;
             ++portIt) {
          const AtomExternalPort &port = **portIt;
          const TimingConstraint &timingConstraint = port.timingConstraint();
          
          // check for resume-eager specific case
          if (!hadResumeEager(port) &&
              !timingConstraint.wait(atom.time()).in(modelClock().time())) {            
            TimeSafetyViolationError &error = *new TimeSafetyViolationError(modelClock().time(), atom);

            if (relaxed()) {            
              logger().logWarning(error);
            }
            else {
              return error;
            }
          }
        }
      }
    }
  }

  return BipError::NoError;
}

TimeValue Executer::tryToExecute() {
  bool stable = true;
  TimeValue ret = TimeValue::MAX;
    
  for (vector<Connector *>::const_iterator connectorIt = top().allNonExportedConnectors().value().begin() ;
       connectorIt != top().allNonExportedConnectors().value().end() ;
       ++connectorIt) {
    Connector &connector = **connectorIt;
    
    if (!connector.isReady()) {
      stable = false;
    }
    else if (jobs.isStopped()) {
      break;
    }
    else {
      const vector<InteractionValue *> &interactions = connector.maximalInteractions();

      if (interactions.empty()) {
        continue;
      }
      
      Interval schedulable = Interval(TimeValue::MIN, connector.deadline());
      
      schedulable &= connector.invariant();

      if (connector.choice().isReset() ||
          !connector.hasChosenInteraction() ||
          !schedulable.in(connector.chosenTime()) ||
          (connector.hasChosenInteraction() && connector.chosenTime() < modelClock().time())) {
        TimeValue currentTime = modelClock().time();

        if (relaxed() &&
            !schedulable.empty() &&
            modelClock().time() > schedulable.right()) {
          currentTime = schedulable.right();
        }

        Scheduler scheduler(currentTime, asap(), firstEnabled());

        connector.clearChoice();

        connector.choice().setIsReset(false);
      
        const vector<InteractionValue *> &interactions = connector.maximalInteractions();
  
        for (vector<InteractionValue *>::const_iterator interactionIt = interactions.begin() ;
             interactionIt != interactions.end() ;
             ++interactionIt) {
          InteractionValue &interaction = **interactionIt;
        
          if (true) { // if (interaction.isReady()) {
            const TimingConstraint &timingConstraint = interaction.timingConstraintAfterPriorities();
 
            Interval interv = timingConstraint.interval() &&
              schedulable;
            
            // check for resume-eager specific case
            if (hadResumeEager(interaction)) {
              connector.setChoice(interaction, interaction.time());
              executeChoice(connector);

              stable = false;
              
              break;
            }
            else if (scheduler.choose(interv, false)) {
              connector.setChoice(interaction, scheduler.plannedTime());
            }
          }
        }
      }

      // execute immediately of wait
      if (connector.hasChosenInteraction()) {
        if (connector.chosenTime() == modelClock().time() ||
            (connector.chosenTime() <= modelClock().time() && relaxed())) {
          if (connector.chosenTime() < modelClock().time()) {
            assert(relaxed());

            TimeSafetyViolationError &error = *new TimeSafetyViolationError(modelClock().time(), connector);

            logger().logWarning(error);
          }
          
          executeChoice(connector);

          stable = false;
        }
        else if (connector.chosenTime() < ret) {
          ret = connector.chosenTime();
        }
      }
    }
  }
  
  for (vector<Atom *>::const_iterator atomIt = top().allAtoms().value().begin() ;
       atomIt != top().allAtoms().value().end() ;
       ++atomIt) {
    Atom &atom = **atomIt;

    if (!atom.isReady()) {
      stable = false;
    }
    else if (jobs.isStopped()) {
      break;
    }
    else {
      if (!atom.waiting().empty()) {
        stable = false;
      }
      
      const vector<AtomInternalPort *> &enabledInternals = atom.internals();

      if (enabledInternals.empty() &&
          atom.externals().empty()) {
        continue;
      }
      
      Interval schedulable = Interval(TimeValue::MIN, atom.deadline());

      if (atom.hasInvariant()) {
        schedulable &= atom.invariant();
      }
      
      if (atom.choice().isReset() ||
          !(atom.hasChosenInternal() && atom.hasChosenExternal()) ||
          !schedulable.in(atom.chosenTime()) ||
          ((atom.hasChosenInternal() || atom.hasChosenExternal()) && atom.chosenTime() < modelClock().time())) {
        TimeValue currentTime = modelClock().time();

        if (relaxed() &&
            !schedulable.empty() &&
            modelClock().time() > schedulable.right()) {
          currentTime = schedulable.right();
        }

        Scheduler scheduler(currentTime, asap(), firstEnabled());
      
        atom.clearChoice();

        atom.choice().setIsReset(false);
        
        for (vector<AtomInternalPort *>::const_iterator portIt = enabledInternals.begin() ;
             portIt != enabledInternals.end() ;
             ++portIt) {
          AtomInternalPort &port = **portIt;
          const TimingConstraint &timingConstraint = port.timingConstraint();
        
          Interval interv = timingConstraint.interval() &&
            schedulable;

          // check for resume-eager specific case
          if (hadResumeEager(port)) {
            atom.setChoice(port, atom.time());
            executeChoice(atom);

            stable = false;

            break;
          }
          else if (scheduler.choose(interv, false)) {
            atom.setChoice(port, scheduler.plannedTime());
          }
        }
      
        for (vector<AtomExternalPort *>::const_iterator portIt = atom.externals().begin() ;
             portIt != atom.externals().end() ;
             ++portIt) {
          AtomExternalPort &port = **portIt;
          const TimingConstraint &timingConstraint = port.timingConstraint();
        
          Interval interv = timingConstraint.interval() &&
            schedulable;
          
          // check for resume-eager specific case
          if (hadResumeEager(port)) {
            atom.setChoice(port, atom.time());
            executeChoice(atom);            

            stable = false;
            
            break;
          }
          else if (scheduler.choose(interv, false)) {
            atom.setChoice(port, scheduler.plannedTime());
          }
        }
      }

      // execute immediately or wait
      if (atom.hasChosenInternal() || atom.hasChosenExternal()) {
        if (atom.chosenTime() == modelClock().time() ||
            (atom.chosenTime() <= modelClock().time() && relaxed())) {
          if (atom.chosenTime() < modelClock().time()) {
            assert(relaxed());

            TimeSafetyViolationError &error = *new TimeSafetyViolationError(modelClock().time(), atom);

            logger().logWarning(error);
          }
          
          executeChoice(atom);
          
          stable = false;
        }
        else if (atom.chosenTime() < ret) {
          ret = atom.chosenTime();
        }
      }
    }
  }

  if (stable && ret == TimeValue::MAX) {
    ret = TimeValue::MIN;
  }

  return ret;
}

void Executer::executeChoice(Connector &connector) {
  if (!logger().reachedLimit()) {
    assert(connector.hasChosenInteraction());
  
    // log the executed interaction
    logger().log(connector.chosenInteraction(), connector.chosenTime(), Interval(), connector.maximalInteractions());

    // down
    connector.down(connector.chosenInteraction());

    // reset the liste of atoms to be restarted
    mAtoms.clear();
    
    // configure jobs and resources of atoms
    connector.chosenInteraction().configureJobs(connector.chosenTime(), mAtoms);
    
    // reset choice
    connector.clearChoice();

    // retart all atoms involved in chosen interaction
    for (vector<Atom *>::const_iterator atomIt = mAtoms.begin() ;
         atomIt != mAtoms.end() ;
         ++atomIt) {
      Atom &atom = **atomIt;

      atom.executeJob().restart();
    }
  }
}
  
void Executer::executeChoice(Atom &atom) {
  if (!logger().reachedLimit()) {
    if (atom.hasChosenInternal()) {    
      // log the executed internal port
      logger().log(atom.chosenInternal(), atom.chosenTime(), Interval(), atom.internals());

      atom.executeJob().setChosenInternal(atom.chosenInternal());
      atom.executeJob().setChosenTime(atom.chosenTime());
      atom.setHasResume(atom.chosenInternal().portValue().hasResume());
    }
    else if (atom.hasChosenExternal()) {            
      // log the executed external port
      logger().log(atom.chosenExternal(), atom.chosenTime(), Interval(), atom.externals());
      
      atom.executeJob().setChosenExternal(atom.chosenExternal());
      atom.executeJob().setChosenTime(atom.chosenTime());
      atom.setHasResume(atom.chosenExternal().hasResume());
    }
    else {
      assert(false);
    }
  
    atom.executeJob().configureResources();

    atom.executeJob().restart();
 
    atom.clearChoice();
  }
}
  

void Executer::resetAllPriorities(const Compound &compound) {
  // recursive call to sub-components
  for (map<string, Component *>::const_iterator componentIt = compound.components().begin() ;
       componentIt != compound.components().end() ;
       ++componentIt) {
    Component &component = *componentIt->second;

    if (component.type() == COMPOUND) {
      Compound &compound = dynamic_cast<Compound &>(component);
      
      resetAllPriorities(compound);
    }
  }

  for (map<string, Connector *>::const_iterator connectorIt = compound.connectors().begin() ;
       connectorIt != compound.connectors().end() ;
       ++connectorIt) {
    Connector &connector = *connectorIt->second;

    connector.invariant().reset();
    connector.urgent().reset();
  }
  
  for (vector<Priority *>::const_iterator priorityIt = compound.priorities().begin() ;
       priorityIt != compound.priorities().end() ;
       ++priorityIt) {
    Priority &priority = **priorityIt;

    priority.active().reset();
  }
}

void Executer::initializeThreads() {
  for (unsigned int i = 0 ; i < mNbThreads ; ++i) {
    Thread *thrd = new Thread(Executer::jobs);

    if (hasInitializeThreadCallBack()) {
      thrd->setInitializeCallBack(initializeThreadCallBack());
    }
    
    mThreads.push_back(thrd);
  }
}

void Executer::startThreads() {
  for (vector<Thread *>::const_iterator thrdIt = mThreads.begin() ;
       thrdIt != mThreads.end() ;
       ++thrdIt) {
    Thread &thrd = **thrdIt;
    thrd.start();
  }
}

void Executer::stopThreads() {
  for (vector<Thread *>::const_iterator thrdIt = mThreads.begin() ;
       thrdIt != mThreads.end() ;
       ++thrdIt) {
    Thread *thrd = *thrdIt;

    thrd->join();

    delete thrd;
  }
}

/**
 * \brief Displays the current state of the system.
 *
 * Useful for debug purpose. Side effect on cout.
 */
void Executer::print() {
  StringTree debugStrTree = print(top());
  string header = "[BIP ENGINE]: ";
  string debugStr = debugStrTree.toString(header);
  cout << debugStr;
}

StringTree Executer::print(const ConnectorExportPort &port) {
  StringTree ret(port.name());

  for (unsigned int i = 0 ; i < port.portValues().size() ; ++i) {
    string portStr = port.portValues()[i]->toString();

    if (portStr.size() == 0) {
      string noValueStr = "<no_value>";
      ret.addChild(noValueStr);
    }
    else {
      ret.addChild(portStr);
    }
  }

  return ret;
}

StringTree Executer::print(const CompoundExportPort &port) {
  StringTree ret(port.name());

  for (unsigned int i = 0 ; i < port.portValues().size() ; ++i) {
    string portStr = port.portValues()[i]->toString();

    if (portStr.size() == 0) {
      string noValueStr = "<no_value>";
      ret.addChild(noValueStr);
    }
    else {
      ret.addChild(portStr);
    }
  }

  return ret;
}

StringTree Executer::print(const AtomInternalPort &port) {
  string ret = port.name();

  if (port.hasPortValue()) {
    string portStr = port.portValue().toString();

    if (portStr.size() > 0) {
      ret = ret + " (" + portStr + ")";
    }
    else {
      ret = ret + " (<no_value>)";
    }
  }

  return StringTree(ret);
}

StringTree Executer::print(const AtomExportPort &port) {
  StringTree ret(port.name());

  for (unsigned int i = 0 ; i < port.internalPorts().size() ; ++i) {
    const AtomInternalPort &internalPort = *(port.internalPorts()[i]);
    ret.addChild(print(internalPort));
  }

  return ret;
}

StringTree Executer::print(const InteractionValue &interaction) {
  ostringstream oss;

  oss << interaction;

  //if (!interaction.isDominated()) oss << "*";

  return StringTree(oss.str());
}

StringTree Executer::print(const Connector &connector) {
  StringTree ret(connector.name());

  if (connector.hasExportedPort()) {
    StringTree portTree = print(connector.exportedPort());
    ret.addChild(portTree);
  }

  const vector<InteractionValue *> &interactions = connector.enabledInteractions();

  for (unsigned int i = 0 ; i < interactions.size() ; ++i) {
    StringTree interactionTree = print(*interactions[i]);
    ret.addChild(interactionTree);
  }
  //connector.release(interactions);

  return ret;
}

StringTree Executer::print(const Atom &atom) {
  StringTree ret(atom.name());

  for (map<string, AtomExportPort *>::const_iterator portIt = atom.ports().begin() ;
       portIt != atom.ports().end() ;
       ++portIt) {
    const AtomExportPort &port = *(portIt->second);
    StringTree portTree = print(port);
    ret.addChild(portTree);
  }

  string atomStr = atom.toString();
  string::const_iterator beginIt = atomStr.begin();
  string::const_iterator strIt = atomStr.begin() ;

  while (strIt != atomStr.end()) {
    if (*strIt == '\n') {
      string childStr = string(beginIt, strIt);
      ret.addChild(childStr);
      beginIt = strIt + 1;
    }

    ++strIt;
  }

  if (beginIt != atomStr.end()) {
    string childStr = string(beginIt, strIt);
    ret.addChild(childStr);
  }

  return ret;
}

StringTree Executer::print(const Compound &compound) {
  StringTree ret(compound.name());

  for (map<string, CompoundExportPort *>::const_iterator portIt = compound.ports().begin() ;
       portIt != compound.ports().end() ;
       ++portIt) {
    const CompoundExportPort &port = *(portIt->second);
    StringTree portTree = print(port);
    ret.addChild(portTree);
  }

  for (map<string, Connector *>::const_iterator connIt = compound.connectors().begin() ;
       connIt != compound.connectors().end() ;
       ++connIt) {
    const Connector &connector = *(connIt->second);
    StringTree connectorTree = print(connector);
    ret.addChild(connectorTree);
  }

  for (map<string, Component *>::const_iterator connIt = compound.components().begin() ;
       connIt != compound.components().end() ;
       ++connIt) {
    const Component &component = *(connIt->second);
    StringTree componentTree = print(component);
    ret.addChild(componentTree);
  }

  return ret;
}

StringTree Executer::print(const Component &component) {
  if (component.type() == COMPOUND) {
    const Compound &compound = (const Compound &) component;
    return print(compound);
  }
  else if (component.type() == ATOM) {
    const Atom &atom = (const Atom &) component;
    return print(atom);
  }
  else {
    assert(false);
  }

  return StringTree();
}

// the size should be greater than the total number of jobs
ReadyQueue<Job> Executer::jobs(100000);

ModelClock Executer::mModelClock;
RealTimeClock Executer::mRealTimeClock;
SimulationClock Executer::mSimulationClock;
NotifiableClock *Executer::mPlatformClock(NULL);
