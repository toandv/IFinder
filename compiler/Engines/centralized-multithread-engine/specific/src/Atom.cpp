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

#include <Atom.hpp>
#include <Compound.hpp>
#include <BipError.hpp>
#include "ExecuteAtomJob.hpp"
#include <Connector.hpp>
#include <CycleInPriorities.hpp>
#include <CycleInPrioritiesError.hpp>
#include <BipError.hpp>
#include <UnexpectedEventError.hpp>
#include <Executer.hpp>

// constructors
Atom::Atom(const string &name) :
  ComponentItf(name, ATOM),
  Component(name, ATOM),
  AtomItf(name),
  mHasUrgentInternalExternalPort(this, &Atom::computeHasUrgentInternalExternalPort),
  mInvariantReset(true),
  mTimeReset(true),
  mInternals(this, &Atom::recomputeInternals),
  mExternalsReset(true),
  mExecuteJob(*this),
  mTimeSafetyValidator(*this),
  mHasResume(false),
  mHadResume(false) {
  mChoice.dependsOn(mInternals);
  mChoice.dependsOn(mExternalsReset);
  
  mInvariantReset.addResource(mInvariantResource);
  
  mResources.push_back(&mPortsResource);
  mResources.push_back(&mDataResource);
  mResources.push_back(&mInvariantResource);

  mPortsResources.push_back(&mPortsResource);
  mDataResources.push_back(&mDataResource);
}

Atom::Atom(const string &name, bool initialHasResume) :
  ComponentItf(name, ATOM),
  Component(name, ATOM),
  AtomItf(name, initialHasResume),
  mHasUrgentInternalExternalPort(this, &Atom::computeHasUrgentInternalExternalPort),
  mInternals(this, &Atom::recomputeInternals),
  mExecuteJob(*this),
  mTimeSafetyValidator(*this),
  mHasResume(false),
  mHadResume(false) {
  mChoice.dependsOn(mInternals);
  mChoice.dependsOn(mExternalsReset);
  
  mInvariantReset.addResource(mInvariantResource);
  
  mResources.push_back(&mPortsResource);
  mResources.push_back(&mDataResource);
  mResources.push_back(&mInvariantResource);

  mPortsResources.push_back(&mPortsResource);
  mDataResources.push_back(&mDataResource);
}

// destructor
Atom::~Atom() {
  /* implement your destructor here */
}

void Atom::setInvariantReady() {
  bool oldIsReady = mInvariantIsReady.exchange(true);

  if (!oldIsReady && !hasResume()) {
    mInvariantReset.resetDependent();
    mInvariantResource.free();
  }
}

/* \brief Execute an internal port.
 *
 * \param internalPort is the target internal port, it should be an internal
 * port of atom 'this'.
 *
 * \return An error if found during the execution of the corresponding
 * transition, BipError::NoError otherwise.
 */
BipError &Atom::execute(AtomInternalPort &internalPort, const TimeValue &time) {
  // execute
  BipError &error = execute(internalPort.portValue(), time);

  if (error.type() != NO_ERROR) {
    return error;
  }

  // reset the interface ports
  // reset();

  return BipError::NoError;
}

void Atom::reset() {
  // current time has probably changed
  mTimeReset.resetDependent();
  
  // force recomputation of internals
  internals().reset();
}
  
