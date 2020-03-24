/**
 * Copyright Verimag laboratory.
 * 
 * contributors:
 *  Jacques Combaz (jacques.combaz@univ-grenoble-alpes.fr)
 *  Marc Pouhliès
 *  Anakreontas Mentis
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
#include <CycleInPriorities.hpp>
#include <CycleInPrioritiesError.hpp>

// constructors
Compound::Compound(const string &name) :
  ComponentItf(name, COMPOUND),
  Component(name, COMPOUND),
  CompoundItf(name),
  mNonExportedConnectors(this, &Compound::computeNonExportedConnectors),
  mNonExportedInternalPorts(this, &Compound::computeNonExportedInternalPorts),
  mExternalPorts(this, &Compound::computeExternalPorts),
  mAllNonExportedConnectors(this, &Compound::computeAllNonExportedConnectors),
  mAllNonExportedInternalPorts(this, &Compound::computeAllNonExportedInternalPorts),
  mAllExternalPorts(this, &Compound::computeAllExternalPorts),
  mAllCyclesInPriorities(this, &Compound::computeAllCyclesInPriorities),
  mTime(TimeValue::ZERO) {
  // set dependencies inside the same component
  mAllNonExportedConnectors.dependsOn(mNonExportedConnectors);
  mAllNonExportedInternalPorts.dependsOn(mNonExportedInternalPorts);
  mAllExternalPorts.dependsOn(mExternalPorts);
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
  for (set<CycleInPriorities *>::const_iterator cycleIt = allCyclesInPriorities().begin() ;
       cycleIt != allCyclesInPriorities().end() ;
       ++cycleIt) {
    CycleInPriorities *cycle = *cycleIt;

    delete cycle;
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

    if (port.hasPortValue(portValue)) {
      // the port value must be associated to a single port
      assert(!found);

      BipError &error = port.execute(portValue, time);

      // execute
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

  // update the port values of the component
  return recursiveUpdate();
}


/* \brief Initialize a compound component.
 *
 * 1) Initialize recursively dependencies of resetable objects of 
 * all connectors and priorities.
 * 2) Initialize all atoms and update port values fo exported ports
 * of all compounds.
 *
 * \return An error if found during the initialization fo atoms or
 * during the update of port values, BipError::NoError otherwise.
 */
BipError &Compound::initialize() {
  // initialize all connectors and priorities
  initializeAllConnectorsPriorities();

  // initialize all atoms
  return initializeAllAtoms();
}


void Compound::initializeAllAtomExternalPorts() {
  for (vector<AtomExternalPort *>::const_iterator portIt = externalPorts().begin() ;
       portIt != externalPorts().end() ;
       ++portIt) {
    AtomExternalPort &port = **portIt;

    port.initialize();
  }
}


/* \brief Initialize all atoms.
 *
 *  Initialize recursively all atoms contains in 'this' or in
 * compounds of 'this', and update port values of exported ports of
 * compounds of 'this', as well as port values of exported ports of
 * 'this'.
 *
 * \return An error if found during the initialization of the atoms
 * or during the update of port values of compounds, BipError::NoError
 * otherwise.
 */
BipError &Compound::initializeAllAtoms() {
  for (map<string, Component *>::const_iterator componentIt = components().begin() ;
       componentIt != components().end() ;
       ++componentIt) {
    Component &component = *componentIt->second;

    if (component.type() == COMPOUND) {
      Compound &compound = dynamic_cast<Compound &>(component);

      // recursive call to sub-components
      BipError &error = compound.initializeAllAtoms();

      if (error.type() != NO_ERROR) {
        return error;
      }
    }
    else if (component.type() == ATOM) {
      Atom &atom = dynamic_cast<Atom &>(component);

      // initialize atom
      BipError &error = atom.initialize();

      if (error.type() != NO_ERROR) {
        return error;
      }

      atom.reset();
    }
  }

  // update the port values of the component
  return update();
}


/* \brief Initialize connectors and priorities.
 *
 * Initialize recursively all dependencies of all connectors and priorities
 * of 'this' and of compounds contained in 'this'.
 */
