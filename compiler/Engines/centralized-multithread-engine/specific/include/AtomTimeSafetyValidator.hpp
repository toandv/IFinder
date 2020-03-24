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

#ifndef _BIP_Engine_AtomTimeSafetyValidator_HPP_
#define _BIP_Engine_AtomTimeSafetyValidator_HPP_

#include <bip-engineiface-config.hpp>
using namespace bipbasetypes;
using namespace biptypes;

#include <Resource.hpp>
#include <Interval.hpp>
#include <FastMutex.hpp>

class Logger;
class Atom;
class TimeValue;
class Interval;
class AtomInternalPort;
class AtomExternalPort;

class AtomTimeSafetyValidator : public Resource::Validator {
 public:
  // constructors
  AtomTimeSafetyValidator(Atom &atom);

  // destructor
  virtual ~AtomTimeSafetyValidator();

  // getters
  Logger &logger() { return *mLogger; }
  bool hasLogger() const { return mLogger != NULL; }
  Atom &atom() const { return mAtom; }
  bool disabledTimeSafety() const { return mDisabledTimeSafety; }
  bool includesInvariants() const { return mIncludesInvariants; }
  const Interval &timeSafe() const { if (disabledTimeSafety()) mTimeSafe = waitInterval(); return mTimeSafe; }
  
  // setters
  void setLogger(Logger &logger) { mLogger = &logger; }
  void clearLogger() { mLogger = NULL; }
  void setDisabledTimeSafety(bool b) { mDisabledTimeSafety = b; }
  void setIncludesInvariants(bool b) { mIncludesInvariants = b; }

  // operations
  virtual void use(Resource &resource);
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
  bool isUrgent(const AtomInternalPort &port) const;
  bool isUrgent(const AtomExternalPort &port) const;
  bool isUrgentInternalPorts(const Resource &resource) const { return mUrgentInternalPorts.find(&resource) != mUrgentInternalPorts.end(); }
  bool isUrgentExternalPorts(const Resource &resource) const { return mUrgentExternalPorts.find(&resource) != mUrgentExternalPorts.end(); } 
  const vector<const AtomInternalPort *> &urgentInternalPorts(const Resource &resource) const;
  const vector<const AtomExternalPort *> &urgentExternalPorts(const Resource &resource) const;
  
  Logger *mLogger;
  Atom &mAtom;
  bool mDisabledTimeSafety;
  bool mIncludesInvariants;
  
  FastMutex mLock;
  atomic<unsigned int> mMissingResources;
  atomic<unsigned int> mMissingUrgencies;
  atomic<unsigned int> mUrgencies;
  mutable Interval mTimeSafe;
  // mutable AtomicTimeValue mDeadline;
  
  map<const Resource *, vector<const AtomInternalPort *>> mUrgentInternalPorts;  
  map<const Resource *, vector<const AtomExternalPort *>> mUrgentExternalPorts;
};

#endif // _BIP_Engine_AtomTimeSafetyValidator_HPP_
