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

#include <ExecuteConnectorJob.hpp>
#include "ExecuteAtomJob.hpp"

#include "Connector.hpp"
#include "Compound.hpp"
#include "InteractionValue.hpp"
#include "Port.hpp"
#include "Atom.hpp"
#include "AtomExportPort.hpp"

#include "Executer.hpp"
#include "Logger.hpp"

#include "BipError.hpp"
#include "Resetable.hpp"
#include "CycleInPriorities.hpp"
#include <TimeSafetyViolationError.hpp>
#include "CycleInPrioritiesError.hpp"

#include <TimeValue.hpp>

ExecuteConnectorJob::ExecuteConnectorJob(Connector &connector) :
  Job(Executer::jobs),
  mConnector(connector),
  mLogger(NULL),
  mReserver(*this),
  mReader(*this),
  mChosenInteraction(NULL),
  mChosenTime(TimeValue::MIN),
  mNbAttempt(0),
  mNbSuccess(0) {
}

ExecuteConnectorJob::~ExecuteConnectorJob() {
}

void ExecuteConnectorJob::initialize() {
  if (!connector().hasExportedPort()) {
    // update job of maximal interactions
    // connector().maximalInteractions().addUser(*this);
    mResetChoice.addUser(*this);
    mResetChoice.dependsOn(connector().maximalInteractions());

    // compute all the atoms involved in the connector
    mAllAtoms = allAtoms();

    // add all port resources of atoms involved in the connector
    for (vector<Atom *>::const_iterator atomIt = mAllAtoms.begin() ;
         atomIt != mAllAtoms.end() ;
         ++atomIt) {
      Atom &atom = **atomIt;

      mReader.addResources(atom.resources()); // mReader.addResources(atom.portsResources());
      mReserver.addResources(atom.resources());
      mObserveReserver.addObservedResources(atom.portsResources());
    }

    // compute all resources used by this
    mReader.addResources(connector().maximalInteractions().allResources());
    mReserver.addObservedResources(connector().maximalInteractions().allResources());
    mObserveReserver.addObservedResources(connector().maximalInteractions().allResources());

    if (isRealTime()) {
      if (!connector().allLowMayHaveUrgency()) {
        connector().timeSafetyValidator().setDisabledTimeSafety(true);
      }
    }
    else {
      connector().timeSafetyValidator().addResource(simulationClockResource());
      connector().timeSafetyValidator().setIncludesInvariants(true);
    }

    if (!isRealTime()) {
      mReader.addResource(simulationClockResource());
      mReserver.addResource(simulationClockResource());
      mObserveReserver.addResource(simulationClockResource());

      mUpdateClockJob.initialize();
    }
  }
}

void ExecuteConnectorJob::initializeReservers() {
  if (!connector().hasExportedPort()) {
    mReserver.initialize();
    mObserveReserver.initialize();
  }
}

void ExecuteConnectorJob::realJob() {  
  // only top-level connectors can schedule interactions
  assert(!connector().hasExportedPort());

  // compute time-safe interval
  if (relaxed()) {
    assert(!connector().timeSafetyValidator().timeSafe().empty());
    assert(!connector().timeSafetyValidator().timeSafe().rightOpen());

    // backtracking is allowed in the relaxed mode only in NDEBUG mode
#ifdef NDEBUG
    mTimeSafe = Interval(TimeValue::MIN, connector().timeSafetyValidator().timeSafe().right())
#else     
    mTimeSafe = connector().timeSafetyValidator().timeSafe()
#endif
      && connector().invariant();
  }
  else {
    mTimeSafe = connector().timeSafetyValidator().timeSafe()
      && connector().invariant();
  }

  if (hasChosen()              &&
      hasWokenUpAtWakeUpTime() &&
      !mResetChoice.isReset()  &&
      mTimeSafe.in(chosenTime())) {
    // in simulation mode interaction are executed directly without sleeping periods
    assert(isRealTime());

    // try to execute the chosen interaction
    tryToExecuteChosenInteraction();
  }
  else {
    // update current time
    updateTime();
    
    // check time-safety
    BipError &error = checkTimeSafety(mTime);

    if (error.type() != NO_ERROR) {
      logger().log(error);
    }
    else {
      // chose an interaction...
      recomputeChoice();

      // ... and try to execute it
      if (hasChosen()) {
        if (!isRealTime() || platformClock().time() >= chosenTime()) {
          tryToExecuteChosenInteraction();
        }
        else {
          restart(chosenTime());
        }
      }
    }
  }
}

