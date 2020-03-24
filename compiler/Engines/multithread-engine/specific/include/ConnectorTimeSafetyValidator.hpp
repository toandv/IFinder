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

#ifndef _BIP_Engine_ConnectorTimeSafetyValidator_HPP_
#define _BIP_Engine_ConnectorTimeSafetyValidator_HPP_

#include <bip-engineiface-config.hpp>
using namespace bipbasetypes;
using namespace biptypes;

#include <Resource.hpp>
#include <Interval.hpp>
#include <Atom.hpp>
#include <AtomExportPort.hpp>

class Logger;
class Connector;
class Atom;
class Port;
class TimeValue;
class Interval;
class Interaction;

class ConnectorTimeSafetyValidator : public Resource::Validator {
 public:
  // constructors
  ConnectorTimeSafetyValidator(const Connector &connector);

  // destructor
  virtual ~ConnectorTimeSafetyValidator();

  // getters
  Logger &logger() { return *mLogger; }
  bool hasLogger() const { return mLogger != NULL; }
  const Connector &connector() const { return mConnector; }
  bool disabledTimeSafety() const { return mDisabledTimeSafety; }
  bool includesInvariants() const { return mIncludesInvariants; }
  const Interval &timeSafe() const { if (disabledTimeSafety()) mTimeSafe = waitInterval(); return mTimeSafe; }
  
  // setters
  void setLogger(Logger &logger) { mLogger = &logger; }
  void clearLogger() { mLogger = NULL; }
  void setDisabledTimeSafety(bool b) { mDisabledTimeSafety = b; }
  void setIncludesInvariants(bool b) { mIncludesInvariants = b; }

  // operations
  virtual void reset(const Group<Reservable> &resources);
  virtual void free(Resource &resource);
  // virtual TimeValue deadline(const Resource &resource) const { return mDeadline; }  

  void initialize();
  
 protected:
  void reset(Resource &resource);
  bool canBeEarlyValidated() const;
  void unvalidate();
  void validate();
  void earlyValidate();
  
  void setTimeSafe(const Interval &interval) { mTimeSafe = interval; }
  void validate(const Interval &interval);
  Interval waitInterval() const;
  vector<const AtomExportPort *> involvedPorts() const;
  vector<const AtomExportPort *> urgentPorts() const;
  void ports(const Connector &connector, vector<const AtomExportPort *> &allPorts, bool onlyUrgent = false) const;
  void ports(const Port &port, vector<const AtomExportPort *> &allPorts, bool onlyUrgent = false) const;
  bool isDisabled(const AtomExportPort &port) const;
  bool isUrgent(const AtomExportPort &port) const;
  bool isInvolvedPort(const Resource &resource) const { return mInvolvedPorts.find(&resource) != mInvolvedPorts.end(); }
  bool isUrgentPorts(const Resource &resource) const { return mUrgentPorts.find(&resource) != mUrgentPorts.end(); }
  const AtomExportPort &involvedPort(const Resource &resource) const;
  const vector<const AtomExportPort *> &urgentPorts(const Resource &resource) const;
    
  Logger *mLogger;
  const Connector &mConnector;
  bool mDisabledTimeSafety;
  bool mIncludesInvariants;
  
  FastMutex mLock;
  atomic<unsigned int> mMissingResources;
  atomic<unsigned int> mMissingUrgencies;
  atomic<unsigned int> mUrgencies;
  atomic<unsigned int> mDisabledPorts;
  mutable Interval mTimeSafe;
  // mutable AtomicTimeValue mDeadline;

  map<const Resource *, const AtomExportPort *> mInvolvedPorts;
  map<const Resource *, vector<const AtomExportPort *>> mUrgentPorts;
};

#endif // _BIP_Engine_ConnectorTimeSafetyValidator_HPP_
