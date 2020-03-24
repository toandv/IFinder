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

#include <Atom.hpp>
#include <AtomExportPort.hpp>
#include <Compound.hpp>
#include <Interaction.hpp>
#include <InteractionValue.hpp>
#include <ConnectorExportPort.hpp>
#include <QuotedPortReference.hpp>
#include <Priority.hpp>
#include <BipError.hpp>
#include "ExecuteAtomJob.hpp"
#include "ExecuteConnectorJob.hpp"
#include <CycleInPriorities.hpp>
#include <CycleInPrioritiesError.hpp>
#include <ModelClock.hpp>
#include <Executer.hpp>
#include <AtomTimeSafetyValidator.hpp>
#include <ConnectorTimeSafetyValidator.hpp>
#include <Logger.hpp>

// constructors
Compound::Compound(const string &name) :
  ComponentItf(name, COMPOUND),
  Component(name, COMPOUND),
  CompoundItf(name),
  mAllCyclesInPriorities(this, &Compound::computeAllCyclesInPriorities),
  mAllNonExportedConnectors(this, &Compound::computeAllNonExportedConnectors),
  mAllAtoms(this, &Compound::computeAllAtoms) {
  /* implement your constructor here */
}

// destructor
Compound::~Compound() {
  // get interactions of top level non exported connectors
  for (map<string, Connector *>::const_iterator connectorIt = connectors().begin() ;
       connectorIt != connectors().end() ;
       ++connectorIt) {
    Connector &connector = *connectorIt->second;
   
    connector.release();
  }

  // delete all precomputed cycles
  if (mAllCyclesInPriorities.isInitialized()) { 
    for (set<CycleInPriorities *>::const_iterator cycleIt = allCyclesInPriorities().begin() ;
         cycleIt != allCyclesInPriorities().end() ;
         ++cycleIt) {
      CycleInPriorities *cycle = *cycleIt;

      delete cycle;
    }
  }
}

/* \brief Execute a port value of a compound component.
 *
 * \param portValue is the target port value, it should be a port value
 * associated to one of the exported ports.
 *
 * \return An error if found during the execution of involved atoms or
 * during the update of port values, BipError::NoError otherwise.
 */
BipError &Compound::execute(PortValue &portValue, const TimeValue &time) {
  // find the port from which comes the port value
#ifndef NDEBUG
  bool found = false;
#endif

  for (map<string, CompoundExportPort *>::const_iterator portIt = ports().begin() ;
       portIt != ports().end() ;
       ++portIt) {
    CompoundExportPort &port = *portIt->second;

    // execute
    if (port.hasPortValue(portValue)) {
      BipError &error = port.execute(portValue, time);

      if (error.type() != NO_ERROR) {
        return error;
      }

#ifndef NDEBUG
      found = true;
#endif
      break;
    }
  }

  // port value must belong to one of the ports
  assert(found);

  return BipError::NoError;
}

/* \brief Execute an interaction value of a connector of a compound.
 *
 * \param interactionValue is the target interaction value, it should an
 * interaction value of a top-level connector.
 *
 * \return An error if found during the execution of involved atoms or
 * during the update of port values, BipError::NoError otherwise.
 */
BipError &Compound::execute(InteractionValue &interactionValue, const TimeValue &time) {
  // execute the interaction
  BipError &error = interactionValue.execute(time);

  if (error.type() != NO_ERROR) {
    return error;
  }

  return BipError::NoError;
}

/* \brief Initialize a compound component.
 *
 * \param Recursively calls initialize of all components, and updates
 * port values of the compound 'this'.
 *
 * \return An error if found during the initialization of the involved
 * components or during the update of port values, BipError::NoError otherwise.
 */
