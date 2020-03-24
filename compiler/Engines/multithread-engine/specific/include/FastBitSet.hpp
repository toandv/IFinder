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

#ifndef _BIP_Engine_FastBitSet_HPP_
#define _BIP_Engine_FastBitSet_HPP_

#include <bip-engineiface-config.hpp>
using namespace bipbasetypes;
using namespace biptypes;

#include <ostream>


class FastBitSet {
 public:  
  // constructors
  FastBitSet(size_t size);
  FastBitSet(const FastBitSet &bitSet);

  // destructor
  virtual ~FastBitSet();

  // getters
  size_t size() const { return mSize; }
  bool test(size_t position) const;
  bool test(const FastBitSet &bitSet) const;
  bool any() const;
  bool any(const FastBitSet &bitSet) const;
  
  class const_iterator;

  const_iterator begin() const;
  const const_iterator &end() const { return mEnd; }

  // setters
  void set(size_t position) { set(index(position), mask(position)); }
  void reset(size_t position) { reset(index(position), mask(position)); }

  void set(const FastBitSet &bitSet);
  void reset(const FastBitSet &bitSet);

  void reset();

  FastBitSet &operator=(const FastBitSet &bitSet);
  FastBitSet &operator|=(const FastBitSet &bitSet) { set(bitSet); return *this; }
  FastBitSet &operator&=(const FastBitSet &bitSet);
  FastBitSet &operator-=(const FastBitSet &bitSet) { reset(bitSet); return *this; }

  FastBitSet operator|(const FastBitSet &bitSet) { FastBitSet ret(*this); ret |= bitSet; return ret; }
  FastBitSet operator&(const FastBitSet &bitSet) { FastBitSet ret(*this); ret &= bitSet; return ret; }
  FastBitSet operator-(const FastBitSet &bitSet) { FastBitSet ret(*this); ret -= bitSet; return ret; }

  friend std::ostream &operator<<(std::ostream &o, const FastBitSet &bitSet);
  friend class const_iterator;
  friend class SparseBitSet;

  class const_iterator {
   public:
    const_iterator() : mPosition(0), mIndex(0), mMask(0), mBitSet(NULL) { }
    ~const_iterator() {}

    // getters
    const size_t position() const { return mPosition; }

    // setters
    const_iterator &operator++();
    bool operator==(const const_iterator &it) const;
    bool operator!=(const const_iterator &it) const { return !(*this == it); }

    friend class FastBitSet;

   protected:
    const_iterator(size_t position, size_t index, unsigned int mask, const FastBitSet *bitSet);
    
    const FastBitSet &bitSet() const { return *mBitSet; }
    
    size_t mPosition;
    size_t mIndex;
    unsigned int mMask;
    const FastBitSet *mBitSet;
  };
 
 protected:
  // protected getters
  bool test(size_t index, unsigned int mask) const;
  bool any(size_t index, unsigned int mask) const;

  // protected setters
  void set(size_t index, unsigned int mask);
  void reset(size_t index, unsigned int mask);

  size_t position(size_t index) const { return index * (sizeof(unsigned int)*8); }
  size_t index(size_t position) const { return position / (sizeof(unsigned int)*8); }
  size_t subIndex(size_t position) const { return position % (sizeof(unsigned int)*8); }
  unsigned int mask(size_t position) const { return 1 << (subIndex(position)); }

  size_t mSize;
  size_t mCapacity;
  unsigned int *mBits;

  const_iterator mEnd;
};

inline
FastBitSet::FastBitSet(size_t size) :
  mSize(size),
  mCapacity(1 + size / (sizeof(unsigned int)*8)),
  mEnd(size, mCapacity, 0, this) {
  // allocate arrays
  mBits = new unsigned int[mCapacity];

  // initialize to empty
  for (size_t i = 0 ; i < mCapacity ; ++i) {
    mBits[i] = 0;
  }
}

