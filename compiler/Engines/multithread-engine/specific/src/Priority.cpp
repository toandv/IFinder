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

#include <Priority.hpp>
#include <Interaction.hpp>
#include <Connector.hpp>
#include <Compound.hpp>
#include <BipError.hpp>
#include <CycleInPrioritiesError.hpp>
#include <Port.hpp>
#include <AtomExportPort.hpp>
#include <Atom.hpp>
#include <AtomExportData.hpp>
#include <CompoundExportData.hpp>
#include <CycleInPriorities.hpp>
#include <InteractionValue.hpp>
#include <TimingConstraint.hpp>

// constructors
Priority::Priority(Interaction *lowInteraction, Interaction *highInteraction) :
  PriorityItf(lowInteraction, highInteraction),
  mAtomData(this, &Priority::computeAtomData),
  mLowConnectors(this, &Priority::computeLowConnectors),
  mHighConnectors(this, &Priority::computeHighConnectors),
  mAllLowConnectors(this, &Priority::computeAllLowConnectors),
  mAllHighConnectors(this, &Priority::computeAllHighConnectors),  
  mDominatingPriorities(this, &Priority::computeDominatingPriorities),
  mDominatedPriorities(this, &Priority::computeDominatedPriorities),
  mAllDominatingPriorities(this, &Priority::computeAllDominatingPriorities),
  mAllDominatedPriorities(this, &Priority::computeAllDominatedPriorities),
  mActive(this, &Priority::recomputeActive) {
  // /!\ necessary for when guards do not depend on any data
  // mActive.recompute();
}

// destructor
Priority::~Priority() {
  /* implement your destructor here */
}


/**
 * \brief 
 *
 * \return 
 */
bool Priority::appliesLow(const Interaction &interaction) const {
  bool ret = false;

  // should be a defined interaction of a top-level connector
  assert(interaction.isDefined() || !interaction.hasPorts());

  if (interaction.connector().isTopLevel()) {
    if (hasLow()) {
      // should be a defined or empty interaction of a top-level connector
      assert(low().isDefined());
      assert(low().connector().isTopLevel());

      if (interaction <= low()) {
        ret = true;
      }
    }
    else {
      // priority rules * < * are not allowed
      assert(hasHigh());

      // should be a defined interaction of a top-level connector
      assert(high().isDefined() || !high().hasPorts());
      assert(high().connector().isTopLevel());

      if (&interaction.connector() != &high().connector()) {
        return true;
      }
    }
  }

  return ret;
}


/**
 * \brief 
 *
 * \return 
 */
bool Priority::appliesHigh(const Interaction &interaction) const {
  bool ret = false;

  // should be a defined interaction of a top-level connector
  assert(interaction.isDefined() || !interaction.hasPorts());

  if (interaction.connector().isTopLevel()) {
    if (hasHigh()) {
      // should be a defined interaction of a top-level connector
      assert(high().isDefined() || !high().hasPorts());
      assert(high().connector().isTopLevel());

      if (high() <= interaction) {
        ret = true;
      }
    }
    else {
      // priority rules * < * are not allowed
      assert(hasLow());

      // should be a defined or empty interaction of a top-level connector
      assert(low().isDefined());
      assert(low().connector().isTopLevel());
    
      if (&low().connector() != &interaction.connector()) {
        return true;
      }
    }
  }

  return ret;
}


/**
 * \brief Detects if there a cycle in the priorities from a priority.
 *
 * Priorities are defined as the transitive closure of the union of maximal
 * progress (i.e. inclusion of interactions values implemented by operator<)
 * and user-defined priority rules (i.e. instances of Priority given by
 * priorities()).
 *
 * \return An instance of CycleInPrioritiesError if a cycle has been found
 * in priorities, BipError::NoError otherwise.
 */
BipError &Priority::detectCycles() const {
  for (vector<const CycleInPriorities *>::const_iterator cycleIt = cycles().begin() ;
       cycleIt != cycles().end() ;
       ++cycleIt) {
    const CycleInPriorities &cycle = **cycleIt;

    if (cycle.allGuardsTrue()) {
      CycleInPrioritiesError &error = *(new CycleInPrioritiesError());

      vector<const Interaction *> interactions = cycle.interactions();

      for (vector<const Interaction *>::const_iterator interactionIt = interactions.begin() ;
           interactionIt != interactions.end() ;
           ++interactionIt) {
        const Interaction &interaction = **interactionIt;

        error.addCycle(interaction);
      }
      

      return error;
    }
  }

  return BipError::NoError;
}

