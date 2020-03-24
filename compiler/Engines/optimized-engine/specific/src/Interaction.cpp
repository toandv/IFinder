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

#include <Interaction.hpp>
#include <Connector.hpp>
#include <Port.hpp>
#include <Priority.hpp>
#include <Compound.hpp>
#include <BipError.hpp>
#include <CycleInPrioritiesError.hpp>
#include <TimeValue.hpp>
#include <TimingConstraint.hpp>

#include <iostream>

// constructors
Interaction::Interaction(const Connector &connector) :
  InteractionItf(connector) {
}

// destructor
Interaction::~Interaction() {
}


/**
 * \brief Compute enabledness of an interaction in its holding connector.
 *
 * Maximality is computed w.r.t. priorities.
 *
 * \return true if interaction 'this' is maximal.
 */
bool Interaction::isEnabled() const {
  // should be a defined interaction
  assert(isDefined());

  bool ret = false;

  // get enabled interactions
  const vector<InteractionValue *> &enabledInteractions = connector().enabledInteractions();

  for (vector<InteractionValue *>::const_iterator interactionIt = enabledInteractions.begin() ;
       interactionIt != enabledInteractions.end() ;
       ++interactionIt) {
    const Interaction &interaction = (*interactionIt)->interaction();

    // if 'this' is in the set of enabled interactions
    if (*this == interaction) {
      ret = true;
      break;
    }
  }
  
  return ret;
}


/**
 * \brief Computes the priorities such that dominates an interaction.
 *
 * A priority P dominates interaction 'this' if 'this' <= P.low(),
 * meaning that P applies to 'this'.
 *
 * \return returns priorities that dominates interaction 'this'.
 */
vector<Priority *> Interaction::dominatingPriorities() const {
  vector<Priority *> ret;

  // must be a defined interaction
  assert(isDefined());

  const Compound &compound = connector().holder();

  for (vector<Priority *>::const_iterator priorityIt = compound.priorities().begin() ;
       priorityIt != compound.priorities().end() ;
       ++priorityIt) {
    Priority &priority = **priorityIt;

    // add priority if it applies
    if (priority.appliesLow(*this)) {
      ret.push_back(&priority);
    }
  }

  return ret;
}


/**
 * \brief Computes the priorities such that dominates an interaction.
 *
 * A priority P dominates interaction 'this' if 'this' <= P.low(),
 * meaning that P applies to 'this'.
 *
 * \return returns priorities that dominates interaction 'this'.
 */
vector<Priority *> Interaction::dominatedPriorities() const {
  vector<Priority *> ret;

  // must be a defined interaction
  assert(isDefined());

  const Compound &compound = connector().holder();

  for (vector<Priority *>::const_iterator priorityIt = compound.priorities().begin() ;
       priorityIt != compound.priorities().end() ;
       ++priorityIt) {
    Priority &priority = **priorityIt;

    // add priority if it applies
    if (priority.appliesHigh(*this)) {
      ret.push_back(&priority);
    }
  }

  return ret;
}


/**
 * \brief Computes the index of a port.
 *
 * The index is computed with respect to the list of ports given by ports().
 *
 * \param port corresponds to the target port.
 *
 * \return the index of port if port is found in ports(), ports().size
 * otherwise.
 */
unsigned int Interaction::index(const Port &port) const {
  unsigned ret = ports().size();

  for (unsigned int i = 0 ;
       i < ports().size() ;
       ++i) {
    const Port &targetPort = *ports()[i];

    if (&targetPort == &port) {
      ret = i;
      break;
    }
  }

  return ret;
}

ostream& operator<<(ostream &o, const Interaction &interaction) {
  o << interaction.connector().name() << ":";

  for (unsigned int i = 0 ; i < interaction.ports().size() ; ++i) {
    o << " " << interaction.ports()[i]->name();
  }

  return o;
}

ostream& operator<<(ostream &o, const Interaction *interaction) {
  return o << *interaction;
}
