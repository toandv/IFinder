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

#include <AtomExportPort.hpp>
#include <AtomInternalPort.hpp>
#include <Atom.hpp>
#include <InteractionValue.hpp>
#include <BipError.hpp>

#include <Job.hpp>
#include "ExecuteAtomJob.hpp"

// constructors
AtomExportPort::AtomExportPort(const string &name) :
  PortItf(name, ATOM_EXPORT),
  Port(name, ATOM_EXPORT),
  AtomExportPortItf(name),
  mReset(true) {
}

AtomExportPort::AtomExportPort(const string &name, bool hasEarlyUpdate, bool hasUrgency) :
  PortItf(name, ATOM_EXPORT),
  Port(name, ATOM_EXPORT),
  AtomExportPortItf(name, hasEarlyUpdate, hasUrgency),
  mReset(true) {
}

// destructor
AtomExportPort::~AtomExportPort() {
  /* implement your destructor here */
}


TimeValue AtomExportPort::time(const PortValue &portValue) const {
  //return holder().time();
  return resource().time();
}

void AtomExportPort::addInternalPort(AtomInternalPort &internalPort) {
  // specific
  internalPort.addConnectedPort(*this);

  // standard addInternalPort() of the interface
  AtomExportPortItf::addInternalPort(internalPort);
}

BipError &AtomExportPort::execute(PortValue &portValue, const TimeValue &time) {
  // should be a port value of this
  assert(find(portValues().begin(),
              portValues().end(),
              &portValue)
         != portValues().end());

  // chosen port value & time
  holder().executeJob().setChosenPortValue(portValue);
  holder().executeJob().setChosenTime(time);
  holder().setHasResume(portValue.hasResume());
  
  // start execute job immediatly
  holder().executeJob().restart();

  // errors & reset are handled in connector jobs
  return BipError::NoError;
}

const Resource &AtomExportPort::resource() const {
  if (hasEarlyUpdate()) {
    return mResource;
  }
  else {
    return holder().portsResource();
  }
}

Resource &AtomExportPort::resource() {
  if (hasEarlyUpdate()) {
    return mResource;
  }
  else {
    return holder().portsResource();
  }
}

void AtomExportPort::setReady() {
  bool oldIsReady = mIsReady.exchange(true);

  if (!oldIsReady && !holder().hasResume()) {
    if (isReset()) {
      reset().resetDependent();
    }

    if (hasEarlyUpdate()) {
      mResource.free();
    }
  }
}

bool AtomExportPort::isModifiedBy(const PortValue &portValue) const {
  bool ret = true;
  
  bool modifiedByTransition = true;
  
  if (portValue.hasUnmodifiedExportPorts()) {
    if (find(portValue.unmodifiedExportPorts().begin(),
             portValue.unmodifiedExportPorts().end(),
             this)
        != portValue.unmodifiedExportPorts().end()) {
      modifiedByTransition = false;
    }
  }

  if (portValue.hasResume()) {
    ret = modifiedByTransition || hasPortValues();
  }
  else {
    ret = modifiedByTransition;
  }

  return ret;
}

void AtomExportPort::configureReserver(Resource::DynamicReserver &reserver, PortValue &portValue) const {
  reserver.add(holder().portsResource());
  reserver.add(holder().dataResource());
  
  if (holder().invariantIsModifiedBy(portValue)) {
    reserver.add(holder().invariantResource());
  }

  const Atom &atom = holder();
  
  for (map<string, AtomExportPort *>::const_iterator portIt = atom.ports().begin() ;
       portIt != atom.ports().end() ;
       ++portIt) {
    AtomExportPort &port = *portIt->second;

    if (port.hasEarlyUpdate() &&
      port.isModifiedBy(portValue)) {
      reserver.add(port.resource());
    }
  }
  
  for (map<string, AtomInternalPort *>::const_iterator portIt = atom.internalPorts().begin() ;
       portIt != atom.internalPorts().end() ;
       ++portIt) {
    AtomInternalPort &port = *portIt->second;

    if (port.hasEarlyUpdate() &&
        !port.isExported() &&
        port.isModifiedBy(portValue)) {
      reserver.add(port.resource());
    }
  }
  
  for (map<string, AtomExternalPort *>::const_iterator portIt = atom.externalPorts().begin() ;
       portIt != atom.externalPorts().end() ;
       ++portIt) {
    AtomExternalPort &port = *portIt->second;

    if (port.hasEarlyUpdate() &&
        port.isModifiedBy(portValue)) {
      reserver.add(port.resource());
    }
  }
  
  for (map<string, AtomExportData *>::const_iterator dataIt = atom.data().begin() ;
       dataIt != atom.data().end() ;
       ++dataIt) {
    AtomExportData &data = *dataIt->second;

    if (data.hasEarlyUpdate() &&
        data.isModifiedBy(portValue)) {
      reserver.add(data.resource());
    }
  }
}
