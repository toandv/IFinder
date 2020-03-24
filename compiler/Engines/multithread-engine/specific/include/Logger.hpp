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

#ifndef _BIP_Engine_Logger_HPP_
#define _BIP_Engine_Logger_HPP_

#include <bip-engineiface-config.hpp>

using namespace bipbasetypes;
using namespace biptypes;

#include <string>
#include <iostream>
#include <sstream>

#include "BipError.hpp"
#include <NonDeterministicPetriNetError.hpp>
#include <NonOneSafePetriNetError.hpp>
#include <CycleInPrioritiesError.hpp>
#include <CycleInAtomPrioritiesError.hpp>
#include <AtomInvariantViolationError.hpp>
#include <UnexpectedEventError.hpp>
#include <TimeSafetyViolationError.hpp>
#include "Connector.hpp"
#include "InteractionValue.hpp"
#include "Atom.hpp"
#include "AtomInternalPort.hpp"
#include "AtomExternalPort.hpp"
#include "Compound.hpp"

#include "Executer.hpp"

/** \brief Represent a node in a tree of strings
 */
class Logger {
 public:
  // constructor and destructor
  Logger(ostream &outoutStream, bool verbose, unsigned int limit);
  virtual ~Logger();
    
  // operations
  void log(const string &s);
  void logWarning(BipError &error);
  void log(BipError &error);
  void log(const InteractionValue &interaction, const TimeValue &time, const Interval &interval, const vector<InteractionValue *> &interactions);
  void log(const AtomInternalPort &port, const TimeValue &time, const Interval &interval, const vector<AtomInternalPort *> &internals);
  void log(const AtomExternalPort &port, const TimeValue &time, const Interval &interval, const vector<AtomExternalPort *> &externals);
  void logDeadlock(const Compound &root);
  void logProfilingExecution(const Compound &root);
  void logProfilingTotalExecution(const Compound &root);
  void logProfilingScheduling(const Compound &root);
  void logProfilingTotalScheduling(const Compound &root);
      
 protected:
  // operations
  void log(BipError &error, bool warning);
  void logTimeSafetyViolation(const TimeValue &timeSafetyViolation, const Connector &connector);
  void logTimeSafetyViolation(const TimeValue &timeSafetyViolation, const Atom &atom);
  
  void begin() { while(mIsDisplaying.exchange(true)); outputStream() << "[BIP ENGINE]: "; }
  void newLine() { outputStream() << endl << "[BIP ENGINE]: "; }
  void end() { outputStream() << endl; mIsDisplaying.store(false); }
  
  void logProfilingTotalExecution(const Compound &root, unsigned int &totalNbOfExecution, TimeValue &totalExecutionTime, TimeValue &totalTransitionExecutionTime, TimeValue &totalSynchronizationTime);
  void logProfilingTotalScheduling(const Compound &root, unsigned int &totalNbOfExecution, unsigned int &totalNbOfAttempt, unsigned int &totalNbOfSuccess, TimeValue &totalExecutionTime, TimeValue &totalWaitingTime, TimeValue &totalSynchronizationTime);
 
  // getters
  bool verbose() const { return mVerbose; }
  ostream &outputStream() { return mOutputStream; }
 
  ostream &mOutputStream;
  bool mVerbose;
  unsigned int mLimit;
  atomic<unsigned int> mState;
  atomic<bool> mIsDisplaying;
 };
 
inline
void Logger::log(const string &s) {
  if (verbose()) {
    begin();
    outputStream() << s;    
    end();
  }
}

inline
void Logger::logWarning(BipError &error) {
  log(error, true);
}
 
inline
void Logger::log(BipError &error) {
  log(error, false);
}

inline
void Logger::log(BipError &error, bool warning) {
  if (error.type() != NO_ERROR) {
    begin();
    
    if (warning) {
      outputStream() << "WARNING: ";
    }
    else {
      outputStream() << "ERROR: ";
    }

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

      cout << "port " << errorUnexpectedEvent.port().name() << " of component " << errorUnexpectedEvent.atom().name() << " has received an event while being disabled and having event consumption policy 'error'!";
    }
    else if (error.type() == TIME_SAFETY_VIOLATION) {
      const TimeSafetyViolationError &errorTimeSafetyViolation = dynamic_cast<const TimeSafetyViolationError &>(error);

      if (errorTimeSafetyViolation.hasConnector()) {
        logTimeSafetyViolation(errorTimeSafetyViolation.time(), errorTimeSafetyViolation.connector());
      }

      if (errorTimeSafetyViolation.hasAtom()) {
        logTimeSafetyViolation(errorTimeSafetyViolation.time(), errorTimeSafetyViolation.atom());
      }
    }
    else {
      // unsupported type of error
      assert(false);
    }

    if (!warning) Executer::jobs.stop(error);
    
    end();
  }
}