void ExecuteConnectorJob::updateTime() {
  // update model-time w.r.t. real-time
  mModelClock.update();
  
  // update time used for scheduling interactions
  if (!connector().timeSafetyValidator().timeSafe().in(mModelClock.time()) && hasResumeEagerInteraction()) {
    assert(!connector().timeSafetyValidator().timeSafe().empty());
    assert(!connector().timeSafetyValidator().timeSafe().rightOpen());

    mTime = connector().timeSafetyValidator().timeSafe().right();
  }
  else if (!mTimeSafe.in(mModelClock.time()) && relaxed()) {
    assert(!connector().timeSafetyValidator().timeSafe().empty());
    assert(!connector().timeSafetyValidator().timeSafe().rightOpen());

    mTime = connector().timeSafetyValidator().timeSafe().right();

    Interval invariant = connector().invariant();

    assert(!invariant.empty());
    assert(!invariant.rightOpen());

    if (invariant.right() < mTime) {
      mTime = invariant.right();
    }
  }
  else {
    if (isRealTime() || mModelClock.time() >= connector().time()) {
      mTime = mModelClock.time();
    }
    else {
      mTime = connector().time();
    }
  }
}

BipError &ExecuteConnectorJob::checkTimeSafety(const TimeValue &time) {
  assert(!mTimeSafe.empty());
  
  if (!mTimeSafe.in(time) && !relaxed()) {
    if (!mReserver.hasReservation()) {
      if (!mTimeSafe.empty() &&
	  (time - mTimeSafe.right() <= TimeValue(10, MILLISECOND))) {
        restart(mTimeSafe.right() + TimeValue(20, MILLISECOND));
      }
      else {
        TimeSafetyViolationError &error = *new TimeSafetyViolationError(time, connector());

        return error;
      }
    }
  }

  return BipError::NoError;
}

BipError &ExecuteConnectorJob::checkTimeSafeExecution(const TimeValue &time) {
  assert(!mTimeSafe.empty());
  
  if (!mTimeSafe.in(time)) {
    TimeSafetyViolationError &error = *new TimeSafetyViolationError(time, connector());

    return error;
  }

  return BipError::NoError;
}

bool ExecuteConnectorJob::tryToExecuteChosenInteraction() {
  if (profiling()) ++mNbAttempt;
  
  // try to lock dependent atoms
  bool success = mReserver.tryToReserve(chosenTime(), relaxed());

  if (success) {
    BipError &error = checkTimeSafeExecution(mModelClock.time());

    if (error.type() != NO_ERROR) {
      if (relaxed()) {
        logger().logWarning(error);
      }
      else {
        assert(hasResumeEagerInteraction());
      }
    }

    // execute chosen interaction
    executeChosenInteraction();
      
    if (profiling()) ++mNbSuccess;
    
    if (!isRealTime()) {
      mUpdateClockJob.setTime(chosenTime());
      mUpdateClockJob.restart();
    }
  }
  
  // self-restart
  restart();

  return success;
}

void ExecuteConnectorJob::logChosenInteraction() {
  assert(hasChosen());
  
  // log the executed interaction
  logger().log(chosenInteraction(), chosenTime(), mSchedulable, connector().maximalInteractions());
}


void ExecuteConnectorJob::executeChosenInteraction() {
  assert(hasChosen());

  // down
  connector().down(chosenInteraction());

  // configure execute atom jobs
  chosenInteraction().execute(chosenTime());
}