BipError &Atom::checkExternals() {
  vector<AtomExternalPort *>::iterator portIt = mWaiting.begin();

  // check incoming events on waiting ports
  while (portIt != mWaiting.end()) {
    AtomExternalPort &port = **portIt;

    assert(port.waiting());

    bool checkRemainingEvents = false;
    bool alreadyNextPort = false;

    do {
      // /!\ hasEvent() may change at any time
      // to keep consistency we take a snapshot of
      // its current value here.
      bool snapshotHasEvent = port.hasEvent();

      checkRemainingEvents = false;
      alreadyNextPort = false;

      if (snapshotHasEvent) {
        // force recomputation of port timing constaint
        port.reset();

        if (port.hasExpectedEvent()) {
#ifndef NDEBUG
          const TimingConstraint &timingConstraint = port.timingConstraint();
#endif
          assert(!timingConstraint.empty());

          // add port to the executable external ports
          mExternals.push_back(&port);
          mExternalsReset.resetDependent();

          // remove port of the waiting external ports & update ready queue
          portIt = mWaiting.erase(portIt);
          Executer::jobs.removeSporadicPush(1);

          // inform that there is no need for incrementing portIt
          alreadyNextPort = true;
        }
        else if (true) { // TO BE DONE: EARLY EVENT! (port.eventTime() <= planningInterval.right()) { // !hasEarlyEvent
          if (port.policy() == REMEMBER) {
            // nothing to do in this case!
          }
          else if (port.policy() == IGNORE) {
            port.popEvent();
            checkRemainingEvents = true;
          }
          else if (port.policy() == ERROR) {
            BipError &error = *new UnexpectedEventError(port.holder(), port);
            return error;
          }
          else {
            assert(false);
          }
        }
      }

      assert(!(checkRemainingEvents && alreadyNextPort));
    } while (checkRemainingEvents);

    // next port
    if (!alreadyNextPort) {
      ++portIt;
    }
  }

  // check incoming events on unexpected ports
  for (vector<AtomExternalPort*>::const_iterator unexpectedIt = mUnexpected.begin() ;
       unexpectedIt != mUnexpected.end() ;
       ++unexpectedIt) {
    AtomExternalPort &unexpectedPort = **unexpectedIt;

    if (unexpectedPort.hasEvent()) {
      if (true) { // TO BE DONE: EARLY EVENT! (port.eventTime() <= planningInterval.right()) { // !hasEarlyEvent if (unexpectedPort.eventTime() <= planningInterval.right()) { // !hasEarlyEvent
        if (unexpectedPort.policy() == REMEMBER) {
          // nothing to do in this case!
        }
        else if (unexpectedPort.policy() == IGNORE) {
          unexpectedPort.purgeEvents();
        }
        else if (unexpectedPort.policy() == ERROR) {
          BipError &error = *new UnexpectedEventError(unexpectedPort.holder(), unexpectedPort);
          return error;
        }
        else {
          assert(false);
        }
      }
    }
  }

  return BipError::NoError;
}

BipError &Atom::recomputeExternals() {
  // inform the ready queue on the number of waiting ports
  Executer::jobs.removeSporadicPush(mWaiting.size());

  // recompute from scratch
  mExternals.clear();
  mWaiting.clear();
  mUnexpected.clear();

  // loop on external ports
  for (map<string, AtomExternalPort *>::const_iterator portIt = externalPorts().begin() ;
       portIt != externalPorts().end() ;
       ++portIt) {
    AtomExternalPort &port = *portIt->second;
    
    if (port.waiting()) {
      if (port.hasInterval()) {
        if (port.interval().next(modelClock().time()) != TimeValue::MAX) {
          mWaiting.push_back(&port);
        }
        else {
          mUnexpected.push_back(&port);
        }
      }
      else {
        mWaiting.push_back(&port);
      }
    }
    else {
      mUnexpected.push_back(&port);
    }
  }

  // inform the ready queue on the number of waiting ports
  Executer::jobs.addSporadicPush(mWaiting.size());

  return checkExternals();
}

void Atom::initializeResources() {  
  // add resource of exported ports
  for (map<string, AtomExportPort *>::const_iterator portIt = ports().begin() ;
       portIt != ports().end() ;
       ++portIt) {
    AtomExportPort &port = *portIt->second;
    
    if (port.hasEarlyUpdate()) {
      port.reset().addResource(port.resource());
      mResources.push_back(&port.resource());
      mPortsResources.push_back(&port.resource());
    }
    else {
      port.reset().addResource(mPortsResource);
    }
  }

  // add resources of internal ports
  for (map<string, AtomInternalPort *>::const_iterator portIt = internalPorts().begin() ;
       portIt != internalPorts().end() ;
       ++portIt) {
    AtomInternalPort &port = *portIt->second;
    
    if (port.hasEarlyUpdate() && !port.isExported()) {
      mResources.push_back(&port.resource());
      mPortsResources.push_back(&port.resource());
    }
  }
  
  // add resources of external ports
  for (map<string, AtomExternalPort *>::const_iterator portIt = externalPorts().begin() ;
       portIt != externalPorts().end() ;
       ++portIt) {
    AtomExternalPort &port = *portIt->second;
    
    if (port.hasEarlyUpdate()) {
      mResources.push_back(&port.resource());
      mPortsResources.push_back(&port.resource());
    }
  }

  // add resources of data  
  for (map<string, AtomExportData *>::const_iterator portIt = data().begin() ;
       portIt != data().end() ;
       ++portIt) {
    AtomExportData &data = *portIt->second;
    
    if (data.hasEarlyUpdate()) {
      data.reset().addResource(data.resource());
      mResources.push_back(&data.resource());
      mDataResources.push_back(&data.resource());
    }
    else {
      data.reset().addResource(mDataResource);
    }
  }
}

