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
#include <Executer.hpp>

void Resource::Reader::addResource(Resource &resource) {
  if (find(mResources.begin(),
           mResources.end(),
           &resource)
      == mResources.end()) {
    mResources.push_back(&resource);

    resource.addReader(*this);
  }  
}

void Resource::Reader::addResources(const vector<Resource *> &resources) {
  for (vector<Resource *>::const_iterator resourceIt = resources.begin() ;
       resourceIt != resources.end() ;
       ++resourceIt) {
    Resource &resource = **resourceIt;

    addResource(resource);
  }  
}

void Resource::Reader::addValidator(Validator &validator) {   
  if (find(mValidators.begin(),
           mValidators.end(),
           &validator)
      == mValidators.end()) {
    mValidators.push_back(&validator);
  }
}

TimeValue Resource::Reader::deadline() const {
  TimeValue ret = TimeValue::MAX;

  assert(mPreventReading.load() == 0);
  
  for (vector<Validator *>::const_iterator validatorIt = mValidators.begin() ;
       validatorIt != mValidators.end() ;
       ++validatorIt) {
    Validator &validator = **validatorIt;

    if (validator.deadline() < ret) {
      ret = validator.deadline();
    }
  }

  return ret;
}

void Resource::Reader::unpreventReading() {
  unsigned int oldPreventReading = mPreventReading.fetch_sub(1);

  assert(oldPreventReading >= 1);
  
  if (oldPreventReading == 1) {
    Executer::notify();
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

void Resource::use() {
#ifndef NDEBUG
  bool oldIsFree =
#endif
    mIsFree.exchange(false);
  
  assert(oldIsFree);
  
  for (vector<Validator *>::const_iterator validatorIt = mValidators.begin() ;
       validatorIt != mValidators.end() ;
       ++validatorIt) {
    Validator &validator = **validatorIt;

    validator.use(*this);
  }
  
  for (vector<Resource::Reader *>::const_iterator readerIt = readers().begin() ;
       readerIt != readers().end() ;
       ++readerIt) {
    Resource::Reader &reader = **readerIt;

    reader.preventReading();
  }
}

void Resource::free() {
  bool oldIsFree = mIsFree.exchange(true);

  if (!oldIsFree) {
    for (vector<Validator *>::const_iterator validatorIt = mValidators.begin() ;
         validatorIt != mValidators.end() ;
         ++validatorIt) {
      Validator &validator = **validatorIt;

      validator.free(*this);
    }
    
    for (vector<Resource::Reader *>::const_iterator readerIt = readers().begin() ;
         readerIt != readers().end() ;
         ++readerIt) {
      Resource::Reader &reader = **readerIt;

      reader.unpreventReading();
    }
  }
}

void Resource::Validator::addResource(Resource &resource) {
  if (find(mResources.begin(),
           mResources.end(),
           &resource)
      == mResources.end()) {
    mResources.push_back(&resource);

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

    reader.addValidator(*this);
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
    mDeadline = TimeValue::MAX;

    // unprevent readers from enqueuing
    for (vector<Reader *>::const_iterator readerIt = readers().begin() ;
         readerIt != readers().end() ;
         ++readerIt) {
      Resource::Reader &reader = **readerIt;

      reader.unpreventReading();
    }
  }
  else if (oldIsValid && !valid) {
    // prevent readers from enqueuing
    for (vector<Reader *>::const_iterator readerIt = readers().begin() ;
         readerIt != readers().end() ;
         ++readerIt) {
      Resource::Reader &reader = **readerIt;

      reader.preventReading();
    }

    mDeadline = TimeValue::MIN;
  }
}

void Resource::Validator::setIsValid(const TimeValue &deadline) {
  bool oldIsValid = mIsValid.exchange(true);

  if (!oldIsValid) {     
    mDeadline = deadline;

    // unprevent readers from enqueuing
    for (vector<Reader *>::const_iterator readerIt = readers().begin() ;
         readerIt != readers().end() ;
         ++readerIt) {
      Resource::Reader &reader = **readerIt;

      reader.unpreventReading();
    }
  }
}
