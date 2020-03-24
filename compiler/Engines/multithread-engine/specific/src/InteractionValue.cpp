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

#include <InteractionValue.hpp>
#include <PortValue.hpp>
#include <Connector.hpp>
#include <Port.hpp>
#include <AtomExportPort.hpp>
#include <CompoundExportPort.hpp>
#include <Atom.hpp>
#include <Compound.hpp>
#include <BipError.hpp>
#include <Job.hpp>
#include <TimeValue.hpp>
#include <TimingConstraint.hpp>

// constructors
InteractionValue::InteractionValue() :
  InteractionValueItf() {
}

// destructor
InteractionValue::~InteractionValue() {
  /* implement your destructor here */
}


/**
 * \brief Execute the interaction.
 *
 * Executing an interaction corresponds to executing all the involved
 * components and connetors. This might trigger recursive calls to
 * execute() methods through the hierarchy of connectors and components.
 *
 * For compounds and atoms, execute() method is called using the
 * corresponding port value as a parameter.
 *
 * For connectors, execute() method of their exported ports is
 * called using the corresponding port value as a parameter.
 *
 * \return An error if found during the execution of involved atoms or
 * during the update of port values, BipError::NoError otherwise.
 */
BipError &InteractionValue::execute(const TimeValue &time) {
  // executed all connected connectors, atoms or compounds
  for (unsigned int i = 0 ; i < ports().size() ; ++i) {
    Port &port = *ports()[i];
    PortValue &portValue = *portValues()[i];

    // recursive call to execute of port
    BipError &error = port.execute(portValue, time);

    if (error.type() != NO_ERROR) {
      return error;
    }
  }

  return BipError::NoError;
}

void InteractionValue::configureReserver(Resource::DynamicReserver &reserver) const {  
  // executed all connected connectors, atoms or compounds
  for (unsigned int i = 0 ; i < ports().size() ; ++i) {
    Port &port = *ports()[i];
    PortValue &portValue = *portValues()[i];

    port.configureReserver(reserver, portValue);
  }
}


/**
 * \brief Determine if an interaction is defined by a its holding connector.
 *
 * \return true if 'this' is define by its holding connector.
 */
bool InteractionValue::isAllDefined() const {
  bool ret = true;

  if (!interaction().isDefined()) {
    ret = false;
  }

  for (unsigned int i = 0 ; i < ports().size() ; ++i) {
    Port &port = *ports()[i];
  
    // if there is a sub-connector
    if (port.type() == CONNECTOR_EXPORT) {
      PortValue &value = *portValues()[i];
      const InteractionValue &subInteraction = value.interaction();
      
      // check interactions of subconnectors
      if (!subInteraction.isAllDefined()) {
        ret = false;
      }
    }
  }
  
  return ret;
}


/**
 * \brief Compute enabledness of an interaction in its holding connector.
 *
 * Enabledness is computed without applying priorities. It corresponds to
 * the existance of an interaction value in the holding connector that
 * equals interaction value 'this'. 
 *
 * \return true if interaction 'this' is enabled.
 */
bool InteractionValue::isEnabled() const {
  // should be a defined interaction
  assert(isAllDefined());

  bool ret = false;

  // get enabled interactions
  const vector<InteractionValue *> &enabledInteractions = connector().enabledInteractions();

  for (vector<InteractionValue *>::const_iterator interactionIt = enabledInteractions.begin() ;
       interactionIt != enabledInteractions.end() ;
       ++interactionIt) {
    const InteractionValue &interaction = **interactionIt;

    // if 'this' is in the set of enabled interactions
    if (*this == interaction) {
      ret = true;
      break;
    }
  }
  
  return ret;
}


/**
 * \brief Tests inclusion of interaction values.

 * Inclusion of interactions corresponds to inclusion of corresponding
 * sets of ports given by ports(), and it only applies for interactions
 * of the same connector. It also checks inclusion of interaction values
 * of sub-connectors.
 *
 * \param interaction corresponds to the higher interaction value in the
 * inclusion relation, whereas 'this' is the smaller interaction.
 *
 * \return true if 'this' is included in interaction.
 */
bool InteractionValue::operator<=(const InteractionValue &interactionValue) const {
  bool ret = true;

  // check if interaction values are from the same connector
  if (!(interaction() <= interactionValue.interaction())) {
    ret = false;
  }
  else {
    // ...and do the same for interactions of subconnectors
    for (unsigned int i = 0 ; i < ports().size() ; ++i) {
      Port &port = *ports()[i];
  
      if (port.type() == CONNECTOR_EXPORT) {
        unsigned int targetIndex = interactionValue.interaction().index(port);

        PortValue &value = *portValues()[i];
        PortValue &targetValue = *interactionValue.portValues()[targetIndex];

        const InteractionValue &subInteraction = value.interaction();
        const InteractionValue &targetSubInteraction = targetValue.interaction();

        // check interactions of subconnectors
        if (!(subInteraction <= targetSubInteraction)) {
          ret = false;
          break;
       	}
      }
    }
  }

  return ret;
}


