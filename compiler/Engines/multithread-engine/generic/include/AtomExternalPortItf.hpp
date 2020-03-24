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

#ifndef _BIP_Engine_AtomExternalPortItf_HPP_
#define _BIP_Engine_AtomExternalPortItf_HPP_

#include <bip-engineiface-config.hpp>
using namespace bipbasetypes;
using namespace biptypes;

// referenced classes
class Atom;
class Component;
class PortValue;
class Interval;
class TimeValue;


class AtomExternalPortItf {
 public:
  // destructor
  virtual ~AtomExternalPortItf();

  // operations
  virtual void notify() = 0;
  virtual TimeValue time() = 0;
  virtual void initialize() = 0;
  virtual bool hasEvent() const = 0;
  virtual void popEvent() = 0;
  virtual void purgeEvents() = 0;
  virtual TimeValue eventTime() const = 0;
  virtual bool isReady() const = 0;
  virtual void setReady() = 0;

  // getters for references
  Interval &interval() const { return *mInterval; }
  bool hasInterval() const { return mInterval != NULL; }

  // getters for attributes
  const string &name() const { return mName; }
  const EventConsumptionPolicy &policy() const { return mPolicy; }
  bool hasEarlyUpdate() const { return mHasEarlyUpdate; }
  bool hasUrgency() const { return mHasUrgency; }
  bool waiting() const { return mWaiting; }
  Urgency urgency() const { return mUrgency; }
  bool hasResume() const { return mHasResume; }

  // setters for references
  void setInterval(Interval &interval) { mInterval = &interval; }
  void clearInterval() { mInterval = NULL; }

  // setters for attributes
  void setWaiting(bool waiting) { mWaiting = waiting; }
  void setUrgency(Urgency urgency) { mUrgency = urgency; }
  void setHasResume(bool resume) { mHasResume = resume; }

  // opposites accessors
  Atom &holder() const { return *mHolder; }
  bool hasHolder() const { return mHolder != NULL; }
  void setHolder(Atom &atom) { mHolder = &atom; }

 protected:
  // protected constructors
  AtomExternalPortItf(const string &name, const EventConsumptionPolicy &policy);
  AtomExternalPortItf(const string &name, const EventConsumptionPolicy &policy, bool hasEarlyUpdate, bool hasUrgency);
  
  // protected setters for attributes
  void setHasEarlyUpdate(bool hasEarlyUpdate) { mHasEarlyUpdate = hasEarlyUpdate; }
  void setHasUrgency(bool hasUrgency) { mHasUrgency = hasUrgency; }

  // attributes
  const string mName;
  const EventConsumptionPolicy mPolicy;
  bool mHasEarlyUpdate;
  bool mHasUrgency;
  bool mWaiting;
  Urgency mUrgency;
  bool mHasResume;

  // references
  Interval *mInterval;

  // opposites
  Atom *mHolder;
  
  // specific rights for deploying the system
  friend Component *deploy(int argc, char** argv);
};

#endif // _BIP_Engine_AtomExternalPortItf_HPP_
