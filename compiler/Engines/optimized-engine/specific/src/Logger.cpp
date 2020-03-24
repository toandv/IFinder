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

#include "Logger.hpp"
#include "Engine.hpp"
#include "BipError.hpp"
#include "InteractionValue.hpp"
#include "AtomInternalPort.hpp"
#include "AtomExternalPort.hpp"
#include "Atom.hpp"
#include "Compound.hpp"
#include "TimeValue.hpp"
#include "TimingConstraint.hpp"
#include "NonDeterministicPetriNetError.hpp"
#include "CycleInPrioritiesError.hpp"
#include "CycleInAtomPrioritiesError.hpp"
#include "NonOneSafePetriNetError.hpp"
#include "AtomInvariantViolationError.hpp"
#include "UnexpectedEventError.hpp"
#include "TimeSafetyViolationError.hpp"

Logger::Logger(Engine &engine, ostream &outputStream, bool verbose, unsigned int limit) :
  mEngine(engine),
  mOutputStream(outputStream),
  mVerbose(verbose),
  mLimit(limit),
  mState(0) {
}

Logger::~Logger() {
}


void Logger::log(const string &s) {
  if (verbose()) {
    begin();
    outputStream() << s;    
    end();
  }
}
 
void Logger::log(const BipError &error) {
  if (error.type() != NO_ERROR) {
    begin();
    
    outputStream() << "ERROR: ";

    if (error.type() == NON_DETERMINISTIC_PETRI_NET) {
      const NonDeterministicPetriNetError &errorNonDetPetriNet = dynamic_cast<const NonDeterministicPetriNetError &>(error);

      if (errorNonDetPetriNet.hasPort()) {
        outputStream() << "non-deterministic choice between transitions labelled by " << errorNonDetPetriNet.port().name();
      }
      else {
        outputStream() << "non-deterministic choice between internal transitions";
      }

      outputStream() << " in " << errorNonDetPetriNet.atom().name() << "!";
    }
    else if (error.type() == CYCLE_IN_PRIORITIES) {
      outputStream() << "cycle in priorities! (";

      const CycleInPrioritiesError &errorPriorities = dynamic_cast<const CycleInPrioritiesError &>(error);

      for (vector<const Interaction *>::const_iterator interactionIt = errorPriorities.cycle().begin() ;
           interactionIt != errorPriorities.cycle().end() ;
           ++interactionIt) {
        const Interaction &interaction = **interactionIt;

        if (interactionIt != errorPriorities.cycle().begin()) {
          outputStream() << " < ";
        }

        outputStream() << interaction;
      }

      outputStream() << ")";
    }
    else if (error.type() == CYCLE_IN_ATOM_PRIORITIES) {
      outputStream() << "cycle in priorities! (";

      const CycleInAtomPrioritiesError &errorPriorities = dynamic_cast<const CycleInAtomPrioritiesError &>(error);

      for (vector<const AtomInternalPort *>::const_iterator portIt = errorPriorities.cycle().begin() ;
           portIt != errorPriorities.cycle().end() ;
           ++portIt) {
        const AtomInternalPort &port = **portIt;

        if (portIt != errorPriorities.cycle().begin()) {
          outputStream() << " < ";
        }

        outputStream() << port.name();
      }

      outputStream() << ")";
    }
    else if (error.type() == NON_ONE_SAFE_PETRI_NET) {
      const NonOneSafePetriNetError &errorNonOneSafe = dynamic_cast<const NonOneSafePetriNetError &>(error);

      outputStream() << "the Peri net of " << errorNonOneSafe.atom().name() << " is not 1-safe! (";
      if (errorNonOneSafe.hasPort()) {        
        outputStream() << "when executing a transition labelled by " << errorNonOneSafe.port().name();
      }
      else {
        outputStream() << "when executing an internal transition";
      }
      outputStream() << ")";
    }
    else if (error.type() == ATOM_INVARIANT_VIOLATION) {
      const AtomInvariantViolationError &errorInvariantViolation = dynamic_cast<const AtomInvariantViolationError &>(error);

      cout << "the invariant " << errorInvariantViolation.invariantName() << " of component " << errorInvariantViolation.atom().name() << " has been violated!";
    }
    else if (error.type() == UNEXPECTED_EVENT) {
      const UnexpectedEventError &errorUnexpectedEvent = dynamic_cast<const UnexpectedEventError &>(error);
      const AtomExternalPort &port = errorUnexpectedEvent.port();

      assert(port.hasEvent());

      if (engine().time() > port.eventTime()) {
        cout << "port " << errorUnexpectedEvent.port().name() << " of component " << errorUnexpectedEvent.atom().name() << " has received an outdated event at " << errorUnexpectedEvent.port().eventTime() << " while current engine time is already " << engine().time() << "!";
      }
      else {
        cout << "port " << errorUnexpectedEvent.port().name() << " of component " << errorUnexpectedEvent.atom().name() << " has received an event at " << errorUnexpectedEvent.port().eventTime() << " while being disabled at current state and having event consumption policy 'error'!";
      }
    }
    else if (error.type() == TIME_SAFETY_VIOLATION) {
      const TimeSafetyViolationError &errorTimeSafetyViolation = dynamic_cast<const TimeSafetyViolationError &>(error);

      logTimeSafetyViolation(errorTimeSafetyViolation.time());
    }
    else {
      // unsupported type of error
      assert(false);
    }
    
    end();

    ErrorType returnedErrorType = error.type();

    delete &error;

    exit(returnedErrorType);
  }
}