inline
FastBitSet::FastBitSet(const FastBitSet &bitSet) :
  mSize(bitSet.mSize),
  mCapacity(bitSet.mCapacity),
  mEnd(bitSet.mSize, mCapacity, 0, this) {
  // allocate arrays
  mBits = new unsigned int[mCapacity];

  // initialize to empty
  for (size_t i = 0 ; i < mCapacity ; ++i) {
    unsigned int mask = bitSet.mBits[i];
    mBits[i] = mask;
  }
}

inline
FastBitSet::~FastBitSet() {
  delete[] mBits;
}

inline
bool FastBitSet::test(size_t position) const {
  return test(index(position), mask(position));
}

inline
bool FastBitSet::test(const FastBitSet &bitSet) const {
  assert(mSize == bitSet.mSize);
  assert(mCapacity == bitSet.mCapacity);
  
  size_t i = 0;
  bool ret = true;

  for (i = 0 ; i < mCapacity ; ++i) {
    if (!test(i, bitSet.mBits[i])) {
      ret = false;
      break;
    }
  }

  return ret;
}

inline
bool FastBitSet::any() const {
  size_t i = 0;
  bool ret = false;
  unsigned int mask = ~(0);

  for (i = 0 ; i < mCapacity ; ++i) {
    if (any(i, mask)) {
      ret = true;
      break;
    }
  }

  return ret;
}

inline
bool FastBitSet::any(const FastBitSet &bitSet) const {
  assert(mSize == bitSet.mSize);
  assert(mCapacity == bitSet.mCapacity);
  
  size_t i = 0;
  bool ret = false;

  for (i = 0 ; i < mCapacity ; ++i) {
    if (any(i, bitSet.mBits[i])) {
      ret = true;
      break;
    }
  }

  return ret;
}

inline
FastBitSet::const_iterator FastBitSet::begin() const {
  const_iterator it(0, 0, mBits[0], this);

  if (!test(0)) {
    ++it;
  }

  return it;
}

inline
void FastBitSet::set(const FastBitSet &bitSet) {
  assert(mSize == bitSet.mSize);
  assert(mCapacity == bitSet.mCapacity);
      
  for (size_t i = 0 ; i < mCapacity ; ++i) {
    set(i, bitSet.mBits[i]);
  }
}

inline
void FastBitSet::reset(const FastBitSet &bitSet) {
  assert(mSize == bitSet.mSize);
  assert(mCapacity == bitSet.mCapacity);
      
  for (size_t i = 0 ; i < mCapacity ; ++i) {
    reset(i, bitSet.mBits[i]);
  }
}

inline
void FastBitSet::reset() {      
  for (size_t i = 0 ; i < mCapacity ; ++i) {
    mBits[i] = 0;
  }
}

inline
FastBitSet &FastBitSet::operator=(const FastBitSet &bitSet) {
  assert(mSize == bitSet.mSize);
  assert(mCapacity == bitSet.mCapacity);
      
  for (size_t i = 0 ; i < mCapacity ; ++i) {
    mBits[i] = bitSet.mBits[i];
  }

  return *this;
}

inline
FastBitSet &FastBitSet::operator&=(const FastBitSet &bitSet) {
  assert(mSize == bitSet.mSize);
  assert(mCapacity == bitSet.mCapacity);
      
  for (size_t i = 0 ; i < mCapacity ; ++i) {
    mBits[i] &= bitSet.mBits[i];
  }

  return *this;
}

inline
bool FastBitSet::test(size_t index, unsigned int mask) const {
  return (mBits[index] & mask) == mask;
}

inline
bool FastBitSet::any(size_t index, unsigned int mask) const {
  return (mBits[index] & mask) != 0;
}

inline
void FastBitSet::set(size_t index, unsigned int mask) {
  mBits[index] |= mask;
}

inline
void FastBitSet::reset(size_t index, unsigned int mask) {
  mBits[index] &= ~mask;
}

inline
FastBitSet::const_iterator::const_iterator(size_t position, size_t index, unsigned int mask, const FastBitSet *bitSet) :
  mPosition(position),
  mIndex(index),
  mMask(mask),
  mBitSet(bitSet) {
}

