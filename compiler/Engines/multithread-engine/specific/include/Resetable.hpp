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

#ifndef _BIP_Engine_Resetable_HPP_
#define _BIP_Engine_Resetable_HPP_

#include <bip-engineiface-config.hpp>
using namespace bipbasetypes;
using namespace biptypes;

#include "FastMutex.hpp"
#include "Job.hpp"
#include "Resource.hpp"
#include "FastBitSet.hpp"
#include "Group.hpp"


class Thread;


/** \brief 
 */
class ResetableItf {
 public:
  // constructor and destructor
  ResetableItf(bool canOnlyResetDependent = false);
  virtual ~ResetableItf();

  // operations
  void reset();
  void resetDependent();
  void dependsOn(ResetableItf &resetable);
  bool isDependentOf(ResetableItf &resetable) const;

  // setters and getters
  void setIsReset(bool b) const {
#ifndef NDEBUG
    mShouldBeInitialized.store(true);
#endif
    
    // mResetGroup.store(b);
    if (b) {
      mIsResetGroup.add(*this);
    }
    else {
      mIsResetGroup.remove(*this);
    }
  }
  bool isReset() const { return mIsResetGroup.in(*this); }

  const vector<Job *> &users() const { return mUsers; }
  void addUser(Job &user);
  const vector<Resource *> &resources() const { return mResources; }
  void addResource(Resource &resource) { mResources.push_back(&resource); }
  vector<Resource *> allResources() const;

  void addTraversalThread(Thread &thrd);
  void addTraversalThreads(const vector<Thread *> &threads);

  bool checkThreadSafety() const { return mCheckThreadSafety; }
  bool isThreadSafe() const { return mIsThreadSafe; }
  bool canOnlyResetDependent() const { return mCanOnlyResetDependent; }
  void initialize();
  size_t index() const { return mIndex; }
  void setIndex(size_t index) const { mIndex = index; }

 protected:
  void restart(const Group<ResetableItf> &group);
  void allResources(vector<Resource *> &ret, vector<const ResetableItf *> &visitedResetables) const;
  void checkAllDependent() const;
  void checkAllDependent(vector<const ResetableItf *> &visited) const;
  void checkAllDependencies() const;
  void checkAllDependencies(vector<const ResetableItf *> &visited) const;
  void allConnected(vector<ResetableItf *> &visited);
  void allDependent(vector<ResetableItf *> &visited);

  // protected attributes
  vector<ResetableItf *> mDependent;
  vector<ResetableItf *> mDependencies;
  // mutable atomic<bool> mIsReset;
  mutable atomic<bool> mShouldBeInitialized;

  vector<Job *> mUsers;
  vector<Resource *> mResources;
  vector<Thread *> mTraversalThreads;

  bool mCheckThreadSafety;
  bool mIsThreadSafe;
  bool mCanOnlyResetDependent;
  bool mHasEmptyResetDependent;

  static AtomicGroup<ResetableItf> mIsResetGroup;
  static Group<ResetableItf> mHasUsers;
  Group<ResetableItf> mResetGroup;
  Group<ResetableItf> mResetDependentGroup;
  Group<ResetableItf> mSuccess;

  mutable size_t mIndex;
};

template<class T, class C>
class Resetable : public ResetableItf {
 public:
  // constructor and destructor// constructor and destructor
  Resetable(C *instance, void (C::*method)(T& t) const) : mInstance(*instance), mMethod(method) { }
  Resetable(C *instance, void (C::*method)(T& t) const, const T &object) : mInstance(*instance), mMethod(method), mObject(object) { }
  Resetable(C *instance, void (C::*method)(T& t) const, const T &object, bool checkThreadSafety) : ResetableItf(checkThreadSafety), mInstance(*instance), mMethod(method), mObject(object) { }
  virtual ~Resetable() { }

  // operations
  operator const T & () const;
  void recompute() const;

 protected:
  // protected attributes
  C &mInstance;
  void (C::*mMethod)(T& t) const;
  mutable T mObject;

  mutable FastMutex mIsRecomputing;
};

inline
void ResetableItf::reset() {
  assert(!canOnlyResetDependent());

  mSuccess.clear();
  
  mIsResetGroup.add(mResetGroup, mSuccess);
  
  mSuccess.intersect(mHasUsers);
  
  restart(mSuccess);
  
#ifndef NDEBUG
  for (vector<ResetableItf *>::const_iterator resetableIt = mDependent.begin() ;
       resetableIt != mDependent.end() ;
       ++resetableIt) {
    const ResetableItf &resetable = **resetableIt;

    resetable.checkAllDependent();
  }
#endif
}

inline
void ResetableItf::resetDependent() {
  if (!mHasEmptyResetDependent) {
    mSuccess.clear();
  
    mIsResetGroup.add(mResetDependentGroup, mSuccess);

    mSuccess.intersect(mHasUsers);

    restart(mSuccess);
  }

#ifndef NDEBUG
    checkAllDependent();
#endif
}

inline
void ResetableItf::dependsOn(ResetableItf &resetable) {
  // should not have been reset previously
  assert(!mShouldBeInitialized);

  if (!isDependentOf(resetable)) {
    // set dependency links in both directions
    resetable.mDependent.push_back(this);
    mDependencies.push_back(&resetable);
  }
}

inline
bool ResetableItf::isDependentOf(ResetableItf &resetable) const {
  return find(resetable.mDependent.begin(),
              resetable.mDependent.end(),
              this)
         != resetable.mDependent.end();
}

inline
void ResetableItf::restart(const Group<ResetableItf> &group) {
  for (Group<ResetableItf>::iterator it = group.begin() ;
       it != group.end() ;
       ++it) {    
    const ResetableItf &resetable = *it;

    // restart jobs
    for (vector<Job *>::const_iterator jobIt = resetable.users().begin() ;
         jobIt != resetable.users().end() ;
         ++jobIt) {
      Job &job = **jobIt;
      
      job.restart();
    }
  }
}

template<class T, class C>
inline Resetable<T, C>::operator const T & () const {
  recompute();
  
  return mObject;
}

template<class T, class C>
inline void Resetable<T, C>::recompute() const {
  // wait for completion of other computation
  // if (checkThreadSafety()) {
  //   if (!isThreadSafe()) {
      mIsRecomputing.lock();
  //   }
  // }

  // recompute if needed (if reset)
  if (isReset()) {
    // call recompute method
    (mInstance.*mMethod)(mObject);

    setIsReset(false);
    
#ifndef NDEBUG
    checkAllDependencies();
#endif
  }
  
  // set ready for other computation
  // if (checkThreadSafety()) {
  //  if (!isThreadSafe()) {
      mIsRecomputing.unlock();
  //   }
  // }
}

#endif // _BIP_Engine_Resetable_HPP_
