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

#ifndef _BIP_Engine_Connector_HPP_
#define _BIP_Engine_Connector_HPP_

// inherited classes
#include <ConnectorItf.hpp>
#include "ConnectorExportPort.hpp"
#include "QuotedPortReference.hpp"

#include "Initializable.hpp"
#include "Resetable.hpp"
#include <ConnectorTimeSafetyValidator.hpp>

#include <TimeValue.hpp>
#include <Interval.hpp>

class InteractionValue;
class Priority;

class Connector : public ConnectorItf {
 public:
  // constructors
  Connector(const string &name, const bool &asyncResume);

  // destructor
  virtual ~Connector();

  // operations
  virtual const vector<Interaction *> &interactions() const { return mInteractions.value(); }

  // specific
  bool isTopLevel() const { return mIsTopLevel.value(); }
  bool isRendezVous() const { return mIsRendezVous.value(); }
  bool areAllRendezVous() const { return mAreAllRendezVous.value(); }
  Resetable<Interaction *, Connector> &allEnabledPorts() const { return mAllEnabledPorts; }
  Resetable<vector<InteractionValue *>, Connector> &enabledInteractions() const { return mEnabledInteractions; }
  Resetable<vector<InteractionValue *>, Connector> &maximalInteractions() const { return mMaximalInteractions; }
  Resetable<vector<InteractionValue *>, Connector> &locallyMaximalInteractions() const  { return mLocallyMaximalInteractions; }
  Initializable<vector<const Atom*>, Connector> &allAtoms() const { return mAllAtoms; }
  bool mayHaveUrgency() const { return mMayHaveUrgency.value(); }
  bool allLowMayHaveUrgency() const { return mAllLowMayHaveUrgency.value(); }
  
  Resetable<TimeValue, Connector> &time() const { return mTime; }
  Resetable<Interval, Connector> &invariant() const { return mInvariant; }
  Resetable<Interval, Connector> &urgent() const { return mUrgent; }
      
  void release() const;
  void release(const vector<Interaction *> &interactions) const;
  void release(const vector<InteractionValue *> &interactions) const;

  Initializable<vector<Priority *>, Connector> &dominatingPriorities() const { return mDominatingPriorities; }
  Initializable<vector<Priority *>, Connector> &dominatedPriorities() const { return mDominatedPriorities; }
  
  string fullName() const;

  void initialize();
  void initialize2();
  void initializeResetable();
  void initializeReader(bool isRealTime);
  
  const ConnectorTimeSafetyValidator &timeSafetyValidator() const { return mTimeSafetyValidator; }
  ConnectorTimeSafetyValidator &timeSafetyValidator() { return mTimeSafetyValidator; }

  bool isReady() const { return mReader.isReady(); }
  TimeValue deadline() const { return mReader.deadline(); }
  ResetableItf &choice() { return mChoice; }
  bool hasChosenInteraction() const { return mChosenInteraction != NULL; }
  InteractionValue &chosenInteraction() const { return *mChosenInteraction; }
  const TimeValue &chosenTime() const { return mChosenTime; }
  void setChoice(InteractionValue &interaction, const TimeValue &time) { mChosenInteraction = &interaction; mChosenTime = time; }
  void clearChoice() { mChosenInteraction = NULL; mChosenTime = TimeValue::MIN; }

 protected:  
  // update methods resetable objects
  void recomputeAllEnabledPorts(Interaction *&allEnabledPorts) const;
  void recomputeEnabledInteractions(vector<InteractionValue *> &enabledInteractions) const;
  void recomputeMaximalInteractions(vector<InteractionValue *> &maximalInteractions) const;
  void recomputeLocallyMaximalInteractions(vector<InteractionValue *> &locallyMaximalInteractions) const;
  void recomputeTime(TimeValue &time) const;
  void recomputeInvariant(Interval &invariant) const;
  void recomputeUrgent(Interval &urgent) const;

  // initialization methods for initializable objects
  void computeInteractions(vector<Interaction *> &interactions);
  void computeDominatingPriorities(vector<Priority *> &dominatingPriorities);
  void computeDominatedPriorities(vector<Priority *> &dominatedPriorities);
  void computeIsTopLevel(bool &isTopLevel);
  void computeIsRendezVous(bool &isRendezVous);
  void computeAreAllRendezVous(bool &areAllRendezVous);
  bool subConnectorsAreAllRendezVous(const Port &port) const;
  void computeAllAtoms(vector<const Atom *> &allAtoms);
  void computeAllAtomsOf(const Port &port, vector<const Atom *> &ret) const;
  void computeMayHaveUrgency(bool &mayHaveUrgency);
  bool mayHaveUrgency(const Connector &connector) const;
  bool mayHaveUrgency(const Port &port) const;
  void computeAllLowMayHaveUrgency(bool &allLowMayHaveUrgency);

  // protected setters for references
  void addPort(QuotedPortReference &quotedPort);
  void setExportedPort(ConnectorExportPort &exportedPort);

  // specific
  void enumerateInteractions(vector<Interaction *> &allInteractions, vector<QuotedPortReference *> &partialInteraction, unsigned int nextPortIndex) const;
  bool enumerateInteractionValues(vector<InteractionValue *> &allInteractions, const Interaction &interaction, vector<PortValue *> &partialValues, unsigned int nextPortIndex, bool keepLocallyMaximalOnly) const;
  void enumerateLocallyMaximalInteractionValues(vector<InteractionValue *> &allInteractions, Interaction &interaction, unsigned int mandatoryIndex) const;
  const Interaction &allEnabledInteraction() const { return *(Interaction *) mAllEnabledPorts; }

  bool mInitializedDependentConnectors;
  vector<Connector *> mDependentConnectors;

  bool mInitializedDependentPriorities;
  vector<Priority *> mDependentPriorities;

  mutable Resetable<Interaction *, Connector> mAllEnabledPorts;
  mutable Interaction *copyOfmAllEnabledPorts; // copy for thread-safety

  mutable Initializable<vector<Interaction *>, Connector> mInteractions;

  mutable Resetable<vector<InteractionValue *>, Connector> mEnabledInteractions;
  mutable Resetable<vector<InteractionValue *>, Connector> mMaximalInteractions;
  mutable Resetable<vector<InteractionValue *>, Connector> mLocallyMaximalInteractions;
  mutable Resetable<TimeValue, Connector> mTime;
  mutable Resetable<Interval, Connector> mInvariant;
  mutable Resetable<Interval, Connector> mUrgent;
  mutable ResetableItf mChoice;  
  
  mutable Initializable<vector<Priority *>, Connector> mDominatingPriorities;
  mutable Initializable<vector<Priority *>, Connector> mDominatedPriorities;
  mutable Initializable<bool, Connector> mIsTopLevel;
  mutable Initializable<bool, Connector> mIsRendezVous;
  mutable Initializable<bool, Connector> mAreAllRendezVous;
  mutable Initializable<vector<const Atom *>, Connector> mAllAtoms;
  mutable Initializable<bool, Connector> mMayHaveUrgency;
  mutable Initializable<bool, Connector> mAllLowMayHaveUrgency;
  
  ConnectorTimeSafetyValidator mTimeSafetyValidator;
  Resource::Reader mReader;

  InteractionValue *mChosenInteraction;
  TimeValue mChosenTime;
};

#endif // _BIP_Engine_Connector_HPP_
