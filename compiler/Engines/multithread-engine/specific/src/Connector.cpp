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

#include <Connector.hpp>
#include <Interaction.hpp>
#include <InteractionValue.hpp>
#include <Compound.hpp>
#include <Atom.hpp>
#include <AtomExportPort.hpp>
#include <ConnectorExportPort.hpp>
#include <CompoundExportPort.hpp>

// constructors
Connector::Connector(const string &name, const bool &asyncResume) :
  ConnectorItf(name, asyncResume),
  mAllEnabledPorts(this, &Connector::recomputeAllEnabledPorts, NULL),
  copyOfmAllEnabledPorts(NULL),
  mInteractions(this, &Connector::computeInteractions),
  mEnabledInteractions(this, &Connector::recomputeEnabledInteractions),
  mMaximalInteractions(this, &Connector::recomputeMaximalInteractions),
  mLocallyMaximalInteractions(this, &Connector::recomputeLocallyMaximalInteractions),
  mTime(this, &Connector::recomputeTime),
  mInvariant(this, &Connector::recomputeInvariant),
  mDominatingPriorities(this, &Connector::computeDominatingPriorities),
  mDominatedPriorities(this, &Connector::computeDominatedPriorities),
  mIsTopLevel(this, &Connector::computeIsTopLevel),
  mIsRendezVous(this, &Connector::computeIsRendezVous),
  mAreAllRendezVous(this, &Connector::computeAreAllRendezVous),
  mAllAtoms(this, &Connector::computeAllAtoms),
  mMayHaveUrgency(this, &Connector::computeMayHaveUrgency),
  mAllLowMayHaveUrgency(this, &Connector::computeAllLowMayHaveUrgency),
  mExecuteJob(*this),
  mTimeSafetyValidator(*this) {
  // set all the dependencies between resetable objects
  mEnabledInteractions.dependsOn(mAllEnabledPorts);
  mLocallyMaximalInteractions.dependsOn(mAllEnabledPorts);
  mMaximalInteractions.dependsOn(mLocallyMaximalInteractions);
  
  // set all the dependencies between initializable objects
  mDominatingPriorities.dependsOn(mIsTopLevel);
  mDominatedPriorities.dependsOn(mIsTopLevel);

  mMayHaveUrgency.dependsOn(mAllAtoms);
  mAllLowMayHaveUrgency.dependsOn(mMayHaveUrgency);
  mAllLowMayHaveUrgency.dependsOn(mDominatedPriorities);
}

// destructor
Connector::~Connector() {
  //if (mAllEnabledPorts != NULL) {
  //  delete mAllEnabledPorts;
  //}
}


/**
 * \brief Determine if a connector is top-level in its holding compound.
 *
 * We consider that a connector is top-level if it has no exported port, or
 * if its exported port is only connected to an exported port of a compound.
 *
 * \return true if the connector 'this' is top-level.
 */
void Connector::computeIsTopLevel(bool &isTopLevel) {
  isTopLevel = true;

  const Compound &compound = holder();

  // in case of exported port, check whether it is connected to a connector or not
  if (hasExportedPort()) {
    // check all connectors of the same hierarchical level
    for (map<string, Connector *>::const_iterator connectorIt = compound.connectors().begin() ;
         connectorIt != compound.connectors().end() ;
         ++connectorIt) {
      const Connector &connector = *connectorIt->second;

      // check all ports of the connector
      for (vector<QuotedPortReference *>::const_iterator portIt = connector.ports().begin() ;
           portIt != connector.ports().end() ;
           ++portIt) {
        const QuotedPortReference &portRef = **portIt;

        // the exported port is connected to a connector => non top-level!
        if (&portRef.port() == &exportedPort()) {
          isTopLevel = false;
          break;
        }
      }
    }
  }
}