inline
FastBitSet::const_iterator &FastBitSet::const_iterator::operator++() {
#ifndef NDEBUG
  size_t p = mPosition;
#endif

  // consume the current bit
  ++mPosition;
  mMask >>= 1;

  if (mMask != 0) {
    size_t shift = __builtin_ctz(mMask);

    mMask >>= shift;

    mPosition += shift;
  }
  else {
    ++mIndex;

    while (mIndex < bitSet().mCapacity) {
      if (bitSet().mBits[mIndex] != 0) {
        break;
      }

      ++mIndex;
    }

    if (mIndex < bitSet().mCapacity) {
      assert(bitSet().mBits[mIndex] != 0);

      mMask = bitSet().mBits[mIndex];

      size_t shift = __builtin_ctz(mMask);

      mMask >>= shift;

      mPosition = bitSet().position(mIndex) + shift;
    }
    else {
      mPosition = bitSet().size();
      
      assert(mIndex == bitSet().mCapacity);
      assert(mMask == 0);
    }
  }
  
#ifndef NDEBUG
  for (size_t j = p + 1 ; j < mPosition ; ++j) {
    assert(!bitSet().test(j));
  }

  if (mPosition < bitSet().size()) {
    assert(bitSet().test(mPosition));
  }
#endif

  return *this;
}

inline
bool FastBitSet::const_iterator::operator==(const const_iterator &it) const {
  assert(mBitSet == it.mBitSet);

  return mPosition == it.mPosition;
}

inline
std::ostream &operator<<(std::ostream &o, const FastBitSet &bitSet) {
  for (size_t i = 0 ; i < bitSet.mCapacity ; ++i) {
    unsigned int positions = bitSet.mBits[i];

    for (unsigned int j = 0 ; j < sizeof(unsigned int)*8 ; ++j) {
      if (positions & 1) o << "1";
      else o << "0";

      positions = positions >> 1;
	}
    o << "-";
  }

  return o;
}

class SparseBitSet : public FastBitSet {
 public:
  // constructors
  SparseBitSet(size_t size);
  SparseBitSet(const SparseBitSet &bitSet);
  SparseBitSet(const FastBitSet &bitSet);

  // destructor
  virtual ~SparseBitSet();

  // getters
  size_t size() const { return mSize; }
  bool test(size_t position) const { return FastBitSet::test(position); }
  bool test(const SparseBitSet &bitSet) const;
  bool any() const;
  bool any(const SparseBitSet &bitSet) const;
  
  class const_iterator;

  const_iterator begin() const;
  const const_iterator &end() const { return mEnd; }

  // setters
  void set(size_t position) { set(index(position), mask(position)); }
  void reset(size_t position) { reset(index(position), mask(position)); }
  
  void set(const SparseBitSet &bitSet);
  void reset(const SparseBitSet &bitSet);

  void reset();

  SparseBitSet &operator=(const SparseBitSet &bitSet);
  SparseBitSet &operator|=(const SparseBitSet &bitSet) { set(bitSet); return *this; }
  SparseBitSet &operator&=(const SparseBitSet &bitSet);
  SparseBitSet &operator-=(const SparseBitSet &bitSet) { reset(bitSet); return *this; }

  SparseBitSet operator|(const SparseBitSet &bitSet) { SparseBitSet ret(*this); ret |= bitSet; return ret; }
  SparseBitSet operator&(const SparseBitSet &bitSet) { SparseBitSet ret(*this); ret &= bitSet; return ret; }
  SparseBitSet operator-(const SparseBitSet &bitSet) { SparseBitSet ret(*this); ret -= bitSet; return ret; }

  friend class AtomicBitSet;
  
  class const_iterator {
   public:
    const_iterator() : mPosition(0), mMask(0), mBitSet(NULL) { }
    ~const_iterator() {}

    // getters
    const size_t position() const { return mPosition; }

