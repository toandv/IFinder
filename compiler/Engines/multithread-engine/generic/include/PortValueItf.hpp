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

#ifndef _BIP_Engine_PortValueItf_HPP_
#define _BIP_Engine_PortValueItf_HPP_

#include <bip-engineiface-config.hpp>
using namespace bipbasetypes;
using namespace biptypes;

// referenced classes
class Interval;
class AtomExportData;
class AtomExportPort;
class AtomInternalPort;
class AtomExternalPort;

class PortValueItf {
 public:
  // destructor
  virtual ~PortValueItf();

  // operations
  virtual string toString() const = 0;

  // getters for references
  const Interval &interval() const { return *mInterval; }
  bool hasInterval() const { return mInterval != NULL; }

  // getters for attributes
  Urgency urgency() const { return mUrgency; }
  bool hasResume() const { return mHasResume; }

  // setters for references
  void setInterval(const Interval &interval) { mInterval = &interval; }
  void clearInterval() { mInterval = NULL; }

  // setters for attributes
  void setUrgency(Urgency urgency) { mUrgency = urgency; }
  void setHasResume(bool resume) { mHasResume = resume; }

  // operations
  virtual bool hasUnmodifiedExportData() const = 0;
  virtual bool hasUnmodifiedExportPorts() const = 0;
  virtual bool hasUnmodifiedInternalPorts() const = 0;
  virtual bool hasUnmodifiedExternalPorts() const = 0;
  virtual const vector<const AtomExportData *> &unmodifiedExportData() const = 0;
  virtual const vector<const AtomExportPort *> &unmodifiedExportPorts() const = 0;
  virtual const vector<const AtomInternalPort *> &unmodifiedInternalPorts() const = 0;
  virtual const vector<const AtomExternalPort *> &unmodifiedExternalPorts() const = 0;
  virtual bool unmodifiedInvariant() const = 0;

 protected:
  // protected constructors
  PortValueItf();

  // attributes
  Urgency mUrgency;
  bool mHasResume;

  // references
  const Interval *mInterval;
};

#endif // _BIP_Engine_PortValueItf_HPP_
