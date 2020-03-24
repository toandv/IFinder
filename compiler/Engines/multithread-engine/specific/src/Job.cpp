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

#include "Job.hpp"
#include "BipError.hpp"
#include "Atom.hpp"
#include "AtomInternalPort.hpp"
#include "PortValue.hpp"
#include "Connector.hpp"
#include "InteractionValue.hpp"
#include "Thread.hpp"
#include "Resetable.hpp"

Job::Job(ReadyQueue<Job> &readyQueue, bool highPriority) :
  mReadyQueue(readyQueue),
  mHighPriority(highPriority),
  mIsRestarted(false),
  mIsEnqueued(true),
  mPreventEnqueuing(0),
  mIsRealTime(false),
  mModelClock(),
  mHasWakeUpTime(false),
  mHasWokenUpAtWakeUpTime(false),
  mWakeUpTime(TimeValue::MIN),
  mProfiling(true),
  mTotalNbOfExecutions(0),
  mTotalExecutionTime(TimeValue::ZERO),
  mTotalPrologueTime(TimeValue::ZERO),
  mTotalEpilogueTime(TimeValue::ZERO),
  mTotalWaitingTime(TimeValue::ZERO) {
}

Job::~Job() {
}

void Job::execute() {
  if (profiling()) ++mTotalNbOfExecutions;
  
  TimeValue startTime;

  if (profiling()) startTime = mRealTimeClock.time();
  
  mExecuting.lock();

  TimeValue waitTime;

  if (profiling()) waitTime = mRealTimeClock.time();

  // wait for wake up time or notifications
  mHasWokenUpAtWakeUpTime = waitForWakeUpTime();

  if (profiling()) mTotalWaitingTime += mRealTimeClock.time() - waitTime;

  if (mPreventEnqueuing.load() > 0) {
#ifdef NDEBUG
    mIsRestarted.store(false);
#else
    bool oldIsRestarted = mIsRestarted.exchange(false);
#endif
    
    assert(oldIsRestarted == true);

    restart();
  }
  else {
    TimeValue prologueTime, epilogueTime;
    
    if (profiling()) prologueTime = mRealTimeClock.time();
    
    // prologue
    prologue();

    if (profiling()) mTotalPrologueTime +=  mRealTimeClock.time() - prologueTime;

    // reset restart/enqueue status
#ifdef NDEBUG
    mIsRestarted.store(false);
#else
    bool oldIsRestarted = mIsRestarted.exchange(false);
#endif

    assert(oldIsRestarted == true);

    // reset notifications
    platformClock().resetNotifications();

    // perform the specific job
    realJob();

    if (profiling()) epilogueTime = mRealTimeClock.time();
    
    if (hasJoiner()) {
      joiner().join();
    }
    else {
      epilogue();
    }

    if (profiling()) mTotalEpilogueTime +=  mRealTimeClock.time() - epilogueTime;
  }

  mExecuting.unlock();

  if (profiling()) mTotalExecutionTime += mRealTimeClock.time() - startTime;
}

void Job::enqueue() {
  if (!mIsEnqueued.exchange(true)) {
    if (mHighPriority) {
      // /!\ WARNING
      // Should be replace by push_front to increase
      // general performances, but push_front is not
      // implemented in the ready queue currently.
      // Several attempts has been made without success
      // in terms of performance
      mReadyQueue.push(*this);
    }
    else {
      mReadyQueue.push(*this);
    }
  }
}

void Job::setIsRealTime(bool isRealTime) {
  mIsRealTime = isRealTime;

  if (isRealTime) {
    modelClock().setPlatformClock(mRealTimeClock);
  }
  else {
    modelClock().setPlatformClock(mSimulationClock);
  }
}

Resource Job::mSimulationClockResource;