void Connector::computeIsRendezVous(bool &isRendezVous) {
  isRendezVous = false;

  vector<Port *> allPorts;

  // compute the maximal interaction
  for (vector<QuotedPortReference *>::const_iterator portRefIt = ports().begin() ;
       portRefIt != ports().end() ;
       ++portRefIt) {
    const QuotedPortReference &portRef = **portRefIt;
    Port &port = portRef.port();

    allPorts.push_back(&port);
  }
  
  Interaction &maximal = createInteraction(allPorts);

  // the maximal interaction
  assert(maximal.isDefined());

  if (!maximal.hasSubDefined()) {
    isRendezVous = true;
  }

  // release interaction
  releaseInteraction(maximal);
}

void Connector::computeAreAllRendezVous(bool &areAllRendezVous) {
  areAllRendezVous = false;
  
  if (isRendezVous()) {
    areAllRendezVous = true;
  
    // compute the maximal interaction
    for (vector<QuotedPortReference *>::const_iterator portRefIt = ports().begin() ;
         portRefIt != ports().end() ;
         ++portRefIt) {
      const QuotedPortReference &portRef = **portRefIt;
      const Port &port = portRef.port();

      if (!subConnectorsAreAllRendezVous(port)) {
        areAllRendezVous = false;
        break;
      }
    }
  }
}

bool Connector::subConnectorsAreAllRendezVous(const Port &port) const {
  bool ret = true;
  
  if (port.type() == ATOM_EXPORT) {
    // nothing to check in this case
  }
  else if (port.type() == COMPOUND_EXPORT) {
    const CompoundExportPort &compoundPort = dynamic_cast<const CompoundExportPort &>(port);

    for (vector<Port *>::const_iterator forwardPortIt = compoundPort.forwardPorts().begin() ;
         forwardPortIt != compoundPort.forwardPorts().end() ;
         ++forwardPortIt) {
      const Port &forwardPort = **forwardPortIt;

      if (!subConnectorsAreAllRendezVous(forwardPort)) {
        ret = false;
        break;
      }
    }        
  }
  else if (port.type() == CONNECTOR_EXPORT) {
    const ConnectorExportPort &connectorPort = dynamic_cast<const ConnectorExportPort &>(port);
    const Connector &connector = connectorPort.holder();

    if (!connector.areAllRendezVous()) {
      ret = false;
    }
  }
  else {
    assert(false);
  }

  return ret;
}

/**
 * \brief Compute the set of maximal interaction values enabled at the current state.
 *
 * Requires up-to-date port values for all connected ports. Maximality is computed
 * based on maximal progress priority amongst the interactions enabled in the same
 * connector, i.e. in connector 'this'.
 *
 * Interactions are created and need to be deleted using release().
 *
 * \return The set of interaction values that are maximal and enabled by connector 'this'.
 */
void Connector::recomputeMaximalInteractions(vector<InteractionValue *> &maximalInteractions) const {
  // clear maximal interactions
  maximalInteractions.clear();

  // check dependencies
  assert(mMaximalInteractions.isDependentOf(mLocallyMaximalInteractions));

  // keeps only maximal interactions
  const vector<InteractionValue *> &locMaxInteractions = locallyMaximalInteractions();

  for (vector<InteractionValue *>::const_iterator interactionIt = locMaxInteractions.begin() ;
       interactionIt != locMaxInteractions.end() ;
       ++interactionIt) {
    InteractionValue &interaction = **interactionIt;

    // maximal interactions has been reset => timing constrain after prio is outdated
    interaction.recomputeTimingConstraintAfterPriorities();

    if (!interaction.timingConstraintAfterPriorities().empty()) {
      maximalInteractions.push_back(&interaction);
    }
  }
}

/**
 * \brief Compute the set of locally maximal interaction values enabled at the current state.
 *
 * Requires up-to-date port values for all connected ports. Maximality is computed
 * based on maximal progress priority amongst the interactions enabled in the same
 * connector, i.e. in connector 'this'.
 *
 * Interactions are created and need to be deleted using release().
 *
 * \return The set of interaction values that are maximal and enabled by connector 'this'.
 */
