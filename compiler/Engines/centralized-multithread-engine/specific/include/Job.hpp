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

#ifndef _BIP_Engine_Job_HPP_
#define _BIP_Engine_Job_HPP_

#include "BipError.hpp"
#include "FastMutex.hpp"
#include "ReadyQueue.hpp"
#include <TimeValue.hpp>
#include <Interval.hpp>
#include <ModelClock.hpp>
#include <NotifiableClock.hpp>
#include <RealTimeClock.hpp>
#include <SimulationClock.hpp>
#include <Resource.hpp>

class ResetableItf;
class Thread;

class Job {
 public:
  class Joiner;

  // constructors
  Job(ReadyQueue<Job> &readyQueue, bool highPriority = false);

  // destructor
  virtual ~Job();

  // getters
  bool isRestarted() const { return mIsRestarted.load(); }

  // operations
  void dependsOn(ResetableItf &resetable);

  void execute();

  void restart();
  void restart(const TimeValue &time);

  void preventEnqueuing() { mPreventEnqueuing.fetch_add(1); }
  void unpreventEnqueuing();

  // FIXME:
  ReadyQueue<Job> &readyQueue() { return mReadyQueue; }
  const vector<Thread *> &threads() const { return mReadyQueue.threads(); }

  const ModelClock &modelClock() const { return mModelClock; }
  const NotifiableClock &platformClock() const;

  static Resource &simulationClockResource() { return mSimulationClockResource; }

  bool isRealTime() const { return mIsRealTime; }
  void setIsRealTime(bool isRealTime);
  bool profiling() const { return mProfiling; }
  void setProfiling(bool profiling) { mProfiling = profiling; }
  unsigned int totalNbOfExecution() const { return mTotalNbOfExecutions; }
  const TimeValue &totalExecutionTime() const { return mTotalExecutionTime; }
  const TimeValue &totalPrologueTime() const { return mTotalPrologueTime; }
  const TimeValue &totalEpilogueTime() const { return mTotalEpilogueTime; }
  const TimeValue &totalWaitingTime() const { return mTotalWaitingTime; }

  // join mechanisms
  class Joiner {
   public:
    // constructors / destructors
    Joiner() : mNbOfJobsToJoin(0) { }
    virtual ~Joiner() { }

    void addJobToJoin(Job &job);
    void join();

   protected:
    virtual void epilogue() { }
    const vector<Job *> &jobsToJoin() const { return mJobsToJoin; }

   private:
    vector<Job *> mJobsToJoin;
    atomic<unsigned int> mNbOfJobsToJoin;
   };

 protected:
  // protected operations
  virtual void realJob() {
    // dummy job by default
  }

  virtual void prologue() { }
  virtual void epilogue() { }

  void enqueue();
  void commonRestart();

  // real-time mechanisms
  ModelClock &modelClock() { return mModelClock; }
  NotifiableClock &platformClock();
  bool waitForWakeUpTime();

  // real-time scheduling
  void setWakeUpTime(const TimeValue &wakeUpTime);
  const TimeValue &wakeUpTime() const { return mWakeUpTime; }
  void clearWakeUpTime();
  bool hasWokenUpAtWakeUpTime() const { return mHasWokenUpAtWakeUpTime; }

  // joining mechanisms
  void setJoiner(Joiner &joiner) { mJoiner = &joiner; }
  void clearJoiner() { mJoiner = NULL; }
  bool hasJoiner() const { return mJoiner != NULL; }
  Joiner &joiner() { return *mJoiner; }

  // attributes
  ReadyQueue<Job> &mReadyQueue;
  bool mHighPriority;

  atomic<bool> mIsRestarted;
  atomic<bool> mIsEnqueued;
  atomic<unsigned int> mPreventEnqueuing;

  // joining mechanisms
  Joiner *mJoiner;
  
  // (b)locking mechanisms
  FastMutex mExecuting;

  // real-time mechanisms
  bool mIsRealTime;
  ModelClock mModelClock;
  RealTimeClock mRealTimeClock;
  SimulationClock mSimulationClock;
  static Resource mSimulationClockResource;

