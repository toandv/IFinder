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

#ifndef _BIP_Engine_Resource_HPP_
#define _BIP_Engine_Resource_HPP_

#include "FastMutex.hpp"
#include "Interval.hpp"
#include "TimeValue.hpp"
#include "RealTimeClock.hpp"
#include "Group.hpp"

class Job;

class Reservable {
 public:
  size_t index() const { return mIndex; }
  void setIndex(size_t index) const { mIndex = index; }

 protected:
  static AtomicGroup<Reservable> mIsReservedGroup;
  
  mutable size_t mIndex;
};

class Resource : public Reservable {
 public:
  class Accessor;
  class Writer;
  class Validator;

  enum Status { FREE, RESERVED, USED };

  // constructors
  Resource() : mStatus(FREE), mNbOfValidation(0), mDebug(false) { }
  Resource(bool debug) : mStatus(FREE), mNbOfValidation(0), mDebug(debug) { }

  // destructor
  virtual ~Resource() { }

  // getters/setters
  Status status() const { return mStatus.load(); }
  void setStatus(Status status);
  TimeValue time() const { return mTime; }
  TimeValue freeTime() const { return mFreeTime; }
  TimeValue deadline() const;
  bool debug() const { return mDebug; }
  bool isValid() const { return mNbOfValidation.load() == 0; }

  // operations
  void initialize();
  void use();
  void free();
  // void unvalidate();
  // void validate();

  class Reserver {
   public:
    Reserver() { }
    virtual ~Reserver() { }

    const vector<Resource *> &resources() const { return mResources; }
    const vector<Resource *> &observedResources() const { return mObservedResources; }
    bool isValid() const;
    TimeValue deadline() const;
    
    void addResource(Resource &resource);
    void addResources(const vector<Resource *> &resources);
    void addObservedResource(Resource &resource);
    void addObservedResources(const vector<Resource *> &resources);
    void initialize();

    const vector<Validator *> &validators() const { return mValidators; } 
    
    bool tryToReserve(const TimeValue &time);
    bool tryToReserve(const TimeValue &time, bool relaxed);
    bool isReserved() const { return mIsReservedGroup.in(mReserveGroup); }
    bool hasReservation() const { return mIsReservedGroup.has(mReserveGroup); }
    virtual void uponReservation(const TimeValue &time) { }
    
   protected:
    void addValidator(Validator &validator);
    //void updateValidator(Validator &validator);
    
    vector<Resource *> mResources;
    vector<Resource *> mObservedResources;
    vector<Validator *> mValidators;

    Group<Reservable> mReserveGroup;
    Group<Reservable> mReserveOnlyGroup;
    Group<Reservable> mObservedReserveGroup;

    Group<Reservable> mTmp;

    friend class Validator;
    friend class Resource;
  };

  class DynamicReserver : public Reserver {
   public:
    DynamicReserver() : Reserver() { }
    virtual ~DynamicReserver() { }

    void reset() { mReserveGroup.clear(); mReserveOnlyGroup.clear(); mObservedReserveGroup.clear(); }
    void resetTo(const Reserver &reserver);
    void add(const Reserver &reserver);
    void add(Resource &resource);
    void add(const vector<Resource *> &resources);
    
    bool tryToReserve(const TimeValue &time);
    bool tryToReserve(const TimeValue &time, bool relaxed);

   protected:
    bool in(const Resource &resource) const { return mReserveGroup.in(resource) && !mObservedReserveGroup.in(resource); }
    bool in(const Validator &validator) const { return mReserveGroup.in(validator); }

    Group<Reservable> mReserveOnlyResourcesGroup;
    Group<Reservable> mReserveOnlyValidatorsGroup;
  };

  class Reader {
   public:
    Reader(Job &job) : mJob(job) { }
    const vector<Resource *> &resources() const { return mResources; }
    Job &job() { return mJob; }
    
    void addResource(Resource &resource);
    void addResources(const vector<Resource *> &resources);
    
    void start();
    void end();
    
   protected:
    void block() { mBlocker.block(); }
    void unblock() { mBlocker.unblock(); }

    vector<Resource *> mResources;
    
    FastBlockableMutex mBlocker;
    Job &mJob;

    friend class Resource;
  };

  class Writer {
   public:
    Writer() { }
    Writer(const vector<Resource *> &resources);
    virtual ~Writer() { }
    
    const vector<Resource *> &resources() const { return mResources; }

    void addResource(Resource &resource);
    void addResources(const vector<Resource *> &resources);
    void reset() { mResources.clear(); }
    
    void start();
    void end();
  
   protected:
    vector<Resource *> mResources;
  };

  class Validator : public Reservable {
   public:
    Validator();
    Validator(bool debug);
    virtual ~Validator() { }

    const vector<Resource *> &resources() const { return mResources; }
    void addResource(Resource &resource);
    void addResources(const vector<Resource *> &resources);
    const vector<Reader *> &readers() const { return mReaders; }
    void addReader(Reader &reader);

    bool isValid() const;
    void setIsValid(bool valid);
    void setIsValid(const TimeValue &deadline);
    TimeValue validationTime() const { return mValidationTime; }
    bool debug() const { return mDebug; }
    
    virtual void reset(const Group<Reservable> &group) = 0;
    virtual void free(Resource &resource) = 0;
    TimeValue deadline() const { return mDeadline; }
    
   protected:
    void setValidationTime(const TimeValue &time) { assert(time >= mValidationTime); mValidationTime = time; }
    
    vector<Resource *> mResources;
    vector<Reader *> mReaders;
    // GroupableFastMutex mReserve;

    atomic<bool> mIsValid;
    AtomicTimeValue mDeadline;
    AtomicTimeValue mTime;
    AtomicTimeValue mValidationTime;

    // real-time debug
    bool mDebug;
    RealTimeClock mRealTimeClock;

    Group<Reservable> mResourcesGroup;

    friend class Reserver;
    friend class DynamicReserver;
  };

 protected:
  // getters
  const vector<Reserver *> &reservers() const { return mReservers; }
  const vector<Reader *> &readers() const { return mReaders; }
  const vector<Writer *> &writers() const { return mWriters; }
  const vector<Validator *> &validators() const { return mValidators; }

  // setters
  void addReserver(Reserver &reserver);
  void addReader(Reader &reader);
  void addWriter(Writer &writer);
  void addValidator(Validator &validator);
  void setTime(const TimeValue &time) { assert(time >= mTime); assert(time <= deadline()); mTime = time; }
  void setFreeTime(const TimeValue &time) { assert(time >= mFreeTime); mFreeTime = time; }
  
  // operations
  void block();
  void unblock();

  void preventReaders();
  void unpreventReaders();
  void notifyValidators();
  
  // attributes
  // GroupableFastMutex mReserve;
  atomic<Status> mStatus;
  atomic<unsigned int > mNbOfValidation;

  // references
  vector<Reserver *> mReservers;
  vector<Reader *> mReaders;
  vector<Writer *> mWriters;
  vector<Validator *> mValidators;
  AtomicTimeValue mTime;
  AtomicTimeValue mFreeTime;

  // real-time debug
  bool mDebug;
  RealTimeClock mRealTimeClock;
};

#endif