void Logger::logEnabledExternals() {
  if (verbose()) {
    begin();

    outputStream() << "state #" << mState << " and global time " << engine().time() << ": ";

    if (engine().externals().size() == 0) {
      outputStream() << "deadlock!";
      end();
    }
    else if (reachedLimit()) {
      outputStream() << "stop (reached limit of " << mLimit << " states)";
      end();
    }
    else {
      if (engine().externals().size() > 0) outputStream() << engine().externals().size() << " external port";
      if (engine().externals().size() > 1) outputStream() << "s";
      outputStream() << ":";

      for (unsigned int i = 0 ; i < engine().externals().size() ; ++i) {
        const AtomExternalPort &port = *(engine().externals()[i]);
      
        newLine();
        outputStream() << "  [" << i + engine().interactions().size() + engine().internals().size() << "] " << port.holder().fullName() << "." << port.name() << " " << engine().interval(port);
      }
    }

    end();
  }
}

void Logger::logTimeSafetyViolation(const TimeValue &timeSafetyViolation) {
  begin();

  outputStream() << "state #" << mState << " and global time " << timeSafetyViolation << ": violation of the following timing constraints:";

  displayViolatedTimingConstraints(timeSafetyViolation, engine().top());
  displayViolatedInvariants(timeSafetyViolation, engine().top());
  displayViolatedResume(timeSafetyViolation, engine().top());
}

