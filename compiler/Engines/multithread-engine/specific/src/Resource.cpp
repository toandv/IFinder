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

#include <Resource.hpp>
#include <Job.hpp>

void Resource::addReserver(Resource::Reserver &reserver) {
  if (find(mReservers.begin(),
           mReservers.end(),
           &reserver)
      == mReservers.end()) {
    mReservers.push_back(&reserver);
  }
}

void Resource::addReader(Resource::Reader &reader) {
  if (find(mReaders.begin(),
           mReaders.end(),
           &reader)
      == mReaders.end()) {
    mReaders.push_back(&reader);
  }

  for (vector<Validator *>::const_iterator validatorIt = validators().begin() ;
       validatorIt != validators().end() ;
       ++validatorIt) {
    Validator &validator = **validatorIt;

    validator.addReader(reader);
  }
}

void Resource::addWriter(Resource::Writer &writer) {
  if (find(mWriters.begin(),
           mWriters.end(),
           &writer)
      == mWriters.end()) {
    mWriters.push_back(&writer);
  }
}

void Resource::addValidator(Resource::Validator &validator) {  
  if (find(mValidators.begin(),
           mValidators.end(),
           &validator)
      == mValidators.end()) {
    mValidators.push_back(&validator);
  }

  for (vector<Reader *>::const_iterator readerIt = readers().begin() ;
       readerIt != readers().end() ;
       ++readerIt) {
    Reader &reader = **readerIt;

    validator.addReader(reader);
  }

  for (vector<Reserver *>::const_iterator reserverIt = reservers().begin() ;
       reserverIt != reservers().end() ;
       ++reserverIt) {
    Reserver &reserver = **reserverIt;

    if (find(reserver.resources().begin(),
             reserver.resources().end(),
             this)
        != reserver.resources().end()) {
      reserver.addValidator(validator);
    }
  }
}

void Resource::setStatus(Status status) {
#ifdef NDEBUG
  mStatus.store(status);
#else
  Status oldStatus = mStatus.exchange(status);
#endif

  assert((oldStatus == FREE && status == RESERVED) ||
         (oldStatus == RESERVED && status == USED) ||
         (oldStatus == RESERVED && status == FREE) ||
         (oldStatus == USED && status == FREE));
}

TimeValue Resource::deadline() const {
  TimeValue ret = TimeValue::MAX;

  // compute the minimal deadline amongst the validators
  for (vector<Validator *>::const_iterator validatorIt = validators().begin() ;
       validatorIt != validators().end() ;
       ++validatorIt) {
    Validator &validator = **validatorIt;
    TimeValue deadline = validator.deadline();

    if (deadline < ret) {
      ret = deadline;
    }
  }

  return ret;
}

void Resource::initialize() {  
  mIsReservedGroup.addToSupport(*this);
  
  setTime(TimeValue::ZERO);
}

void Resource::use() {
  block();

  assert(status() == RESERVED);
  assert(mIsReservedGroup.in(*this));
      
  setStatus(USED);
}

void Resource::free() {
  assert(status() == RESERVED || status() == USED);
  assert(mIsReservedGroup.in(*this));

  bool used = (status() == USED);

  setStatus(FREE);
  if (debug()) setFreeTime(mRealTimeClock.time());

  notifyValidators();
  
  unpreventReaders();
  
  mIsReservedGroup.testAndRemove(*this);
  
  if (used) {
    unblock();
  }
}

/*
void Resource::unvalidate() {
  // update validation number
  unsigned int oldNbOfValidation = mNbOfValidation.fetch_add(1);

  if (oldNbOfValidation == 0) {
    preventReaders();
  }
}

void Resource::validate() {
  // update validation number
  unsigned int oldNbOfValidation = mNbOfValidation.fetch_sub(1);

  assert(oldNbOfValidation > 0);

  if (oldNbOfValidation - 1 == 0) {
    unpreventReaders();
    
    // TO BE OPTIMIZED LATER BY CHECKING WHETHER THE DEADLINE REALLY CHANGED
    // restart readers
    for (vector<Reader *>::const_iterator readerIt = readers().begin() ;
         readerIt != readers().end() ;
         ++readerIt) {
      Reader &reader = **readerIt;
       
      reader.job().restart();
    }
  }
}
*/

