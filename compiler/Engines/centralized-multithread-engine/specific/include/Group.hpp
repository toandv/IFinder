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

#ifndef _BIP_Engine_Group_HPP_
#define _BIP_Engine_Group_HPP_

#include <bip-engineiface-config.hpp>
using namespace bipbasetypes;
using namespace biptypes;

#include <FastBitSet.hpp>

template <class C>
class AtomicGroup;

template<class C>
class GroupItf {
 public:
  GroupItf() : mInitialized(false), mInitializeToSupport(true) { }
  GroupItf(bool b) : mInitialized(false), mInitializeToSupport(b) { }
  ~GroupItf() { }

  void addToSupport(C &object);
  void addToSupport(const vector<C *> &objects);

  size_t size() const;
  size_t index(const C &object) const;
  C &object(size_t index) const;

  virtual void initialize() = 0;

  static void initializeAll();
  
 protected:
  bool initializeToSupport() const { return mInitializeToSupport; }
  void reg() { if (find(mAllGroups.begin(), mAllGroups.end(), this) == mAllGroups.end()) mAllGroups.push_back(this); }
  
  vector<C *> mSupport;
  static vector<C *> mCommonSupport;
  static vector<GroupItf<C> *> mAllGroups;
  
  bool mInitialized;
  bool mInitializeToSupport;
};

template<class C>
class Group : public GroupItf<C> {
 public:
  Group() : mMask(NULL) { }
  Group(bool b) : GroupItf<C>::GroupItf(b), mMask(NULL) { }
  ~Group() { delete mMask; }

  bool in(const C &object) const;
  bool in(const Group<C> &group) const;
  bool has(const Group<C> &group) const;
  
  class iterator;

  iterator begin() const { return iterator(mMask->begin(), this); }
  iterator end() const { return iterator(mMask->end(), this); }
  
  void add(const C &object);
  void add(const Group<C> &group);
  void remove(const C &object);
  void remove(const Group<C> &group);
  void intersect(const Group<C> &group);
  void clear();

  Group<C> &operator=(const Group<C> &group);
  
  virtual void initialize();

  class iterator {
   public:
    iterator() : mGroup(NULL) {}
    ~iterator() {}

    iterator &operator++() { ++mIt; return *this; }
    C &operator*() { return mGroup->object(mIt.position()); }

    bool operator==(const iterator &it) { return mIt == it.mIt; }
    bool operator!=(const iterator &it) { return mIt != it.mIt; }
      
    friend class Group<C>;
    
   protected:
    iterator(const SparseBitSet::const_iterator &it, const Group<C> *group) : mIt(it), mGroup(group) { }
    
    SparseBitSet::const_iterator mIt;
    const Group<C> *mGroup;
  };

  friend class AtomicGroup<C>;
  
 protected:
  bool mInitialized;
  SparseBitSet *mMask;
};

template<class C>
class AtomicGroup : public GroupItf<C> {
 public:
  AtomicGroup() : mMask(NULL) { }
  AtomicGroup(bool b) : GroupItf<C>::GroupItf(b), mMask(NULL) { }
  ~AtomicGroup() { delete mMask; }
  
  bool in(const C &object) const;
  bool in(const Group<C> &group) const;
  bool has(const Group<C> &group) const;
  
  void add(const C &object);
  void add(const Group<C> &group);
  void add(const Group<C> &group, Group<C> &success);
  bool testAndAdd(const Group<C> &group);
  void remove(const C &object);
  void remove(const Group<C> &group);
  void testAndRemove(const C &object);
  void testAndRemove(const Group<C> &group);
  void clear();

  virtual void initialize();
  
 protected:  
  bool mInitialized;
  AtomicBitSet *mMask;
};

template<class C>
inline
void GroupItf<C>::addToSupport(C &object) {
  reg();

  assert(!mInitialized);
  
  assert(find(mSupport.begin(),
               mSupport.end(),
               &object)
         == mSupport.end());

  mSupport.push_back(&object);

  if (find(mCommonSupport.begin(),
            mCommonSupport.end(),
            &object)
      == mCommonSupport.end()) {
    object.setIndex(mCommonSupport.size());
    mCommonSupport.push_back(&object);
  }
}

template<class C>
inline
void GroupItf<C>::addToSupport(const vector<C *> &objects) {
  for (typename vector<C *>::const_iterator it = objects.begin() ;
       it != objects.end() ;
       ++it) {
    C &object = **it;
    
    addToSupport(object);
  }
}

template<class C>
inline
size_t GroupItf<C>::size() const {
  return mCommonSupport.size();
}

template<class C>
inline
size_t GroupItf<C>::index(const C &object) const {
  return object.index();
}

template<class C>
inline
C &GroupItf<C>::object(size_t index) const {
  assert(index < mCommonSupport.size());

  return *mCommonSupport[index];
}