void Priority::computeAtomData(vector<AtomExportData *> &atomData) {
  // retreive all atom atom that the guard depends on
  for (vector<Data *>::const_iterator dataIt = data().begin() ;
       dataIt != data().end() ;
       ++dataIt) {
    Data *data = *dataIt;

    // go through the hierarchy to reach the corresponding atom
    while (data->type() == COMPOUND_EXPORT) {
      CompoundExportData &compoundData = dynamic_cast<CompoundExportData &>(*data);
      data = &compoundData.forwardData();
    }

    // should be a data exported by an atom
    assert(data->type() == ATOM_EXPORT);

    AtomExportData &atomExportData = dynamic_cast<AtomExportData &>(*data);

    // add a dependency to atomData
    atomData.push_back(&atomExportData);
  }
}

void Priority::computeLowConnectors(vector<const Connector *> &connectors) {
  if (hasLow()) {
    connectors.push_back(&low().connector());
  }
  else {
    assert(hasHigh());

    const Compound &compound = holder();
    
    for (map<string, Connector *>::const_iterator connectorIt = compound.connectors().begin() ;
         connectorIt != compound.connectors().end() ;
         ++connectorIt) {
      Connector &connector = *connectorIt->second;

      if (connector.isTopLevel() &&
          &connector != &high().connector()) {
        connectors.push_back(&connector);
      }
    }
  }
}

void Priority::computeHighConnectors(vector<const Connector *> &connectors) {
  if (hasHigh()) {
    connectors.push_back(&high().connector());
  }
  else {
    assert(hasLow());
    
    const Compound &compound = holder();
    
    for (map<string, Connector *>::const_iterator connectorIt = compound.connectors().begin() ;
         connectorIt != compound.connectors().end() ;
         ++connectorIt) {
      Connector &connector = *connectorIt->second;

      if (connector.isTopLevel() &&
          &connector != &low().connector()) {
        connectors.push_back(&connector);
      }
    }
  }
}

void Priority::computeAllLowConnectors(vector<const Connector *> &connectors) {
  connectors.insert(connectors.begin(),
                    mLowConnectors.value().begin(),
                    mLowConnectors.value().end());

  for (vector<Priority *>::const_iterator priorityIt = mAllDominatedPriorities.value().begin() ;
       priorityIt != mAllDominatedPriorities.value().end() ;
       ++priorityIt) {
    Priority &priority = **priorityIt;

    for (vector<const Connector *>::const_iterator connectorIt = priority.mLowConnectors.value().begin() ;
         connectorIt != priority.mLowConnectors.value().end() ;
         ++connectorIt) {
      const Connector &connector = **connectorIt;
    
      if (find(connectors.begin(),
               connectors.end(),
               &connector)
          == connectors.end()) {
        connectors.push_back(&connector);
      }
    }
  }  
}

void Priority::computeAllHighConnectors(vector<const Connector *> &connectors) {
  connectors.insert(connectors.begin(),
                    mHighConnectors.value().begin(),
                    mHighConnectors.value().end());

  for (vector<Priority *>::const_iterator priorityIt = mAllDominatingPriorities.value().begin() ;
       priorityIt != mAllDominatingPriorities.value().end() ;
       ++priorityIt) {
    Priority &priority = **priorityIt;

    for (vector<const Connector *>::const_iterator connectorIt = priority.mHighConnectors.value().begin() ;
         connectorIt != priority.mHighConnectors.value().end() ;
         ++connectorIt) {
      const Connector &connector = **connectorIt;
    
      if (find(connectors.begin(),
               connectors.end(),
               &connector)
          == connectors.end()) {
        connectors.push_back(&connector);
      }
    }
  }
}

