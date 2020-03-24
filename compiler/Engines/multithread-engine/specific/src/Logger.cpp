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

void Logger::logProfilingExecution(const Compound &root) {
  for (map<string, Component *>::const_iterator componentIt = root.components().begin() ;
       componentIt != root.components().end() ;
       ++componentIt) {  
    Component &component = *componentIt->second;
    
    if (component.type() == COMPOUND) {
      Compound &compound = dynamic_cast<Compound &>(component);

      logProfilingExecution(compound);
    }
    else if (component.type() == ATOM) {
      Atom &atom = dynamic_cast<Atom &>(component);
      
      cout << "execution of " << atom.fullName() << ": ";
      cout << "executed " << atom.executeJob().totalNbOfExecution() << " times ";
      cout << "during " << atom.executeJob().totalExecutionTime() - atom.executeJob().totalWaitingTime();
      cout << " (";
      cout << (100*atom.executeJob().totalTransitionExecutionTime()) / (atom.executeJob().totalExecutionTime() - atom.executeJob().totalWaitingTime()) << "% transition execution, ";
      cout << (100*(atom.executeJob().totalPrologueTime() + atom.executeJob().totalEpilogueTime() + atom.executeJob().totalResumingTime())) / (atom.executeJob().totalExecutionTime() - atom.executeJob().totalWaitingTime()) << "% synchronization";
      cout << ")";
      cout << endl;
    }
  }
}

void Logger::logProfilingTotalExecution(const Compound &root) {
  unsigned int totalNbOfExecution = 0;
  TimeValue totalExecutionTime = TimeValue::ZERO;
  TimeValue totalTransitionExecutionTime = TimeValue::ZERO;
  TimeValue totalSynchronizationTime = TimeValue::ZERO;

  logProfilingTotalExecution(root, totalNbOfExecution, totalExecutionTime, totalTransitionExecutionTime, totalSynchronizationTime);

  assert(totalExecutionTime >= totalTransitionExecutionTime + totalSynchronizationTime);

  begin();
  
  outputStream() << "atomic component(s) (total): ";
  outputStream() << "executed " << totalNbOfExecution << " times ";
  outputStream() << "during " << totalExecutionTime;

  if (totalExecutionTime != TimeValue::ZERO) {
    outputStream() << " (";
    outputStream() << (100*totalTransitionExecutionTime) / totalExecutionTime << "% transition execution, ";
    outputStream() << (100*totalSynchronizationTime) / totalExecutionTime << "% synchronization";
    outputStream() << ")";
  }

  end();
}

void Logger::logProfilingTotalExecution(const Compound &root, unsigned int &totalNbOfExecution, TimeValue &totalExecutionTime, TimeValue &totalTransitionExecutionTime, TimeValue &totalSynchronizationTime) {  
  for (map<string, Component *>::const_iterator componentIt = root.components().begin() ;
       componentIt != root.components().end() ;
       ++componentIt) {  
    Component &component = *componentIt->second;
    
    if (component.type() == COMPOUND) {
      Compound &compound = dynamic_cast<Compound &>(component);

      logProfilingTotalExecution(compound, totalNbOfExecution, totalExecutionTime, totalTransitionExecutionTime, totalSynchronizationTime);
    }
    else if (component.type() == ATOM) {
      Atom &atom = dynamic_cast<Atom &>(component);
      
      totalNbOfExecution += atom.executeJob().totalNbOfExecution();
      totalExecutionTime += atom.executeJob().totalExecutionTime();
      totalTransitionExecutionTime += atom.executeJob().totalTransitionExecutionTime();
      totalSynchronizationTime += atom.executeJob().totalPrologueTime() + atom.executeJob().totalEpilogueTime() + atom.executeJob().totalResumingTime();
    }
  }
}
  
void Logger::logProfilingScheduling(const Compound &root) {  
  for (map<string, Component *>::const_iterator componentIt = root.components().begin() ;
       componentIt != root.components().end() ;
       ++componentIt) {  
    Component &component = *componentIt->second;
    
    if (component.type() == COMPOUND) {
      Compound &compound = dynamic_cast<Compound &>(component);

      logProfilingScheduling(compound);
    }
    else if (component.type() == ATOM) {
      Atom &atom = dynamic_cast<Atom &>(component);

      cout << "scheduling of internals ports of " << atom.fullName() << ": ";
      cout << "executed " << atom.executeInternalsJob().totalNbOfExecution() << " times";
      cout << ", during " << atom.executeInternalsJob().totalExecutionTime() - atom.executeInternalsJob().totalWaitingTime();
      cout << " (";
      cout << (100*(atom.executeInternalsJob().totalPrologueTime() + atom.executeInternalsJob().totalEpilogueTime())) / (atom.executeInternalsJob().totalExecutionTime() - atom.executeInternalsJob().totalWaitingTime()) << "% synchronization";
      cout << ")";
      cout << ", slept " << atom.executeInternalsJob().totalWaitingTime();
      cout << endl;
      
      cout << "scheduling of external ports of " << atom.fullName() << ": ";
      cout << "executed " << atom.executeExternalsJob().totalNbOfExecution() << " times";
      cout << ", during " << atom.executeExternalsJob().totalExecutionTime() - atom.executeExternalsJob().totalWaitingTime();
      cout << " (";
      cout << (100*(atom.executeExternalsJob().totalPrologueTime() + atom.executeExternalsJob().totalEpilogueTime())) / (atom.executeExternalsJob().totalExecutionTime() - atom.executeExternalsJob().totalWaitingTime()) << "% synchronization";
      cout << ")";
      cout << ", slept " << atom.executeExternalsJob().totalWaitingTime();
      cout << endl;
    }
  }
  
  for (vector<Connector *>::const_iterator connectorIt = root.allNonExportedConnectors().value().begin() ;
       connectorIt != root.allNonExportedConnectors().value().end() ;
       ++connectorIt) {
    Connector &connector = **connectorIt;      
    
    cout << "scheduling of interactions of " << connector.fullName() << ": ";
    cout << "executed " << connector.executeJob().totalNbOfExecution() << " times ";
    cout << "(";
    cout << (100*connector.executeJob().nbAttempt()) / connector.executeJob().totalNbOfExecution() << "% attempt, ";
    cout << (100*connector.executeJob().nbSuccess()) / connector.executeJob().totalNbOfExecution() << "% success";
    cout << ")";
    cout << ", during " << connector.executeJob().totalExecutionTime() - connector.executeJob().totalWaitingTime();
    cout << " (";
    cout << (100*(connector.executeJob().totalPrologueTime() + connector.executeJob().totalEpilogueTime())) / (connector.executeJob().totalExecutionTime() - connector.executeJob().totalWaitingTime()) << "% synchronization";
    cout << ")";
    cout << ", slept " << connector.executeJob().totalWaitingTime();
    cout << endl;
  }
}