    // setters
    const_iterator &operator++();
    bool operator==(const const_iterator &it) const;
    bool operator!=(const const_iterator &it) const { return !(*this == it); }

    friend class SparseBitSet;

   protected:
    const_iterator(size_t position, const FastBitSet::const_iterator &it, unsigned int mask, const SparseBitSet *bitSet);
    
    const SparseBitSet &bitSet() const { return *mBitSet; }
    
    size_t mPosition;
    FastBitSet::const_iterator mIt;
    unsigned int mMask;
    const SparseBitSet *mBitSet;
  };

 protected:
  // protected getters
  bool hasWellFormedIndexes() const;
  
  // protected setters
  void set(size_t index, unsigned int mask);
  void reset(size_t index, unsigned int mask);
  
  FastBitSet mIndexes;

  const_iterator mEnd;
};

inline
SparseBitSet::SparseBitSet(size_t size) :
  FastBitSet(size),
  mIndexes(1 + size / (sizeof(unsigned int)*8)),
  mEnd(size, mIndexes.end(), 0, this) {
  assert(hasWellFormedIndexes());
}

inline
SparseBitSet::SparseBitSet(const SparseBitSet &bitSet) :
  FastBitSet(bitSet.size()),
  mIndexes(bitSet.mIndexes),
  mEnd(bitSet.size(), mIndexes.end(), 0, this) {
  for (FastBitSet::const_iterator it = bitSet.mIndexes.begin() ;
       it != bitSet.mIndexes.end() ;
       ++it) {
    size_t i = it.position();
    
    mBits[i] = bitSet.mBits[i];
  }
  
  assert(hasWellFormedIndexes());
}

inline
SparseBitSet::SparseBitSet(const FastBitSet &bitSet) :
  FastBitSet(bitSet.size()),
  mIndexes(1 + bitSet.size() / (sizeof(unsigned int)*8)),
  mEnd(bitSet.size(), mIndexes.end(), 0, this) {
  for (size_t i = 0 ; i < mIndexes.size() ; ++i) {
    mBits[i] = bitSet.mBits[i];

    if (mBits[i] == 0) {
      mIndexes.set(i);
    }
  }
  
  assert(hasWellFormedIndexes());
}

inline
SparseBitSet::~SparseBitSet() {
}

inline
bool SparseBitSet::test(const SparseBitSet &bitSet) const {
  assert(mSize == bitSet.mSize);
  assert(mCapacity == bitSet.mCapacity);
  
  bool ret = true;

  for (FastBitSet::const_iterator it = mIndexes.begin() ;
       it != mIndexes.end() ;
       ++it) {
    size_t i = it.position();
    
    if (!FastBitSet::test(i, bitSet.mBits[i])) {
      ret = false;
      break;
    }
  }

  return ret;
}

inline
bool SparseBitSet::any() const {
  bool ret = false;
  unsigned int mask = ~(0);

  for (FastBitSet::const_iterator it = mIndexes.begin() ;
       it != mIndexes.end() ;
       ++it) {
    size_t i = it.position();
    
    if (FastBitSet::any(i, mask)) {
      ret = true;
      break;
    }
  }

  return ret;
}

inline
bool SparseBitSet::any(const SparseBitSet &bitSet) const {
  assert(mSize == bitSet.mSize);
  assert(mCapacity == bitSet.mCapacity);
  
  bool ret = false;
      
  for (FastBitSet::const_iterator it = mIndexes.begin() ;
       it != mIndexes.end() ;
       ++it) {
    size_t i = it.position();
    
    if (FastBitSet::any(i, bitSet.mBits[i])) {
      ret = true;
      break;
    }
  }

  return ret;
}

inline
SparseBitSet::const_iterator SparseBitSet::begin() const {
  FastBitSet::const_iterator it = mIndexes.begin();

  if (it != mIndexes.end()) {
    size_t index = it.position();
    size_t pos = position(index);

    const_iterator ret(pos, it, mBits[index], this);

    if (!test(pos)) {
      ++ret;
    }

    return ret;
  }

  return end();
}