bool InteractionValue::operator==(const InteractionValue &interactionValue) const {
  bool ret = true;

  // check if interaction values are from the same connector
  if (!(interaction() == interactionValue.interaction())) {
    ret = false;
  }
  else {
    // ...and do the same for interactions of subconnectors
    for (unsigned int i = 0 ; i < ports().size() ; ++i) {
      Port &port = *ports()[i];
  
      if (port.type() == CONNECTOR_EXPORT) {
        unsigned int targetIndex = interactionValue.interaction().index(port);

        PortValue &value = *portValues()[i];
        PortValue &targetValue = *interactionValue.portValues()[targetIndex];

        const InteractionValue &subInteraction = value.interaction();
        const InteractionValue &targetSubInteraction = targetValue.interaction();

        // check interactions of subconnectors
        if (subInteraction != targetSubInteraction) {
          ret = false;
          break;
        }
      }
    }
  }

  return ret;
}


/**
 * \brief Restrict input timing constraint considering priorities.
 *
 * \param constraint is the target timing constraint to restrict w.r.t.
 * priorities.
 */
void InteractionValue::applyDomination(TimingConstraint &constraint) const {
  // apply maximal progress locally (in the same connector)
  applyLocalDomination(constraint);

  // apply priority rules
  applyPrioritiesDomination(constraint);
}


/**
 * \brief Restrict input timing constraint considering maximal progress
 * priorities.
 *
 * \param constraint is the target timing constraint to restrict w.r.t.
 * maximal progress priorities.
 */
void InteractionValue::applyLocalDomination(TimingConstraint &constraint) const {
  // should be a defined interaction
  assert(isAllDefined());

  const vector<InteractionValue *> &interactions = connector().locallyMaximalInteractions();

  // look for interactions with higher priority
  for (vector<InteractionValue *>::const_iterator targetIt = interactions.begin() ;
       targetIt != interactions.end() ;
       ++targetIt) {
    InteractionValue &target = **targetIt;

    // check if maximal progress applies
    if (*this < target) {
      applyDominationBy(target, constraint);  
    }
  }
}

void InteractionValue::applyPrioritiesDomination(TimingConstraint &constraint) const {
  // must be a defined interaction
  assert(interaction().isDefined());

  const vector<Priority *> &priorities = connector().dominatingPriorities().value();

  for (vector<Priority *>::const_iterator priorityIt = priorities.begin() ;
       priorityIt != priorities.end() ;
       ++priorityIt) {
    Priority &priority = **priorityIt;

    assert(priority.detectCycles().type() == NO_ERROR);

    if (priority.appliesLow(interaction())) {
      priority.applyPrioritiesDomination(*this, constraint);  
    }
  }
}

void InteractionValue::applyDominationBy(const InteractionValue &interaction, TimingConstraint &constraint) const {
  const TimingConstraint &dominatorConstraint = interaction.timingConstraint();
  Interval invariant = connector().invariant();
  invariant &= interaction.connector().invariant();
  
  constraint.applyPriority(dominatorConstraint, invariant, time());
}

/**
 * \brief Restrict input timing constraint considering priorities.
 *
 * \param constraint is the target timing constraint to restrict w.r.t.
 * priorities.
 */
void InteractionValue::inheritDominatedUrgencies(TimingConstraint &constraint) const {
  // apply maximal progress locally (in the same connector)
  inheritLocalDominatedUrgencies(constraint);

  // apply priority rules
  inheritPrioritiesDominatedUrgencies(constraint);
}


/**
 * \brief Restrict input timing constraint considering maximal progress
 * priorities.
 *
 * \param constraint is the target timing constraint to restrict w.r.t.
 * maximal progress priorities.
 */
void InteractionValue::inheritLocalDominatedUrgencies(TimingConstraint &constraint) const {
  // should be a defined interaction
  assert(isAllDefined());

  const vector<InteractionValue *> interactions = connector().locallyMaximalInteractions();

  // look for interactions with higher priority
  for (vector<InteractionValue *>::const_iterator targetIt = interactions.begin() ;
       targetIt != interactions.end() ;
       ++targetIt) {
    InteractionValue &target = **targetIt;

    // check if maximal progress applies
    if (target < *this) {
      inheritDominatedUrgencyOf(target, constraint);
    }
  }
}

void InteractionValue::inheritPrioritiesDominatedUrgencies(TimingConstraint &constraint) const {
  // must be a defined interaction
  assert(interaction().isDefined());

  const vector<Priority *> &priorities = connector().dominatedPriorities().value();

  for (vector<Priority *>::const_iterator priorityIt = priorities.begin() ;
       priorityIt != priorities.end() ;
       ++priorityIt) {
    Priority &priority = **priorityIt;

    assert(priority.detectCycles().type() == NO_ERROR);

    if (priority.appliesHigh(interaction())) {
      priority.inheritPrioritiesDominatedUrgencies(*this, constraint);
    }
  }
}

