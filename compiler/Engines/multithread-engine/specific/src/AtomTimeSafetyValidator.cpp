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

#include <AtomTimeSafetyValidator.hpp>
#include <Logger.hpp>
#include <BipError.hpp>
#include <TimeSafetyViolationError.hpp>
#include <Atom.hpp>
#include <Port.hpp>
#include <AtomExportPort.hpp>
#include <AtomInternalPort.hpp>
#include <AtomExternalPort.hpp>


// constructors
AtomTimeSafetyValidator::AtomTimeSafetyValidator(Atom &atom) :
  mAtom(atom),
  mDisabledTimeSafety(false),
  mIncludesInvariants(false),
  mMissingResources(0),
  mMissingUrgencies(0),
  mUrgencies(0) {
}

// destructor
AtomTimeSafetyValidator::~AtomTimeSafetyValidator() {
}

void AtomTimeSafetyValidator::reset(const Group<Reservable> &resetResources) {
  // protect
  mLock.lock();
  
  for (Group<Reservable>::iterator resourceIt = resetResources.begin() ;
       resourceIt != resetResources.end() ;
       ++resourceIt) {
    Reservable &reservable = *resourceIt;

    assert(static_cast<Resource *>(&reservable) != NULL);
    
    Resource &resource = static_cast<Resource &>(reservable);
    
    assert(resource.status() == Resource::RESERVED);
      
    assert(find(resources().begin(),
                resources().end(),
                &resource)
           != resources().end());
           
    reset(resource);
  }

  assert(mMissingResources.load() > 0);
  assert(isValid());
    
  unvalidate();

  if (canBeEarlyValidated()) {
    earlyValidate();
  }
  
  // unprotect
  mLock.unlock();
}

void AtomTimeSafetyValidator::free(Resource &resource) {
  // protect
  mLock.lock();

  assert(find(resources().begin(),
              resources().end(),
              &resource)
         != resources().end());
  
  assert(resource.status() == Resource::FREE);
  
  // update urgent internal ports
  if (isUrgentInternalPorts(resource)) {
    for (vector<const AtomInternalPort *>::const_iterator portIt = urgentInternalPorts(resource).begin() ;
         portIt != urgentInternalPorts(resource).end() ;
         ++portIt) {
      const AtomInternalPort &port = **portIt;

      assert(port.hasUrgency());
      
      mMissingUrgencies.fetch_sub(1);

      if (isUrgent(port)) {
        mUrgencies.fetch_add(1);
      }
    }
  }
  
  // update urgent external ports
  if (isUrgentExternalPorts(resource)) {
    for (vector<const AtomExternalPort *>::const_iterator portIt = urgentExternalPorts(resource).begin() ;
         portIt != urgentExternalPorts(resource).end() ;
         ++portIt) {
      const AtomExternalPort &port = **portIt;

      assert(port.hasUrgency());
      
      mMissingUrgencies.fetch_sub(1);

      if (isUrgent(port)) {
        mUrgencies.fetch_add(1);
      }
    }
  }
  
  // update missing resources
  unsigned int oldMissingResources = mMissingResources.fetch_sub(1);

  assert(oldMissingResources >= 1);

  if (!isValid()) {
    if (canBeEarlyValidated()) {
      earlyValidate();
    }
    else if (oldMissingResources == 1) {
      validate();
    }
  }
  
  // unprotect
  mLock.unlock();
}

void AtomTimeSafetyValidator::initialize() {
  if (!disabledTimeSafety()) {
    // add resources corresponding to ports
    addResources(atom().portsResources());

    // add resource corresponding to invariant
    if (includesInvariants()) {
      addResource(atom().invariantResource());
    }

    const Atom &constAtom = atom();

    // map non exported internal ports
    for (map<string, AtomInternalPort *>::const_iterator portIt = constAtom.internalPorts().begin() ;
         portIt != constAtom.internalPorts().end() ;
         ++portIt) {
      const AtomInternalPort &port = *portIt->second;
      const Resource &resource = port.resource();

      if (!port.isExported() && port.hasUrgency()) {
        mUrgentInternalPorts[&resource].push_back(&port);
      }
    }
    
    // map external ports
    for (map<string, AtomExternalPort *>::const_iterator portIt = constAtom.externalPorts().begin() ;
         portIt != constAtom.externalPorts().end() ;
         ++portIt) {
      const AtomExternalPort &port = *portIt->second;
      const Resource &resource = port.resource();

      if (port.hasUrgency()) {
        mUrgentExternalPorts[&resource].push_back(&port);
      }
    }
  }

  mTimeSafe = Interval(TimeValue::MIN, TimeValue::MAX);
  // mDeadline = TimeValue::MAX;
}

void AtomTimeSafetyValidator::reset(Resource &resource) {
  assert(resource.status() == Resource::RESERVED);
  
  // update urgent internal ports
  if (isUrgentInternalPorts(resource)) {
    for (vector<const AtomInternalPort *>::const_iterator portIt = urgentInternalPorts(resource).begin() ;
         portIt != urgentInternalPorts(resource).end() ;
         ++portIt) {
      const AtomInternalPort &port = **portIt;

      assert(port.hasUrgency());
      
      mMissingUrgencies.fetch_add(1);

      if (isUrgent(port)) {
        mUrgencies.fetch_sub(1);
      }
    }
  }
  
  // update urgent external ports
  if (isUrgentExternalPorts(resource)) {
    for (vector<const AtomExternalPort *>::const_iterator portIt = urgentExternalPorts(resource).begin() ;
         portIt != urgentExternalPorts(resource).end() ;
         ++portIt) {
      const AtomExternalPort &port = **portIt;

      assert(port.hasUrgency());
      
      mMissingUrgencies.fetch_add(1);

      if (isUrgent(port)) {
        mUrgencies.fetch_sub(1);
      }
    }
  }
  
  // update missing resources
  mMissingResources.fetch_add(1);
}

