/**
 * Copyright Verimag laboratory.
 * 
 * contributors:
 *  Jacques Combaz (jacques.combaz@univ-grenoble-alpes.fr)
 *  Marc Pouhliès
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

#ifndef _BIP_Engine_Compound_HPP_
#define _BIP_Engine_Compound_HPP_

// inherited classes
#include <CompoundItf.hpp>
#include "Component.hpp"
#include "CompoundExportPort.hpp"
#include "CompoundExportData.hpp"
#include "Connector.hpp"
#include "Priority.hpp"

class InteractionValue;
class AtomInternalPort;
class AtomExternalPort;
class BipError;
class CycleInPriorities;

class Compound : public virtual ComponentItf, public Component, public CompoundItf {
 public:
  // constructors
  Compound(const string &name);

  // destructor
  virtual ~Compound();

  // specific
  virtual BipError &execute(PortValue &portValue, const TimeValue &time);
  BipError &execute(InteractionValue &interactionValue, const TimeValue &time);
  virtual BipError &initialize();
  void initializeAllAtomExternalPorts();
  BipError &update();
  void resetAll();
  BipError &recursiveUpdate();
  const Interval &invariant() const { return mInvariant; }
  const Interval &resume() const { return mResume; }
  const Interval &wait() const { return mWait; }
  BipError &resume(const TimeValue &time);
  bool hasResume() const { return mHasResume; }
  void setTime(const TimeValue &time);
  virtual const TimeValue &time() const { return mTime; }

  const vector<Connector *> &nonExportedConnectors() const { return mNonExportedConnectors.value(); }
  const vector<AtomInternalPort *> &nonExportedInternalPorts() const { return mNonExportedInternalPorts.value(); }
  const vector<AtomExternalPort *> &externalPorts() const { return mExternalPorts.value(); }
  const vector<Connector *> &allNonExportedConnectors() const { return mAllNonExportedConnectors.value(); }
  const vector<AtomInternalPort *> &allNonExportedInternalPorts() const { return mAllNonExportedInternalPorts.value(); }
  const vector<AtomExternalPort *> &allExternalPorts() const { return mAllExternalPorts.value(); }

 protected:
  // protected setters for references
  void addComponent(Component &component) {
    mComponents[component.name()] = &component;
    component.setHolder(*this);
  }

  void addPort(CompoundExportPort &port) {
    mPorts[port.name()] = &port;
    port.setHolder(*this);
    Component::addPort(port);
  }

  void addPriority(Priority &priority) {
    mPriorities.push_back(&priority);
    priority.setHolder(*this);
  }

  void addConnector(Connector &connector) {
    mConnectors[connector.name()] = &connector;
    connector.setHolder(*this);
  }

  void addData(CompoundExportData &data) {
    mData[data.name()] = &data;
    data.setHolder(*this);
    Component::addData(data);
  }

  // specific
  void computeNonExportedConnectors(vector<Connector *> &nonExportedConnectors);
  void computeNonExportedInternalPorts(vector<AtomInternalPort *> &nonExportedInternalPorts);
  void computeExternalPorts(vector<AtomExternalPort *> &externalPorts);
  void computeAllNonExportedConnectors(vector<Connector *> &allNonExportedConnectors);
  void computeAllNonExportedInternalPorts(vector<AtomInternalPort *> &allNonExportedInternalPorts);
  void computeAllExternalPorts(vector<AtomExternalPort *> &allExternalPorts);
  void computeAllCyclesInPriorities(set<CycleInPriorities *> &cycles);

  void computeCycles() const;
  void allCyclesFrom(Priority &priority, set<CycleInPriorities> &cycles, vector<Priority *> &path);

  BipError &initializeAllAtoms();
  void initializeAllConnectorsPriorities();

  void recomputeInvariantResumeWait();
  void recomputeWaitAfterResume();

  const set<CycleInPriorities *> &allCyclesInPriorities() const { return mAllCyclesInPriorities.value(); }

  mutable Initializable<vector<Connector *>, Compound> mNonExportedConnectors;
  mutable Initializable<vector<AtomInternalPort *>, Compound> mNonExportedInternalPorts;
  mutable Initializable<vector<AtomExternalPort *>, Compound> mExternalPorts;
  mutable Initializable<vector<Connector *>, Compound> mAllNonExportedConnectors;
  mutable Initializable<vector<AtomInternalPort *>, Compound> mAllNonExportedInternalPorts;
  mutable Initializable<vector<AtomExternalPort *>, Compound> mAllExternalPorts;
  mutable Initializable<set<CycleInPriorities *>, Compound> mAllCyclesInPriorities;

  TimeValue mTime;
  Interval mInvariant;
  Interval mResume;
  bool mHasResume;
  Interval mWait;

  // specific rights for deploying the system
  friend Component* deploy(int argc, char** argv);
};

#endif // _BIP_Engine_Compound_HPP_
