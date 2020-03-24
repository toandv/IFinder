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

#include <ExecuteExternalsJob.hpp>
#include <ExecuteAtomJob.hpp>
#include "ExecuteConnectorJob.hpp"

#include "AtomInternalPort.hpp"
#include "AtomExternalPort.hpp"

#include "Connector.hpp"
#include "Compound.hpp"
#include "Atom.hpp"
#include "Logger.hpp"

#include "Executer.hpp"
#include "BipError.hpp"
#include "UnexpectedEventError.hpp"

#include <TimeValue.hpp>

ExecuteExternalsJob::ExecuteExternalsJob(Atom &atom) :
  Job(Executer::jobs),
  mAtom(atom),
  mLogger(NULL),
  mReserver(*this),
  mReader(*this),
  mChosenExternal(NULL),
  mChosenTime(TimeValue::MIN),
  mNbAttempt(0),
  mNbSuccess(0) {
  mResetChoice.dependsOn(atom.externalsReset());
}

ExecuteExternalsJob::~ExecuteExternalsJob() {
}

void ExecuteExternalsJob::initialize() {
  mResetChoice.addUser(*this);
  
  mReserver.addResources(atom().resources());
  mReader.addResources(atom().resources());

  if (!isRealTime()) {
    mReserver.addResource(simulationClockResource());
    mReader.addResource(simulationClockResource());
    
    mUpdateClockJob.initialize();
  }
}

void ExecuteExternalsJob::realJob() {
  // check newly arrived externals
  BipError &error = atom().checkExternals();

  if (error.type() != NO_ERROR) {
    logger().log(error);
  }
  else {
    // compute time-safe interval
    if (relaxed()) {
      assert(!atom().timeSafetyValidator().timeSafe().empty());
      assert(!atom().timeSafetyValidator().timeSafe().rightOpen());

      // backtracking is allowed in the relaxed mode only in NDEBUG mode
#ifdef NDEBUG
      mTimeSafe = Interval(TimeValue::MIN, atom().timeSafetyValidator().timeSafe().right());
#else
      mTimeSafe = atom().timeSafetyValidator().timeSafe();
#endif
    }
    else {
      mTimeSafe = atom().timeSafetyValidator().timeSafe();
    }

    if (atom().hasInvariant()) {
      mTimeSafe &= atom().invariant();
    }

    if (hasChosen()              &&
        hasWokenUpAtWakeUpTime() &&
        !mResetChoice.isReset()  &&
        mTimeSafe.in(chosenTime())) {
      // in simulation mode interaction are executed directly without sleeping periods
      assert(isRealTime());

      // try to execute the chosen interaction
      tryToExecuteChosenExternal();
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
            tryToExecuteChosenExternal();
          }
          else {
            restart(chosenTime());
          }
        }
      }
    }
  }
}

void ExecuteExternalsJob::updateTime() {
  // update model-time w.r.t. real-time
  mModelClock.update();
  
  // update time used for scheduling internals
if (!atom().timeSafetyValidator().timeSafe().in(mModelClock.time()) && hasResumeEager()) {
    assert(!atom().timeSafetyValidator().timeSafe().empty());
    assert(!atom().timeSafetyValidator().timeSafe().rightOpen());

    mTime = atom().timeSafetyValidator().timeSafe().right();
  }
  else if (!mTimeSafe.in(mModelClock.time()) && relaxed()) {
    assert(!atom().timeSafetyValidator().timeSafe().empty());
    assert(!atom().timeSafetyValidator().timeSafe().rightOpen());

    mTime = atom().timeSafetyValidator().timeSafe().right();

    Interval invariant = Interval(TimeValue::MIN, TimeValue::MAX);

    if (atom().hasInvariant()) {
      invariant &= atom().invariant();
    }

    assert(!invariant.empty());
    assert(!invariant.rightOpen());

    if (invariant.right() < mTime) {
      mTime = invariant.right();
    }
  }
  else {
    if (isRealTime() || mModelClock.time() >= atom().time()) {
      mTime = mModelClock.time();
    }
    else {
      mTime = atom().time();
    }
  }
}

