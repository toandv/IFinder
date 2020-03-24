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

#ifndef _BIP_Engine_NotifiableClock_HPP_
#define _BIP_Engine_NotifiableClock_HPP_

#include <bip-engineiface-config.hpp>
using namespace bipbasetypes;
using namespace biptypes;

#include <chrono>
#include <ctime>

#include <GlobalClock.hpp>

/** \brief Provides the basic interface for implementing the clock used by
 * the scheduler.
 *
 * 
 */
class NotifiableClock : public GlobalClock {
 public:
  NotifiableClock() { }
  virtual ~NotifiableClock() { }

  virtual TimeValue time() const = 0;
  virtual bool wait(const TimeValue &time) = 0;

  void notify();
  void resetNotifications();
  bool isNotified() const { return mIsNotified.load(); }

 protected:
  bool waitForNotifications();
  bool waitForNotificationsFor(const chrono::nanoseconds &delay);

  atomic<bool> mIsNotified;
  atomic<unsigned int> mWaiters;
  mutex mMutex;
  condition_variable mConditionVariable;
};

inline
void NotifiableClock::notify() {
  mIsNotified.store(true);

  if (mWaiters.load() > 0) {
    unique_lock<mutex> lock(mMutex);
    
    mIsNotified.store(true);
    
    mConditionVariable.notify_all();
  }
}

inline
void NotifiableClock::resetNotifications() {
  assert(mWaiters.load() == 0);
  
  mIsNotified.store(false);
}

inline
bool NotifiableClock::waitForNotifications() {
  if (!isNotified()) {
    mWaiters.fetch_add(1);

    unique_lock<mutex> lock(mMutex);

    if (!isNotified()) {
      mConditionVariable.wait(lock);

      assert(isNotified());
    }
  
    mWaiters.fetch_sub(1);
  }

  assert(isNotified());

  return isNotified();
}

inline
bool NotifiableClock::waitForNotificationsFor(const chrono::nanoseconds &delay) {
  if (!isNotified()) {
    mWaiters.fetch_add(1);

    unique_lock<mutex> lock(mMutex);

    if (!isNotified()) {
#ifndef NDEBUG
      cv_status status =
#endif

      mConditionVariable.wait_for(lock, delay);

      assert(isNotified() || status == cv_status::timeout);
    }
  
    mWaiters.fetch_sub(1);
  }

  return isNotified();
}

#endif // _BIP_Engine_NotifiableClock_HPP_