inline
void SparseBitSet::set(const SparseBitSet &bitSet) {
  assert(mSize == bitSet.mSize);
  assert(mCapacity == bitSet.mCapacity);

  mIndexes |= bitSet.mIndexes;
  
  for (FastBitSet::const_iterator it = bitSet.mIndexes.begin() ;
       it != bitSet.mIndexes.end() ;
       ++it) {
    size_t i = it.position();
    
    mBits[i] |= bitSet.mBits[i];
  }
  
  assert(hasWellFormedIndexes());
}

inline
void SparseBitSet::reset(const SparseBitSet &bitSet) {
  assert(mSize == bitSet.mSize);
  assert(mCapacity == bitSet.mCapacity);
      
  for (FastBitSet::const_iterator it = bitSet.mIndexes.begin() ;
       it != bitSet.mIndexes.end() ;
       ++it) {
    size_t i = it.position();
    
    reset(i, bitSet.mBits[i]);
  }
  
  assert(hasWellFormedIndexes());
}

inline
void SparseBitSet::reset() {
  for (FastBitSet::const_iterator it = mIndexes.begin() ;
       it != mIndexes.end() ;
       ++it) {
    size_t i = it.position();
    
    mBits[i] = 0;
  }

  mIndexes.reset();
  
  assert(hasWellFormedIndexes());
}

inline
SparseBitSet &SparseBitSet::operator=(const SparseBitSet &bitSet) {
  assert(mSize == bitSet.mSize);
  assert(mCapacity == bitSet.mCapacity);

  FastBitSet::const_iterator it1 = mIndexes.begin();
  FastBitSet::const_iterator it2 = bitSet.mIndexes.begin();
  
  while (it2 != bitSet.mIndexes.end()) {    
    while (it1.position() < it2.position()) {
      assert(it1.position() < mIndexes.size());

      mBits[it1.position()] = 0;
      
      ++it1;
    }

    mBits[it2.position()] = bitSet.mBits[it2.position()];
    
    if (it1.position() == it2.position()) {
      ++it1;
    }
    
    ++it2;
  }

  while (it1 != mIndexes.end()) {
    mBits[it1.position()] = 0;
    
    ++it1;
  }
  
  mIndexes = bitSet.mIndexes;

#ifndef NDEBUG
  for (size_t i = 0 ; i < mIndexes.size() ; ++i) {
    assert(mIndexes.test(i) == bitSet.mIndexes.test(i));
    assert(mBits[i] == bitSet.mBits[i]);
  }
#endif
  
  assert(hasWellFormedIndexes());

  return *this;
}

inline
SparseBitSet &SparseBitSet::operator&=(const SparseBitSet &bitSet) {
  assert(mSize == bitSet.mSize);
  assert(mCapacity == bitSet.mCapacity);

  FastBitSet::const_iterator it1 = mIndexes.begin();
  FastBitSet::const_iterator it2 = bitSet.mIndexes.begin();

  while (it1 != mIndexes.end()) {
    while (it2.position() < it1.position()) {
      assert(it2.position() < bitSet.mIndexes.size());
      
      ++it2;
    }

    if (it1.position() == it2.position()) {
      mBits[it1.position()] &= bitSet.mBits[it1.position()];

      if (mBits[it1.position()] == 0) {
        mIndexes.reset(it1.position());
      }
    }
    else if (it1.position() < it2.position()) {
      mBits[it1.position()] = 0;
    }
    else {
      assert(false);
    }

    ++it1;
  }

  mIndexes &= bitSet.mIndexes;
  
#ifndef NDEBUG
  for (size_t i = 0 ; i < size() ; ++i) {
    assert(!test(i) || bitSet.test(i));
  }
#endif

  assert(hasWellFormedIndexes());

  return *this;
}

inline
bool SparseBitSet::hasWellFormedIndexes() const {
  bool ret = true;
  
  for (size_t i = 0 ; i < mCapacity ; ++i) {
    // i set <=> mBits[i] != 0
    if (mIndexes.test(i) != (mBits[i] != 0)) {
      ret = false;
    }
  }

  if (!ret) {
    cout << mIndexes << ": " << *this << endl;
  }

  return ret;
}

