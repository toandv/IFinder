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

#include "Resetable.hpp"
#include "Thread.hpp"
#include "Job.hpp"
#include "ReadyQueue.hpp"

ResetableItf::ResetableItf(bool canOnlyResetDependent) :
  mShouldBeInitialized(false),
  mCheckThreadSafety(true),
  mIsThreadSafe(true),
  mCanOnlyResetDependent(canOnlyResetDependent),
  mIndex(0) {
#ifndef NDEBUG
  // if (mResetGroup) mShouldBeInitialized.store(true);
#endif
}

ResetableItf::~ResetableItf() {
}

void ResetableItf::addUser(Job &user) {  
  // check no duplication of users
  assert(find(mUsers.begin(),
              mUsers.end(),
              &user)
         == mUsers.end());

  // add the connector job
  mUsers.push_back(&user);

  // update traversal threads for all dependencies
  addTraversalThreads(user.threads());
}

vector<Resource *> ResetableItf::allResources() const {
  vector<Resource *> ret;
  vector<const ResetableItf *> visitedResetables;

  allResources(ret, visitedResetables);

  return ret;
}

/**
 * \brief Compute all reachable execute atom jobs.
 *
 * \return The set of execute atom jobs reachable from 'this'.
 */
void ResetableItf::allResources(vector<Resource *> &ret, vector<const ResetableItf *> &visitedResetables) const {
  // check if this has been already visited
  if (find(visitedResetables.begin(),
           visitedResetables.end(),
           this)
      == visitedResetables.end()) {
    // add resources of this
    for (vector<Resource *>::const_iterator resourceIt = mResources.begin() ;
         resourceIt != mResources.end() ;
         ++resourceIt) {
      Resource &resource = **resourceIt;

      if (find(ret.begin(), ret.end(), &resource) == ret.end()) {
        ret.push_back(&resource);
      }
    }

    // mark this as visited
    visitedResetables.push_back(this);

    // add all resources of childrens
    for (vector<ResetableItf *>::const_iterator resetableIt = mDependencies.begin() ;
         resetableIt != mDependencies.end() ;
         ++resetableIt) {
      ResetableItf &resetable = **resetableIt;

      resetable.allResources(ret, visitedResetables);
    }
  }
}

/**
 * \brief Specify an new accessor thread.
 *
 * 
 */
void ResetableItf::addTraversalThread(Thread &thrd) {
  // check if thrd is already in the traversal threads
  if (find(mTraversalThreads.begin(),
           mTraversalThreads.end(),
           &thrd)
      == mTraversalThreads.end()) {
    // add thrd to the traversal threads
    mTraversalThreads.push_back(&thrd);

    // check thread-safety
    if (mTraversalThreads.size() > 1) {
      mIsThreadSafe = false;
    }

    // propagate thrd to dependencies
    for (vector<ResetableItf *>::const_iterator resetableIt = mDependencies.begin() ;
         resetableIt != mDependencies.end() ;
         ++resetableIt) {
      ResetableItf &resetable = **resetableIt;

      resetable.addTraversalThread(thrd);
    }
  }
}

/**
 * \brief Specify a list of accessor threads.
 *
 */
void ResetableItf::addTraversalThreads(const vector<Thread *> &threads) {
  for (vector<Thread *>::const_iterator thrdIt = threads.begin() ;
       thrdIt != threads.end() ;
       ++thrdIt) {
    Thread &thrd = **thrdIt;

    addTraversalThread(thrd);
  }
}

/**
 * \brief Check that all dependent are reset
 *
 */
void ResetableItf::checkAllDependent() const {
  vector<const ResetableItf *> visited;
}

void ResetableItf::checkAllDependent(vector<const ResetableItf *> &visited) const {
  if (find(visited.begin(),
           visited.end(),
           this)
      == visited.end()) {  
    assert(isReset());

    visited.push_back(this);
  
    for (vector<ResetableItf *>::const_iterator resetableIt = mDependent.begin() ;
         resetableIt != mDependent.end() ;
         ++resetableIt) {
      const ResetableItf &resetable = **resetableIt;

      resetable.checkAllDependent(visited);
    }
  }
}

void ResetableItf::checkAllDependencies() const {
  vector<const ResetableItf *> visited;
}

void ResetableItf::checkAllDependencies(vector<const ResetableItf *> &visited) const {  
  if (find(visited.begin(),
           visited.end(),
           this)
      == visited.end()) {  
    assert(!isReset());

    visited.push_back(this);
  
    for (vector<ResetableItf *>::const_iterator resetableIt = mDependencies.begin() ;
         resetableIt != mDependencies.end() ;
         ++resetableIt) {
      const ResetableItf &resetable = **resetableIt;

      resetable.checkAllDependencies(visited);
    }
  }
}

void ResetableItf::initialize() {
  // 
  vector<ResetableItf *> dependent;

  allDependent(dependent);

  assert(!dependent.empty());
  
  mHasEmptyResetDependent = true;
  
  for (vector<ResetableItf *>::const_iterator resetableIt = dependent.begin() ;
       resetableIt != dependent.end() ;
       ++resetableIt) {
    ResetableItf &resetable = **resetableIt;

    if (&resetable != this) {
      mResetDependentGroup.addToSupport(resetable);

      mHasEmptyResetDependent = false;
    }
    
    mResetGroup.addToSupport(resetable);
  }

  mIsResetGroup.addToSupport(*this);
}

void ResetableItf::allConnected(vector<ResetableItf *> &visited) {
  if (find(visited.begin(),
           visited.end(),
           this)
      == visited.end()) {
    visited.push_back(this);

    for (vector<ResetableItf *>::const_iterator resetableIt = mDependent.begin() ;
         resetableIt != mDependent.end() ;
         ++resetableIt) {
      ResetableItf &resetable = **resetableIt;

      resetable.allConnected(visited);
    }
    
    for (vector<ResetableItf *>::const_iterator resetableIt = mDependencies.begin() ;
         resetableIt != mDependencies.end() ;
         ++resetableIt) {
      ResetableItf &resetable = **resetableIt;

      resetable.allConnected(visited);
    }
  }
}

void ResetableItf::allDependent(vector<ResetableItf *> &visited) {
  if (find(visited.begin(), 
           visited.end(),
           this)
      == visited.end()) {
    visited.push_back(this);

    for (vector<ResetableItf *>::const_iterator resetableIt = mDependent.begin() ;
         resetableIt != mDependent.end() ;
         ++resetableIt) {
      ResetableItf &resetable = **resetableIt;

      resetable.allDependent(visited);
    }
  }
}

AtomicGroup<ResetableItf> ResetableItf::mIsResetGroup(false);
