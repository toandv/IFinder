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

#include "Logger.hpp"
#include "BipError.hpp"
#include "InteractionValue.hpp"
#include "AtomInternalPort.hpp"

Logger::Logger(ostream &outputStream, bool verbose, unsigned int limit) :
  mOutputStream(outputStream),
  mVerbose(verbose),
  mLimit(limit),
  mState(0),
  mIsDisplaying(false) {
}

Logger::~Logger() {
}

void Logger::logTimeSafetyViolation(const TimeValue &timeSafetyViolation, const Connector &connector) {
  outputStream() << "state #" << mState << " and global time " << timeSafetyViolation << ": violation of the following timing constraints in " << connector.fullName() << ":";

  const vector<InteractionValue *> &maximalInteractions = connector.maximalInteractions();

  for (vector<InteractionValue *>::const_iterator interactionIt = maximalInteractions.begin() ;
       interactionIt != maximalInteractions.end() ;
       ++interactionIt) {
    InteractionValue &interaction = **interactionIt;
        
    if (!interaction.timingConstraint().wait(interaction.time()).in(timeSafetyViolation)) {
      newLine();
      outputStream() << "  " << interaction.connector().fullName() << ": " << interaction << " " << interaction.timingConstraint();
    }
  }

  for (vector<const Atom *>::const_iterator atomIt = connector.allAtoms().value().begin() ;
       atomIt != connector.allAtoms().value().end() ;
       ++atomIt) {
    const Atom &atom = **atomIt;

    if (atom.hasResume()) {
      if (!atom.resume().in(timeSafetyViolation)) {
        newLine();
        outputStream() << "  " << atom.fullName() << " resume " << atom.resume();
      }
    }
    else {
      // display invariant violated by timeSafetyViolation
      if (atom.hasInvariant()) {
        if (!atom.invariant().in(timeSafetyViolation)) {
          newLine();
          outputStream() << "  " << atom.fullName() << " invariant " << atom.invariant();
        }
      }
    }
  }
}

void Logger::logTimeSafetyViolation(const TimeValue &timeSafetyViolation, const Atom &atom) {
  outputStream() << "state #" << mState << " and global time " << timeSafetyViolation << ": violation of the following timing constraint " << atom.fullName() << ":";
  if (atom.hasResume()) {
    if (!atom.resume().in(timeSafetyViolation)) {
      newLine();
      outputStream() << "  " << atom.fullName() << " resume " << atom.resume();
    }
  }
  else {
    // display invariant violated by timeSafetyViolation
    if (atom.hasInvariant()) {
      if (!atom.invariant().in(timeSafetyViolation)) {
        newLine();
        outputStream() << "  " << atom.fullName() << " invariant " << atom.invariant();
      }
    }

    // display timing constraints of internal ports violated by timeSafetyViolation
    for (map<string, AtomInternalPort *>::const_iterator portIt = atom.internalPorts().begin() ;
         portIt != atom.internalPorts().end() ;
         ++portIt) {
      AtomInternalPort &port = *portIt->second;

      if (!port.isExported()) {
        if (port.hasPortValue()) {
          TimingConstraint constraint = TimingConstraint(port.portValue().urgency(), Interval(TimeValue::MIN, TimeValue::MAX));

          if (port.portValue().hasInterval()) {
            constraint &= port.portValue().interval();
          }

          if (!constraint.wait(atom.time()).in(timeSafetyViolation)) {
            newLine();
            outputStream() << "  " << port.holder().fullName() << "." << port.name() << " " << constraint;
          }
        }
      }
    }
    
    // display timing constraints of external ports violated by timeSafetyViolation
    for (map<string, AtomExternalPort *>::const_iterator portIt = atom.externalPorts().begin() ;
         portIt != atom.externalPorts().end() ;
         ++portIt) {
      AtomExternalPort &port = *portIt->second;

      if (port.hasEvent()) {
        TimingConstraint constraint = TimingConstraint(port.urgency(), Interval(port.eventTime(), TimeValue::MAX));

        if (port.hasInterval()) {
          constraint &= port.interval();
        }

        if (!constraint.wait(atom.time()).in(timeSafetyViolation)) {
          newLine();
          outputStream() << "  " << port.holder().fullName() << "." << port.name() << " " << constraint;
        }
      }
    }
  }
}

void Logger::logDeadlock(const Compound &root) {
  if (verbose()) {
    begin();

    Interval wait = Interval(TimeValue::MIN, TimeValue::MAX);

    TimeValue time = TimeValue::MIN;

    // compute waiting times allowed w.r.t. atoms invariants
    for (vector<Atom *>::const_iterator atomIt = root.allAtoms().value().begin() ;
         atomIt != root.allAtoms().value().end() ;
         ++atomIt) {
      Atom &atom = **atomIt;

      wait &= atom.timeSafetyValidator().timeSafe();

      if (atom.hasInvariant()) {
        wait &= atom.invariant();
      }

      if (atom.time() > time) {
        time = atom.time();
      }
    }

    // compute waiting times allowed w.r.t. connectors
    for (vector<Connector *>::const_iterator connectorIt = root.allNonExportedConnectors().value().begin() ;
         connectorIt != root.allNonExportedConnectors().value().end() ;
         ++connectorIt) {
      Connector &connector = **connectorIt;

      wait &= connector.timeSafetyValidator().timeSafe();
    }

    outputStream() << "state #" << mState << " and global time " << time << ": ";

    if (wait.right() == TimeValue::MAX) {
      outputStream() << "deadlock!";
    }
    else {
      if (wait.rightOpen()) {
        outputStream() << "time-lock right before " << wait.right() << "!";
      }
      else {
        outputStream() << "time-lock at " << wait.right() << "!";
      }
    }

    end();
  }
}