void Connector::recomputeLocallyMaximalInteractions(vector<InteractionValue *> &locallyMaximalInteractions) const {
  // release allocated interactions
  release(locallyMaximalInteractions);

  // clear maximal interactions
  locallyMaximalInteractions.clear();

  // check dependencies
  assert(mLocallyMaximalInteractions.isDependentOf(mAllEnabledPorts));

  // force uptate of mAllEnabledPorts and copyOfmAllEnabledPorts (if needed)
  mAllEnabledPorts.recompute();

  // compute interactions maximal w.r.t. maximal progress
  enumerateLocallyMaximalInteractionValues(locallyMaximalInteractions, *copyOfmAllEnabledPorts, 0);
}

/**
 * \brief Compute the set of interaction values enabled at the current state.
 *
 * Requires up-to-date port values for all connected ports.
 * Interactions are created and need to be deleted using release().
 *
 * \return The set of interaction values enabled by connector 'this'.
 */
void Connector::recomputeEnabledInteractions(vector<InteractionValue *> &enabledInteractions) const {
  // release previously created interactions
  release(enabledInteractions);

  // clear enabled and maximal interactions
  enabledInteractions.clear();
    
  vector<PortValue *> partialValues;
  partialValues.reserve(ports().size());

  // check dependencies
  assert(mEnabledInteractions.isDependentOf(mAllEnabledPorts));

  for (vector<Interaction *>::const_iterator interactionIt = interactions().begin() ;
       interactionIt != interactions().end() ;
       ++interactionIt) {
    const Interaction &interaction = **interactionIt;

    if (interaction <= allEnabledInteraction()) {
      enumerateInteractionValues(enabledInteractions, interaction, partialValues, 0, false);
    }
  }
}

void Connector::recomputeTime(TimeValue &time) const {
  time = TimeValue::MIN;

  // compute the latest execution time of components
  for (vector<const Atom *>::const_iterator atomIt = allAtoms().value().begin() ;
       atomIt != allAtoms().value().end() ;
       ++atomIt) {
    const Atom &atom = **atomIt;

    TimeValue atomTime = atom.time();

    if (atomTime > time) {
      time = atomTime;
    }
  }

  assert(time != TimeValue::MIN);
}

void Connector::recomputeInvariant(Interval &invariant) const {  
  invariant = Interval(TimeValue::MIN, TimeValue::MAX);

  // take into account invariants of components
  for (vector<const Atom *>::const_iterator atomIt = allAtoms().value().begin() ;
       atomIt != allAtoms().value().end() ;
       ++atomIt) {
    const Atom &atom = **atomIt;

    if (!atom.hasResume()) {
      if (atom.hasInvariant()) {
        invariant &= atom.invariant();
      }
    }
  }
}

void Connector::computeInteractions(vector<Interaction *> &interactions) {
  vector<QuotedPortReference *> emptyInteraction;
  enumerateInteractions(interactions, emptyInteraction, 0);
}


/**
 * \brief Release all interactions of a list of interactions.
 *
 * Calls relaseInteraction for all interactions.
 *
 * \param interactions is a list of interactions to release.
 */
void Connector::release() const {
  //release(mInteractions);
  //release(mEnabledInteractions);
  //release(mLocallyMaximalInteractions);
}

/**
 * \brief Release all interactions of a list of interactions.
 *
 * Calls relaseInteraction for all interactions.
 *
 * \param interactions is a list of interactions to release.
 */
void Connector::release(const vector<Interaction *> &interactions) const {
  for (vector<Interaction *>::const_iterator interactionIt = interactions.begin() ;
       interactionIt != interactions.end() ;
       ++interactionIt) {
    Interaction &interaction = **interactionIt;

    releaseInteraction(interaction);
  }
}


/**
 * \brief Release all interaction values of a list of interactions values.
 *
 * Calls relaseInteractionValue for all interaction values.
 *
 * \param interactions is a list of interaction values to release.
 */
void Connector::release(const vector<InteractionValue *> &interactions) const {
  for (vector<InteractionValue *>::const_iterator interactionIt = interactions.begin() ;
       interactionIt != interactions.end() ;
       ++interactionIt) {
    InteractionValue &interaction = **interactionIt;

    releaseInteractionValue(interaction);
  }
}