void ExecuteConnectorJob::recomputeChoice() {  
  // reset chosen interaction and time
  mChosenInteraction = NULL;
  mChosenTime = TimeValue::MIN;

  // reinitialize real-time scheduler
  Scheduler scheduler(mTime, asap(), firstEnabled());
  
  mSchedulable = mTimeSafe && Interval(mTime, mReserver.deadline());
  
  const vector<InteractionValue *> &interactions = connector().maximalInteractions();

  for (vector<InteractionValue *>::const_iterator interactionIt = interactions.begin() ;
       interactionIt != interactions.end() ;
       ++interactionIt) {
    InteractionValue &interaction = **interactionIt;

    Interval interv = interaction.timingConstraintAfterPriorities().interval() &&
      mSchedulable;
    
    if (scheduler.choose(interv, false)) {
      mChosenInteraction = &interaction;
    }
  }

  if (mChosenInteraction != NULL) {
    mChosenTime = scheduler.plannedTime();

    assert(mChosenTime != TimeValue::MIN);

    // update reserver
    mReserver.resetTo(mObserveReserver);

    // add all resources of the involved atoms
    mChosenInteraction->configureReserver(mReserver);
  }

  // mark choice as non-reset
  mResetChoice.setIsReset(false);
}

bool ExecuteConnectorJob::hasResumeEagerInteraction() const {
  bool ret = false;
  
  if (lastAtomHasResume()) {
    const vector<InteractionValue *> &interactions = connector().maximalInteractions();
    
    for (vector<InteractionValue *>::const_iterator interactionIt = interactions.begin() ;
         interactionIt != interactions.end() ;
         ++interactionIt) {
      const InteractionValue &interaction = **interactionIt;
      const TimingConstraint &timingConstraint = interaction.timingConstraintAfterPriorities();

      // TO BE FIXED!
      // if (timingConstraint.in(connector().time()) && timingConstraint.urgency() == EAGER) {
      if (timingConstraint.wait(interaction.time()).right() == interaction.time()) {
        ret = true;
        break;
      }
    }
  }

  return ret;
}

bool ExecuteConnectorJob::lastAtomHasResume() const {
  bool ret = false;
  
  for (vector<Atom *>::const_iterator atomIt = mAllAtoms.begin() ;
       atomIt != mAllAtoms.end() ;
       ++atomIt) {
    Atom &atom = **atomIt;

    if (atom.hadResume() &&
        atom.time() == connector().time()) {
      ret = true;
      break;
    }
  }

  return ret;
}

vector<Atom *> ExecuteConnectorJob::allAtoms() {
  vector<Atom *> ret;

  allAtoms(ret, connector());

  return ret;
}

void ExecuteConnectorJob::allAtoms(vector<Atom *> &atoms, const Connector &connector) {
  for (vector<QuotedPortReference *>::const_iterator quotedPortIt = connector.ports().begin() ;
       quotedPortIt != connector.ports().end() ;
       ++quotedPortIt) {
    const QuotedPortReference &quotedPort = **quotedPortIt;
    const Port &port = quotedPort.port();

    allAtoms(atoms, port);
  }
}

void ExecuteConnectorJob::allAtoms(vector<Atom *> &atoms, const Port &port) {
  if (port.type() == ATOM_EXPORT) {
    const AtomExportPort &atomPort = dynamic_cast<const AtomExportPort &>(port);
    Atom &atom = atomPort.holder();

    if (find(atoms.begin(),
             atoms.end(),
             &atom)
        == atoms.end()) {
      atoms.push_back(&atom);
    }
  }
  else if (port.type() == CONNECTOR_EXPORT) {
    const ConnectorExportPort &connectorPort = dynamic_cast<const ConnectorExportPort &>(port);
    const Connector &connector = connectorPort.holder();    

    allAtoms(atoms, connector);
  }
  else if (port.type() == COMPOUND_EXPORT) {
    const CompoundExportPort &compoundPort = dynamic_cast<const CompoundExportPort &>(port);

    for (vector<Port *>::const_iterator forwardPortIt = compoundPort.forwardPorts().begin() ;
         forwardPortIt != compoundPort.forwardPorts().end() ;
         ++forwardPortIt) {
      const Port &forwardPort = **forwardPortIt;

      allAtoms(atoms, forwardPort);
    }
  }
  else {
    assert(false);
  }
}

