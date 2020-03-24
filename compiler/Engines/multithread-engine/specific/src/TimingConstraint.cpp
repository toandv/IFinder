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

#include <TimingConstraint.hpp>

// constructors
TimingConstraint::TimingConstraint() :
  mUrgency(LAZY),
  mInterval(Interval(TimeValue::MIN, TimeValue::MAX)) {
}

TimingConstraint::TimingConstraint(const TimingConstraint &constraint) :
  mUrgency(constraint.urgency()),
  mInterval(constraint.interval()) {
}

TimingConstraint::TimingConstraint(Urgency urgency) :
  mUrgency(urgency),
  mInterval(Interval(TimeValue::MIN, TimeValue::MAX)) {
}

TimingConstraint::TimingConstraint(Urgency urgency, const TimeValue &left, const TimeValue &right) :
  mUrgency(urgency),
  mInterval(Interval(left, right)) {
}

TimingConstraint::TimingConstraint(Urgency urgency, const Interval &interval) :
  mUrgency(urgency),
  mInterval(interval) {
}

// destructor
TimingConstraint::~TimingConstraint() {
}

TimeValue TimingConstraint::random(const TimeValue &granularity) const {
  assert(!empty());

  TimeValue randomShift = Interval(TimeValue::ZERO, size()).random(granularity);
  TimeValue ret = shift(randomShift);

  assert(in(ret));

  return ret;
}



TimingConstraint &TimingConstraint::operator=(const TimingConstraint &constraint) {
  // set urgency
  setUrgency(constraint.urgency());
  
  // set interval
  mInterval = constraint.interval();

  return *this;
}

TimingConstraint &TimingConstraint::operator&=(const TimingConstraint &constraint) {
  // keep the maximal urgency
  if (constraint.urgency() > urgency()) {
    setUrgency(constraint.urgency());
  }

  // intersect with target interval
  mInterval &= constraint.interval();

  return *this;
}

void TimingConstraint::applyPriority(const TimingConstraint &constraint, const Interval &invariant, const TimeValue &time) {  
  Interval waitInterval = wait(time) && invariant;

  if (!waitInterval.empty()) {
#ifndef NDEBUG
    TimeValue potentialTimeLock = waitInterval.right();
    bool localTimeLock = !in(potentialTimeLock) && !constraint.in(potentialTimeLock);
#endif
    Interval effectiveInterval = constraint.interval() && waitInterval;

    if (!effectiveInterval.empty()) {
      Interval complementRight(effectiveInterval.right(), TimeValue::MAX, !effectiveInterval.rightOpen(), false);
      mInterval &= complementRight;
    }
    else {
      Interval complementLeft(TimeValue::MIN, constraint.interval().left(), false, !constraint.interval().leftOpen());
      Interval complementRight(constraint.interval().right(), TimeValue::MAX, !constraint.interval().rightOpen(), false);

      if (complementLeft.in(time)) {
        mInterval &= complementLeft;
      }
      else {
        mInterval &= complementRight;
      }
    }

    // deadline of lower priority interaction should not decrease
    assert(potentialTimeLock <= wait(time).right());

    // no time-lock is introduced by the application of priorities
    assert(localTimeLock || in(potentialTimeLock) || constraint.in(potentialTimeLock));
  }
}

void TimingConstraint::applyUrgencyInheritance(const TimingConstraint &constraint, const Interval &invariant, const TimeValue &time) {
  Interval waitInterval = constraint.wait(time) && invariant;
  if (!waitInterval.empty()) {
    TimeValue potentialTimeLock = waitInterval.right();

    if (wait(time).right() > potentialTimeLock) {
      if (in(potentialTimeLock)) {
        *this &= Interval(TimeValue::MIN, potentialTimeLock);

        if (urgency() == LAZY) {
          if (time == potentialTimeLock &&
              constraint.urgency() == EAGER) {
            setUrgency(EAGER);
          }
          else {
            setUrgency(DELAYABLE);
          }
        }
      
        assert(wait(time).right() <= potentialTimeLock);
      }
    }
  }
}

std::ostream &operator<<(std::ostream &o, const TimingConstraint &constraint) {
  o << constraint.interval();

  if (constraint.urgency() == LAZY) {
    o << " lazy";
  }
  else if (constraint.urgency() == DELAYABLE) {
    o << " delayable";
  }
  else if (constraint.urgency() == EAGER) {
    o << " eager";
  }
  else {
    assert(false);
  }

  return o;
}