inline
void SparseBitSet::set(size_t index, unsigned int mask) {
  mBits[index] |= mask;

  if (mask != 0) {
    mIndexes.set(index);
  }

  assert(hasWellFormedIndexes());
}

inline
void SparseBitSet::reset(size_t index, unsigned int mask) {
  mBits[index] &= ~mask;

  if (mask != 0) {
    if (mBits[index] == 0) {      
      mIndexes.reset(index);
    }
  }
  
  assert(hasWellFormedIndexes());
}

inline
SparseBitSet::const_iterator::const_iterator(size_t position, const FastBitSet::const_iterator &it, unsigned int mask, const SparseBitSet *bitSet) :
  mPosition(position),
  mIt(it),
  mMask(mask),
  mBitSet(bitSet) {
}

inline
SparseBitSet::const_iterator &SparseBitSet::const_iterator::operator++() {
#ifndef NDEBUG
  size_t p = mPosition;
#endif

  // consume the current bit
  ++mPosition;
  mMask >>= 1;

  if (mMask != 0) {
    size_t shift = __builtin_ctz(mMask);

    mMask >>= shift;

    mPosition += shift;
  }
  else {
    ++mIt;

    if (mIt != bitSet().mIndexes.end()) {
      size_t index = mIt.position();
      
      mMask = bitSet().mBits[index];

      size_t shift = __builtin_ctz(mMask);

      mMask >>= shift;

      mPosition = bitSet().position(index) + shift;
    }
    else {
      mPosition = bitSet().size();

      assert(mIt == bitSet().mIndexes.end());
      assert(mMask == 0);
    }
  }
  
#ifndef NDEBUG
  for (size_t j = p + 1 ; j < mPosition ; ++j) {
    assert(!bitSet().test(j));
  }

  if (mPosition < bitSet().size()) {
    assert(bitSet().test(mPosition));
  }
#endif

  return *this;
}

inline
bool SparseBitSet::const_iterator::operator==(const const_iterator &it) const {
  assert(mBitSet == it.mBitSet);

  return mPosition == it.mPosition;
}

class AtomicBitSet {
 public:
  // constructors
  AtomicBitSet(size_t size);

  // destructor
  virtual ~AtomicBitSet();

  // getters
  size_t size() const { return mSize; }
  bool test(size_t position) const;
  bool test(const SparseBitSet &bitSet) const;
  bool any() const;
  bool any(const SparseBitSet &bitSet) const;

  // setters
  bool set(size_t position);
  bool reset(size_t position);
  bool testAndSet(size_t position);
  void testAndReset(size_t position);

  void set(const SparseBitSet &bitSet);
  void set(const SparseBitSet &bitSet, SparseBitSet &success);
  void reset(const SparseBitSet &bitSet);
  void reset(const SparseBitSet &bitSet, SparseBitSet &success);
  bool testAndSet(const SparseBitSet &bitSet);
  void testAndReset(const SparseBitSet &bitSet);

  friend std::ostream &operator<<(std::ostream &o, const AtomicBitSet &bitSet);
 
 protected:
  // protected getters
  bool test(size_t index, unsigned int mask) const;
  bool any(size_t index, unsigned int mask) const;

  // protected setters
  unsigned int set(size_t index, unsigned int mask);
  unsigned int reset(size_t index, unsigned int mask);
  bool testAndSet(size_t index, unsigned int mask);
  void testAndReset(size_t index, unsigned int mask);

  size_t mSize;
  size_t mCapacity;
  atomic<unsigned int> *mBits;
};

inline
AtomicBitSet::AtomicBitSet(size_t size) :
  mSize(size),
  mCapacity(1 + size / (sizeof(unsigned int)*8)) {
  // allocate arrays
  mBits = new atomic<unsigned int>[mCapacity];

  // initialize to empty
  for (size_t i = 0 ; i < mCapacity ; ++i) {
    mBits[i] = 0;
  }
}