const Resource &ExecuteConnectorJob::latestDependentFreedResource() const {
  const Resource *ret = NULL;
  
  const Resource &latestResource = latestFreed(mReserver.resources());
  const Resource &latestObservedResource = latestFreed(mReserver.observedResources());
  const Resource::Validator &latestValidator = latestValidated(mReserver.validators());
  
  if (latestResource.freeTime() >= latestObservedResource.freeTime() &&
      latestResource.freeTime() >= latestValidator.validationTime()) {
    ret = &latestResource;
  }
  else if (latestObservedResource.freeTime() >= latestValidator.validationTime()) {
    ret = &latestObservedResource;
  }
  else {
    ret = &latestFreed(latestValidator);    
  }

  assert(ret != NULL);

  return *ret;
}

const Resource &ExecuteConnectorJob::latestFreed(const vector<Resource *> &resources) const {
  const Resource *ret = NULL;
  TimeValue latestFreeTime = TimeValue::MIN;
    
  for (vector<Resource *>::const_iterator resourceIt = resources.begin() ;
       resourceIt != resources.end() ;
       ++resourceIt) {
    const Resource &resource = **resourceIt;

    assert(resource.debug());
    
    if (resource.freeTime() > latestFreeTime) {      
      latestFreeTime = resource.freeTime();
      ret = &resource;
    }
  }

  assert(latestFreeTime != TimeValue::MIN);
  assert(ret != NULL);

  return *ret;
}

const Resource &ExecuteConnectorJob::latestFreed(const Resource::Validator &validator) const {
  const Resource *ret = NULL;
  TimeValue latestFreeTime = TimeValue::MIN;
    
  for (vector<Resource *>::const_iterator resourceIt = validator.resources().begin() ;
       resourceIt != validator.resources().end() ;
       ++resourceIt) {
    const Resource &resource = **resourceIt;

    assert(resource.debug());
    assert(validator.debug());
    
    if (resource.freeTime() > latestFreeTime &&
        resource.freeTime() <= validator.validationTime()) {
      latestFreeTime = resource.freeTime();
      ret = &resource;
    }
  }

  assert(latestFreeTime != TimeValue::MIN);
  assert(ret != NULL);

  return *ret;
}

const Resource::Validator &ExecuteConnectorJob::latestValidated(const vector<Resource::Validator *> &validators) const {
  const Resource::Validator *ret = NULL;
  TimeValue latestValidationTime = TimeValue::MIN;
    
  for (vector<Resource::Validator *>::const_iterator validatorIt = validators.begin() ;
       validatorIt != validators.end() ;
       ++validatorIt) {
    const Resource::Validator &validator = **validatorIt;

    assert(validator.debug());
    
    if (validator.validationTime() > latestValidationTime) {
      latestValidationTime = validator.validationTime();
      ret = &validator;
    }
  }
  
  assert(latestValidationTime != TimeValue::MIN);
  assert(ret != NULL);

  return *ret;
}

const Atom &ExecuteConnectorJob::holder(const Resource &resource) const {
  const Atom *ret = NULL;

  for (vector<Atom *>::const_iterator atomIt = connector().holder().root().allAtoms().value().begin() ;
       atomIt != connector().holder().root().allAtoms().value().end() ;
       ++atomIt) {
    const Atom &atom = **atomIt;

    if (find(atom.resources().begin(),
             atom.resources().end(),
             &resource)
        != atom.resources().end()) {
      ret = &atom;
      break;
    }
  }

  assert(ret != NULL);

  return *ret;
}

void ExecuteConnectorJob::log() {  
  mSchedulable = mTimeSafe && Interval(mTime, mReserver.deadline());

  const vector<InteractionValue *> &interactions = connector().maximalInteractions();

  for (vector<InteractionValue *>::const_iterator interactionIt = interactions.begin() ;
       interactionIt != interactions.end() ;
       ++interactionIt) {
    InteractionValue &interaction = **interactionIt;

    cout << connector().name() << ": " << interaction << " " << interaction.timingConstraint() << " " << interaction.timingConstraintAfterPriorities() << " " << mTime << " " << connector().timeSafetyValidator().timeSafe() << " "  << " " << connector().invariant() << mReserver.deadline() << endl;
  }
}