void Resource::block() {
  for (vector<Resource::Reader *>::const_iterator readerIt = readers().begin() ;
       readerIt != readers().end() ;
       ++readerIt) {
    Resource::Reader &reader = **readerIt;

    reader.block();
  }

  preventReaders();
}

void Resource::unblock() {
  for (vector<Resource::Reader *>::const_iterator readerIt = readers().begin() ;
       readerIt != readers().end() ;
       ++readerIt) {
    Resource::Reader &reader = **readerIt;

    reader.unblock();
  }

  unpreventReaders();
}

void Resource::preventReaders() {
  for (vector<Resource::Reader *>::const_iterator readerIt = readers().begin() ;
       readerIt != readers().end() ;
       ++readerIt) {
    Resource::Reader &reader = **readerIt;

    reader.job().preventEnqueuing();
  }
}

void Resource::unpreventReaders() {
  for (vector<Resource::Reader *>::const_iterator readerIt = readers().begin() ;
       readerIt != readers().end() ;
       ++readerIt) {
    Resource::Reader &reader = **readerIt;

    reader.job().unpreventEnqueuing();
  }
}

void Resource::notifyValidators() {
  for (vector<Validator *>::const_iterator validatorIt = validators().begin() ;
       validatorIt != validators().end() ;
       ++validatorIt) {
    Validator &validator = **validatorIt;

    validator.free(*this);
  }
}

TimeValue Resource::Reserver::deadline() const {
  TimeValue ret = TimeValue::MAX;
  
  for (vector<Validator *>::const_iterator validatorIt = mValidators.begin() ;
       validatorIt != mValidators.end() ;
       ++validatorIt) {
    Validator &validator = **validatorIt;
    TimeValue deadline = validator.deadline();

    if (deadline < ret) {
      ret = deadline;
    }
  }

  return ret;
}

void Resource::Reserver::addResource(Resource &resource) {
  if (find(mResources.begin(),
           mResources.end(),
           &resource)
      == mResources.end()) {
    mResources.push_back(&resource);
      
    if (find(mObservedResources.begin(),
             mObservedResources.end(),
             &resource)
        == mObservedResources.end()) {
      // /!\ Add resources to all groups to enforce conflicts
      // (and thus the use of common mutex vectors)
      // groups are fixed in initialize()
      mReserveGroup.addToSupport(resource);
      mReserveOnlyGroup.addToSupport(resource);
      mObservedReserveGroup.addToSupport(resource);
  
      mTmp.addToSupport(resource);
    }

    resource.addReserver(*this);
  
    for (vector<Validator *>::const_iterator validatorIt = resource.validators().begin() ;
         validatorIt != resource.validators().end() ;
         ++validatorIt) {
      Validator &validator = **validatorIt;

      addValidator(validator);
    }
  }
}

void Resource::Reserver::addResources(const vector<Resource *> &resources) {
  for (vector<Resource *>::const_iterator resourceIt = resources.begin() ;
       resourceIt != resources.end() ;
       ++resourceIt) {
    Resource &resource = **resourceIt;

    addResource(resource);
  }
}

void Resource::Reserver::addObservedResource(Resource &resource) {
  if (find(mObservedResources.begin(),
           mObservedResources.end(),
           &resource)
      == mObservedResources.end()) {
    mObservedResources.push_back(&resource);
    
    if (find(mResources.begin(),
             mResources.end(),
             &resource)
        == mResources.end()) {
      // /!\ Add resources to all groups to enforce conflicts
      // (and thus the use of common mutex vectors)
      // groups are fixed in initialize()
      mReserveGroup.addToSupport(resource);
      mReserveOnlyGroup.addToSupport(resource);
      mObservedReserveGroup.addToSupport(resource);
    }

    resource.addReserver(*this);
  }
}

