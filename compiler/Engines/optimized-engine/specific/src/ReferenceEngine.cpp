/**
 * Copyright Verimag laboratory.
 * 
 * contributors:
 *  Jacques Combaz (jacques.combaz@univ-grenoble-alpes.fr)
 *  Marc Pouhliès
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

#include "ReferenceEngine.hpp"

#include <InteractionValue.hpp>
#include <AtomInternalPort.hpp>
#include <AtomExternalPort.hpp>
#include <Component.hpp>
#include <Compound.hpp>
#include <CompoundExportPort.hpp>
#include <Port.hpp>
#include <Atom.hpp>
#include <BipError.hpp>
#include <CycleInPrioritiesError.hpp>
#include <UnexpectedEventError.hpp>
#include <Clock.hpp>

void serialize(char **, size_t *);
void deserialize(const char *, size_t, const TimeValue &time);

ReferenceEngine::ReferenceEngine(Compound &top, ModelClock &modelClock) :
  Engine(top),
  mModelClock(modelClock) {
}

ReferenceEngine::~ReferenceEngine() {
}

BipError &ReferenceEngine::resume(const TimeValue &time) {
  // should meet the next deadline
  assert(top().invariant().in(time));
  assert(resumeInterval().in(time));

  // wait for time
  modelClock().wait(time);
  top().setTime(time);

  // mark components as resumed
  BipError &error = top().resume(time);

  if (error.type() != NO_ERROR) {
    return error;
  }

  // update interactions and internals
  update();

  return BipError::NoError;
}

void ReferenceEngine::wait(const TimeValue &time) {
  // should meet the timing constraints
  assert(top().invariant().in(time));
  assert(waitInterval().in(time));

  // wait for time
  modelClock().wait(time);
  top().setTime(time);

  // update interactions and internals
  update();
}

BipError& ReferenceEngine::initialize() {
  // configure component clocks
  Clock::configureAllClocks(modelClock());

  // initialize top component
  BipError &error = top().initialize();

  if (error.type() != NO_ERROR) {
    return error;
  }

  // initialize all atom external ports
  top().initializeAllAtomExternalPorts();

  return update();
}

/* \brief Execute an interaction value.
 *
 * \param interactionValue is the target interaction value, it should an
 * enabled interaction value of a top-level connector, i.e. it should belong
 * to the set of interactions returned by interactions().
 *
 * \return An error if found during the execution of involved atoms or
 * during the update of port values, BipError::NoError otherwise.
 */
BipError& ReferenceEngine::execute(InteractionValue &interaction, const TimeValue &time) {
  // should be an interaction proposed by the engine
  assert(find(interactions().begin(),
              interactions().end(),
              &interaction)
         != interactions().end());

  // should meet timing constraints
  assert(interval(interaction).in(time));
  assert(top().invariant().in(time));
  assert(waitInterval().in(time));

  // holder component
  Compound &compound = interaction.connector().holder();

  // wait for time
  modelClock().wait(time);
  top().setTime(time);

  // down
  interaction.connector().down(interaction);

  // execute
  BipError &error = compound.execute(interaction, time);

  if (error.type() != NO_ERROR) {
    return error;
  }

  // update component port values
  BipError &errorUpdate = compound.updateUpperLevels();

  if (errorUpdate.type() != NO_ERROR) {
    return error;
  }

  // updates the interaction values and internal ports
  return update();
}

/* \brief Execute an internal port.
 *
 * \param internal is the target internal port, it should an enabled
 * internal port of an atom, i.e. it should belong to the set of internal
 * ports returned by internals().
 *
 * \return An error if found during the execution of involved atoms or
 * during the update of port values, BipError::NoError otherwise.
 */
BipError& ReferenceEngine::execute(AtomInternalPort &internal, const TimeValue &time) {
  // should be an internal proposed by the engine
  assert(find(internals().begin(),
              internals().end(),
              &internal)
         != internals().end());

  // should meet timing constraints
  assert(interval(internal).in(time));
  assert(top().invariant().in(time));
  assert(waitInterval().in(time));

  // wait for time
  modelClock().wait(time);
  top().setTime(time);

  // execute
  BipError &error = internal.holder().execute(internal, time);

  if (error.type() != NO_ERROR) {
    return error;
  }

  // update component port values
  BipError &errorUpdate = internal.holder().updateUpperLevels();

  if (errorUpdate.type() != NO_ERROR) {
    return errorUpdate;
  }

  // updates the interaction values and internal ports
  return update();
}

/* \brief Execute an external port.
 *
 * \param external is the target external port, it should a waiting
 * external port of an atom, i.e. it should belong to the set of external
 * ports returned by externals(), and it should also have a pending event.
 *
 * \return An error if found during the execution of involved atoms or
 * during the update of port values, BipError::NoError otherwise.
 */
