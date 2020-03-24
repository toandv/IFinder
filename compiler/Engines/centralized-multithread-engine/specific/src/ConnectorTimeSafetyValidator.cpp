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

#include <ConnectorTimeSafetyValidator.hpp>
#include <Logger.hpp>
#include <Connector.hpp>
#include <BipError.hpp>
#include <TimeSafetyViolationError.hpp>
#include <Atom.hpp>
#include <Port.hpp>
#include <AtomExportPort.hpp>
#include <ConnectorExportPort.hpp>
#include <CompoundExportPort.hpp>
#include <Interaction.hpp>
#include <Job.hpp>

// constructors
ConnectorTimeSafetyValidator::ConnectorTimeSafetyValidator(const Connector &connector) :
  mConnector(connector),
  mDisabledTimeSafety(false),
  mIncludesInvariants(false),
  mMissingResources(0),
  mMissingUrgencies(0),
  mUrgencies(0),
  mDisabledPorts(0) {
}

// destructor
ConnectorTimeSafetyValidator::~ConnectorTimeSafetyValidator() {
}

void ConnectorTimeSafetyValidator::use(Resource &resource) {
  // protect
  mLock.lock();
  
  // update involved ports
  if (isInvolvedPort(resource)) {
    const AtomExportPort &port = involvedPort(resource);
    
    if (isDisabled(port)) {
      mDisabledPorts.fetch_sub(1);
    }
  }
  
  // update urgent ports
  if (isUrgentPorts(resource)) {
    for (vector<const AtomExportPort *>::const_iterator portIt = urgentPorts(resource).begin() ;
         portIt != urgentPorts(resource).end() ;
         ++portIt) {
      const AtomExportPort &port = **portIt;

      assert(port.hasUrgency());
      
      mMissingUrgencies.fetch_add(1);

      if (isUrgent(port)) {
        mUrgencies.fetch_sub(1);
      }
    }
  }
  
  // update missing resources
  mMissingResources.fetch_add(1);
    
  unvalidate();

  /*
  if (canBeEarlyValidated()) {
    earlyValidate();
    }*/
  
  // unprotect
  mLock.unlock();
}