void Resource::Reserver::addObservedResources(const vector<Resource *> &resources) {
  for (vector<Resource *>::const_iterator resourceIt = resources.begin() ;
       resourceIt != resources.end() ;
       ++resourceIt) {
    Resource &resource = **resourceIt;

    addObservedResource(resource);
  }
}

void Resource::Reserver::addValidator(Validator &validator) {  
  if (find(mValidators.begin(),
           mValidators.end(),
           &validator)
      == mValidators.end()) {        
    mValidators.push_back(&validator);
    
    mReserveGroup.addToSupport(validator);
  }
}

void Resource::Reserver::initialize() {  
  for (vector<Resource *>::const_iterator resourceIt = mResources.begin() ;
       resourceIt != mResources.end() ;
       ++resourceIt) {
    Resource &resource = **resourceIt;

    mObservedReserveGroup.remove(resource);
  }

  mReserveOnlyGroup.remove(mObservedReserveGroup);
  
  assert(mReserveGroup.in(mReserveOnlyGroup));
  assert(mReserveGroup.in(mObservedReserveGroup));
  assert(!mReserveOnlyGroup.has(mObservedReserveGroup));
  assert(!mObservedReserveGroup.has(mReserveOnlyGroup));
}

bool Resource::Reserver::tryToReserve(const TimeValue &time) {
  return tryToReserve(time, false);
}

bool Resource::Reserver::tryToReserve(const TimeValue &time, bool relaxed) {  
  assert(mReserveGroup.in(mReserveOnlyGroup));
  assert(mReserveGroup.in(mObservedReserveGroup));
  assert(!mReserveOnlyGroup.has(mObservedReserveGroup));
  assert(!mObservedReserveGroup.has(mReserveOnlyGroup));
  
  bool success = mIsReservedGroup.testAndAdd(mReserveGroup);

  if (success) {
    // check if all resources are not outdated
    for (vector<Resource *>::const_iterator resourceIt = mResources.begin() ;
         resourceIt != mResources.end() ;
         ++resourceIt) {
      Resource &resource = **resourceIt;

      if ((time < resource.time() && !relaxed)) { // ||
          //time > resource.deadline()) {
        success = false;
        break;
      }
    }

    if (!success) {
      // unreserve resources
      mIsReservedGroup.testAndRemove(mReserveGroup);
    }
    else {
      // reserve specific action
      uponReservation(time);
    
      // let observed resources
      mIsReservedGroup.testAndRemove(mObservedReserveGroup);
          
      // set resource time
      for (vector<Resource *>::const_iterator resourceIt = mResources.begin() ;
           resourceIt != mResources.end() ;
           ++resourceIt) {
        Resource &resource = **resourceIt;

        resource.setTime(time);
      }

      // prevent other reservers from enqueuing
      for (vector<Resource *>::const_iterator resourceIt = mResources.begin() ;
         resourceIt != mResources.end() ;
         ++resourceIt) {
        Resource &resource = **resourceIt;
        
        resource.preventReaders();
      }

      // update status of modified resources (FREE->RESERVED)
      for (vector<Resource *>::const_iterator resourceIt = mResources.begin() ;
           resourceIt != mResources.end() ;
           ++resourceIt) {
        Resource &resource = **resourceIt;
        
        assert(resource.status() == FREE);
        assert(mIsReservedGroup.in(resource));
        
        resource.setStatus(RESERVED);
      }
      
      // reset all impacted validators
      for (vector<Validator *>::const_iterator validatorIt = mValidators.begin() ;
           validatorIt != mValidators.end() ;
           ++validatorIt) {
        Validator &validator = **validatorIt;

        mTmp = mReserveOnlyGroup;
        mTmp.intersect(validator.mResourcesGroup);
        
        validator.reset(mTmp);
      }
    }
  }

  return success;
}

