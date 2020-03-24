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

#ifndef _BIP_Engine_TimingConstraint_HPP_
#define _BIP_Engine_TimingConstraint_HPP_

// inherited classes
#include <TimeValue.hpp>
#include <Interval.hpp>

class TimingConstraint {
 public:
  // constructor
  TimingConstraint();
  TimingConstraint(const TimingConstraint &constraint);
  TimingConstraint(Urgency urgency);
  TimingConstraint(Urgency urgency, const TimeValue &left, const TimeValue &right);
  TimingConstraint(Urgency urgency, const Interval &interval);

  // destructor
  virtual ~TimingConstraint();

  // getters for attributes
  const Interval &interval() const { return mInterval; }
  Urgency urgency() const { return mUrgency; }

  // setters for attributes
  void setUrgency(Urgency urgency) { mUrgency = urgency; }

  // operations
  bool empty() const { return interval().empty(); }
  bool in(const TimeValue &time) const { return interval().in(time); }
  TimeValue size() const { return interval().size(); }
  TimeValue next(const TimeValue &time) const { return interval().next(time); }
  Interval urgent() const { return interval().urgent(urgency()); }
  Interval wait(const TimeValue &time) const { return interval().wait(time, urgency()); }
  TimeValue shift(const TimeValue &time) const { return interval().shift(time); }
  TimeValue random(const TimeValue &granularity) const;

  TimingConstraint &operator=(const TimingConstraint &constraint);
  TimingConstraint &operator=(const Interval &i) { mInterval = i; return *this; }

  TimingConstraint &operator&=(const Interval &i) { mInterval &= i; return *this; }
  TimingConstraint &operator&=(const TimingConstraint &constraint);

  void applyPriority(const TimingConstraint &constraint, const Interval &invariant, const TimeValue &time);
  void applyUrgencyInheritance(const TimingConstraint &constraint, const Interval &invariant, const TimeValue &time);

 protected:
  // attributes
  Urgency mUrgency;
  Interval mInterval;
};

std::ostream &operator<<(std::ostream &o, const TimingConstraint &constraint);

#endif // _BIP_Engine_TimingConstraint_HPP_