bool AtomTimeSafetyValidator::canBeEarlyValidated() const {
  bool ret = false;

  if (!includesInvariants()) {
    ret = mMissingUrgencies.load() == 0 && mUrgencies.load() == 0;
  }
  
  return ret;
}

void AtomTimeSafetyValidator::unvalidate() {
  // mDeadline = TimeValue::MIN;

  setIsValid(false);

  // unvalidate all resources
  /*
  for (vector<Resource *>::const_iterator resourceIt = resources().begin() ;
       resourceIt != resources().end() ;
       ++resourceIt) {
    Resource &resource = **resourceIt;

    resource.unvalidate();
  }
  */
}

void AtomTimeSafetyValidator::validate(const Interval &interval) {
  assert(!isValid());

  setTimeSafe(interval);
  
  // FIXME: replace it by a true map from resources to deadlines
  // mDeadline = timeSafe().right();

  // validate all resources
  /*
  for (vector<Resource *>::const_iterator resourceIt = resources().begin() ;
       resourceIt != resources().end() ;
       ++resourceIt) {
    Resource &resource = **resourceIt;
    
    resource.validate();
  }
  */
  
  setIsValid(timeSafe().right());
}

void AtomTimeSafetyValidator::validate() {
  validate(waitInterval());
}

void AtomTimeSafetyValidator::earlyValidate() {  
  validate(Interval(TimeValue::MIN, TimeValue::MAX));
}

Interval AtomTimeSafetyValidator::waitInterval() const {
  Interval wait = Interval(TimeValue::MIN, TimeValue::MAX);

  if (atom().hasUrgentInternalExternalPort() &&
      !atom().hasResume()) {
    TimeValue time = atom().time();

    // timing constraints of internal ports
    const vector<AtomInternalPort *> &enabledInternals = atom().internals();

    for (vector<AtomInternalPort *>::const_iterator portIt = enabledInternals.begin() ;
         portIt != enabledInternals.end() ;
         ++portIt) {
      const AtomInternalPort &port = **portIt;
      const TimingConstraint &timingConstraint = port.timingConstraint();

      assert(port.hasPortValue());
      assert(!timingConstraint.empty());

      wait &= timingConstraint.wait(time);
    }

    // timing constraints of external ports
    for (vector<AtomExternalPort *>::const_iterator portIt = atom().externals().begin() ;
         portIt != atom().externals().end() ;
         ++portIt) {
      const AtomExternalPort &port = **portIt;
      const TimingConstraint &timingConstraint = port.timingConstraint();

      if (port.waiting()) {
        wait &= timingConstraint.wait(time);
      }
    }

    // invariant
    if (includesInvariants()) {
      if (atom().hasInvariant()) {
        wait &= atom().invariant();
      }
    }

    // urgencies ensure this
    assert(!wait.rightOpen());
  }

  return wait;
}

bool AtomTimeSafetyValidator::isUrgent(const AtomInternalPort &port) const {
  bool ret = false;

  assert(port.resource().status() == Resource::RESERVED ||
         port.resource().status() == Resource::FREE);
  
  if (!port.holder().hasResume()) {
    if (port.hasPortValue()) {
      const PortValue &portValue = port.portValue();
      
      if (portValue.urgency() != LAZY) {
        if (portValue.hasInterval()) {
          Interval updatedInterval = portValue.interval();

          updatedInterval &= Interval(port.holder().time(), TimeValue::MAX);
          
          if (!updatedInterval.empty()) {
            ret = true;
          }
        }
        else {
          ret = true;
        }
      }
    }
  }
  
  return ret;
}

bool AtomTimeSafetyValidator::isUrgent(const AtomExternalPort &port) const {
  bool ret = false;

  assert(port.resource().status() == Resource::RESERVED ||
         port.resource().status() == Resource::FREE);
  
  if (!port.holder().hasResume()) {      
    if (port.urgency() != LAZY) {
      if (port.hasInterval()) {
        Interval updatedInterval = port.interval();

        updatedInterval &= Interval(port.holder().time(), TimeValue::MAX);
          
        if (!updatedInterval.empty()) {
          ret = true;
        }
      }
      else {
        ret = true;
      }
    }
  }
  
  return ret;
}

const vector<const AtomInternalPort *> &AtomTimeSafetyValidator::urgentInternalPorts(const Resource &resource) const {
  map<const Resource *, vector< const AtomInternalPort *>>::const_iterator it =
    mUrgentInternalPorts.find(&resource);

  assert(it != mUrgentInternalPorts.end());

  return it->second;
}

const vector<const AtomExternalPort *> &AtomTimeSafetyValidator::urgentExternalPorts(const Resource &resource) const {
  map<const Resource *, vector<const AtomExternalPort *>>::const_iterator it =
    mUrgentExternalPorts.find(&resource);

  assert(it != mUrgentExternalPorts.end());
  
  return it->second;
}