  // wake-up mechanisms
  atomic<bool> mHasWakeUpTime;
  bool mHasWokenUpAtWakeUpTime;
  TimeValue mWakeUpTime;

  // profiling
  bool mProfiling;
  unsigned int mTotalNbOfExecutions;
  TimeValue mTotalExecutionTime;
  TimeValue mTotalPrologueTime;
  TimeValue mTotalEpilogueTime;
  TimeValue mTotalWaitingTime;
};

inline void Job::restart() {
  // immediate restart
  clearWakeUpTime();

  commonRestart();
}

inline void Job::restart(const TimeValue &time) {
  // delayed restart
  setWakeUpTime(time);

  commonRestart();  
}

inline void Job::commonRestart() {
  if (!mIsRestarted.exchange(true)) {
    // restarted jobs require enqueuing at some point
#ifdef NDEBUG
    mIsEnqueued.store(false);
#else
    bool oldIsEnqueued = mIsEnqueued.exchange(false);
#endif

    assert(oldIsEnqueued == true);
    
    // enqueue if no before job has been restarted
    if (mPreventEnqueuing.load() == 0) {
      enqueue();
    }
  }
}

inline void Job::unpreventEnqueuing() {
  unsigned int oldPreventEnqueuing = mPreventEnqueuing.fetch_sub(1);

  assert(oldPreventEnqueuing > 0);

  if (oldPreventEnqueuing - 1 == 0) {
    enqueue();
  }
}

inline const NotifiableClock &Job::platformClock() const {
  if (isRealTime()) {
    return mRealTimeClock;
  }
  else {
    return mSimulationClock;
  }
}

inline NotifiableClock &Job::platformClock() {
  if (isRealTime()) {
    return mRealTimeClock;
  }
  else {
    return mSimulationClock;
  }
}

inline bool Job::waitForWakeUpTime() {
  bool ret = false;

  if (mHasWakeUpTime.exchange(false)) {
    bool hasNotifications = platformClock().wait(wakeUpTime());

    ret = !hasNotifications;

    assert(platformClock().time() >= wakeUpTime() || !mHasWokenUpAtWakeUpTime);

    mWakeUpTime = TimeValue::MIN;
  }

  return ret;
}

inline void Job::setWakeUpTime(const TimeValue &wakeUpTime) {
  mWakeUpTime = wakeUpTime;
  mHasWokenUpAtWakeUpTime = false;

#ifndef NDEBUG
  bool oldHasWakeUpTime =
#endif
  mHasWakeUpTime.exchange(true);

  assert(!oldHasWakeUpTime);
}

inline void Job::clearWakeUpTime() {
  if (mHasWakeUpTime.exchange(false)) {
    mWakeUpTime = TimeValue::MIN;
  }
  
  platformClock().notify();
}

inline void Job::Joiner::addJobToJoin(Job &job) {
  mJobsToJoin.push_back(&job);
  mNbOfJobsToJoin.fetch_add(1);

  assert(!job.hasJoiner());
  job.setJoiner(*this);
}

inline void Job::Joiner::join() {
  unsigned int oldNbOfJobsToJoin = mNbOfJobsToJoin.fetch_sub(1);

  assert(oldNbOfJobsToJoin > 0);
  
  if (oldNbOfJobsToJoin - 1 == 0) {
    // joiner-specific epilogue 
    epilogue();

    bool empty = mJobsToJoin.empty();

    assert(!empty);

    while (!empty) {
      vector<Job *>::iterator jobIt  = mJobsToJoin.end() - 1;
      Job &job = **jobIt;

      // remove job from the jobs to join
      mJobsToJoin.erase(jobIt);

      // WARNING: order is critical!
      // it should be placed before job.epilogue()!
      empty = mJobsToJoin.empty();

      assert(find(mJobsToJoin.begin(),
                  mJobsToJoin.end(),
                  &job) == mJobsToJoin.end());

      // remove job's joiner
      job.clearJoiner();

      assert(mNbOfJobsToJoin.load() == 0);

      // terminate job's execution
      job.epilogue();
    }
  }
}


#endif // _BIP_Engine_Job_HPP_
