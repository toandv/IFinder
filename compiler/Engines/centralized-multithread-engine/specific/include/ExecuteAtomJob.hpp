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

#ifndef _BIP_Engine_ExecuteAtomJob_HPP_
#define _BIP_Engine_ExecuteAtomJob_HPP_

// inherited classes
#include <Job.hpp>
#include <Scheduler.hpp>
#include "Resource.hpp"

class BipError;
class Atom;
class PortValue;
class BipError;
class Logger;
class ExecuteConnectorJob;
class AtomExportData;
class AtomExportPort;
class AtomInternalPort;
class AtomExternalPort;
class TimeValue;

class ExecuteAtomJob : public Job {
 public:
  // constructors
  ExecuteAtomJob(Atom &atom);

  // destructor
  virtual ~ExecuteAtomJob();

  // getters for references
  Atom &atom() const { return mAtom; }
  Logger &logger() { return *mLogger; }
  bool hasLogger() const { return mLogger != NULL; }
  bool asap() const { return mAsap; }
  bool firstEnabled() const { return mFirstEnabled; }
  bool relaxed() const { return mRelaxed; }
  bool disabledTimeSafety() const { return mDisabledTimeSafety; }

  bool hasChosen() const { return hasChosenPortValue() || hasChosenInternal() || hasChosenExternal(); }
  PortValue &chosenPortValue() { return *mChosenPortValue; }
  bool hasChosenPortValue() const { return mChosenPortValue != NULL; }
  AtomInternalPort &chosenInternal() { return *mChosenInternal; }
  bool hasChosenInternal() const { return mChosenInternal != NULL; }
  AtomExternalPort &chosenExternal() { return *mChosenExternal; }
  bool hasChosenExternal() const { return mChosenExternal != NULL; }
  const TimeValue &chosenTime() const { return mChosenTime; }
  const TimeValue &time() const { return mTime; }
  const TimeValue &totalResumingTime() const { return mTotalResumingTime; }
  const TimeValue &totalTransitionExecutionTime() const { return mTotalTransitionExecutionTime; }

  // setters
  void setLogger(Logger &logger) { mLogger = &logger; }
  void clearLogger() { mLogger = NULL; }
  void setAsap(bool b) { mAsap = b; }
  void setFirstEnabled(bool b) { mFirstEnabled = b; }
  void setRelaxed(bool b) { mRelaxed = b; }
  void setDisabledTimeSafety(bool b) { mDisabledTimeSafety = b; }
  void setChosenPortValue(PortValue &chosenPortValue) { assert(!hasChosen()); mChosenPortValue = &chosenPortValue; }
  void clearChosenPortValue() { mChosenPortValue = NULL; }
  void setChosenInternal(AtomInternalPort &chosenInternal) { assert(!hasChosen()); mChosenInternal = &chosenInternal; }
  void clearChosenInternal() { mChosenInternal = NULL; }
  void setChosenExternal(AtomExternalPort &chosenExternal) { assert(!hasChosen()); mChosenExternal = &chosenExternal; }
  void clearChosenExternal() { mChosenExternal = NULL; }
  void setChosenTime(const TimeValue &chosenTime) { mChosenTime = chosenTime; }

  void addReserver(Job &job);

  // operations
  BipError &checkTimeSafetyAndResume();
  BipError &checkTimeSafety(const TimeValue &time);
  BipError &resume(const TimeValue &time);

  void configureResources();

  class ResumeJoiner : public Job::Joiner {
   public:
    // constructors
    ResumeJoiner() { }
    // ResumeJoiner(FastMutexGroup &reserved) : mReserved(&reserved) { }

    // destructor
    virtual ~ResumeJoiner() { }

   protected:
    // bool hasReserved() const { return mReserved != NULL; }
    // FastMutexGroup &reserved() { return *mReserved; }

    virtual void epilogue();
    // FastMutexGroup *mReserved;
  };

 protected:
  // operations
  virtual void realJob();
  BipError &execute();
  virtual void epilogue();

  Interval timeSafe() const;

  void initializeAllExternalPorts();

  void setReady();
  void unsetReady();
  void recomputeResources();
  void recomputeModifications();

  bool mInitialized;
  Atom &mAtom;
  Logger *mLogger;
  
  // reservation mechanisms
  bool mInvariantIsModified;
  vector<AtomExportPort *> mModifiedPorts;
  vector<AtomInternalPort *> mModifiedInternalPorts;
  vector<AtomExternalPort *> mModifiedExternalPorts;
  vector<AtomExportData *> mModifiedData;
  vector<Resource *> mResources;
  vector<Resource *> mResumeResources;

  PortValue *mChosenPortValue;
  AtomInternalPort *mChosenInternal;
  AtomExternalPort *mChosenExternal;
  TimeValue mChosenTime;
  TimeValue mTime;

  bool mAsap;
  bool mFirstEnabled;
  bool mRelaxed;
  bool mDisabledTimeSafety;

  // profiling
  TimeValue mTotalResumingTime;
  TimeValue mTotalTransitionExecutionTime;
};

#endif // _BIP_Engine_ExecuteAtomJob_HPP_