void Logger::logEnabled() {
  if (verbose()) {
    begin();

    outputStream() << "state #" << mState << " and global time " << engine().time() << ": ";

    if (engine().interactions().size() == 0 && engine().internals().size() == 0 && engine().externals().size() == 0) {
      if (engine().waitInterval().right() == TimeValue::MAX) {
        outputStream() << "deadlock!";
      }
      else {
        if (engine().waitInterval().rightOpen()) {
          outputStream() << "time-lock right before " << engine().waitInterval().right() << "!";
        }
        else {
          outputStream() << "time-lock at " << engine().waitInterval().right() << "!";
        }
      }
    }
    else if (reachedLimit()) {
      outputStream() << "stop (reached limit of " << mLimit << " states)";
    }
    else {
      if (engine().interactions().size() > 0) outputStream() << engine().interactions().size() << " interaction";
      if (engine().interactions().size() > 1) outputStream() << "s";
      if (engine().interactions().size() > 0 && engine().internals().size() > 0) outputStream() << " and ";
      if (engine().internals().size() > 0) outputStream() << engine().internals().size() << " internal port";
      if (engine().internals().size() > 1) outputStream() << "s";
      if ((engine().interactions().size() > 0 || engine().internals().size() > 0) && engine().externals().size() > 0) outputStream() << " and ";
      if (engine().externals().size() > 0) outputStream() << engine().externals().size() << " external port";
      if (engine().externals().size() > 1) outputStream() << "s";
      outputStream() << ":";

      for (unsigned int i = 0 ; i < engine().interactions().size() ; ++i) {
        const InteractionValue &interaction = *(engine().interactions()[i]);

        newLine();
        outputStream() << "  [" << i << "] " << interaction.connector().fullName() << ": " << interaction << " " << engine().interval(interaction);
      }

      for (unsigned int i = 0 ; i < engine().internals().size() ; ++i) {
        const AtomInternalPort &port = *(engine().internals()[i]);
      
        newLine();
        outputStream() << "  [" << i + engine().interactions().size() << "] " << port.holder().fullName() << "." << port.name() << " " << engine().interval(port);
      }

      for (unsigned int i = 0 ; i < engine().externals().size() ; ++i) {
        const AtomExternalPort &port = *(engine().externals()[i]);
      
        newLine();
        outputStream() << "  [" << i + engine().interactions().size() + engine().internals().size() << "] " << port.holder().fullName() << "." << port.name() << " " << engine().interval(port);
      }
    }

    end();
  }
}

void Logger::log(const InteractionValue &interaction, const TimeValue &time) {
  if (verbose()) {
    // compute the index of interaction
    vector<InteractionValue *>::const_iterator interactionIt =
      find(engine().interactions().begin(),
           engine().interactions().end(),
           &interaction);

    assert(interactionIt != engine().interactions().end());

    unsigned int index = distance(engine().interactions().begin(), interactionIt);

    begin();
    outputStream() << "-> choose [" << index << "] " << interaction.connector().fullName() << ": " << interaction << " at global time " << time;
    end();
  }

  newState();
}

void Logger::log(const AtomInternalPort &port, const TimeValue &time) {
  if (verbose()) {
    // compute the index of interaction
    vector<AtomInternalPort *>::const_iterator portIt =
      find(engine().internals().begin(),
           engine().internals().end(),
           &port);

    assert(portIt != engine().internals().end());

    unsigned int index = distance(engine().internals().begin(), portIt) + engine().interactions().size();

    begin();
    outputStream() << "-> choose [" << index << "] " << port.holder().fullName() << "." << port.name() << " at global time " << time;
    end();
  }

  newState();
}

void Logger::log(const AtomExternalPort &port, const TimeValue &time) {
  if (verbose()) {
    vector<AtomExternalPort *>::const_iterator portIt =
      find(engine().externals().begin(),
           engine().externals().end(),
           &port);

    assert(portIt != engine().externals().end());

    unsigned int index = distance(engine().externals().begin(), portIt) + engine().interactions().size() + engine().internals().size();

    begin();
    outputStream() << "-> choose [" << index << "] " << port.holder().fullName() << "." << port.name() << " at global time " << time;
    end();
  }

  newState();
}