BipError& ReferenceEngine::execute(AtomExternalPort &external, const TimeValue &time) {
  // should be an internal proposed by the engine
  assert(find(externals().begin(),
              externals().end(),
              &external)
         != externals().end());

  // should meet timing constraints
  assert(interval(external).in(time));
  assert(top().invariant().in(time));
  assert(waitInterval().in(time));

  // wait for time
  modelClock().wait(time);
  top().setTime(time);
  
  // set resume information
  external.holder().setHasResume(external.hasResume());

  // execute
  BipError &error = external.holder().execute(external, time);

  if (error.type() != NO_ERROR) {
    return error;
  }

  // reset the corresponding atom
  external.holder().reset();

  // update component port values
  BipError &errorUpdate = external.holder().updateUpperLevels();

  if (errorUpdate.type() != NO_ERROR) {
    return errorUpdate;
  }

  return update();
}

BipError& ReferenceEngine::checkExternals() {
  vector<AtomExternalPort *>::iterator portIt = waiting().begin();

  // check incoming events on waiting ports
  while (portIt != waiting().end()) {
    AtomExternalPort &port = **portIt;

    assert(port.waiting());

    bool checkRemainingEvents = false;
    bool alreadyNextPort = false;

    do {
      // /!\ hasEvent() may change at any time
      // to keep consistency we take a snapshot of
      // its current value here.
      bool snapshotHasEvent = port.hasEvent();

      checkRemainingEvents = false;
      alreadyNextPort = false;

      if (snapshotHasEvent) {
        // force recomputation of port timing constaint
        port.reset();

        if (port.hasExpectedEvent()) {
          assert(!port.timingConstraint().empty());

          // add port to the executable external ports
          enabledExternals().push_back(&port);

          // update waiting/resume interval w.r.t. port
          if (!port.holder().hasResume()) {
            mWaitInterval &= port.timingConstraint().wait(time());
          }

          // remove port of the waiting external ports
          portIt = waiting().erase(portIt);

          // inform that there is no need for incrementing portIt
          alreadyNextPort = true;
        }
        else if (!hasEarlyEvent(port)) {
          if (port.policy() == REMEMBER) {
            // nothing to do in this case!
          }
          else if (port.policy() == IGNORE) {
            port.popEvent();
            checkRemainingEvents = true;
          }
          else if (port.policy() == ERROR) {
            BipError &error = *new UnexpectedEventError(port.holder(), port);
            return error;
          }
          else {
            assert(false);
          }
        }
      }

      assert(!(checkRemainingEvents && alreadyNextPort));
    } while (checkRemainingEvents);

    // next port
    if (!alreadyNextPort) {
      ++portIt;
    }
  }

  // check incoming events on unexpected ports
  for (vector<AtomExternalPort*>::const_iterator unexpectedIt = unexpected().begin() ;
       unexpectedIt != unexpected().end() ;
       ++unexpectedIt) {
    AtomExternalPort &unexpectedPort = **unexpectedIt;

    if (unexpectedPort.hasEvent()) {
      if (!hasEarlyEvent(unexpectedPort)) {
        if (unexpectedPort.policy() == REMEMBER) {
          // nothing to do in this case!
        }
        else if (unexpectedPort.policy() == IGNORE) {
          unexpectedPort.purgeEvents();
        }
        else if (unexpectedPort.policy() == ERROR) {
          BipError &error = *new UnexpectedEventError(unexpectedPort.holder(), unexpectedPort);
          return error;
        }
        else {
          assert(false);
        }
      }
    }
  }
  
  // remove interaction/internal ports that are not enabled in the interval
  computeInteractionsInternalsExternals();

  return BipError::NoError;
}

Interval ReferenceEngine::interval(const InteractionValue &interaction) const {
  // /!\ WARNING: taking into account priorities is done here.
  Interval ret = interaction.timingConstraintAfterPriorities().interval();

  ret &= waitInterval();

  return ret;
}

Interval ReferenceEngine::interval(const AtomInternalPort &internal) const {
  assert(internal.hasPortValue());
  assert(!internal.holder().hasResume());
  
  Interval ret = internal.timingConstraint().interval();

  ret &= waitInterval();

  return ret;
}

Interval ReferenceEngine::interval(const AtomExternalPort &external) const {
  assert(external.hasEvent());
  assert(!external.holder().hasResume());
  
  Interval ret = external.timingConstraint().interval();

  ret &= waitInterval();
 
  return ret;
}

/* \brief Returns the current state of the model.
 *
 * \return an instance of State representing the current state of the model.
 */