void Resource::DynamicReserver::resetTo(const Reserver &reserver) {
  // check that resources are allowed to added
#ifndef NDEBUG
  for (vector<Resource *>::const_iterator resourceIt = reserver.mResources.begin() ;
       resourceIt != reserver.mResources.end() ;
       ++resourceIt) {
    Resource &resource = **resourceIt;
#endif

    assert(find(mResources.begin(),
                mResources.end(),
                &resource)
           != mResources.end());
#ifndef NDEBUG
  }
#endif

  // check that observed resources are allowed to added
#ifndef NDEBUG
  for (vector<Resource *>::const_iterator resourceIt = reserver.mObservedResources.begin() ;
       resourceIt != reserver.mObservedResources.end() ;
       ++resourceIt) {
    Resource &resource = **resourceIt;
#endif

    assert(find(mResources.begin(),
                mResources.end(),
                &resource)
           != mResources.end() ||
           find(mObservedResources.begin(),
                mObservedResources.end(),
                &resource)
           != mObservedResources.end());
#ifndef NDEBUG
  }
#endif  
  
  mReserveGroup = reserver.mReserveGroup;
  mReserveOnlyGroup = reserver.mReserveOnlyGroup;
  mObservedReserveGroup = reserver.mObservedReserveGroup;
  
  assert(mReserveGroup.in(mReserveOnlyGroup));
  assert(mReserveGroup.in(mObservedReserveGroup));
  assert(!mReserveOnlyGroup.has(mObservedReserveGroup));
  assert(!mObservedReserveGroup.has(mReserveOnlyGroup));
}

void Resource::DynamicReserver::add(const Reserver &reserver) {
  // check that resources are allowed to added
#ifndef NDEBUG
  for (vector<Resource *>::const_iterator resourceIt = reserver.mResources.begin() ;
       resourceIt != reserver.mResources.end() ;
       ++resourceIt) {
    Resource &resource = **resourceIt;
#endif

    assert(find(mResources.begin(),
                mResources.end(),
                &resource)
           != mResources.end());
#ifndef NDEBUG
  }
#endif

  // check that observed resources are allowed to added
#ifndef NDEBUG
  for (vector<Resource *>::const_iterator resourceIt = reserver.mObservedResources.begin() ;
       resourceIt != reserver.mObservedResources.end() ;
       ++resourceIt) {
    Resource &resource = **resourceIt;
#endif

    assert(find(mResources.begin(),
                mResources.end(),
                &resource)
           != mResources.end() ||
           find(mObservedResources.begin(),
                mObservedResources.end(),
                &resource)
           != mObservedResources.end());
#ifndef NDEBUG
  }
#endif

  // /!\ DO NOT CHANGE ORDER, IT IS CRITICAL!

  // observed resources is the union of non reserve only observed resources
  mObservedReserveGroup.add(reserver.mObservedReserveGroup);  
  mObservedReserveGroup.remove(mReserveOnlyGroup);
  mObservedReserveGroup.remove(reserver.mReserveOnlyGroup);

  // reserve only is the union of reserve only
  mReserveOnlyGroup.add(reserver.mReserveOnlyGroup);
  
  // reserve is the union of reserve
  mReserveGroup.add(reserver.mReserveGroup);
}

void Resource::DynamicReserver::add(Resource &resource) {
  // check that resource is allowed to added
  assert(find(mResources.begin(),
              mResources.end(),
              &resource)
         != mResources.end());

  // add resource to the reserve group
  mReserveGroup.add(resource);
  
  // update reserve only group
  mReserveOnlyGroup.add(resource);
  
  // update observed group
  mObservedReserveGroup.remove(resource);
  
  // add validators
  for (vector<Validator *>::const_iterator validatorIt = resource.validators().begin() ;
       validatorIt != resource.validators().end() ;
       ++validatorIt) {
    Validator &validator = **validatorIt;

    mReserveGroup.add(validator);
  }
}

void Resource::DynamicReserver::add(const vector<Resource *> &resources) {
  for (vector<Resource *>::const_iterator resourceIt = resources.begin() ;
       resourceIt != resources.end() ;
       ++resourceIt) {
    Resource &resource = **resourceIt;

    add(resource);
  }
}

bool Resource::DynamicReserver::tryToReserve(const TimeValue &time) {
  return tryToReserve(time, false);
}