void Compound::initializeAllConnectorsPriorities() {
  // recursive call to sub-components
  for (map<string, Component *>::const_iterator componentIt = components().begin() ;
       componentIt != components().end() ;
       ++componentIt) {
    Component &component = *componentIt->second;

    if (component.type() == COMPOUND) {
      Compound &compound = dynamic_cast<Compound &>(component);
      compound.initializeAllConnectorsPriorities();
    }
  }

  // initialize connectors in an order compatible with hierarchy
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

  // set dependencies of cycles in priorities
  for (vector<Priority* >::const_iterator priorityIt = priorities().begin() ;
       priorityIt != priorities().end() ;
       ++priorityIt) {
    Priority &priority = **priorityIt;

    mAllCyclesInPriorities.dependsOn(priority.dominatedPriorities());
  }

  // set dependencies to other components
  for (map<string, Component *>::const_iterator componentIt = components().begin() ;
       componentIt != components().end() ;
       ++componentIt) {
    Component &component = *componentIt->second;

    if (component.type() == COMPOUND) {
      Compound &compound = dynamic_cast<Compound &>(component);

      mAllNonExportedConnectors.dependsOn(compound.mAllNonExportedConnectors);
      mAllNonExportedInternalPorts.dependsOn(compound.mAllNonExportedInternalPorts);
      mAllExternalPorts.dependsOn(compound.mAllExternalPorts);
    }
  }

  // initialize cycles and non exported connectors / internal / external ports
  mAllCyclesInPriorities.initialize();
  mNonExportedConnectors.initialize();
  mNonExportedInternalPorts.initialize();
  mExternalPorts.initialize();
  mAllNonExportedConnectors.initialize();
  mAllNonExportedInternalPorts.initialize();
  mAllExternalPorts.initialize();
}



/**
 * \brief Update port values of connectors, exported ports, and
 * compounds of upper levels.
 *
 * Update port values of exported ports of connectors, exported ports of compound
 * 'this', and compounds containing 'this'.
 * Requires that port values of component exported ports of 'this' are up-to-date.
 *
 * \return An instance of CycleInPrioritiesError if a cycle has been found
 * in priorities, BipError::NoError otherwise.
 */
BipError &Compound::update() {
  // recompute priorities if needed
  for (vector<Priority *>::const_iterator priorityIt = priorities().begin() ;
       priorityIt != priorities().end() ;
       ++priorityIt) {
    Priority &priority = **priorityIt;

    if (priority.changed()) {
      priority.updatePreviousGuard();
      priority.active().reset();

      BipError &error = priority.detectCycles();
      
      if (error.type() != NO_ERROR) {
        return error;
      }
    }
  }

  // 
  recomputeInvariantResumeWait();

  return BipError::NoError;
}


void Compound::resetAll() {
  // recursive reset of compounds, and reset of atoms
  for (map<string, Component *>::const_iterator componentIt = components().begin() ;
       componentIt != components().end() ;
       ++componentIt) {
    Component &component = *componentIt->second;

    if (component.type() == COMPOUND) {
      Compound &compound = dynamic_cast<Compound &>(component);

      compound.resetAll();

    }
    else if (component.type() == ATOM) {
      Atom &atom = dynamic_cast<Atom &>(component);

      atom.reset();
    }
  }

  // reset all priorities
  for (vector<Priority *>::const_iterator priorityIt = priorities().begin() ;
       priorityIt != priorities().end() ;
       ++priorityIt) {
    Priority &priority = **priorityIt;

    priority.active().reset();
  }
}

/**
 * \brief Update port values of connectors and exported ports.
 *
 * Update recursively port values of exported ports of connectors, exported
 * ports of compound 'this'.
 *
 * \return An instance of CycleInPrioritiesError if a cycle has been found
 * in priorities, BipError::NoError otherwise.
 */
BipError &Compound::recursiveUpdate() {
  // recursive call to sub-components
  for (map<string, Component *>::const_iterator componentIt = components().begin() ;
       componentIt != components().end() ;
       ++componentIt) {
    Component &component = *componentIt->second;

    if (component.type() == COMPOUND) {
      Compound &compound = dynamic_cast<Compound &>(component);

      BipError &error = compound.recursiveUpdate();

      if (error.type() != NO_ERROR) {
        return error;
      }
    }
  }

  return update();
}


/**
 * \brief Resume atomic components of this and lower levels compound components.
 *
 * \return An error if one occurred during the resume, BipError::NoError otherwise.
 */
