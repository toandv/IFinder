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

#include <ExecuteAtomJob.hpp>
#include "AtomInternalPort.hpp"
#include "AtomExternalPort.hpp"

#include "Connector.hpp"
#include "Compound.hpp"
#include "Atom.hpp"
#include "Logger.hpp"

#include "Executer.hpp"
#include "BipError.hpp"
#include "UnexpectedEventError.hpp"

#include <TimeValue.hpp>

ExecuteAtomJob::ExecuteAtomJob(Atom &atom) :
  Job(Executer::jobs, true),
  mInitialized(false),
  mAtom(atom),
  mLogger(NULL),
  mChosenPortValue(NULL),
  mChosenInternal(NULL),
  mChosenExternal(NULL),
  mChosenTime(TimeValue::MIN),
  mTime(TimeValue::ZERO),
  mTotalResumingTime(TimeValue::ZERO),
  mTotalTransitionExecutionTime(TimeValue::ZERO) {
}

ExecuteAtomJob::~ExecuteAtomJob() {
}

void ExecuteAtomJob::realJob() {
  TimeValue transitionTime;

  if (profiling()) transitionTime = mRealTimeClock.time();
  
  // execute initial transition or choice
  BipError &error = execute();

  if (profiling()) mTotalTransitionExecutionTime += mRealTimeClock.time() - transitionTime;

  if (error.type() != NO_ERROR) {
    logger().log(error);
  }
  
  if (!hasJoiner()) {
    // check events on external ports
    BipError &error = atom().recomputeExternals();

    if (error.type() != NO_ERROR) {
      logger().log(error);
    }
    else {
      TimeValue resumingTime;

      if (profiling()) resumingTime = mRealTimeClock.time();
      
      // check time-safety and resume component
      BipError &error = checkTimeSafetyAndResume();

      if (profiling()) mTotalResumingTime += mRealTimeClock.time() - resumingTime;

      if (error.type() != NO_ERROR) {
        logger().log(error);
      }
    }
  }
}

void ExecuteAtomJob::configureResources() {
  // aquire common resources
  atom().portsResource().use();
  atom().dataResource().use();

  recomputeModifications();
  recomputeResources();

  for (vector<Resource *>::const_iterator resourceIt = mResources.begin() ;
       resourceIt != mResources.end() ;
       ++resourceIt) {
    Resource &resource = **resourceIt;

    resource.use();
  }
}

void ExecuteAtomJob::epilogue() {
  // reset necessary values
  atom().reset();
  
  // mark invariant, ports, data as ready
  setReady();
  
  // free remaining resources
  for (vector<Resource *>::const_iterator resourceIt = mResources.begin() ;
       resourceIt != mResources.end() ;
       ++resourceIt) {
    Resource &resource = **resourceIt;

    resource.free();
  }

  // release common resources
  atom().portsResource().free();
  atom().dataResource().free();
}

BipError &ExecuteAtomJob::execute() {
  BipError *error = NULL;

  if (hasChosen()) {
    mTime = chosenTime();
  }

  // mark invariant, ports, data as not ready
  unsetReady();
  
  // in case of resume 
  if (atom().hasResume()) {
    // reset invariants
    if (mInvariantIsModified) {
      atom().invariantReset().resetDependent();
    }

    // reset ports
    for (vector<AtomExportPort *>::const_iterator portIt = mModifiedPorts.begin() ;
       portIt != mModifiedPorts.end() ;
       ++portIt) {
      AtomExportPort &port = **portIt;

      port.reset().resetDependent();
    }
  }
  
  if (hasChosenPortValue()) {
    error = &atom().execute(chosenPortValue(), chosenTime());

    // reset choice
    clearChosenPortValue();
  }
  else if (hasChosenInternal()) {
    error = &atom().execute(chosenInternal(), chosenTime());

    // reset choice
    clearChosenInternal();
  }
  else if (hasChosenExternal()) {
    error = &atom().execute(chosenExternal(), chosenTime());

    // reset choice
    clearChosenExternal();
  }
  else {
    assert(!mInitialized);
    
    error = &atom().initialize();

    // initialize external ports
    initializeAllExternalPorts();
    
    mInitialized = true;
  }

  if (error->type() != NO_ERROR) {
    return *error;
  }

  // reset dependencies
  // atom().reset();

  return BipError::NoError;
}

