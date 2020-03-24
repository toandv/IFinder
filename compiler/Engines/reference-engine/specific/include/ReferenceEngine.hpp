/**
 * Copyright Verimag laboratory.
 * 
 * contributors:
 *  Marc Pouhliès
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

#ifndef _BIP_Engine_ReferenceEngine_HPP_
#define _BIP_Engine_ReferenceEngine_HPP_

#include "Engine.hpp"
#include "Compound.hpp"
#include "ModelClock.hpp"
#include "Interval.hpp"
#include "TimingConstraint.hpp"

#include <bip-engineiface-config.hpp>

class InteractionValue;
class AtomInternalPort;
class BipError;

/** \brief Direct implementation of the BIP semantics, without any
 * optimization.
 *
 * It can be used as a reference for testing other engines, or as a
 * starting point for understanding the BIP semantics.
 */
class ReferenceEngine : public Engine {
 public:
  ReferenceEngine(Compound &top, ModelClock &modelClock, bool disableMaximalProgress = false);
  virtual ~ReferenceEngine();

  virtual TimeValue time() const { return modelClock().time(); }
  virtual BipError &resume(const TimeValue &time);
  virtual void wait(const TimeValue &time);

  virtual BipError& initialize();
  virtual BipError& execute(InteractionValue &interaction, const TimeValue &time);
  virtual BipError& execute(AtomInternalPort &internal, const TimeValue &time);
  virtual BipError& execute(AtomExternalPort &external, const TimeValue &time);
  virtual BipError& checkExternals();
  virtual const vector<InteractionValue *> &interactions() const { return mInteractions; }
  virtual const vector<AtomInternalPort *> &internals() const { return mInternals; }
  virtual const vector<AtomExternalPort *> &externals() const { return mExternals; }
  virtual const vector<AtomExternalPort *> &waiting() const { return mWaiting; }
  
  virtual Interval interval(const InteractionValue &interaction) const;
  virtual Interval interval(const AtomInternalPort &internal) const;
  virtual Interval interval(const AtomExternalPort &external) const;

  virtual State getState() const;
  virtual void setState(const State &state);

  bool disableMaximalProgress() const { return mDisableMaximalProgress; }

 protected:
  /**
   * \brief Update interactions and internals w.r.t. to the current 
   * state of the system.
   */
  BipError &update();

  /**
   * \brief Compute the set of interactions enabled at the current state.
   */
  void computeEnabledInteractions();

  /**
   * \brief Compute the set of internal ports enabled at the current state.
   */
  void computeEnabledInternals();

  void computeWaitAndResumeIntervals();

  void computeInteractionsInternalsExternals();

  bool hasEarlyEvent(AtomExternalPort &port) const;

  /**
   * \brief Returns the set of interactions enabled at the current state.
   */
  vector<InteractionValue *> &interactions() { return mInteractions; }
  vector<InteractionValue *> &enabledInteractions() { return mEnabledInteractions; }

  /**
   * \brief Returns the set of internal ports enabled at the current state.
   */
  vector<AtomInternalPort *> &internals() { return mInternals; }
  vector<AtomInternalPort *> &enabledInternals() { return mEnabledInternals; }
  /**
   * \brief Returns the set of exernal ports enabled at the current state.
   */
  vector<AtomExternalPort *> &externals() { return mExternals; }
  vector<AtomExternalPort *> &enabledExternals() { return mEnabledExternals; }

  /**
   * \brief Returns the set of exernal ports waiting at the current state.
   */
  vector<AtomExternalPort *> &waiting() { return mWaiting; }

  /**
   * \brief Returns the set of exernal ports with unexpected events at the current state.
   */
  vector<AtomExternalPort *> &unexpected() { return mUnexpected; }

  const ModelClock &modelClock() const { return mModelClock; }
  ModelClock &modelClock() { return mModelClock; }

  /**
   * \brief Set of interactions enabled at the current state.
   */
  vector<InteractionValue *> mInteractions;
  vector<InteractionValue *> mEnabledInteractions;

  /**
   * \brief Set of internal ports enabled at the current state.
   */
  vector<AtomInternalPort *> mInternals;
  vector<AtomInternalPort *> mEnabledInternals;

  /**
   * \brief Set of external ports enabled at the current state.
   */
  vector<AtomExternalPort *> mExternals;
  vector<AtomExternalPort *> mEnabledExternals;

  /**
   * \brief Set of external ports waiting at the current state.
   */
  vector<AtomExternalPort *> mWaiting;

  /**
   * \brief Set of external ports with unexpected events at the current state.
   */
  vector<AtomExternalPort *> mUnexpected;


  ModelClock &mModelClock;

  bool mDisableMaximalProgress;
};

#endif // _BIP_Engine_ReferenceEngine_HPP_