void Connector::computeDominatingPriorities(vector<Priority *> &dominatingPriorities) {
  const Compound &compound = holder();

  for (vector<Priority *>::const_iterator priorityIt = compound.priorities().begin() ;
       priorityIt != compound.priorities().end() ;
       ++priorityIt) {
    Priority &priority = **priorityIt;

    if (priority.hasLow()) {
      // should be a defined interaction of a top-level connector
      // assert(priority.low().isDefined());
      // assert(priority.low().connector().isTopLevel());

      if (&priority.low().connector() == this) {
        dominatingPriorities.push_back(&priority);
      }
    }
    else {
      assert(priority.hasHigh());
      if (&priority.high().connector() != this) {
        dominatingPriorities.push_back(&priority);
      }
    }
  }
}


void Connector::computeDominatedPriorities(vector<Priority *> &dominatedPriorities) {
  const Compound &compound = holder();

  for (vector<Priority *>::const_iterator priorityIt = compound.priorities().begin() ;
       priorityIt != compound.priorities().end() ;
       ++priorityIt) {
    Priority &priority = **priorityIt;

    if (priority.hasHigh()) {
      // should be a defined interaction of a top-level connector
      // assert(priority.high().isDefined() || !priority.high().hasPorts());
      // assert(priority.high().connector().isTopLevel());

      if (&priority.high().connector() == this) {
        dominatedPriorities.push_back(&priority);
      }
    }
    else {
      assert(priority.hasLow());

      if (&priority.low().connector() != this) {
        dominatedPriorities.push_back(&priority);
      }
    }
  }
}

void Connector::computeAllAtoms(vector<const Atom *> &allAtoms) {
  for (vector<QuotedPortReference *>::const_iterator quotedPortIt = ports().begin() ;
       quotedPortIt != ports().end() ;
       ++quotedPortIt) {
    const QuotedPortReference &quotedPort = **quotedPortIt;
    const Port &port = quotedPort.port();

    computeAllAtomsOf(port, allAtoms);
  }
}

void Connector::computeAllAtomsOf(const Port &port, vector<const Atom *> &ret) const {      
  if (port.type() == ATOM_EXPORT) {
    const AtomExportPort &atomPort = dynamic_cast<const AtomExportPort &>(port);
    const Atom &atom = atomPort.holder();
    
    if (find(ret.begin(),
             ret.end(),
             &atom)
        == ret.end()) {
      ret.push_back(&atom);
    }
  }
  else if (port.type() == COMPOUND_EXPORT) {
    const CompoundExportPort &compoundPort = dynamic_cast<const CompoundExportPort &>(port);

    for (vector<Port *>::const_iterator forwardPortIt = compoundPort.forwardPorts().begin() ;
         forwardPortIt != compoundPort.forwardPorts().end() ;
         ++forwardPortIt) {
      const Port &forwardPort = **forwardPortIt;

      computeAllAtomsOf(forwardPort, ret);
    }
  }
  else if (port.type() == CONNECTOR_EXPORT) {
    const ConnectorExportPort &connectorPort = dynamic_cast<const ConnectorExportPort &>(port);
    const Connector &connector = connectorPort.holder();

    for (vector<QuotedPortReference *>::const_iterator quotedPortIt = connector.ports().begin() ;
         quotedPortIt != connector.ports().end() ;
         ++quotedPortIt) {
      const QuotedPortReference &quotedPort = **quotedPortIt;
      const Port &port = quotedPort.port();

      computeAllAtomsOf(port, ret);
    }
  }
  else {
    assert(false);
  }
}

void Connector::computeMayHaveUrgency(bool &b) {
  b = mayHaveUrgency(*this);
}

bool Connector::mayHaveUrgency(const Connector &connector) const {
  bool ret = false;
  
  for (vector<QuotedPortReference *>::const_iterator quotedPortIt = connector.ports().begin() ;
       quotedPortIt != connector.ports().end() ;
       ++quotedPortIt) {
    const QuotedPortReference &quotedPort = **quotedPortIt;
    const Port &port = quotedPort.port();

    ret |= mayHaveUrgency(port);
    
    if (ret) break;
  }

  return ret;
}