void Atom::initializeResetable() {
  // initialize export port resetable
  for (map<string, AtomExportPort *>::const_iterator portIt = ports().begin() ;
       portIt != ports().end() ;
       ++portIt) {
    AtomExportPort &port = *portIt->second;

    port.reset().initialize();
  }

  // initialize internal port resetable
  for (map<string, AtomInternalPort *>::const_iterator portIt = internalPorts().begin() ;
       portIt != internalPorts().end() ;
       ++portIt) {
    AtomInternalPort &port = *portIt->second;

    port.timingConstraint().initialize();
  }
  
  // initialize external port resetable
  for (map<string, AtomExternalPort *>::const_iterator portIt = externalPorts().begin() ;
       portIt != externalPorts().end() ;
       ++portIt) {
    AtomExternalPort &port = *portIt->second;

    port.timingConstraint().initialize();
  }

  // initialize export data resetable
  for (map<string, AtomExportData *>::const_iterator dataIt = data().begin() ;
       dataIt != data().end() ;
       ++dataIt) {
    AtomExportData &data = *dataIt->second;

    data.reset().initialize();
  }  

  // initialize atom resetable
  mInvariantReset.initialize();
  mTimeReset.initialize();
  mInternals.initialize();
  mExternalsReset.initialize();
}

void Atom::initializeReader(bool isRealTime) {
  if (!internalPorts().empty() ||
      !externalPorts().empty()) {  
    mReader.addResources(mResources);

    if (!isRealTime) {
      mReader.addResource(Job::simulationClockResource());
    }
  }
}

bool Atom::invariantIsModifiedBy(const PortValue &portValue) const {
  bool ret = true;

  if (portValue.hasResume()) {
    if (hasInvariant()) {
      ret = !portValue.unmodifiedInvariant() ||
        (invariant() != Interval(TimeValue::MIN, TimeValue::MAX));
    }
    else {
      ret = !portValue.unmodifiedInvariant();
    }
  }
  else {
    ret = !portValue.unmodifiedInvariant();
  }
  
  return ret;
}

void Atom::computeHasUrgentInternalExternalPort(bool &hasUrgentInternalExternalPort) {
  hasUrgentInternalExternalPort = false;

  // loop on intenal ports
  for (map<string, AtomInternalPort *>::const_iterator portIt = internalPorts().begin() ;
       portIt != internalPorts().end() ;
       ++portIt) {
    AtomInternalPort &port = *portIt->second;

    if (!port.isExported() && port.hasUrgency()) {
      hasUrgentInternalExternalPort = true;

      return;
    }
  }
  
  // loop on external ports
  for (map<string, AtomExternalPort *>::const_iterator portIt = externalPorts().begin() ;
       portIt != externalPorts().end() ;
       ++portIt) {
    AtomExternalPort &port = *portIt->second;

    if (port.hasUrgency()) {
      hasUrgentInternalExternalPort = true;

      return;
    }
  }
}

void Atom::recomputeInternals(vector<AtomInternalPort *> &internals) const {
  // recompute from scratch
  internals.clear();

  // loop on intenal ports
  for (map<string, AtomInternalPort *>::const_iterator portIt = internalPorts().begin() ;
       portIt != internalPorts().end() ;
       ++portIt) {
    AtomInternalPort &port = *portIt->second;

    if (!port.isExported()) { // TO BE OPTIMIZED LATER
      if (port.hasPortValue()) {
        const TimingConstraint &timingConstraint = port.timingConstraint();

        if (!timingConstraint.empty()) {
          internals.push_back(&port);
        }
      }
    }
  }
}
  
ostream& operator<<(ostream &o, const Atom &atom) {
  return o << atom.toString();
}

ostream& operator<<(ostream &o, const Atom *atom) {
  return o << atom->toString();
}