BipError &Compound::initialize() {
  // recursive call to sub-components
  for (map<string, Component *>::const_iterator componentIt = components().begin() ;
       componentIt != components().end() ;
       ++componentIt) {
    Component &component = *componentIt->second;

    if (component.type() == COMPOUND) {
      Compound &compound = dynamic_cast<Compound &>(component);

      compound.initialize();
    }
    else if (component.type() == ATOM) {
      Atom &atom = dynamic_cast<Atom &>(component);

      atom.initialize0();
      atom.initializeResources();
    }
    else {
      assert(false);
    }
  }

  // initialize connectors
  for (map<string, Connector *>::const_iterator connectorIt = connectors().begin() ;
       connectorIt != connectors().end() ;
       ++connectorIt) {
    Connector &connector = *connectorIt->second;

    connector.initialize();
  }

  // initialize priorities
  for (vector<Priority *>::const_iterator priorityIt = priorities().begin() ;
       priorityIt != priorities().end() ;
       ++priorityIt) {
    Priority &priority = **priorityIt;

    priority.initialize();
  }

  // initialize priorities second pass
  for (vector<Priority *>::const_iterator priorityIt = priorities().begin() ;
       priorityIt != priorities().end() ;
       ++priorityIt) {
    Priority &priority = **priorityIt;

    priority.initialize2();
  }

  // set dependencies between initializable objects and initialize them
  for (vector<Priority* >::const_iterator priorityIt = priorities().begin() ;
       priorityIt != priorities().end() ;
       ++priorityIt) {
    Priority &priority = **priorityIt;

    mAllCyclesInPriorities.dependsOn(priority.dominatedPriorities());
  }

  mAllCyclesInPriorities.initialize();

  // set dependencies to other components
  for (map<string, Component *>::const_iterator componentIt = components().begin() ;
       componentIt != components().end() ;
       ++componentIt) {
    Component &component = *componentIt->second;

    if (component.type() == COMPOUND) {
      Compound &compound = dynamic_cast<Compound &>(component);

      mAllNonExportedConnectors.dependsOn(compound.mAllNonExportedConnectors);
      mAllAtoms.dependsOn(compound.mAllAtoms);
    }
  }

  mAllNonExportedConnectors.initialize();
  mAllAtoms.initialize();

  return BipError::NoError;
}

void Compound::initialize2() {
  // recursive call to sub-components
  for (map<string, Component *>::const_iterator componentIt = components().begin() ;
       componentIt != components().end() ;
       ++componentIt) {
    Component &component = *componentIt->second;

    if (component.type() == COMPOUND) {
      Compound &compound = dynamic_cast<Compound &>(component);
      compound.initialize2();
    }
  }

  // initialize connectors
  for (map<string, Connector *>::const_iterator connectorIt = connectors().begin() ;
       connectorIt != connectors().end() ;
       ++connectorIt) {
    Connector &connector = *connectorIt->second;

    connector.initialize2();
  }
}

void Compound::initialize3() {
  // recursive call to sub-components
  for (map<string, Component *>::const_iterator componentIt = components().begin() ;
       componentIt != components().end() ;
       ++componentIt) {
    Component &component = *componentIt->second;

    if (component.type() == COMPOUND) {
      Compound &compound = dynamic_cast<Compound &>(component);
      compound.initialize3();
    }
  }

  // initialize atoms
  for (map<string, Component *>::const_iterator componentIt = components().begin() ;
       componentIt != components().end() ;
       ++componentIt) {
    Component &component = *componentIt->second;

    if (component.type() == ATOM) {
      Atom &atom = dynamic_cast<Atom &>(component);

      atom.executeJob().initialize();
    }
  }
}

void Compound::initialize4() {
  // recursive call to sub-components
  for (map<string, Component *>::const_iterator componentIt = components().begin() ;
       componentIt != components().end() ;
       ++componentIt) {
    Component &component = *componentIt->second;

    if (component.type() == COMPOUND) {
      Compound &compound = dynamic_cast<Compound &>(component);
      compound.initialize4();
    }
  }

  // initialize atoms
  for (map<string, Component *>::const_iterator componentIt = components().begin() ;
       componentIt != components().end() ;
       ++componentIt) {
    Component &component = *componentIt->second;

    if (component.type() == ATOM) {
      Atom &atom = dynamic_cast<Atom &>(component);

      atom.executeInternalsJob().initialize();
      atom.executeExternalsJob().initialize();
      atom.timeSafetyValidator().initialize();
    }
  }

  // initialize connector jobs dependencies
  for (map<string, Connector *>::const_iterator connectorIt = connectors().begin() ;
       connectorIt != connectors().end() ;
       ++connectorIt) {
    Connector &connector = *connectorIt->second;

    connector.executeJob().initialize();
    connector.timeSafetyValidator().initialize();
  }
}

void Compound::initializeValidators(Logger &logger) {
  // recursive call to sub-components
  for (map<string, Component *>::const_iterator componentIt = components().begin() ;
       componentIt != components().end() ;
       ++componentIt) {
    Component &component = *componentIt->second;

    if (component.type() == COMPOUND) {
      Compound &compound = dynamic_cast<Compound &>(component);
      
      compound.initializeValidators(logger);
    }
  }
  
  // initialize validators
  for (set<CycleInPriorities *>::const_iterator cycleIt = allCyclesInPriorities().begin() ;
       cycleIt != allCyclesInPriorities().end() ;
       ++cycleIt) {
    CycleInPriorities &cycle = **cycleIt;

    cycle.setLogger(logger);

    // check for cycles not detected by validators
    if (cycle.resources().empty()) {
      if (cycle.allGuardsTrue()) {
        CycleInPrioritiesError &error = *(new CycleInPrioritiesError());

        vector<const Interaction *> interactionsOfCycle = cycle.interactions();

        for (vector<const Interaction *>::const_iterator interactionIt = interactionsOfCycle.begin() ;
             interactionIt != interactionsOfCycle.end() ;
             ++interactionIt) {
          const Interaction &interaction = **interactionIt;

          error.addCycle(interaction);
        }

        logger.log(error);
      }
    }
  }
}