inline
AtomicBitSet::~AtomicBitSet() {
  delete[] mBits;
}

inline
bool AtomicBitSet::test(size_t position) const {
  size_t index = position / (sizeof(unsigned int)*8);
  unsigned int mask = 1 << (position % (sizeof(unsigned int)*8));

  return test(index, mask);
}

inline
bool AtomicBitSet::test(const SparseBitSet &bitSet) const {
  assert(mSize == bitSet.mSize);
  assert(mCapacity == bitSet.mCapacity);
  
  bool ret = true;

  for (FastBitSet::const_iterator it = bitSet.mIndexes.begin() ;
       it != bitSet.mIndexes.end() ;
       ++it) {
    size_t i = it.position();
    
    if (!test(i, bitSet.mBits[i])) {
      ret = false;
      break;
    }
  }

  return ret;
}

inline
bool AtomicBitSet::any() const {
  size_t i = 0;
  bool ret = false;
  unsigned int mask = ~(0);

  for (i = 0 ; i < mCapacity ; ++i) {
    if (any(i, mask)) {
      ret = true;
      break;
    }
  }

  return ret;
}

inline
bool AtomicBitSet::any(const SparseBitSet &bitSet) const {
  assert(mSize == bitSet.mSize);
  assert(mCapacity == bitSet.mCapacity);
  
  bool ret = false;

  for (FastBitSet::const_iterator it = bitSet.mIndexes.begin() ;
       it != bitSet.mIndexes.end() ;
       ++it) {
    size_t i = it.position();
    
    if (any(i, bitSet.mBits[i])) {
      ret = true;
      break;
    }
  }

  return ret;
}

inline
bool AtomicBitSet::set(size_t position) {
  size_t index = position / (sizeof(unsigned int)*8);
  unsigned int mask = 1 << (position % (sizeof(unsigned int)*8));

  unsigned int oldBits = mBits[index].fetch_or(mask);
  unsigned int newBits = (~oldBits) & mask;

  return newBits == mask;
}

inline
bool AtomicBitSet::reset(size_t position) {
  size_t index = position / (sizeof(unsigned int)*8);
  unsigned int mask = 1 << (position % (sizeof(unsigned int)*8));

  unsigned int oldBits = mBits[index].fetch_and(~mask);
  unsigned int newBits = (~oldBits) & mask;
  
  return newBits == 0;
}

inline
bool AtomicBitSet::testAndSet(size_t position) {
  size_t index = position / (sizeof(unsigned int)*8);
  unsigned int mask = 1 << (position % (sizeof(unsigned int)*8));

  return testAndSet(index, mask);
}

inline
void AtomicBitSet::testAndReset(size_t position) {
  size_t index = position / (sizeof(unsigned int)*8);
  unsigned int mask = 1 << (position % (sizeof(unsigned int)*8));

  testAndReset(index, mask);
}

inline
void AtomicBitSet::set(const SparseBitSet &bitSet) {
  assert(mSize == bitSet.mSize);
  assert(mCapacity == bitSet.mCapacity);

  for (FastBitSet::const_iterator it = bitSet.mIndexes.begin() ;
       it != bitSet.mIndexes.end() ;
       ++it) {
    size_t i = it.position();

    set(i, bitSet.mBits[i]);
  }
}
inline
void AtomicBitSet::set(const SparseBitSet &bitSet, SparseBitSet &successBitSet) {
  assert(mSize == bitSet.mSize);
  assert(mCapacity == bitSet.mCapacity);

  for (FastBitSet::const_iterator it = bitSet.mIndexes.begin() ;
       it != bitSet.mIndexes.end() ;
       ++it) {
    size_t i = it.position();

    unsigned int success = set(i, bitSet.mBits[i]);

    successBitSet.set(i, success);
  }
}