State ReferenceEngine::getState() const {
  char *buffer;
  size_t size;

  serialize(&buffer, &size);

  return State(buffer, size);
}

/* \brief Modify the current state of the model.
 *
 * \param state is the new state for the model.
 */
void ReferenceEngine::setState(const State &state) {
  deserialize(state.buffer(), state.size(), time());

  // reset all atoms to force recomputation of connectors / ports values
  top().resetAll();

  // update the components interface (port values)
  top().recursiveUpdate();

  // update the interactions and internal ports of the engine
  update();
}

/* \brief Update interactions values and internal ports.
 *
 * Update interactions values and internal ports w.r.t. current state, so
 * that interactions() and internals() return up-to-date values.
 */
BipError &ReferenceEngine::update() {
  // recompute resume & wait intervals
  mResumeInterval = top().resume();
  mWaitInterval = top().wait();

  // compute interactions and internal ports enabled at the current state
  computeEnabledInteractionsInternals();

  // sort out external ports between waiting and unexpected
  computeWaitingUnexpected();

  // check the presence of external events
  // and compute wait & resume intervals
  return checkExternals();
}

/* \brief Update interactions values.
 *
 * Update interactions values w.r.t. current state, so that interactions()
 * returns up-to-date values.
 */
void ReferenceEngine::computeEnabledInteractionsInternals() {
  // compute internals from scratch
  enabledInternals().clear();

  for (vector<AtomInternalPort *>::const_iterator portIt = top().allNonExportedInternalPorts().begin() ;
       portIt != top().allNonExportedInternalPorts().end() ;
       ++portIt) {
    AtomInternalPort &port = **portIt;

    // add non exported internal ports enabled by transitions
    if (port.hasPortValue() && !port.holder().hasResume()) {
      if (!port.timingConstraint().empty()) {
        enabledInternals().push_back(&port);
      }
    }
  }

  // compute interactions from scratch
  enabledInteractions().clear();

  for (vector<Connector *>::const_iterator connectorIt = top().allNonExportedConnectors().begin() ;
       connectorIt != top().allNonExportedConnectors().end() ;
       ++connectorIt) {
    const Connector &connector = **connectorIt;

    // add maximal interactions of all non exported connectors
    const vector<InteractionValue *> &maximalInteractions = connector.maximalInteractions();

    // keep one maximal interaction
    enabledInteractions().insert(enabledInteractions().end(),
                         maximalInteractions.begin(),
                         maximalInteractions.end());
  }
}

void ReferenceEngine::computeInteractionsInternalsExternals() {
  // recompute all from scratch
  interactions().clear();
  internals().clear();
  externals().clear();

  // keep interactions having non empty interval of execution
  for (vector<InteractionValue *>::iterator interactionIt = enabledInteractions().begin() ;
       interactionIt != enabledInteractions().end() ;
       ++interactionIt) {
    InteractionValue &interaction = **interactionIt;

    if (!interval(interaction).empty()) {
      interactions().push_back(&interaction);
    }
  }

  // keep internal ports having non empty interval of execution
  for (vector<AtomInternalPort *>::iterator internalPortIt = enabledInternals().begin() ;
       internalPortIt != enabledInternals().end() ;
       ++internalPortIt) {
    AtomInternalPort &internalPort = **internalPortIt;

    if (!interval(internalPort).empty()) {
      internals().push_back(&internalPort);
    }
  }

  // keep internal ports having non empty interval of execution
  for (vector<AtomExternalPort *>::iterator externalPortIt = enabledExternals().begin() ;
       externalPortIt != enabledExternals().end() ;
       ++externalPortIt) {
    AtomExternalPort &externalPort = **externalPortIt;

    if (!interval(externalPort).empty()) {
      externals().push_back(&externalPort);
    }
  }
}

void ReferenceEngine::computeWaitingUnexpected() {
  // recompute all from scratch
  enabledExternals().clear();
  waiting().clear();
  unexpected().clear();

  for (vector<AtomExternalPort *>::const_iterator portIt = top().allExternalPorts().begin() ;
       portIt != top().allExternalPorts().end() ;
       ++portIt) {
    AtomExternalPort &port = **portIt;
    
    if (port.waiting() && !port.holder().hasResume()) {
      if (port.hasInterval()) {
        if (port.interval().next(time()) != TimeValue::MAX) {
          waiting().push_back(&port);
        }
        else {
          unexpected().push_back(&port);
        }
      }
      else {
        waiting().push_back(&port);
      }
    }
    else {
      unexpected().push_back(&port);
    }
  }
}

bool ReferenceEngine::hasEarlyEvent(AtomExternalPort &port) const {
  assert(port.hasEvent());

  return waitInterval().right() < port.eventTime();
}