bool Connector::mayHaveUrgency(const Port &port) const {
  bool ret = false;
  
  if (port.type() == ATOM_EXPORT) {
    const AtomExportPort &atomPort = dynamic_cast<const AtomExportPort &>(port);
    
    ret = atomPort.hasUrgency();
  }
  else if (port.type() == COMPOUND_EXPORT) {
    const CompoundExportPort &compoundPort = dynamic_cast<const CompoundExportPort &>(port);

    for (vector<Port *>::const_iterator forwardPortIt = compoundPort.forwardPorts().begin() ;
         forwardPortIt != compoundPort.forwardPorts().end() ;
         ++forwardPortIt) {
      const Port &forwardPort = **forwardPortIt;

      ret |= mayHaveUrgency(forwardPort);

      if (ret) break;
    }
  }
  else if (port.type() == CONNECTOR_EXPORT) {
    const ConnectorExportPort &connectorPort = dynamic_cast<const ConnectorExportPort &>(port);
    const Connector &connector = connectorPort.holder();

    ret = mayHaveUrgency(connector);
  }
  else {
    assert(false);
  }

  return ret;
}

void Connector::computeAllLowMayHaveUrgency(bool &allLowMayHaveUrgency) {
  allLowMayHaveUrgency = mayHaveUrgency();

  if (!allLowMayHaveUrgency) {
    // set dependencies of mAllLowMayHaveUrgency
    for (vector<Priority *>::const_iterator priorityIt = dominatedPriorities().value().begin() ;
         priorityIt != dominatedPriorities().value().end() ;
         ++priorityIt) {
      const Priority &priority = **priorityIt;
    
      mAllLowMayHaveUrgency.dependsOn(priority.allLowConnectors());

      for (vector<const Connector *>::const_iterator connectorIt = priority.allLowConnectors().value().begin() ;
           connectorIt != priority.allLowConnectors().value().end() ;
           ++connectorIt) {
        const Connector &connector = **connectorIt;

        if (connector.mayHaveUrgency()) {
          allLowMayHaveUrgency = true;
          
          return;
        }
      }
    }
  }
}

void Connector::addPort(QuotedPortReference &quotedPort) {
  // add the port to the connector ports
  mPorts.push_back(&quotedPort);
  quotedPort.setHolder(*this);
    
  // manage dependencies
  Port &port = quotedPort.port();

  // add dependencies to port values
  mAllEnabledPorts.dependsOn(port.reset());
}

void Connector::setExportedPort(ConnectorExportPort &exportedPort) {
  mExportedPort = &exportedPort;
  exportedPort.setHolder(*this);

  // manage dependencies
  exportedPort.reset().dependsOn(mEnabledInteractions);
  exportedPort.maximalPortValues().dependsOn(mMaximalInteractions);
}


/**
 * \brief Compute the set of interactions defined by the connector recursively.
 */
void Connector::enumerateInteractions(vector<Interaction *> &allInteractions, vector<QuotedPortReference *> &partialInteraction, unsigned int nextPortIndex) const {
  if (nextPortIndex < ports().size()) {
    QuotedPortReference *port = ports()[nextPortIndex];

    // all interactions that do     involve port
    partialInteraction.push_back(port);
    enumerateInteractions(allInteractions, partialInteraction, nextPortIndex + 1);

    // remove
    partialInteraction.pop_back();

    // all interactions that do not involve port
    enumerateInteractions(allInteractions, partialInteraction, nextPortIndex + 1);
  }
  else {
    vector<Port *> portsOfInteraction;

    bool maximal = partialInteraction.size() == ports().size();
    bool trigger = false;

    for (vector<QuotedPortReference *>::const_iterator portIt = partialInteraction.begin() ;
         portIt != partialInteraction.end() ;
         ++portIt) {
      QuotedPortReference *portRef = *portIt;

      if (portRef->trigger()) {
        trigger = true;
      }

      portsOfInteraction.push_back(&portRef->port());
    }

    // an interaction is maximal if a trigger is involved or if it is maximal
    if (trigger || maximal) {
      Interaction &interaction = createInteraction(portsOfInteraction);
      allInteractions.push_back(&interaction);
    }
  }
}