inline
void AtomicBitSet::reset(const SparseBitSet &bitSet) {
  assert(mSize == bitSet.mSize);
  assert(mCapacity == bitSet.mCapacity);

  for (FastBitSet::const_iterator it = bitSet.mIndexes.begin() ;
       it != bitSet.mIndexes.end() ;
       ++it) {
    size_t i = it.position();

    reset(i, bitSet.mBits[i]);
  }
}

inline
void AtomicBitSet::reset(const SparseBitSet &bitSet, SparseBitSet &successBitSet) {
  assert(mSize == bitSet.mSize);
  assert(mCapacity == bitSet.mCapacity);

  for (FastBitSet::const_iterator it = bitSet.mIndexes.begin() ;
       it != bitSet.mIndexes.end() ;
       ++it) {
    size_t i = it.position();

    unsigned int success = reset(i, bitSet.mBits[i]);

    successBitSet.set(i, success);
  }
}

inline
bool AtomicBitSet::testAndSet(const SparseBitSet &bitSet) {
  assert(mSize == bitSet.mSize);
  assert(mCapacity == bitSet.mCapacity);
  
  bool success = true;

  FastBitSet::const_iterator it = bitSet.mIndexes.begin();

  while (it != bitSet.mIndexes.end()) {
    size_t i = it.position();
    
    success = testAndSet(i, bitSet.mBits[i]);

    if (!success) {
      break;
    }
    
    ++it;
  }
  
  if (!success) {
    for (FastBitSet::const_iterator itReset = bitSet.mIndexes.begin() ;
         itReset != it ;
         ++itReset) {
      size_t i = itReset.position();
    
      testAndReset(i, bitSet.mBits[i]);
    }
  }

  return success;
}

inline
void AtomicBitSet::testAndReset(const SparseBitSet &bitSet) {
  assert(size() == bitSet.size());
  
  for (FastBitSet::const_iterator it = bitSet.mIndexes.begin() ;
       it != bitSet.mIndexes.end() ;
       ++it) {
    size_t i = it.position();
    
    testAndReset(i, bitSet.mBits[i]);
  }
}

inline
bool AtomicBitSet::test(size_t index, unsigned int mask) const {
  return (mBits[index] & mask) == mask;
}

inline
bool AtomicBitSet::any(size_t index, unsigned int mask) const {
  return (mBits[index] & mask) != 0;
}

inline
unsigned int AtomicBitSet::set(size_t index, unsigned int mask) {  
  unsigned int oldBits = mBits[index].fetch_or(mask);
  unsigned int newBits = (~oldBits) & mask;
  
  return newBits;
}

inline
unsigned int AtomicBitSet::reset(size_t index, unsigned int mask) {
  unsigned int oldBits = mBits[index].fetch_and(~mask);
  unsigned int removedBits = oldBits & mask;

  return removedBits;
}

inline
void AtomicBitSet::testAndReset(size_t index, unsigned int mask) {
  // reset bits
#ifdef NDEBUG
  mBits[index].fetch_and(~mask);
#else
  unsigned int oldBits = mBits[index].fetch_and(~mask);
#endif

  // 
  assert(((~oldBits) & mask) == 0);
}

inline
bool AtomicBitSet::testAndSet(size_t index, unsigned int mask) {
  bool success = false;

  if (!(mBits[index] & mask)) {
    unsigned int oldBits = mBits[index].fetch_or(mask);
    unsigned int newBits = (~oldBits) & mask;

    success = (newBits == mask);

    if (!success && newBits != 0) {
      testAndReset(index, newBits);
    }
  }

  return success;
}

inline
std::ostream &operator<<(std::ostream &o, const AtomicBitSet &bitSet) {
  for (size_t i = 0 ; i < bitSet.mCapacity ; ++i) {
    unsigned int positions = bitSet.mBits[i];

    for (unsigned int j = 0 ; j < sizeof(unsigned int)*8 ; ++j) {
      if (positions & 1) o << "1";
      else o << "0";

      positions = positions >> 1;
	}
    o << "-";
  }

  return o;
}

#endif // _BIP_Engine_FastBitSet_HPP_
