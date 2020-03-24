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

#ifndef _BIP_Engine_RandomScheduler_HPP_
#define _BIP_Engine_RandomScheduler_HPP_

#include "Scheduler.hpp"
#include "StringTree.hpp"
#include <TimeValue.hpp>
#include <State.hpp>

class Engine;
class Logger;
class BipError;
class Atom;
class Component;
class Connector;
class Compound;
class InteractionValue;
class ConnectorExportPort;
class CompoundExportPort;
class AtomInternalPort;
class AtomExternalPort;
class AtomExportPort;
class NotifiableClock;
class TimingConstraint;

/** \brief Compute execution sequences in which interactions
 * are chosen randomly.
 */
class RandomScheduler : public Scheduler {
 public:
  RandomScheduler(Engine &engine, NotifiableClock &platformClock, Logger &logger, bool debug = false, bool interactive = false, bool randomSeed = true,
                  unsigned int seed = 0, bool isRealTime = false, bool asap = false, bool firstEnabled = false, bool relaxed = false);
  virtual ~RandomScheduler();

  virtual BipError &initialize();
  virtual BipError &run();

  const NotifiableClock &platformClock() const { return mPlatformClock; }

  bool debug() const { return mDebug; }
  bool interactive() const { return mInteractive; }
  bool randomSeed() const { return mRandomSeed; }
  unsigned int seed() const { return mSeed; }
  bool isRealTime() const { return mIsRealTime; }
  bool asap() const { return mAsap; }
  bool firstEnabled() const { return mFirstEnabled; }
  bool relaxed() const { return mRelaxed; }

  static void notify();
  static NotifiableClock &staticPlatformClock() { assert(mStaticPlatformClock != NULL); return *mStaticPlatformClock; }


 protected:
  bool choose();
  void chooseRandom();
  bool chooseInteractive();
  bool choose(InteractionValue &interaction);
  bool choose(AtomInternalPort &internal);
  bool choose(AtomExternalPort &external);
  bool hasChosen() const;
  bool choose(const Interval &interval, const TimeValue &plannedTime, bool hasPriority);
  TimeValue plan(const Interval &interval) const;
  TimeValue schedulingCriterion(const Interval &interval, const TimeValue &planned) const;
  
  BipError &checkTimeSafety();
  BipError &checkTimeSafetyAndResume();
  
  bool deadlock() const;
  static NotifiableClock *mStaticPlatformClock;
  
  void print();
  StringTree print(const ConnectorExportPort &port);
  StringTree print(const CompoundExportPort &port);
  StringTree print(const AtomInternalPort &port);
  StringTree print(const AtomExternalPort &port);
  StringTree print(const AtomExportPort &port);
  StringTree print(const InteractionValue &port);
  StringTree print(const Connector &connector);
  StringTree print(const Atom &atom);
  StringTree print(const Compound &compound);
  StringTree print(const Component &component);

  NotifiableClock &platformClock() { return mPlatformClock; }

  NotifiableClock &mPlatformClock;

  bool checkIsSerializeEnabled();
  string parse(const string &cmdline, size_t &index);
  
  Logger &logger() const { return mLogger; }
  bool hasChosenInteraction() const { return mChosenInteraction != NULL; }
  bool hasChosenInternal() const { return mChosenInternal != NULL; }
  bool hasChosenExternal() const { return mChosenExternal != NULL; }
  InteractionValue &chosenInteraction() const { return *mChosenInteraction; }
  AtomInternalPort &chosenInternal() const { return *mChosenInternal; }
  AtomExternalPort &chosenExternal() const { return *mChosenExternal; }
  const TimeValue &wakeUpTime() const { return mWakeUpTime; }

  Logger &mLogger;
  bool mDebug;
  bool mInteractive;
  bool mRandomSeed;
  unsigned int mSeed;

  bool mIsRealTime;
  bool mAsap;
  bool mFirstEnabled;
  bool mRelaxed;

  InteractionValue *mChosenInteraction;
  AtomInternalPort *mChosenInternal;
  AtomExternalPort *mChosenExternal;
  TimeValue mWakeUpTime;
  TimeValue mMinCriterion;
  unsigned int mNbMinCriterion;

  map<unsigned int, State> mSavedStates;
};

#endif // _BIP_Engine_RandomScheduler_HPP_