BipError &ExecuteExternalsJob::checkTimeSafety(const TimeValue &time) {
  assert(!mTimeSafe.empty());
  
  if (!mTimeSafe.in(time)) {
    if (!mReserver.hasReservation()) {
      if (!mTimeSafe.empty() &&
	  (time - mTimeSafe.right() <= TimeValue(10, MILLISECOND))) {
        restart(mTimeSafe.right() + TimeValue(20, MILLISECOND));
      }
      else {
        TimeSafetyViolationError &error = *new TimeSafetyViolationError(time, atom());

        return error;
      }
    }
  }

  return BipError::NoError;
}

BipError &ExecuteExternalsJob::checkTimeSafeExecution(const TimeValue &time) {
  assert(!mTimeSafe.empty());
  
  if (!mTimeSafe.in(time)) {
    TimeSafetyViolationError &error = *new TimeSafetyViolationError(time, atom());

    return error;
  }

  return BipError::NoError;
}

bool ExecuteExternalsJob::tryToExecuteChosenExternal() {
  if (profiling()) ++mNbAttempt;
  
  bool success = mReserver.tryToReserve(chosenTime(), relaxed());

  if (success) {
    BipError &error = checkTimeSafeExecution(mModelClock.time());

    if (error.type() != NO_ERROR) {
      if (relaxed()) {
        logger().logWarning(error);
      }
      else {
        assert(hasResumeEager());
      }
    }

    // execute chosen interaction
    executeChosenExternal();
      
    if (profiling()) ++mNbSuccess;
    
    if (!isRealTime()) {
      mUpdateClockJob.setTime(chosenTime());
      mUpdateClockJob.restart();
    }
  }

  // self-retart
  restart();

  return success;
}

void ExecuteExternalsJob::logChosenExternal() {
  assert(hasChosen());
  
  // log the choices
  logger().log(chosenExternal(), chosenTime(), mSchedulable, atom().externals());
}

void ExecuteExternalsJob::executeChosenExternal() {
  assert(hasChosen());
  
  // configure atom job for execution of chosen external
  atom().executeJob().setChosenExternal(chosenExternal());
  atom().executeJob().setChosenTime(chosenTime());
  atom().setHasResume(chosenExternal().hasResume());
  
  atom().executeJob().restart();
}

void ExecuteExternalsJob::recomputeChoice() {
  // reset chosen external and time
  mChosenExternal = NULL;
  mChosenTime = TimeValue::MIN;
  
  // reinitialize real-time scheduler
  Scheduler scheduler(mTime, asap(), firstEnabled());
  
  mSchedulable = mTimeSafe && Interval(mTime, mReserver.deadline());

  for (vector<AtomExternalPort *>::const_iterator portIt = atom().externals().begin() ;
       portIt != atom().externals().end() ;
       ++portIt) {
    AtomExternalPort &port = **portIt;
    const TimingConstraint &timingConstraint = port.timingConstraint();

    Interval interv = timingConstraint.interval() &&
      mSchedulable;

    if (scheduler.choose(interv, false)) {
      mChosenExternal = &port;
    }
  }

  if (mChosenExternal != NULL) {
    mChosenTime = scheduler.plannedTime();
  }

  // mark choice as non-reset
  mResetChoice.setIsReset(false);
}

bool ExecuteExternalsJob::hasResumeEager() const {
  bool ret = false;
  
  if (atom().hadResume()) {
    for (vector<AtomExternalPort *>::const_iterator portIt = atom().externals().begin() ;
         portIt != atom().externals().end() ;
         ++portIt) {
      const AtomExternalPort &port = **portIt;
      const TimingConstraint &timingConstraint = port.timingConstraint();
      
      if (timingConstraint.in(atom().time()) && timingConstraint.urgency() == EAGER) {
        ret = true;
        break;
      }
    }

    const vector<AtomInternalPort *> &enabledInternals = atom().internals();
    
    for (vector<AtomInternalPort *>::const_iterator portIt = enabledInternals.begin() ;
         portIt != enabledInternals.end() ;
         ++portIt) {
      const AtomInternalPort &port = **portIt;
      const TimingConstraint &timingConstraint = port.timingConstraint();
      
      if (timingConstraint.in(atom().time()) && timingConstraint.urgency() == EAGER) {
        ret = true;
        break;
      }
    }
  }

  return ret;
}