/**
 * \brief Compute the set of interaction values for the current state recursively.
 */
bool Connector::enumerateInteractionValues(vector<InteractionValue *> &allInteractions, const Interaction &interaction, vector<PortValue *> &partialValues, unsigned int nextPortIndex, bool keepLocallyMaximalOnly) const {
  bool ret = true;

  if (nextPortIndex < interaction.ports().size()) {
    const Port *port = interaction.ports()[nextPortIndex];

    if (port->hasPortValues()) {
      for (vector<PortValue *>::const_iterator valueIt = port->portValues().begin() ;
           valueIt != port->portValues().end() ;
           ++valueIt) {
        PortValue *value = *valueIt;

        // enumerate using value
        partialValues.push_back(value);
        bool hasTimingConstraint = enumerateInteractionValues(allInteractions, interaction, partialValues, nextPortIndex + 1, keepLocallyMaximalOnly);
        ret = ret && hasTimingConstraint;

        // remove value for next iteration (avoids using multiple vector<PortValue *>)
        // if (valueIt + 1 != port->portValues().end()) {
          partialValues.pop_back();
        // }
      }
    }
  }
  else {
    InteractionValue &interactionValue = createInteractionValue(interaction, partialValues);

    // force recomputation of timing constraints in case of recycling
    interactionValue.recomputeTime();
    interactionValue.recomputeTimingConstraint();

    // check the boolean guard of the built interaction value, if true, keep it
    bool keepIt = false;

    if (guard(interactionValue)) {
      const Interval &interval = interactionValue.timingConstraint().interval();

      // check the timing constraint
      if (!interval.empty()) {
        keepIt = true;

        // inform that there is no need to look for sub-interactions
        if (interval.left() == TimeValue::MIN &&
            interval.right() == TimeValue::MAX) {
          ret = false;
        }

        if (keepLocallyMaximalOnly) {
          vector<InteractionValue *>::iterator targetIt = allInteractions.begin();

          while (targetIt != allInteractions.end()) {
            InteractionValue &target = **targetIt;
            const Interval &targetInterval = target.timingConstraint().interval();

            if (interactionValue < target &&
                interval.isIncludedIn(targetInterval)) {
              keepIt = false;
              break;
            }
            else if (interactionValue > target &&
                     targetInterval.isIncludedIn(interval)) {
              // release the previously created interaction
              releaseInteractionValue(target);

              // remove it from the set of maximal interactions
              targetIt = allInteractions.erase(targetIt);
            }
            else {
              ++targetIt;
            }
          }
        }
      }
    }

    if (keepIt) {
      allInteractions.push_back(&interactionValue);
    }
    else {
      // release the created interaction
      releaseInteractionValue(interactionValue);
    }
  }

  return ret;
}


void Connector::enumerateLocallyMaximalInteractionValues(vector<InteractionValue *> &allInteractions, Interaction &interaction, unsigned int mandatoryIndex) const {
  vector<PortValue *> partialValues;
  partialValues.reserve(ports().size());

  unsigned int oldSize = allInteractions.size();

  // if the interaction is defined, compute all enabled interaction values if exist
  if (interaction.isDefined()) {
    enumerateInteractionValues(allInteractions, interaction, partialValues, 0, true);
  }

  // if no interaction is found, check sub interactions
  if (allInteractions.size() == oldSize && interaction.hasSubDefined()) {
    unsigned int subMandatory =  mandatoryIndex;

    // try interactions immediatly dominated by maximal progress
    for (vector<Port *>::const_iterator portIt = interaction.ports().begin() + mandatoryIndex;
         portIt != interaction.ports().end() ;
         ++portIt) {
      Port &port = **portIt;

      // try sub-interaction without port
      interaction.removePort(port);

      // enumerate without port
      enumerateLocallyMaximalInteractionValues(allInteractions, interaction, subMandatory);
      // put back port in the subInteraction
      interaction.addPort(port);

      ++subMandatory;
    }
  }
}