void Logger::displayViolatedTimingConstraints(const TimeValue &timeSafetyViolation, const Compound &top) {
  // display timing constraints of interactions violated by timeSafetyViolation  
  for (vector<Connector *>::const_iterator connectorIt = top.nonExportedConnectors().begin() ;
       connectorIt != top.nonExportedConnectors().end() ;
       ++connectorIt) {
    const Connector &connector = **connectorIt;

    const vector<InteractionValue *> &maximalInteractions = connector.maximalInteractions();

    for (vector<InteractionValue *>::const_iterator interactionIt = maximalInteractions.begin() ;
         interactionIt != maximalInteractions.end() ;
         ++interactionIt) {
      InteractionValue &interaction = **interactionIt;

      if (!interaction.hasResume()) {
        if (!interaction.timingConstraint().wait(engine().time()).in(timeSafetyViolation)) {
          newLine();
          outputStream() << "  " << interaction.connector().fullName() << ": " << interaction << " " << interaction.timingConstraint();
        }
      }
    }
  }

  // display timing constraints of internal ports violated by timeSafetyViolation
  for (vector<AtomInternalPort *>::const_iterator portIt = top.nonExportedInternalPorts().begin() ;
       portIt != top.nonExportedInternalPorts().end() ;
       ++portIt) {
    AtomInternalPort &port = **portIt;

    if (!port.holder().hasResume()) {
      if (port.hasPortValue()) {
        TimingConstraint constraint = TimingConstraint(port.portValue().urgency(), Interval(TimeValue::MIN, TimeValue::MAX));

        if (port.portValue().hasInterval()) {
          constraint &= port.portValue().interval();
        }

        if (!constraint.wait(engine().time()).in(timeSafetyViolation)) {
          newLine();
          outputStream() << "  " << port.holder().fullName() << "." << port.name() << " " << constraint;
        }
      }
    }
  }

  // display timing constraints of external ports violated by timeSafetyViolation
  for (vector<AtomExternalPort *>::const_iterator portIt = top.externalPorts().begin() ;
       portIt != top.externalPorts().end() ;
       ++portIt) {
    AtomExternalPort &port = **portIt;

    if (!port.holder().hasResume()) {
      if (port.hasEvent()) {
        TimingConstraint constraint = TimingConstraint(port.urgency(), Interval(port.eventTime(), TimeValue::MAX));

        if (port.hasInterval()) {
          constraint &= port.interval();
        }

        if (!constraint.wait(engine().time()).in(timeSafetyViolation)) {
          newLine();
          outputStream() << "  " << port.holder().fullName() << "." << port.name() << " " << constraint;
        }
      }
    }
  }
}

void Logger::displayViolatedInvariants(const TimeValue &timeSafetyViolation, const Compound &top) {
  // get interactions of sub-components
  for (map<string, Component *>::const_iterator componentIt = top.components().begin() ;
       componentIt != top.components().end() ;
       ++componentIt) {
    Component &component = *componentIt->second;

    if (component.type() == COMPOUND) {
      Compound &compound = dynamic_cast<Compound &>(component);

      // intersect with local timing constraint of component
      displayViolatedInvariants(timeSafetyViolation, compound);
    }
    else if (component.type() == ATOM) {
      Atom &atom = dynamic_cast<Atom &>(component);

      // intersect with local timing constraint of component
      if (atom.hasInvariant()) {
        if (!atom.invariant().in(timeSafetyViolation)) {
          newLine();
          outputStream() << "  " << atom.fullName() << " invariant " << atom.invariant();
        }
      }
    }
    else {
      assert(false);
    }
  }
}

void Logger::displayViolatedResume(const TimeValue &timeSafetyViolation, const Compound &top) {
  // get interactions of sub-components
  for (map<string, Component *>::const_iterator componentIt = top.components().begin() ;
       componentIt != top.components().end() ;
       ++componentIt) {
    Component &component = *componentIt->second;

    if (component.type() == COMPOUND) {
      Compound &compound = dynamic_cast<Compound &>(component);

      // intersect with local timing constraint of component
      displayViolatedInvariants(timeSafetyViolation, compound);
    }
    else if (component.type() == ATOM) {
      Atom &atom = dynamic_cast<Atom &>(component);

      // intersect with local timing constraint of component
      if (atom.hasResume()) {
        if (!atom.resume().in(timeSafetyViolation)) {
           newLine();
           outputStream() << "  " << atom.fullName() << " resume " << atom.resume();
        }
      }
    }
    else {
      assert(false);
    }
  }
}