void Priority::computeDominatingPriorities(vector<Priority *> &priorities) {
  if (hasHigh()) {
    for (vector<Priority *>::const_iterator priorityIt = high().connector().dominatingPriorities().value().begin() ;
         priorityIt != high().connector().dominatingPriorities().value().end() ;
         ++priorityIt) {
      Priority &priority = **priorityIt;

      if (priority.appliesLow(high())) {
        priorities.push_back(&priority);
      }
    }
  }
  else {
    // this is of the from ... < *:*
    const Compound &compound = holder();

    for (vector<Priority *>::const_iterator priorityIt = compound.priorities().begin() ;
         priorityIt != compound.priorities().end() ;
         ++priorityIt) {
      Priority &priority = **priorityIt;

      assert(hasLow());

      if (priority.hasLow()) {
        if (&priority.low().connector() != &low().connector()) {
          priorities.push_back(&priority);
        }
      }
      else {
        // priority is of the from *:* < ...
        assert(priority.hasHigh());

        for (map<string, Connector *>::const_iterator connectorIt = compound.connectors().begin() ;
             connectorIt != compound.connectors().end() ;
             ++connectorIt) {
          Connector &connector = *connectorIt->second;

          // if exists a connector statisfying both wildcards
          if (connector.isTopLevel() &&
              &connector != &low().connector() &&
              &connector != &priority.high().connector()) {
            // connector must have at least one defined interaction
            // to match the wildcard
            assert(!connector.interactions().empty());

            priorities.push_back(&priority);
          }
        }
      } 
    }
  }
}


void Priority::computeDominatedPriorities(vector<Priority *> &priorities) {
  if (hasLow()) {
    for (vector<Priority *>::const_iterator priorityIt = low().connector().dominatedPriorities().value().begin() ;
         priorityIt != low().connector().dominatedPriorities().value().end() ;
         ++priorityIt) {
      Priority &priority = **priorityIt;

      if (priority.appliesHigh(low())) {
        priorities.push_back(&priority);
      }
    }
  }
  else {
    // this is of the from *:* < ...
    const Compound &compound = holder();

    for (vector<Priority *>::const_iterator priorityIt = compound.priorities().begin() ;
         priorityIt != compound.priorities().end() ;
         ++priorityIt) {
      Priority &priority = **priorityIt;

      assert(hasHigh());

      if (priority.hasHigh()) {
        if (&priority.high().connector() != &high().connector()) {
          priorities.push_back(&priority);
        }
      }
      else {
        // priority is of the from ... < *:*
        assert(priority.hasLow());

        for (map<string, Connector *>::const_iterator connectorIt = compound.connectors().begin() ;
             connectorIt != compound.connectors().end() ;
             ++connectorIt) {
          Connector &connector = *connectorIt->second;

          // if exists a connector statisfying both wildcards
          if (connector.isTopLevel() &&
              &connector != &high().connector() &&
              &connector != &priority.low().connector()) {
            // connector must have at least one defined interaction
            // to match the wildcard
            assert(!connector.interactions().empty());

            priorities.push_back(&priority);
          }
        }
      } 
    }
  }
}

void Priority::recomputeActive(bool &activity) const  {
  // cycles need to be checked before
  assert(detectCycles().type() == NO_ERROR);
  
  activity = guard();
}

void Priority::computeAllDominatingPriorities(vector<Priority *> &priorities) {
  for (vector<Priority *>::const_iterator priorityIt = dominatingPriorities().value().begin() ;
       priorityIt != dominatingPriorities().value().end() ;
       ++priorityIt) {
    Priority &priority = **priorityIt;

    if (find(priorities.begin(),
             priorities.end(),
             &priority)
        == priorities.end()) {
      priorities.push_back(&priority);

      priority.computeAllDominatingPriorities(priorities);
    }
  }
}

void Priority::computeAllDominatedPriorities(vector<Priority *> &priorities) {
  for (vector<Priority *>::const_iterator priorityIt = dominatedPriorities().value().begin() ;
       priorityIt != dominatedPriorities().value().end() ;
       ++priorityIt) {
    Priority &priority = **priorityIt;

    if (find(priorities.begin(),
             priorities.end(),
             &priority)
        == priorities.end()) {
      priorities.push_back(&priority);

      priority.computeAllDominatedPriorities(priorities);
    }
  }
}