bool Resource::DynamicReserver::tryToReserve(const TimeValue &time, bool relaxed) {
  assert(mReserveGroup.in(mReserveOnlyGroup));
  assert(mReserveGroup.in(mObservedReserveGroup));
  assert(!mReserveOnlyGroup.has(mObservedReserveGroup));
  assert(!mObservedReserveGroup.has(mReserveOnlyGroup));
  
  bool success = mIsReservedGroup.testAndAdd(mReserveGroup);

  if (success) {
    // check if all resources are not outdated
    for (vector<Resource *>::const_iterator resourceIt = mResources.begin() ;
         resourceIt != mResources.end() ;
         ++resourceIt) {
      Resource &resource = **resourceIt;

      if (in(resource)) {
        if ((time < resource.time() && !relaxed)) { // ||
            //time > resource.deadline()) {
          success = false;
          break;
        }
      }
    }

    if (!success) {
      // unreserve resources
      mIsReservedGroup.testAndRemove(mReserveGroup);
    }
    else {
      // reserve specific action
      uponReservation(time);
    
      // let observed resources
      mIsReservedGroup.testAndRemove(mObservedReserveGroup);
      
      // set resource time
      for (vector<Resource *>::const_iterator resourceIt = mResources.begin() ;
           resourceIt != mResources.end() ;
           ++resourceIt) {
        Resource &resource = **resourceIt;

        if (in(resource)) {
          resource.setTime(time);
        }
      }
    
      // prevent other reservers from enqueuing 
      for (vector<Resource *>::const_iterator resourceIt = mResources.begin() ;
         resourceIt != mResources.end() ;
         ++resourceIt) {
        Resource &resource = **resourceIt;

        if (in(resource)) {
          resource.preventReaders();
        }
      }

      // update status of modified resources (FREE->RESERVED)
      for (vector<Resource *>::const_iterator resourceIt = mResources.begin() ;
           resourceIt != mResources.end() ;
           ++resourceIt) {
        Resource &resource = **resourceIt;
        
        if (in(resource)) {
          assert(resource.status() == FREE);
          assert(mIsReservedGroup.in(resource));
        
          resource.setStatus(RESERVED);
        }
      }

      // reset all impacted validators
      for (vector<Validator *>::const_iterator validatorIt = mValidators.begin() ;
           validatorIt != mValidators.end() ;
           ++validatorIt) {
        Validator &validator = **validatorIt;

        if (in(validator)) {
          mTmp = mReserveOnlyGroup;
          mTmp.intersect(validator.mResourcesGroup);
          
          validator.reset(mTmp);
        }
      }
    }
  }

  return success;
}

void Resource::Reader::addResource(Resource &resource) {
  if (find(mResources.begin(),
           mResources.end(),
           &resource)
      == mResources.end()) {
    mResources.push_back(&resource);
  }

  resource.addReader(*this);
}

void Resource::Reader::addResources(const vector<Resource *> &resources) {
  for (vector<Resource *>::const_iterator resourceIt = resources.begin() ;
       resourceIt != resources.end() ;
       ++resourceIt) {
    Resource &resource = **resourceIt;

    addResource(resource);
  }
}

void Resource::Reader::start() {
  mBlocker.lock();
}

void Resource::Reader::end() {
  mBlocker.unlock();
}

void Resource::Writer::addResource(Resource &resource) {
  if (find(mResources.begin(),
           mResources.end(),
           &resource)
      == mResources.end()) {
    mResources.push_back(&resource);
  }

  resource.addWriter(*this);
}

void Resource::Writer::addResources(const vector<Resource *> &resources) {
  for (vector<Resource *>::const_iterator resourceIt = resources.begin() ;
       resourceIt != resources.end() ;
       ++resourceIt) {
    Resource &resource = **resourceIt;

    addResource(resource);
  }
}
/*
void Resource::Writer::free() {
  for (vector<Resource *>::const_iterator resourceIt = resources().begin() ;
       resourceIt != resources().end() ;
       ++resourceIt) {
    Resource &resource = **resourceIt;

    assert(resource.status() == RESERVED);

    if (resource.status() == USEresource.free();
  }
  }*/