BipError &ExecuteAtomJob::checkTimeSafetyAndResume() {
  // keep track of resume status for further urgency management
  atom().setHadResume(atom().hasResume());

  if (atom().hasResume()) {
    if (!isRealTime()) {
      Interval resume = atom().resume() && Interval(mChosenTime, TimeValue::MAX);
      
      if (!resume.empty()) {
        mTime = resume.random(TimeValue(1, NANOSECOND));
      }
      else {
        TimeSafetyViolationError &error = *new TimeSafetyViolationError(mChosenTime, atom());
      
        return error;
      }
    }
    else {
      mTime = platformClock().time();
    }
    
    // check time-safety before resuming
    BipError &error = checkTimeSafety(mTime);

    if (error.type() != NO_ERROR) {
      if (relaxed()) {
        logger().logWarning(error);

        assert(!timeSafe().empty());
        assert(!timeSafe().rightOpen());

        mTime = timeSafe().right();
      }
      else {
        return error;
      }
    }

    // actual resume
    BipError &errorResume = resume(mTime);

    if (errorResume.type() != NO_ERROR) {
      return errorResume;
    }
  }

  return BipError::NoError;
}

BipError &ExecuteAtomJob::checkTimeSafety(const TimeValue &time) {
  if (!timeSafe().in(time)) {    
    TimeSafetyViolationError &error = *new TimeSafetyViolationError(time, atom());
    
    return error;
  }

  return BipError::NoError;
}

Interval ExecuteAtomJob::timeSafe() const {
  Interval ret = Interval(mChosenTime, TimeValue::MAX);

  if (atom().hasResume()) {
    ret &= atom().resume();
  }
  else if (atom().hasInvariant()) {
    ret &= atom().invariant();
  }

  return ret;
}

BipError &ExecuteAtomJob::resume(const TimeValue &time) {
  // only resumable atom can be resumed!
  assert(atom().hasResume());
    
  BipError &error = atom().resume(time);

  if (error.type() != NO_ERROR) {
    return error;
  }

  // mark atom as resumed
  atom().setHasResume(false);

  // recompute externals
  BipError &errorExternals = atom().recomputeExternals();

  if (errorExternals.type() != NO_ERROR) {
    return errorExternals;
  }

  return BipError::NoError;
}

void ExecuteAtomJob::initializeAllExternalPorts() {
  const Atom &constAtom = atom();

  // initialize external ports
  for (map<string, AtomExternalPort *>::const_iterator portIt = constAtom.externalPorts().begin() ;
         portIt != constAtom.externalPorts().end() ;
         ++portIt) {
    AtomExternalPort &port = *portIt->second;

    port.initialize();
  }
}

void ExecuteAtomJob::ResumeJoiner::epilogue() {
  assert(false);
  
  // real-time
  assert(!jobsToJoin().empty());

  Job &job = **jobsToJoin().begin();
  ExecuteAtomJob &atomJob = dynamic_cast<ExecuteAtomJob &>(job);

  TimeValue time = atomJob.platformClock().time();

  // check events on external ports
  for (vector<Job *>::const_iterator jobIt = jobsToJoin().begin() ;
       jobIt != jobsToJoin().end() ;
       ++jobIt) {
    Job &job = **jobIt;
    ExecuteAtomJob &atomJob = dynamic_cast<ExecuteAtomJob &>(job);

    BipError &error = atomJob.atom().recomputeExternals();

    if (error.type() != NO_ERROR) {
      atomJob.logger().log(error);

      return;
    }
  }

  // perform resume
  for (vector<Job *>::const_iterator jobIt = jobsToJoin().begin() ;
       jobIt != jobsToJoin().end() ;
       ++jobIt) {
    Job &job = **jobIt;
    ExecuteAtomJob &atomJob = dynamic_cast<ExecuteAtomJob &>(job);

    BipError &error = atomJob.resume(time);

    if (error.type() != NO_ERROR) {
      atomJob.logger().log(error);

      return;
    }
  }
  
  // unreserve components
  for (vector<Job *>::const_iterator jobIt = jobsToJoin().begin() ;
       jobIt != jobsToJoin().end() ;
       ++jobIt) {
    Job &job = **jobIt;
    ExecuteAtomJob &atomJob = dynamic_cast<ExecuteAtomJob &>(job);

    atomJob.epilogue();
  }
}

