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

#ifndef _BIP_Engine_Atom_HPP_
#define _BIP_Engine_Atom_HPP_

// inherited classes
#include <AtomItf.hpp>
#include "AtomExportPort.hpp"
#include "AtomExportData.hpp"
#include "AtomInternalPort.hpp"
#include "Component.hpp"
#include "ExecuteAtomJob.hpp"
#include "ExecuteInternalsJob.hpp"
#include "ExecuteExternalsJob.hpp"
#include <Initializable.hpp>
#include <ModelClock.hpp>
#include "Resource.hpp"
#include <AtomTimeSafetyValidator.hpp>

class BipError;
class Compound;
class TimeValue;
class Connector;
class CycleInPriorities;

class Atom : public virtual ComponentItf, public Component, public AtomItf {
 public:
  // constructors
  Atom(const string &name);
  Atom(const string &name, bool initialHasResume);

  // destructor
  virtual ~Atom();

  // operations
  virtual BipError &execute(PortValue &portValue, const TimeValue &time) = 0;
  virtual BipError &execute(AtomExternalPort &port, const TimeValue &time) = 0;
  virtual string toString() const { return "-"; };
  virtual const Interval &invariant() const = 0;
  virtual bool hasInvariant() const = 0;
  virtual BipError &resume(const TimeValue &time) = 0;
  virtual const Interval &resume() const = 0;
  virtual bool invariantIsReady() const { return mInvariantIsReady.load(); }
  virtual void setInvariantReady();
  
  // specific
  BipError &execute(AtomInternalPort &internalPort, const TimeValue &time);
  void reset();
  BipError &checkExternals();
  BipError &recomputeExternals();
  void initialize0() { mHasUrgentInternalExternalPort.initialize(); }
  void initializeResources();
  void initializeResetable();

  const ExecuteAtomJob &executeJob() const { return mExecuteJob; }
  ExecuteAtomJob &executeJob() { return mExecuteJob; }
  const ExecuteInternalsJob &executeInternalsJob() const { return mExecuteInternalsJob; }
  ExecuteInternalsJob &executeInternalsJob() { return mExecuteInternalsJob; }
  const ExecuteExternalsJob &executeExternalsJob() const { return mExecuteExternalsJob; }
  ExecuteExternalsJob &executeExternalsJob() { return mExecuteExternalsJob; }
  AtomTimeSafetyValidator &timeSafetyValidator() { return mTimeSafetyValidator; }
  ResetableItf &invariantReset() const { return mInvariantReset; }
  ResetableItf &timeReset() const { return mTimeReset; }
  const vector<Resource *> &resources() const { return mResources; }
  const vector<Resource *> &portsResources() const { return mPortsResources; }
  const vector<Resource *> &dataResources() const { return mDataResources; }
  Resource &portsResource() { return mPortsResource; }
  Resource &dataResource() { return mDataResource; }
  Resource &invariantResource() { return mInvariantResource; }

  Resetable<vector<AtomInternalPort *>, Atom> &internals() const { return mInternals; }
  ResetableItf &externalsReset() const { return mExternalsReset; }
  const vector<AtomExternalPort *> &externals() const { return mExternals; }

  const ModelClock &modelClock() const { return mExecuteJob.modelClock(); }
  TimeValue time() const { return mExecuteJob.time(); }

  bool hasResume() const { return mHasResume; }
  void setHasResume(bool b) { mHasResume = b; }
  bool hadResume() const { return mHadResume; }
  void setHadResume(bool b) { mHadResume = b; }

  void unsetInvariantReady() { mInvariantIsReady.store(false); }
  bool invariantIsModifiedBy(const PortValue &portValue) const;
  
  bool hasUrgentInternalExternalPort() const { return mHasUrgentInternalExternalPort.value(); }

 protected:
  // references accessors
  void addInternalPort(AtomInternalPort &internalPort) {
    mInternalPorts[internalPort.name()] = &internalPort;
    internalPort.setHolder(*this);

    // dependencies
    // mInternals.dependsOn(internalPort.timingConstraint());
  }

  void addExternalPort(AtomExternalPort &externalPort) {
    mExternalPorts[externalPort.name()] = &externalPort;
    externalPort.setHolder(*this);    
  }

  void addPort(AtomExportPort &port) {
    mPorts[port.name()] = &port;
    port.setHolder(*this);
    Component::addPort(port);
  }

  void addData(AtomExportData &data) {
    mData[data.name()] = &data;
    data.setHolder(*this);
    Component::addData(data);
  }

  // specific
  void computeHasUrgentInternalExternalPort(bool &hasUrgentInternalExternalPort);
  void recomputeInternals(vector<AtomInternalPort *> &internals) const;

  mutable Initializable<bool, Atom> mHasUrgentInternalExternalPort;
  mutable ResetableItf mInvariantReset;
  mutable ResetableItf mTimeReset;
  mutable Resetable<vector<AtomInternalPort *>, Atom> mInternals;

  mutable ResetableItf mExternalsReset;
  vector<AtomExternalPort *> mExternals;
  vector<AtomExternalPort *> mWaiting;
  vector<AtomExternalPort *> mUnexpected;

  ExecuteAtomJob mExecuteJob;
  ExecuteInternalsJob mExecuteInternalsJob;
  ExecuteExternalsJob mExecuteExternalsJob;
  AtomTimeSafetyValidator mTimeSafetyValidator;

  Resource::Reserver mReserver;
  Resource::Reserver mObservePortsReserver;

  vector<Resource *> mResources;
  vector<Resource *> mPortsResources;
  vector<Resource *> mDataResources;

  Resource mPortsResource;
  Resource mDataResource;
  Resource mInvariantResource;

  bool mHasResume;
  bool mHadResume;
  
  atomic<bool> mInvariantIsReady;

  friend ostream& operator<<(ostream &o, const Atom &atom);
  friend ostream& operator<<(ostream &o, const Atom *atom);
};

#endif // _BIP_Engine_Atom_HPP_