void InteractionValue::inheritDominatedUrgencyOf(const InteractionValue &interaction, TimingConstraint &constraint) const {
  const TimingConstraint &dominatedConstraint = interaction.timingConstraint();
  Interval invariant = connector().invariant();
  invariant &= interaction.connector().invariant();

  constraint.applyUrgencyInheritance(dominatedConstraint,
    invariant,
    interaction.time());
}

/**
 * \brief Tests strict inclusion of interaction values.
 *
 * Strict inclusion of interactions corresponds to strict inclusion of
 * corresponding sets of ports given by ports(), and it only applies for
 * interactions of the same connector. It also checks inclusion of
 * interaction values of sub-connectors.
 *
 * \param interaction corresponds to the higher interaction value in the
 * strict inclusion relation, whereas 'this' is the smaller interaction.
 *
 * \return true if 'this' is strictly included in interaction.
 */
bool InteractionValue::includedIn(const InteractionValue &interactionValue, bool &eq) const {
  bool inc = true;
  eq = true;

  // check if interaction values are from the same connector
  if (!(interaction() <= interactionValue.interaction())) {
    inc = false;
  }
  else {
    eq = eq && (interaction() == interactionValue.interaction());

    // ...and do the same for interactions of subconnectors
    for (unsigned int i = 0 ; i < ports().size() ; ++i) {
      Port &port = *ports()[i];
  
      if (port.type() == CONNECTOR_EXPORT) {
        unsigned int targetIndex = interactionValue.interaction().index(port);

        PortValue &value = *portValues()[i];
        PortValue &targetValue = *interactionValue.portValues()[targetIndex];

        const InteractionValue &subLeft = value.interaction();
        const InteractionValue &subRight = targetValue.interaction();

        // check interactions of subconnectors
        bool subEq;

        if (!subLeft.includedIn(subRight, subEq)) {
          inc = false;
          break;
        }
        else {
          eq = eq && subEq;
        }
      }
    }
  }

  return inc;
}

void InteractionValue::recomputeTime() {
  mTime = TimeValue::MIN;
  
  // executed all connected connectors, atoms or compounds
  for (unsigned int i = 0 ; i < ports().size() ; ++i) {
    Port &port = *ports()[i];
    PortValue &portValue = *portValues()[i];

    TimeValue portTime = port.time(portValue);
    
    if (portTime > mTime) {
      mTime = portTime;
    }
  }
}

void InteractionValue::recomputeTimingConstraint() {
  // re-initialize to default constraint [-infty,+infty] lazy
  mTimingConstraint = TimingConstraint(LAZY, Interval(TimeValue::MIN, TimeValue::MAX));

  // intersect with the constraints of the ports (values)
  for (vector<PortValue *>::const_iterator portValueIt = portValues().begin() ;
       portValueIt != portValues().end() ;
       ++portValueIt) {
    const PortValue &portValue = **portValueIt;

    if (portValue.hasInterval()) {
      mTimingConstraint &= TimingConstraint(portValue.urgency(), portValue.interval());
    }
    else {
      mTimingConstraint &= TimingConstraint(portValue.urgency(), Interval(TimeValue::MIN, TimeValue::MAX));
    }
  }
}

void InteractionValue::recomputeTimingConstraintAfterPriorities() {
  // reinitialize with the timing constraint
  mTimingConstraintAfterPriorities = timingConstraint();
  
  // inherit urgencies of lower-levels
  inheritDominatedUrgencies(mTimingConstraintAfterPriorities);

  // restrict w.r.t. higher-priority interactions
  applyDomination(mTimingConstraintAfterPriorities);
}

ostream& operator<<(ostream &o, const InteractionValue &value) {
  for (unsigned int i = 0 ; i < value.ports().size() ; ++i) {
    const Port &port = *(value.ports()[i]);

    if (i > 0) o << " ";

    if (port.type() == ATOM_EXPORT) {
      const AtomExportPort &atomPort = dynamic_cast<const AtomExportPort &>(port);
      o << atomPort.holder().name();
    }
    else if (port.type() == COMPOUND_EXPORT) {
      const CompoundExportPort &compoundPort = dynamic_cast<const CompoundExportPort &>(port);
      o << compoundPort.holder().name();
    }
    else if (port.type() == CONNECTOR_EXPORT) {
      const ConnectorExportPort &connectorPort = dynamic_cast<const ConnectorExportPort &>(port);
      o << connectorPort.holder().name();
    }
    else {
      assert(false);
    }
    o << "."; 
    o << value.ports()[i]->name();
    o << "(" << value.portValues()[i] << ")";
  }

  return o;
}

ostream& operator<<(ostream &o, const InteractionValue *value) {
  return o << *value;
}