inline
void Logger::log(const InteractionValue &interaction, const TimeValue &time, const Interval &interval, const vector<InteractionValue *> &interactions) {
  unsigned int newState = 0;

  if (verbose()) {
    begin();    
    outputStream() << "state #" << mState << ": ";
    
    outputStream() << interactions.size() << " interaction";
    if (interactions.size() > 1) outputStream() << "s";
    outputStream() << ":";
        
    unsigned int index = 0;

    for (unsigned int i = 0 ; i < interactions.size() ; ++i) {
      const InteractionValue &possibleInteraction = *(interactions[i]);
      const TimingConstraint &timingConstraint = possibleInteraction.timingConstraintAfterPriorities();
      Interval validity = timingConstraint.interval() && interval;
      
      if (&interaction == &possibleInteraction) {
        index = i;
      }

      newLine();
      outputStream() << "  [" << i << "] " << possibleInteraction.connector().fullName() << ": " << possibleInteraction << " " << validity;
    }
    
    
    newLine();
    outputStream() << "-> choose [" << index << "] " << interaction.connector().fullName() << ": " << interaction << " at global time " << time;

    if (time < interaction.time()) {
      outputStream() << " (backtracking from " << interaction.time() << ")";
    }

    // /!\ order is critical!
    // for consistency of displayed states, the following incrementation
    // should be placed before end() in the verbose mode!
    newState = ++mState;

    end();
  }
  else {
    newState = ++mState;
  }

  if (mLimit != 0) {
    if (newState == mLimit) {
      Executer::jobs.stop(BipError::NoError);

      if (verbose()) {
        begin();
        outputStream() << "state #" << mState << ": stop (reached limit of " << mLimit << " interactions)";
        end();
      }
    }
  }
}

inline
void Logger::log(const AtomInternalPort &port, const TimeValue &time, const Interval &interval, const vector<AtomInternalPort *> &internals) {
  unsigned int newState = 0;

  if (verbose()) {
    begin();
    outputStream() << "state #" << mState << ": ";

    outputStream() << internals.size() << " internal port";
    if (internals.size() > 1) outputStream() << "s";
    outputStream() << ":";
        
    unsigned int index = 0;

    for (unsigned int i = 0 ; i < internals.size() ; ++i) {
      const AtomInternalPort &possibleInternal = *(internals[i]);
      const TimingConstraint &timingConstraint = possibleInternal.timingConstraint();
      Interval validity = timingConstraint.interval() && interval;
      
      if (&port == &possibleInternal) {
        index = i;
      }

      newLine();
      outputStream() << "  [" << i << "] " << possibleInternal.holder().fullName() << "." << possibleInternal.name() << " " << validity;
    }
    
    // /!\ order is critical!
    // for consistency of displayed states, the following incrementation
    // should be placed before end() in the verbose mode!
    newState = ++mState;
    
    newLine(); 
    outputStream() << "-> choose [" << index << "] " << port.holder().fullName() << "." << port.name() << " at global time " << time;
    if (time < port.holder().time()) {
      outputStream() << " (backtracking from " << port.holder().time() << ")";
    }

    end();
  }
  else {
    newState = ++mState;
  }

  if (mLimit != 0) {
    if (newState == mLimit) {
      Executer::jobs.stop(BipError::NoError);

      if (verbose()) {
        begin();
        outputStream() << "state #" << mState << ": stop (reached limit of " << mLimit << " interactions)";
        end();
      }
    }
  }
}

inline
void Logger::log(const AtomExternalPort &port, const TimeValue &time, const Interval &interval, const vector<AtomExternalPort *> &externals) {
  unsigned int newState = 0;

  if (verbose()) {
    begin();
    outputStream() << "state #" << mState << ": ";

    outputStream() << externals.size() << " external port";
    if (externals.size() > 1) outputStream() << "s";
    outputStream() << ":";
        
    unsigned int index = 0;

    for (unsigned int i = 0 ; i < externals.size() ; ++i) {
      const AtomExternalPort &possibleExternal = *(externals[i]);
      const TimingConstraint &timingConstraint = possibleExternal.timingConstraint();
      Interval validity = timingConstraint.interval() && interval;
      
      if (&port == &possibleExternal) {
        index = i;
      }

      newLine();
      outputStream() << "  [" << i << "] " << possibleExternal.holder().fullName() << "." << possibleExternal.name() << " " << validity;
    }
    
    
    newLine(); 
    outputStream() << "-> choose [" << index << "] " << port.holder().fullName() << "." << port.name() << " at global time " << time;
    if (time < port.holder().time()) {
      outputStream() << " (backtracking from " << port.holder().time() << ")";
    }

    // /!\ order is critical!
    // for consistency of displayed states, the following incrementation
    // should be placed before end() in the verbose mode!
    newState = ++mState;

    end();
  }
  else {
    newState = ++mState;
  }

  if (mLimit != 0) {
    if (newState == mLimit) {
      Executer::jobs.stop(BipError::NoError);

      if (verbose()) {
        begin();
        outputStream() << "state #" << mState << ": stop (reached limit of " << mLimit << " states)";
        end();
      }
    }
  }
}

#endif // _BIP_Engine_Logger_HPP_