BipError &Compound::resume(const TimeValue &time) {
  if (hasResume()) {
    for (map<string, Component *>::const_iterator componentIt = components().begin() ;
         componentIt != components().end() ;
         ++componentIt) {
      Component &component = *componentIt->second;

      if (component.type() == COMPOUND) {
        Compound &compound = dynamic_cast<Compound &>(component);
      
        // resume component if needed
        BipError &error = compound.resume(time);

        if (error.type() != NO_ERROR) {
          return error;
        }
      }
      else if (component.type() == ATOM) {
        Atom &atom = dynamic_cast<Atom &>(component);

        // resume component if needed
        if (atom.hasResume()) {
          BipError &error = atom.resume(time);

          if (error.type() != NO_ERROR) {
            return error;
          }

          // reset the interface ports
          atom.reset();

          // reset resume information
          atom.setHasResume(false);
        }
      }
      else {
        assert(false);
      }
    }

    // recompute wait
    recomputeInvariantResumeWait();
  }

  return BipError::NoError;
}

void Compound::setTime(const TimeValue &time) {
  // logical time cannot decrease
  assert(mTime <= time);

  // assign new value to the logical time
  mTime = time;

  // update sub-components recursively
  for (map<string, Component *>::const_iterator componentIt = components().begin() ;
       componentIt != components().end() ;
       ++componentIt) {
    Component &component = *componentIt->second;

    if (component.type() == COMPOUND) {
      Compound &compound = dynamic_cast<Compound &>(component);

      compound.setTime(time);
    }
  }

  // update wait & resume
  mWait &= Interval(mTime, TimeValue::MAX);
  mResume &= Interval(mTime, TimeValue::MAX);
}

void Compound::computeNonExportedConnectors(vector<Connector *> &nonExportedConnectors) {
  // add non exported connectors of the current hierarchy level
  for (map<string, Connector *>::const_iterator connectorIt = connectors().begin() ;
       connectorIt != connectors().end() ;
       ++connectorIt) {
    Connector &connector = *connectorIt->second;

    if (!connector.hasExportedPort()) {
      nonExportedConnectors.push_back(&connector);
    }
  }
}

void Compound::computeNonExportedInternalPorts(vector<AtomInternalPort *> &internalPorts) {
  for (map<string, Component *>::const_iterator componentIt = components().begin() ;
       componentIt != components().end() ;
       ++componentIt) {
    const Component &component = *componentIt->second;

    // add non exported internal ports of atomic components of the current hierarchy level
    if (component.type() == ATOM) {
      const Atom &atom = dynamic_cast<const Atom &>(component);

      for (map<string, AtomInternalPort *>::const_iterator portIt = atom.internalPorts().begin() ;
	       portIt != atom.internalPorts().end() ;
           ++portIt) {
        AtomInternalPort &port = *portIt->second;

        if (!port.isExported()) {
          internalPorts.push_back(&port);
        }
      }
    }
  }
}


void Compound::computeExternalPorts(vector<AtomExternalPort *> &externalPorts) {
  for (map<string, Component *>::const_iterator componentIt = components().begin() ;
       componentIt != components().end() ;
       ++componentIt) {
    const Component &component = *componentIt->second;

    // add non exported internal ports of atomic components of the current hierarchy level
    if (component.type() == ATOM) {
      const Atom &atom = dynamic_cast<const Atom &>(component);

      for (map<string, AtomExternalPort *>::const_iterator portIt = atom.externalPorts().begin() ;
	   portIt != atom.externalPorts().end() ;
           ++portIt) {
        AtomExternalPort &port = *portIt->second;

        externalPorts.push_back(&port);
      }
    }
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
                                      compound.allNonExportedConnectors().begin(),
                                      compound.allNonExportedConnectors().end());
    }
  }

  // add non exported connectors of the current hierarchy level
  allNonExportedConnectors.insert(allNonExportedConnectors.end(),
                                  nonExportedConnectors().begin(),
                                  nonExportedConnectors().end());
}

void Compound::computeAllNonExportedInternalPorts(vector<AtomInternalPort *> &allNonExportedInternalPorts) {
  for (map<string, Component *>::const_iterator componentIt = components().begin() ;
       componentIt != components().end() ;
       ++componentIt) {
    const Component &component = *componentIt->second;

    if (component.type() == COMPOUND) {
      const Compound &compound = dynamic_cast<const Compound &>(component);

      allNonExportedInternalPorts.insert(allNonExportedInternalPorts.end(),
                                         compound.allNonExportedInternalPorts().begin(),
                                         compound.allNonExportedInternalPorts().end());
    }
  }
  
  // add non exported connectors of the current hierarchy level
  allNonExportedInternalPorts.insert(allNonExportedInternalPorts.end(),
                                     nonExportedInternalPorts().begin(),
                                     nonExportedInternalPorts().end());
}