void Priority::initialize() {
  // set dependencies between initializable objects
  if (hasHigh()) {
    mDominatingPriorities.dependsOn(high().connector().dominatingPriorities());
  }
  if (hasLow()) {
    mDominatedPriorities.dependsOn(low().connector().dominatedPriorities());
  }

  mAtomData.initialize();
  mLowConnectors.initialize();
  mHighConnectors.initialize();
  mDominatingPriorities.initialize();
  mDominatedPriorities.initialize();

  // set dependencies to data of components
  for (vector<AtomExportData *>::const_iterator dataIt = atomData().value().begin() ;
       dataIt != atomData().value().end() ;
       ++dataIt) {
    AtomExportData &atomExportData = **dataIt;

    // add a dependency to atomData
    mActive.dependsOn(atomExportData.reset());
  }
}
     
void Priority::initialize2() {
  // initialize information useful for dependencies
  initializeAllDominatingDominatedPriorities();
  initializeAllLowConnectors();
  initializeAllHighConnectors();  

  // initialize dependencies
  initializeApplyDominationDependencies();
  initializeInheritUrgenciesDependencies();
}

void Priority::initializeResetable() {
  mActive.initialize();
}

void Priority::initializeApplyDominationDependencies() {
  for (vector<const Connector *>::const_iterator connectorIt = mLowConnectors.value().begin() ;
       connectorIt != mLowConnectors.value().end() ;
       ++connectorIt) {
    const Connector &lowConnector = **connectorIt;

    // set dependencies to guards of priorities
    lowConnector.maximalInteractions().dependsOn(mActive);

    for (vector<Priority *>::const_iterator priorityIt = mAllDominatingPriorities.value().begin() ;
         priorityIt != mAllDominatingPriorities.value().end() ;
         ++priorityIt) {
      Priority &priority = **priorityIt;
      
      lowConnector.maximalInteractions().dependsOn(priority.mActive);
    }

    // set dependencies to locally maximal interactions
    for (vector<const Connector *>::const_iterator connectorIt = mAllHighConnectors.value().begin() ;
         connectorIt != mAllHighConnectors.value().end() ;
         ++connectorIt) {
      const Connector &highConnector = **connectorIt;

      lowConnector.maximalInteractions().dependsOn(lowConnector.invariant());
      lowConnector.maximalInteractions().dependsOn(highConnector.locallyMaximalInteractions());
      lowConnector.maximalInteractions().dependsOn(highConnector.invariant());
    }
  }
}

void Priority::initializeInheritUrgenciesDependencies() {
  for (vector<const Connector *>::const_iterator connectorIt = mHighConnectors.value().begin() ;
       connectorIt != mHighConnectors.value().end() ;
       ++connectorIt) {
    const Connector &highConnector = **connectorIt;

    // set dependencies to guards of priorities
    highConnector.maximalInteractions().dependsOn(mActive);

    for (vector<Priority *>::const_iterator priorityIt = mAllDominatedPriorities.value().begin() ;
         priorityIt != mAllDominatedPriorities.value().end() ;
         ++priorityIt) {
      Priority &priority = **priorityIt;
      
      highConnector.maximalInteractions().dependsOn(priority.mActive);
    }

    // set dependencies to locally maximal interactions
    for (vector<const Connector *>::const_iterator connectorIt = mAllLowConnectors.value().begin() ;
         connectorIt != mAllLowConnectors.value().end() ;
         ++connectorIt) {
      const Connector &lowConnector = **connectorIt;
      
      highConnector.maximalInteractions().dependsOn(highConnector.invariant());
      highConnector.maximalInteractions().dependsOn(lowConnector.locallyMaximalInteractions());
      highConnector.maximalInteractions().dependsOn(lowConnector.invariant());
    }
  }
}

void Priority::initializeAllDominatingDominatedPriorities() {
  // set dependencies
  const Compound &compound = holder();
  
  for (vector<Priority *>::const_iterator priorityIt = compound.priorities().begin() ;
       priorityIt != compound.priorities().end() ;
       ++priorityIt) {
    Priority &priority = **priorityIt;

    mAllDominatingPriorities.dependsOn(priority.mDominatingPriorities);
    mAllDominatedPriorities.dependsOn(priority.mDominatedPriorities);
  }

  // intialize
  mAllDominatingPriorities.initialize();
  mAllDominatedPriorities.initialize();
}