template<class C>
void GroupItf<C>::initializeAll() {
  for (typename vector<GroupItf<C> *>::const_iterator groupIt = mAllGroups.begin() ;
       groupIt != mAllGroups.end() ;
       ++groupIt) {
    GroupItf<C> &group = **groupIt;

    group.initialize();
  }
}

template<class C>
inline
bool Group<C>::in(const C &object) const {
  assert(mInitialized);

  return mMask->test(GroupItf<C>::index(object));
}

template<class C>
inline
bool Group<C>::in(const Group<C> &group) const {
  assert(mInitialized);

  return mMask->test(*group.mMask);
}

template<class C>
inline
bool Group<C>::has(const Group<C> &group) const {
  assert(mInitialized);

  return mMask->any(*group.mMask);
}

template<class C>
inline
void Group<C>::add(const C &object) {
  assert(mInitialized);

  mMask->set(GroupItf<C>::index(object));
}

template<class C>
inline
void Group<C>::add(const Group<C> &group) {
  assert(mInitialized);

  mMask->set(*group.mMask);
}

template<class C>
inline
void Group<C>::remove(const C &object) {
  assert(mInitialized);

  mMask->reset(GroupItf<C>::index(object));
}

template<class C>
inline
void Group<C>::remove(const Group<C> &group) {
  assert(mInitialized);

  mMask->reset(*group.mMask);
}


template<class C>
inline
void Group<C>::intersect(const Group<C> &group) {
  assert(mInitialized);

  *mMask &= *group.mMask;
}

template<class C>
inline
void Group<C>::clear() {
  assert(mInitialized);

  mMask->reset();
}

template<class C>
inline
Group<C> &Group<C>::operator=(const Group &group) {
  assert(mInitialized);

  *mMask = *group.mMask;

  return *this;
}

template<class C>
inline
void Group<C>::initialize() {
  assert(!mInitialized);

  mMask = new SparseBitSet(GroupItf<C>::size());

  if (GroupItf<C>::initializeToSupport()) {
    for (typename vector<C *>::const_iterator it = GroupItf<C>::mSupport.begin() ;
         it != GroupItf<C>::mSupport.end() ;
         ++it) {
      C &object = **it;

      mMask->set(GroupItf<C>::index(object));
    }
  }
  
  mInitialized = true;
}

template<class C>
inline
bool AtomicGroup<C>::in(const C &object) const {
  assert(mInitialized);

  return mMask->test(GroupItf<C>::index(object));
}

template<class C>
inline
bool AtomicGroup<C>::in(const Group<C> &group) const {
  assert(mInitialized);

  return mMask->test(*group.mMask);
}

template<class C>
inline
bool AtomicGroup<C>::has(const Group<C> &group) const {
  assert(mInitialized);

  return mMask->any(*group.mMask);
}

template<class C>
inline
void AtomicGroup<C>::add(const C &object) {
  assert(mInitialized);

  mMask->set(GroupItf<C>::index(object));
}

template<class C>
inline
void AtomicGroup<C>::add(const Group<C> &group) {
  assert(mInitialized);

  mMask->set(*group.mMask);
}

template<class C>
inline
void AtomicGroup<C>::add(const Group<C> &group, Group<C> &success) {
  assert(mInitialized);

  mMask->set(*group.mMask, *success.mMask);
}

template<class C>
inline
bool AtomicGroup<C>::testAndAdd(const Group<C> &group) {
  assert(mInitialized);

  return mMask->testAndSet(*group.mMask);
}

template<class C>
inline
void AtomicGroup<C>::remove(const C &object) {
  assert(mInitialized);

  mMask->reset(GroupItf<C>::index(object));
}

template<class C>
inline
void AtomicGroup<C>::remove(const Group<C> &group) {
  assert(mInitialized);

  mMask->reset(*group.mMask);
}

template<class C>
inline
void AtomicGroup<C>::testAndRemove(const C &object) {
  assert(mInitialized);

  mMask->testAndReset(GroupItf<C>::index(object));
}

template<class C>
inline
void AtomicGroup<C>::testAndRemove(const Group<C> &group) {
  assert(mInitialized);

  mMask->testAndReset(*group.mMask);
}

template<class C>
inline
void AtomicGroup<C>::initialize() {
  assert(!mInitialized);

  mMask = new AtomicBitSet(GroupItf<C>::size());

  if (GroupItf<C>::initializeToSupport()) {
    for (typename vector<C *>::const_iterator it = GroupItf<C>::mSupport.begin() ;
         it != GroupItf<C>::mSupport.end() ;
         ++it) {
      const C &object = **it;

      mMask->set(GroupItf<C>::index(object));
    }
  }
  
  mInitialized = true;
}

template<class C>
vector<C *> GroupItf<C>::mCommonSupport;

template<class C>
vector<GroupItf<C> *> GroupItf<C>::mAllGroups;

#endif // _BIP_Engine_Group_HPP_