void Compound::computeAllExternalPorts(vector<AtomExternalPort *> &allExternalPorts) {
  for (map<string, Component *>::const_iterator componentIt = components().begin() ;
       componentIt != components().end() ;
       ++componentIt) {
    const Component &component = *componentIt->second;

    if (component.type() == COMPOUND) {
      const Compound &compound = dynamic_cast<const Compound &>(component);

      allExternalPorts.insert(allExternalPorts.end(),
                              compound.allExternalPorts().begin(),
                              compound.allExternalPorts().end());
    }
  }

  // add non exported connectors of the current hierarchy level
  allExternalPorts.insert(allExternalPorts.end(),
                          externalPorts().begin(),
                          externalPorts().end());
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


void Compound::recomputeInvariantResumeWait() {
  // recompute from scratch
  mInvariant = Interval(TimeValue::MIN, TimeValue::MAX);
  mResume = Interval(time(), TimeValue::MAX);
  mWait = Interval(time(), TimeValue::MAX);

  mHasResume = false;

  // take into account low level of hierarchy
  for (map<string, Component *>::const_iterator componentIt = components().begin() ;
       componentIt != components().end() ;
       ++componentIt) {
    const Component &component = *componentIt->second;

    if (component.type() == COMPOUND) {
      const Compound &compound = dynamic_cast<const Compound &>(component);

      mInvariant &= compound.invariant();
      mResume    &= compound.resume();
      mWait      &= compound.wait();

      mHasResume |= compound.hasResume();
    }
    else if (component.type() == ATOM) {      
      const Atom &atom = dynamic_cast<const Atom &>(component);

      if (atom.hasResume()) {
        mResume &= atom.resume();
        mHasResume = true;
      }
      else {
        if (atom.hasInvariant()) {
          mInvariant &= atom.invariant();
        }
      }
    }
    else {
      assert(false);
    }
  }

  // wait & resume should satisfy invariants too
  mWait &= mInvariant;

  // wait & resume should satisfy timing constraint of interactions
  for (vector<Connector *>::const_iterator connectorIt = allNonExportedConnectors().begin() ;
       connectorIt != allNonExportedConnectors().end() ;
       ++connectorIt) {
    const Connector &connector = **connectorIt;
    const vector<InteractionValue *> &interactions = connector.locallyMaximalInteractions();

    for (vector<InteractionValue *>::const_iterator interactionIt = interactions.begin() ;
         interactionIt != interactions.end() ;
         ++interactionIt) {
      const InteractionValue &interaction = **interactionIt;
      const TimingConstraint &constraint = interaction.timingConstraintAfterPriorities();

      if (!interaction.hasResume()) {
         mWait &= constraint.wait(time());
      }
    }
  }

  // wait & resume should satisfy timing constraint of internal ports
  for (vector<AtomInternalPort *>::const_iterator portIt = allNonExportedInternalPorts().begin() ;
       portIt != allNonExportedInternalPorts().end() ;
       ++portIt) {
    const AtomInternalPort &port = **portIt;
          
    if (port.hasPortValue()) {
      const TimingConstraint &constraint = port.timingConstraint();

      if (!port.holder().hasResume()) {
        mWait &= constraint.wait(time());
      }
    }
  }
}

void Compound::recomputeWaitAfterResume() {
  // recompute from scratch
  mWait = Interval(time(), TimeValue::MAX);
  mWait &= mInvariant;

  // wait & resume should satisfy timing constraint of interactions
  for (vector<Connector *>::const_iterator connectorIt = allNonExportedConnectors().begin() ;
       connectorIt != allNonExportedConnectors().end() ;
       ++connectorIt) {
    const Connector &connector = **connectorIt;
    const vector<InteractionValue *> &interactions = connector.locallyMaximalInteractions();

    for (vector<InteractionValue *>::const_iterator interactionIt = interactions.begin() ;
         interactionIt != interactions.end() ;
         ++interactionIt) {
      const InteractionValue &interaction = **interactionIt;
      const TimingConstraint &constraint = interaction.timingConstraintAfterPriorities();

      mWait &= constraint.wait(time());
    }
  }

  // wait & resume should satisfy timing constraint of internal ports
  for (vector<AtomInternalPort *>::const_iterator portIt = allNonExportedInternalPorts().begin() ;
       portIt != allNonExportedInternalPorts().end() ;
       ++portIt) {
    const AtomInternalPort &port = **portIt;
          
    if (port.hasPortValue()) {
      const TimingConstraint &constraint = port.timingConstraint();

      mWait &= constraint.wait(time());
    }
  }  
}