void Priority::initializeAllLowConnectors() {
  // set dependencies
  mAllLowConnectors.dependsOn(mAllDominatedPriorities);

  for (vector<Priority *>::const_iterator priorityIt = mAllDominatedPriorities.value().begin() ;
       priorityIt != mAllDominatedPriorities.value().end() ;
       ++priorityIt) {
    Priority &priority = **priorityIt;

    mAllLowConnectors.dependsOn(priority.mLowConnectors);
  }

  // initialize
  mAllLowConnectors.initialize();
}

void Priority::initializeAllHighConnectors() {
  // set dependencies
  mAllHighConnectors.dependsOn(mAllDominatingPriorities);

  for (vector<Priority *>::const_iterator priorityIt = mAllDominatingPriorities.value().begin() ;
       priorityIt != mAllDominatingPriorities.value().end() ;
       ++priorityIt) {
    Priority &priority = **priorityIt;

    mAllHighConnectors.dependsOn(priority.mHighConnectors);
  }

  // initialize
  mAllHighConnectors.initialize();
}

void Priority::applyPrioritiesDomination(const InteractionValue &interaction, TimingConstraint &constraint) const {
  if (active()) {
    // apply immediate domination
    for (vector<const Connector *>::const_iterator connectorIt = mHighConnectors.value().begin() ;
         connectorIt != mHighConnectors.value().end() ;
         ++connectorIt) {
      const Connector &targetConnector = **connectorIt;
      const vector<InteractionValue *> &dominators = targetConnector.locallyMaximalInteractions();

      for (vector<InteractionValue *>::const_iterator dominatorIt = dominators.begin() ;
           dominatorIt != dominators.end() ;
           ++dominatorIt) {
        const InteractionValue &dominator = **dominatorIt;

        bool applyDomination = true;
        
        if (hasHigh()) {
          assert(&high().connector() == &targetConnector);
          
          if (high().hasPorts()) {
            if (!(high() <= dominator.interaction())) {
              applyDomination = false;
            }
          }
        }

        if (applyDomination) {
          interaction.applyDominationBy(dominator, constraint);
        }
      }
    }

    // apply priority that are applicable indirectly
    for (vector<Priority *>::const_iterator priorityIt = dominatingPriorities().value().begin() ;
         priorityIt != dominatingPriorities().value().end() ;
         ++priorityIt) {
      Priority &priority = **priorityIt;

      priority.applyPrioritiesDomination(interaction, constraint);
    }
  }
}

void Priority::inheritPrioritiesDominatedUrgencies(const InteractionValue &interaction, TimingConstraint &constraint) const {
  if (active()) {
    for (vector<const Connector *>::const_iterator connectorIt = mLowConnectors.value().begin() ;
         connectorIt != mLowConnectors.value().end() ;
         ++connectorIt) {
      const Connector &targetConnector = **connectorIt;
      const vector<InteractionValue *> &allDominated = targetConnector.locallyMaximalInteractions();

      for (vector<InteractionValue *>::const_iterator dominatedIt = allDominated.begin() ;
           dominatedIt != allDominated.end() ;
           ++dominatedIt) {
        const InteractionValue &dominated = **dominatedIt;

        bool inheritDominatedUrgency = true;

        if (hasLow()) {
          assert(&low().connector() == &targetConnector);
          
          if (!(dominated.interaction() <= low())) {
            inheritDominatedUrgency = false;
          }
        }

        if (inheritDominatedUrgency) {
          interaction.inheritDominatedUrgencyOf(dominated, constraint);
        }
      }
    }

    // apply priority that are applicable indirectly
    for (vector<Priority *>::const_iterator priorityIt = dominatingPriorities().value().begin() ;
         priorityIt != dominatingPriorities().value().end() ;
         ++priorityIt) {
      Priority &priority = **priorityIt;

      priority.inheritPrioritiesDominatedUrgencies(interaction, constraint);
    }
  }
}