void Compound::initializeReservers() {
  // recursive call to sub-components
  for (map<string, Component *>::const_iterator componentIt = components().begin() ;
       componentIt != components().end() ;
       ++componentIt) {
    Component &component = *componentIt->second;

    if (component.type() == COMPOUND) {
      Compound &compound = dynamic_cast<Compound &>(component);
      
      compound.initializeReservers();
    }
    else if (component.type() == ATOM) {
      Atom &atom = dynamic_cast<Atom &>(component);
      
      // allocate mutexs groups in fast mutex vectors
      atom.executeJob().initializeReservers();
      atom.executeInternalsJob().initializeReservers();
      atom.executeExternalsJob().initializeReservers();
    }
  }
  
  // allocate mutexs groups in fast mutex vectors
  for (map<string, Connector *>::const_iterator connectorIt = connectors().begin() ;
       connectorIt != connectors().end() ;
       ++connectorIt) {
    Connector &connector = *connectorIt->second;

    connector.executeJob().initializeReservers();
  }
}

void Compound::initializeResetable() {
  // initialize resetable of ports
  for (map<string, CompoundExportPort *>::const_iterator portIt = ports().begin() ;
       portIt != ports().end() ;
       ++portIt) {
    CompoundExportPort &port = *portIt->second;

    port.initializeResetable();
  }
  
  // 
  for (map<string, Component *>::const_iterator componentIt = components().begin() ;
       componentIt != components().end() ;
       ++componentIt) {
    Component &component = *componentIt->second;

    if (component.type() == COMPOUND) {
      Compound &compound = dynamic_cast<Compound &>(component);
      
      compound.initializeResetable();
    }
    else if (component.type() == ATOM) {
      Atom &atom = dynamic_cast<Atom &>(component);

      atom.initializeResetable();
    }
  }
  
  // 
  for (map<string, Connector *>::const_iterator connectorIt = connectors().begin() ;
       connectorIt != connectors().end() ;
       ++connectorIt) {
    Connector &connector = *connectorIt->second;

    connector.initializeResetable();
  }

  for (vector<Priority *>::const_iterator priorityIt = priorities().begin() ;
       priorityIt != priorities().end() ;
       ++priorityIt) {
    Priority &priority = **priorityIt;

    priority.initializeResetable();
  }
}

void Compound::restartAllAtoms() {
  for (map<string, Component *>::const_iterator componentIt = components().begin() ;
       componentIt != components().end() ;
       ++componentIt) {
    Component &component = *componentIt->second;

    if (component.type() == COMPOUND) {
      Compound &compound = dynamic_cast<Compound &>(component);

      compound.restartAllAtoms();
    }
    else if (component.type() == ATOM) {
      Atom &atom = dynamic_cast<Atom &>(component);

      atom.setHasResume(atom.initialHasResume());
      atom.executeJob().restart();
    }
  }
}

const Compound &Compound::root() const {
  const Compound *ret = this;

  while (ret->hasHolder()) {
    ret = &ret->holder();
  }

  // compound must be the root component
  assert(!ret->hasHolder());

  return *ret;
}

void Compound::configureJobs(Logger &logger, bool isRealTime, bool asap, bool firstEnabled, bool relaxed, bool disabledTimeSafety, bool profiling) {
  // recursive call to sub-components
  for (map<string, Component *>::const_iterator componentIt = components().begin() ;
       componentIt != components().end() ;
       ++componentIt) {
    Component &component = *componentIt->second;

    if (component.type() == COMPOUND) {
      Compound &compound = dynamic_cast<Compound &>(component);
      compound.configureJobs(logger, isRealTime, asap, firstEnabled, relaxed, disabledTimeSafety, profiling);
    }
    else if (component.type() == ATOM) {
      Atom &atom = dynamic_cast<Atom &>(component);

      atom.executeJob().setLogger(logger);
      atom.executeJob().setIsRealTime(isRealTime);
      atom.executeJob().setAsap(asap);
      atom.executeJob().setFirstEnabled(firstEnabled);
      atom.executeJob().setRelaxed(relaxed);
      atom.executeJob().setDisabledTimeSafety(disabledTimeSafety);
      atom.executeJob().setProfiling(profiling);

      atom.executeInternalsJob().setLogger(logger);
      atom.executeInternalsJob().setIsRealTime(isRealTime);
      atom.executeInternalsJob().setAsap(asap);
      atom.executeInternalsJob().setFirstEnabled(firstEnabled);
      atom.executeInternalsJob().setRelaxed(relaxed);
      atom.executeInternalsJob().setProfiling(profiling);

      atom.executeExternalsJob().setLogger(logger);
      atom.executeExternalsJob().setIsRealTime(isRealTime);
      atom.executeExternalsJob().setAsap(asap);
      atom.executeExternalsJob().setFirstEnabled(firstEnabled);
      atom.executeExternalsJob().setRelaxed(relaxed);
      atom.executeExternalsJob().setProfiling(profiling);
      
      atom.timeSafetyValidator().setDisabledTimeSafety(disabledTimeSafety);
    }
  }

  // initialize connectors
  for (map<string, Connector *>::const_iterator connectorIt = connectors().begin() ;
       connectorIt != connectors().end() ;
       ++connectorIt) {
    Connector &connector = *connectorIt->second;
    
    if (!connector.hasExportedPort()) {
      connector.executeJob().setLogger(logger);
      connector.executeJob().setIsRealTime(isRealTime);
      connector.executeJob().setAsap(asap);
      connector.executeJob().setFirstEnabled(firstEnabled);
      connector.executeJob().setRelaxed(relaxed);
      connector.executeJob().setDisabledTimeSafety(disabledTimeSafety);
      connector.executeJob().setProfiling(profiling);

      connector.timeSafetyValidator().setDisabledTimeSafety(disabledTimeSafety);
    }
  }
}