void Logger::logProfilingTotalScheduling(const Compound &root) {
  unsigned int totalNbOfExecution = 0;
  unsigned int totalNbOfAttempt = 0;
  unsigned int totalNbOfSuccess = 0;
  TimeValue totalExecutionTime = TimeValue::ZERO;
  TimeValue totalWaitingTime = TimeValue::ZERO;
  TimeValue totalSynchronizationTime = TimeValue::ZERO;

  logProfilingTotalScheduling(root, totalNbOfExecution, totalNbOfAttempt, totalNbOfSuccess,
                              totalExecutionTime, totalWaitingTime, totalSynchronizationTime);

  assert(totalExecutionTime >= totalSynchronizationTime);

  begin();
  
  outputStream() << "scheduler(s) (total): ";
  outputStream() << "executed " << totalNbOfExecution << " times ";
  
  if (totalNbOfExecution != 0) {
    outputStream() << "(";
    outputStream() << (100*totalNbOfSuccess) / totalNbOfExecution << "% success, ";
    outputStream() << (100*(totalNbOfAttempt - totalNbOfSuccess)) / totalNbOfExecution << "% unsuccessful attempts";
    outputStream() << ") ";
  }
  
  outputStream() << "during " << totalExecutionTime;

  if (totalExecutionTime != TimeValue::ZERO) {
    outputStream() << " (";
    outputStream() << (100*totalSynchronizationTime) / totalExecutionTime << "% synchronization";
    outputStream() << ")";
  }

  outputStream() << ", slept " << totalWaitingTime;

  end();
}

void Logger::logProfilingTotalScheduling(const Compound &root, unsigned int &totalNbOfExecution, unsigned int &totalNbOfAttempt, unsigned int &totalNbOfSuccess, TimeValue &totalExecutionTime, TimeValue &totalWaitingTime, TimeValue &totalSynchronizationTime) {
  for (map<string, Component *>::const_iterator componentIt = root.components().begin() ;
       componentIt != root.components().end() ;
       ++componentIt) {  
    Component &component = *componentIt->second;
    
    if (component.type() == COMPOUND) {
      Compound &compound = dynamic_cast<Compound &>(component);

      logProfilingTotalScheduling(compound, totalNbOfExecution, totalNbOfAttempt, totalNbOfSuccess,
                                  totalExecutionTime, totalWaitingTime, totalSynchronizationTime);
    }
    else if (component.type() == ATOM) {
      Atom &atom = dynamic_cast<Atom &>(component);

      totalNbOfExecution += atom.executeInternalsJob().totalNbOfExecution();
      totalNbOfExecution += atom.executeExternalsJob().totalNbOfExecution();

      totalNbOfAttempt += atom.executeInternalsJob().nbAttempt();
      totalNbOfAttempt += atom.executeExternalsJob().nbAttempt();
      
      totalNbOfSuccess += atom.executeInternalsJob().nbSuccess();
      totalNbOfSuccess += atom.executeExternalsJob().nbSuccess();

      totalExecutionTime += atom.executeInternalsJob().totalExecutionTime() - atom.executeInternalsJob().totalWaitingTime();
      totalExecutionTime += atom.executeExternalsJob().totalExecutionTime() - atom.executeExternalsJob().totalWaitingTime();

      totalWaitingTime += atom.executeInternalsJob().totalWaitingTime();;
      totalWaitingTime += atom.executeExternalsJob().totalWaitingTime();;

      totalSynchronizationTime += atom.executeInternalsJob().totalPrologueTime() + atom.executeInternalsJob().totalEpilogueTime();
      totalSynchronizationTime += atom.executeExternalsJob().totalPrologueTime() + atom.executeExternalsJob().totalEpilogueTime();
    }
  }
  
  for (vector<Connector *>::const_iterator connectorIt = root.allNonExportedConnectors().value().begin() ;
       connectorIt != root.allNonExportedConnectors().value().end() ;
       ++connectorIt) {
    Connector &connector = **connectorIt;

    totalNbOfExecution += connector.executeJob().totalNbOfExecution();
    totalNbOfAttempt += connector.executeJob().nbAttempt();
    totalNbOfSuccess +=  connector.executeJob().nbSuccess();
    totalExecutionTime += connector.executeJob().totalExecutionTime() - connector.executeJob().totalWaitingTime();
    totalWaitingTime += connector.executeJob().totalWaitingTime();
    totalSynchronizationTime += connector.executeJob().totalPrologueTime() + connector.executeJob().totalEpilogueTime();
  }
}