void ConnectorTimeSafetyValidator::free(Resource &resource) {
  // protect
  mLock.lock();

  assert(find(resources().begin(),
              resources().end(),
              &resource)
         != resources().end());
  
  // update involved ports
  if (isInvolvedPort(resource)) {
    const AtomExportPort &port = involvedPort(resource);
    
    if (isDisabled(port)) {
      mDisabledPorts.fetch_add(1);
    }
  }
  
  // update urgent ports
  if (isUrgentPorts(resource)) {
    for (vector<const AtomExportPort *>::const_iterator portIt = urgentPorts(resource).begin() ;
         portIt != urgentPorts(resource).end() ;
         ++portIt) {
      const AtomExportPort &port = **portIt;

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

void ConnectorTimeSafetyValidator::initialize() {
  if (!connector().hasExportedPort() && !disabledTimeSafety()) {
    // add interactions resources
    vector<Resource *> allInteractionsResources = connector().maximalInteractions().allResources();
    addResources(allInteractionsResources);

    // add inviarant resources
    if (includesInvariants()) {
      vector<Resource *> allInvariantResources = connector().invariant().allResources();
      addResources(allInvariantResources);
    }

    // map resources of involved ports
    if (connector().areAllRendezVous()) {      
      vector<const AtomExportPort *> ports = involvedPorts();

      for (vector<const AtomExportPort *>::const_iterator portIt = ports.begin() ;
           portIt != ports.end() ;
           ++portIt) {
        const AtomExportPort &port = **portIt;
        const Resource &resource = port.resource();

        assert(mInvolvedPorts.find(&resource) == mInvolvedPorts.end());
        
        mInvolvedPorts[&resource] = &port;
      }
    }
    
    // required for by first reset of ports
    mDisabledPorts.fetch_add(mInvolvedPorts.size());

    // map resources of urgent ports
    vector<const AtomExportPort *> ports = urgentPorts();
    
    for (vector<const AtomExportPort *>::const_iterator portIt = ports.begin() ;
         portIt != ports.end() ;
         ++portIt) {
      const AtomExportPort &port = **portIt;
      const Resource &resource = port.resource();

      assert(port.hasUrgency());
      
      mUrgentPorts[&resource].push_back(&port);      
    }
  }

  mTimeSafe = Interval(TimeValue::MIN, TimeValue::MAX);
}

bool ConnectorTimeSafetyValidator::canBeEarlyValidated() const {
  bool ret = false;

  if (!includesInvariants()) {
    ret = (connector().areAllRendezVous() && mDisabledPorts.load() > 0) ||
      (mMissingUrgencies.load() == 0 && mUrgencies.load() == 0);
  }

  return ret;
}

void ConnectorTimeSafetyValidator::unvalidate() {
  setIsValid(false);
}

void ConnectorTimeSafetyValidator::validate() {
  mTimeSafe = connector().urgent();
  
  if (includesInvariants()) {
    mTimeSafe &= connector().invariant();
  }

  setIsValid(mTimeSafe.right());
}

void ConnectorTimeSafetyValidator::earlyValidate() {
  mTimeSafe = Interval(TimeValue::MIN, TimeValue::MAX);

  setIsValid(mTimeSafe.right());
}

vector<const AtomExportPort *> ConnectorTimeSafetyValidator::involvedPorts() const {
  vector<const AtomExportPort *> ret;

  ports(connector(), ret);

  return ret;
}

vector<const AtomExportPort *> ConnectorTimeSafetyValidator::urgentPorts() const {
  vector<const AtomExportPort *> ret;

  // add all involved ports
  ports(connector(), ret, true);

  // add all ports of lower priority connectors
  for (vector<Priority *>::const_iterator priorityIt = connector().dominatedPriorities().value().begin() ;
       priorityIt != connector().dominatedPriorities().value().end() ;
       ++priorityIt) {
    const Priority &priority = **priorityIt;
      
    for (vector<const Connector *>::const_iterator connectorIt = priority.allLowConnectors().value().begin() ;
         connectorIt != priority.allLowConnectors().value().end() ;
         ++connectorIt) {
      const Connector &connector = **connectorIt;
      
      ports(connector, ret, true);
    }
  }

  return ret;
}

void ConnectorTimeSafetyValidator::ports(const Connector &connector, vector<const AtomExportPort *> &allPorts, bool onlyUrgent) const {
  for (vector<QuotedPortReference *>::const_iterator quotedPortIt = connector.ports().begin() ;
       quotedPortIt != connector.ports().end() ;
       ++quotedPortIt) {
    const QuotedPortReference &quotedPort = **quotedPortIt;
    const Port &connectorPort = quotedPort.port();

    ports(connectorPort, allPorts, onlyUrgent);
  }
}

void ConnectorTimeSafetyValidator::ports(const Port &port, vector<const AtomExportPort *> &allPorts, bool onlyUrgent) const {
  if (port.type() == ATOM_EXPORT) {
    const AtomExportPort &atomPort = dynamic_cast<const AtomExportPort &>(port);

    if (!onlyUrgent || atomPort.hasUrgency()) {
      if (find(allPorts.begin(),
               allPorts.end(),
               &atomPort)
          == allPorts.end()) {        
        allPorts.push_back(&atomPort);
      }
    }
  }
  else if (port.type() == COMPOUND_EXPORT) {
    const CompoundExportPort &compoundPort = dynamic_cast<const CompoundExportPort &>(port);

    for (vector<Port *>::const_iterator forwardPortIt = compoundPort.forwardPorts().begin() ;
         forwardPortIt != compoundPort.forwardPorts().end() ;
         ++forwardPortIt) {
      const Port &forwardPort = **forwardPortIt;

      ports(forwardPort, allPorts, onlyUrgent);
    }
  }
  else if (port.type() == CONNECTOR_EXPORT) {
    const ConnectorExportPort &connectorPort = dynamic_cast<const ConnectorExportPort &>(port);
    const Connector &connector = connectorPort.holder();
    
    ports(connector, allPorts, onlyUrgent);
  }
  else {
    assert(false);
  }
}

bool ConnectorTimeSafetyValidator::isDisabled(const AtomExportPort &port) const {
  bool ret = true;
  
  //if (!port.holder().hasResume()) {
    if (port.hasPortValues()) {
      for (vector<PortValue *>::const_iterator portValueIt = port.portValues().begin() ;
           portValueIt != port.portValues().end() ;
           ++portValueIt) {
        const PortValue &portValue = **portValueIt;

        if (portValue.hasInterval()) {
          Interval updatedInterval = portValue.interval();

          updatedInterval &= Interval(port.holder().time(), TimeValue::MAX);
          
          if (!updatedInterval.empty()) {
            ret = false;
            break;
          }
        }
        else {
          ret = false;
          break;
        }
      }
    }
    //}
  
  return ret;
}

bool ConnectorTimeSafetyValidator::isUrgent(const AtomExportPort &port) const {
  bool ret = false;
  
  if (!port.holder().hasResume()) {
    if (port.hasPortValues()) {
      for (vector<PortValue *>::const_iterator portValueIt = port.portValues().begin() ;
           portValueIt != port.portValues().end() ;
           ++portValueIt) {
        const PortValue &portValue = **portValueIt;

        if (portValue.urgency() != LAZY) {
          if (portValue.hasInterval()) {
            Interval updatedInterval = portValue.interval();

            updatedInterval &= Interval(port.holder().time(), TimeValue::MAX);
          
            if (!updatedInterval.empty()) {
              ret = true;
              break;
            }
          }
          else {
            ret = true;
            break;
          }
        }
      }
    }
  }
  
  return ret;
}

const AtomExportPort &ConnectorTimeSafetyValidator::involvedPort(const Resource &resource) const {
  map<const Resource *, const AtomExportPort *>::const_iterator it =
    mInvolvedPorts.find(&resource);

  assert(it != mInvolvedPorts.end());

  return *it->second;
}

const vector<const AtomExportPort *> &ConnectorTimeSafetyValidator::urgentPorts(const Resource &resource) const {
  map<const Resource *, vector<const AtomExportPort *>>::const_iterator it =
    mUrgentPorts.find(&resource);

  assert(it != mUrgentPorts.end());

  return it->second;
}