void Resource::Writer::start() {
  for (vector<Resource *>::const_iterator resourceIt = resources().begin() ;
       resourceIt != resources().end() ;
       ++resourceIt) {
    Resource &resource = **resourceIt;

    assert(resource.status() == RESERVED);
    
    resource.use();
  }
}

void Resource::Writer::end() {
  for (vector<Resource *>::const_iterator resourceIt = resources().begin() ;
       resourceIt != resources().end() ;
       ++resourceIt) {
    Resource &resource = **resourceIt;

    if (resource.status() == USED) {
      resource.free();
    }
  }
}

Resource::Validator::Validator() :
  mIsValid(true),
  mDebug(false) {
}

Resource::Validator::Validator(bool debug) :
  mIsValid(true),
  mDebug(debug) {
}

void Resource::Validator::addResource(Resource &resource) {  
  if (find(mResources.begin(),
           mResources.end(),
           &resource)
      == mResources.end()) {
    mResources.push_back(&resource);
    
    mResourcesGroup.addToSupport(resource);

    resource.addValidator(*this);
  }
}

void Resource::Validator::addResources(const vector<Resource *> &resources) {
  for (vector<Resource *>::const_iterator resourceIt = resources.begin() ;
       resourceIt != resources.end() ;
       ++resourceIt) {
    Resource &resource = **resourceIt;

    addResource(resource);
  }
}

void Resource::Validator::addReader(Reader &reader) {
  if (find(mReaders.begin(),
           mReaders.end(),
           &reader)
      == mReaders.end()) {
    mReaders.push_back(&reader);
  }
}

bool Resource::Validator::isValid() const {
  bool ret = false;
  
  //if (!mIsReservedGroup.in(*this)) {
    if (mIsValid.load()) {
      ret = true;
    }
    //}

  return ret;
}

void Resource::Validator::setIsValid(bool valid) {
  bool oldIsValid = mIsValid.exchange(valid);

  if (!oldIsValid && valid) {
    assert(mIsReservedGroup.in(*this));

    mDeadline = TimeValue::MAX;
    
    mIsReservedGroup.testAndRemove(*this);

    // unprevent readers from enqueuing
    for (vector<Reader *>::const_iterator readerIt = readers().begin() ;
         readerIt != readers().end() ;
         ++readerIt) {
      Resource::Reader &reader = **readerIt;

      reader.job().unpreventEnqueuing();

      // TO BE OPTIMIZED LATER BY CHECKING WHETHER THE DEADLINE REALLY CHANGED
      reader.job().restart();
    }
    
    if (debug()) setValidationTime(mRealTimeClock.time());
  }
  else if (oldIsValid && !valid) {
    assert(mIsReservedGroup.in(*this));

    // prevent readers from enqueuing
    for (vector<Reader *>::const_iterator readerIt = readers().begin() ;
         readerIt != readers().end() ;
         ++readerIt) {
      Resource::Reader &reader = **readerIt;

      reader.job().preventEnqueuing();
    }

    mDeadline = TimeValue::MIN;
  }
}

void Resource::Validator::setIsValid(const TimeValue &deadline) {
  bool oldIsValid = mIsValid.exchange(true);

  if (!oldIsValid) {
    assert(mIsReservedGroup.in(*this));
     
    mDeadline = deadline;

    mIsReservedGroup.testAndRemove(*this);

    // unprevent readers from enqueuing
    for (vector<Reader *>::const_iterator readerIt = readers().begin() ;
         readerIt != readers().end() ;
         ++readerIt) {
      Resource::Reader &reader = **readerIt;

      reader.job().unpreventEnqueuing();

      // TO BE OPTIMIZED LATER BY CHECKING WHETHER THE DEADLINE REALLY CHANGED
      reader.job().restart();
    }
    
    if (debug()) setValidationTime(mRealTimeClock.time());
  }
}

AtomicGroup<Reservable> Reservable::mIsReservedGroup(false);
