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

#ifndef _BIP_Engine_FastMutex_HPP_
#define _BIP_Engine_FastMutex_HPP_

#include <bip-engineiface-config.hpp>
using namespace bipbasetypes;
using namespace biptypes;

#include <FastBitSet.hpp>

/** \brief A fast implementation for mutual exclusion
 * mechanisms.
 *
 * 
 */

class FastMutex {
 public:
  FastMutex();
  ~FastMutex() { }

  void lock();
  void unlock();
  bool tryToLock();
  bool isLocked() const { return mIsLocked.load(); }

 protected:
  atomic<bool> mIsLocked;
  atomic<unsigned int> mWaiters;
  mutex mMutex;
  condition_variable mConditionVariable;
};

inline
FastMutex::FastMutex() :
  mWaiters(0) {
  mIsLocked.store(false);
}

inline
void FastMutex::lock() {
  // wait for unlocked and aquire the lock
  if (!tryToLock()) {
    mWaiters.fetch_add(1);

    unique_lock<mutex> lock(mMutex);
    while (!tryToLock()) {
      mConditionVariable.wait(lock);
    }

    mWaiters.fetch_sub(1);
  }
}

inline
void FastMutex::unlock() {
#ifdef NDEBUG
  mIsLocked.store(false);
#else
  bool oldIsLocked = mIsLocked.exchange(false);
#endif

  assert(oldIsLocked);

  if (mWaiters.load() > 0) {
    unique_lock<mutex> lock(mMutex);
    mConditionVariable.notify_one();
  }
}

inline
bool FastMutex::tryToLock() {
  bool fail = mIsLocked.exchange(true);

  return !fail;
}

class FastBlockableMutex {
 public:
  FastBlockableMutex() : mIsLocked(false), mWaiters(0), mBlockers(0) { }
  ~FastBlockableMutex() { }

  void lock();
  void unlock();
  bool tryToLock();
  void block();
  void unblock();

 protected:
  atomic<bool> mIsLocked;
  atomic<int> mWaiters;
  atomic<int> mBlockers;
  mutex mMutex;
  condition_variable mConditionVariable;
};

inline
void FastBlockableMutex::lock() {
  // wait for unlocked and aquire the lock
  bool oldIsLocked = !tryToLock();

  if (oldIsLocked || mBlockers.load() > 0) {
    mWaiters.fetch_add(1);

    unique_lock<mutex> lock(mMutex);

    if (oldIsLocked) oldIsLocked = !tryToLock();

    while (oldIsLocked || mBlockers.load() > 0) {
      if (!oldIsLocked) {
        mIsLocked.store(false);

        if (mWaiters.load() > 1) {
          mConditionVariable.notify_all();
        }
      }

      mConditionVariable.wait(lock);

      oldIsLocked = !tryToLock();
    }

    mWaiters.fetch_sub(1);
  }
}

inline
void FastBlockableMutex::unlock() {
  bool oldIsLock = mIsLocked.exchange(false);

  assert(oldIsLock);

  if (oldIsLock && mWaiters.load()  > 0) {
    unique_lock<mutex> lock(mMutex);
    mConditionVariable.notify_all();
  }
}

inline
bool FastBlockableMutex::tryToLock() {
  bool fail = mIsLocked.exchange(true);
  return !fail;
}

inline
void FastBlockableMutex::block() {
  mBlockers.fetch_add(1);

  if (mIsLocked.load()) {
    mWaiters.fetch_add(1);

    unique_lock<mutex> lock(mMutex);

    while (mIsLocked.load()) {
      mConditionVariable.wait(lock);
    }

    mWaiters.fetch_sub(1);
  }
}

inline
void FastBlockableMutex::unblock() {
  unsigned int remainingBlockers = mBlockers.fetch_sub(1) - 1;

  if (remainingBlockers == 0) {
    if (mWaiters.load()  > 0) {
      unique_lock<mutex> lock(mMutex);
      mConditionVariable.notify_one();
    }
  }
}

#endif // _BIP_Engine_FastMutex_HPP_