void ExecuteAtomJob::setReady() {
  // mark invariant as ready
  if (mInvariantIsModified) {
    if (atom().hadResume()) {
      atom().invariantReset().resetDependent();
    }

    atom().setInvariantReady();
  }

  // mark port as ready
  for (vector<AtomExportPort *>::const_iterator portIt = mModifiedPorts.begin() ;
       portIt != mModifiedPorts.end() ;
       ++portIt) {
    AtomExportPort &port = **portIt;

    if (atom().hadResume()) {
      port.reset().resetDependent();
    }

    port.setReady();
  }

  // mark internal port as ready
  for (vector<AtomInternalPort *>::const_iterator portIt = mModifiedInternalPorts.begin() ;
       portIt != mModifiedInternalPorts.end() ;
       ++portIt) {
    AtomInternalPort &port = **portIt;

    port.setReady();
  }

  // mark internal port as ready
  for (vector<AtomExternalPort *>::const_iterator portIt = mModifiedExternalPorts.begin() ;
       portIt != mModifiedExternalPorts.end() ;
       ++portIt) {
    AtomExternalPort &port = **portIt;

    port.setReady();
  }

  // mark modified data as ready
  for (vector<AtomExportData *>::const_iterator dataIt = mModifiedData.begin() ;
       dataIt != mModifiedData.end() ;
       ++dataIt) {
    AtomExportData &data = **dataIt;

    data.setReady();
  }
}

void ExecuteAtomJob::unsetReady() {
  // mark invariant as not ready
  if (mInvariantIsModified) {
    atom().unsetInvariantReady();
  }

  // mark port as not ready
  for (vector<AtomExportPort *>::const_iterator portIt = mModifiedPorts.begin() ;
       portIt != mModifiedPorts.end() ;
       ++portIt) {
    AtomExportPort &port = **portIt;

    port.unsetReady();
  }

  // mark internal port as not ready
  for (vector<AtomInternalPort *>::const_iterator portIt = mModifiedInternalPorts.begin() ;
       portIt != mModifiedInternalPorts.end() ;
       ++portIt) {
    AtomInternalPort &port = **portIt;

    port.unsetReady();
  }

  // mark internal port as not ready
  for (vector<AtomExternalPort *>::const_iterator portIt = mModifiedExternalPorts.begin() ;
       portIt != mModifiedExternalPorts.end() ;
       ++portIt) {
    AtomExternalPort &port = **portIt;

    port.unsetReady();
  }

  // mark modified data as not ready
  for (vector<AtomExportData *>::const_iterator dataIt = mModifiedData.begin() ;
       dataIt != mModifiedData.end() ;
       ++dataIt) {
    AtomExportData &data = **dataIt;
    
    data.unsetReady();
  }
}