void Connector::recomputeAllEnabledPorts(Interaction *&allEnabledPorts) const {
  if (allEnabledPorts == NULL) {
    allEnabledPorts = &createInteraction();
    copyOfmAllEnabledPorts = &createInteraction();
  }
  else {
    allEnabledPorts->recycle();
    copyOfmAllEnabledPorts->recycle();
  }

  for (vector<QuotedPortReference *>::const_iterator portRefIt = ports().begin() ;
       portRefIt != ports().end() ;
       ++portRefIt) {
    const QuotedPortReference &portRef = **portRefIt;
    Port &port = portRef.port();

    // check dependencies
    assert(mAllEnabledPorts.isDependentOf(port.reset()));

    bool portIsEnabled = false;

    if (port.type() == ATOM_EXPORT) {
      AtomExportPort &atomPort = dynamic_cast<AtomExportPort &>(port);

      if (!atomPort.holder().hasResume() &&
          atomPort.hasPortValues()) {
        portIsEnabled = true;
      }
    }
    else if (port.hasPortValues()) {
      portIsEnabled = true;
    }

    // if port is enabled, add it to the interaction
    if (portIsEnabled) {
      allEnabledPorts->addPort(port);
      copyOfmAllEnabledPorts->addPort(port);
    }
    else if (isRendezVous()) {
      // optimization for rendez-vous connectors
      break;
    }
  }
}

string Connector::fullName() const {
  return holder().fullName() + "." + name();
}

void Connector::initialize() {
  mDominatingPriorities.initialize();
  mDominatedPriorities.initialize();
  mIsTopLevel.initialize();
  mIsRendezVous.initialize();
  
  // initialize allAtoms
  mAllAtoms.initialize();
  mMayHaveUrgency.initialize();
  
  // set dependencies of mInvariant and mTime to reset of atoms
  for (vector<const Atom *>::const_iterator atomIt = allAtoms().value().begin() ;
       atomIt != allAtoms().value().end() ;
       ++atomIt) {
    const Atom &atom = **atomIt;

    mInvariant.dependsOn(atom.invariantReset());    
    mTime.dependsOn(atom.timeReset());
  }

  // set dependencies for mAreAllRendezVous
  for (vector<QuotedPortReference *>::const_iterator portRefIt = ports().begin() ;
       portRefIt != ports().end() ;
       ++portRefIt) {
    const QuotedPortReference &portRef = **portRefIt;
    const Port &port = portRef.port();

    if (port.type() == CONNECTOR_EXPORT) {
      const ConnectorExportPort &connectorPort = dynamic_cast<const ConnectorExportPort &>(port);
      const Connector &connector = connectorPort.holder();

      mAreAllRendezVous.dependsOn(connector.mAreAllRendezVous);
    }
  }

  // set dependencies of mAllLowMayHaveUrgency
  for (vector<Priority *>::const_iterator priorityIt = dominatedPriorities().value().begin() ;
       priorityIt != dominatedPriorities().value().end() ;
       ++priorityIt) {
    const Priority &priority = **priorityIt;
    
    mAllLowMayHaveUrgency.dependsOn(priority.allLowConnectors());
  }
}

void Connector::initialize2() {
  mAreAllRendezVous.initialize();
  mAllLowMayHaveUrgency.initialize();
}

void Connector::initializeResetable() {
  if (hasExportedPort()) {
    exportedPort().initializeResetable();
  }
  
  mAllEnabledPorts.initialize();
  mEnabledInteractions.initialize();
  mMaximalInteractions.initialize();
  mLocallyMaximalInteractions.initialize();
  mTime.initialize();
  mInvariant.initialize();

  mExecuteJob.initializeResetable();
}