void Compound::computeAllCyclesInPriorities(set<CycleInPriorities *> &cycles) {
  set<CycleInPriorities> allCycles;

  for (vector<Priority *>::const_iterator priorityIt = priorities().begin() ;
       priorityIt != priorities().end() ;
       ++priorityIt) {
    Priority &priority = **priorityIt;

    vector<Priority *> path;

    allCyclesFrom(priority, allCycles, path);
  }

  for (set<CycleInPriorities>::const_iterator cycleIt = allCycles.begin() ;
       cycleIt != allCycles.end() ;
       ++cycleIt) {
    const CycleInPriorities &cycle = *cycleIt;

    // allocate a cycle and update the set of cycles
    CycleInPriorities *newCycle = new CycleInPriorities(cycle.priorities());
    cycles.insert(newCycle);

    // initialize the cycle
    newCycle->initialize();
  }
}

void Compound::computeAllNonExportedConnectors(vector<Connector *> &allNonExportedConnectors) {
  // add non exported connectors of compounds of lower hierarchical levels
  for (map<string, Component *>::const_iterator componentIt = components().begin() ;
       componentIt != components().end() ;
       ++componentIt) {
    Component &component = *componentIt->second;

    if (component.type() == COMPOUND) {
      Compound &compound = dynamic_cast<Compound &>(component);

      allNonExportedConnectors.insert(allNonExportedConnectors.end(),
                                      compound.allNonExportedConnectors().value().begin(),
                                      compound.allNonExportedConnectors().value().end());
    }
  }

  // add non exported connectors of the current hierarchy level
  for (map<string, Connector *>::const_iterator connectorIt = connectors().begin() ;
       connectorIt != connectors().end() ;
       ++connectorIt) {
    Connector &connector = *connectorIt->second;

    if (!connector.hasExportedPort()) {
      allNonExportedConnectors.push_back(&connector);
    }
  }
}

void Compound::computeAllAtoms(vector<Atom *> &allAtoms) {
  for (map<string, Component *>::const_iterator componentIt = components().begin() ;
       componentIt != components().end() ;
       ++componentIt) {
    Component &component = *componentIt->second;

    if (component.type() == COMPOUND) {
      Compound &compound = dynamic_cast<Compound &>(component);

      allAtoms.insert(allAtoms.end(),
                      compound.allAtoms().value().begin(),
                      compound.allAtoms().value().end());
    }
    else if (component.type() == ATOM) {
      Atom &atom = dynamic_cast<Atom &>(component);

      allAtoms.push_back(&atom);
    }
    else {
      assert(false);
    }
  }
}

void Compound::allCyclesFrom(Priority &priority, set<CycleInPriorities> &cycles, vector<Priority *> &path) {
  // search for a cycle
  vector<Priority *>::iterator foundIt = find(path.begin(), path.end(), &priority);

  // cycle found!
  if (foundIt != path.end()) {
    cycles.insert(CycleInPriorities(vector<Priority *>(foundIt, path.end())));
  }
  else {
    // add nextPriority to the current path
    path.push_back(&priority);

    for (vector<Priority *>::const_iterator nextPriorityIt = priority.dominatingPriorities().value().begin() ;
         nextPriorityIt != priority.dominatingPriorities().value().end() ;
         ++nextPriorityIt) {
      Priority &nextPriority = **nextPriorityIt;

      // recursive search of cycles from nextPriority
      allCyclesFrom(nextPriority, cycles, path);
    }

    // put back path to its original value
    path.pop_back();
  }
}