void ExecuteAtomJob::recomputeResources() {
  // recompute resources from scratch
  mResources.clear();
  
  // recompute resume resources from scratch
  mResumeResources.clear();
  mResumeResources.push_back(&atom().portsResource());

  // add invariant resource
  if (mInvariantIsModified) {
    mResources.push_back(&atom().invariantResource());
    mResumeResources.push_back(&atom().invariantResource());
  }

  // add port resources
  for (vector<AtomExportPort *>::const_iterator portIt = mModifiedPorts.begin() ;
       portIt != mModifiedPorts.end() ;
       ++portIt) {
    AtomExportPort &port = **portIt;

    if (port.hasEarlyUpdate()) {
      mResources.push_back(&port.resource());
      mResumeResources.push_back(&port.resource());
    }
  }

  // add internal port resources
  for (vector<AtomInternalPort *>::const_iterator portIt = mModifiedInternalPorts.begin() ;
       portIt != mModifiedInternalPorts.end() ;
       ++portIt) {
    AtomInternalPort &port = **portIt;

    if (port.hasEarlyUpdate() && !port.isExported()) {
      mResources.push_back(&port.resource());
      mResumeResources.push_back(&port.resource());
    }
  }

  // add external port resources
  for (vector<AtomExternalPort *>::const_iterator portIt = mModifiedExternalPorts.begin() ;
       portIt != mModifiedExternalPorts.end() ;
       ++portIt) {
    AtomExternalPort &port = **portIt;

    if (port.hasEarlyUpdate()) {
      mResources.push_back(&port.resource());
      mResumeResources.push_back(&port.resource());
    }
  }

  // add data resources
  for (vector<AtomExportData *>::const_iterator dataIt = mModifiedData.begin() ;
       dataIt != mModifiedData.end() ;
       ++dataIt) {
    AtomExportData &data = **dataIt;
    
    if (data.hasEarlyUpdate()) {
      mResources.push_back(&data.resource());
    }
  }
}

void ExecuteAtomJob::recomputeModifications() {
  const Atom &constAtom = atom();

  // recompute modified invariant
  mInvariantIsModified = true;
  
  if (hasChosenPortValue()) {
    if (!atom().invariantIsModifiedBy(chosenPortValue())) {
      mInvariantIsModified = false;
    }
  }
  
  // recompute modified ports from scratch
  mModifiedPorts.clear();
  
  for (map<string, AtomExportPort *>::const_iterator portIt = constAtom.ports().begin() ;
       portIt != constAtom.ports().end() ;
       ++portIt) {
    AtomExportPort &port = *portIt->second;
    bool modified = true;

    if (hasChosenPortValue()) {
      if (!port.isModifiedBy(chosenPortValue())) {
        modified = false;
      }
    }

    if (modified) {
      mModifiedPorts.push_back(&port);
    }
  }
  
  // recompute modified ports from scratch
  mModifiedInternalPorts.clear();
  
  for (map<string, AtomInternalPort *>::const_iterator portIt = constAtom.internalPorts().begin() ;
       portIt != constAtom.internalPorts().end() ;
       ++portIt) {
    AtomInternalPort &port = *portIt->second;
    bool modified = true;

    if (hasChosenPortValue()) {
      if (!port.isModifiedBy(chosenPortValue())) {
        modified = false;
      }
    }

    if (modified) {
      mModifiedInternalPorts.push_back(&port);
    }
  }
  
  // recompute modified ports from scratch
  mModifiedExternalPorts.clear();
  
  for (map<string, AtomExternalPort *>::const_iterator portIt = constAtom.externalPorts().begin() ;
       portIt != constAtom.externalPorts().end() ;
       ++portIt) {
    AtomExternalPort &port = *portIt->second;
    bool modified = true;

    if (hasChosenPortValue()) {
      if (!port.isModifiedBy(chosenPortValue())) {
        modified = false;
      }
    }

    if (modified) {
      mModifiedExternalPorts.push_back(&port);
    }
  }

  // recompute modified data from scratch
  mModifiedData.clear();
  
  for (map<string, AtomExportData *>::const_iterator dataIt = constAtom.data().begin() ;
       dataIt != constAtom.data().end() ;
       ++dataIt) {
    AtomExportData &data = *dataIt->second;
    bool modified = true;

    if (hasChosenPortValue()) {
      if (!data.isModifiedBy(chosenPortValue())) {
        modified = false;
      }
    }

    if (modified) {
      mModifiedData.push_back(&data);
    }
  }
}
