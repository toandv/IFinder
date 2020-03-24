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

#include <Interval.hpp>
#include <TimeValue.hpp>

#include <stdlib.h>
#include <math.h>

// constructors
Interval::Interval() :
  IntervalItf(TimeValue::MIN, TimeValue::MAX) {
}

Interval::Interval(const Interval &interval) :
  IntervalItf(interval.left(), interval.right(), interval.leftOpen(), interval.rightOpen()) {
}

Interval::Interval(bool b) :
  IntervalItf(b ? TimeValue::MIN : TimeValue::MAX, b ? TimeValue::MAX : TimeValue::MIN) {
}

Interval::Interval(const TimeValue &left, const TimeValue &right) :
  IntervalItf(left, right) {
}

Interval::Interval(const TimeValue &left, const TimeValue &right, bool leftOpen, bool rightOpen) :
  IntervalItf(left, right, leftOpen, rightOpen) {
}

// destructor
Interval::~Interval() {
  /* implement your destructor here */
}

// operations
Interval &Interval::operator=(const Interval &interval) {
  setLeftOpen(interval.leftOpen());
  setRightOpen(interval.rightOpen());
  setLeft(interval.left());
  setRight(interval.right());

  return *this;
}

Interval &Interval::operator=(bool b) {
  if (b) {
    *this = Interval(TimeValue::MIN, TimeValue::MAX);
  }
  else {
    *this = Interval(TimeValue::MAX, TimeValue::MIN);
  }

  return *this;
}

// specific
bool Interval::empty() const {
  return left() > right() ||
        (left() == right() && (leftOpen() || rightOpen()));
}

bool Interval::in(const TimeValue &time) const {
  return (left() < time  || (left() == time  && !(leftOpen()))) &&
         (time < right() || (time == right() && !(rightOpen())));
}

TimeValue Interval::size() const {
  TimeValue ret = TimeValue::ZERO;

  if (!empty()) {
    ret = right() - left();
  }

  assert(ret >= TimeValue::ZERO);

  return ret;
}

TimeValue Interval::next(const TimeValue &time) const {
  TimeValue ret = TimeValue::MAX;

  if (!empty()) {
    if (in(time)) {
      ret = time;
    }
    else if (time <= left()) {
      if (leftOpen()) {
        // /!\ to handle ASAP in case of left opened interval
        // should be modified!
        TimeValue targetNext = left() + TimeValue(1, NANOSECOND);

        if (in(targetNext)) {
          ret = targetNext;
        }
      }
      else {
        ret = left();
      }
    }
  }

  return ret;
}

Interval Interval::urgent(Urgency urgency) const {
  // default empty interval
  Interval ret = Interval(TimeValue::MAX, TimeValue::MIN);

  if (!empty()) {
    switch (urgency) {
      case LAZY:
        break;

      case DELAYABLE:
        if (right() != TimeValue::MAX && !rightOpen()) {
          ret = Interval(right(), right());
        }
        break;

      case EAGER:
        ret = *this;
        break;

      default:
        assert(false);
        break;
    }
  }

  return ret;
}

Interval Interval::wait(const TimeValue &time, Urgency urgency) const {
  Interval ret = Interval(time, TimeValue::MAX);
  Interval urg = urgent(urgency);

  if (!urg.empty()) {
    if (time <= urg.left()) {
      ret = Interval(time, urgent(urgency).left());
    }
    else if (urg.in(time)) {
      ret = Interval(time, time);
    }
  }

  return ret;
}

TimeValue Interval::random(const TimeValue &granularity) const {
  assert(!empty());

  TimeValue ret;
  long long shiftInGr;

  TimeValue discreteSize = size();

  if (leftOpen()) {
    discreteSize -= TimeValue(1, NANOSECOND);
  }

  if (rightOpen()) {
    discreteSize -= TimeValue(1, NANOSECOND);
  }

  if (size() == TimeValue::MAX) {
    // exponential law
    double random0_1 = ((double) rand()) / ((double) RAND_MAX);
    shiftInGr = (long long) (- 10.0 * log(random0_1));
  }
  else {
    // unfiform law
    long long sizeInGr = discreteSize / granularity;
    shiftInGr = (long long) (((double) sizeInGr + 1) * (rand() / (RAND_MAX + 1.0)));
  }

  if (leftOpen()) {
    ret = left() + TimeValue(1, NANOSECOND) + granularity * shiftInGr;
  }
  else {
    ret = left() + granularity * shiftInGr;
  }

  assert(in(ret));

  return ret;
}

bool Interval::isIncludedIn(const Interval &interval) const {
  bool ret = false;

  if (empty()) {
    ret = true;
  }
  else if (!interval.empty()) {
    ret = ((interval.left() < left() ||
           (interval.left() == left() && (!interval.leftOpen() || leftOpen())))) &&
          ((interval.right() > right() ||
            (interval.right() == right() && (!interval.rightOpen() || rightOpen()))));
  }

  return ret;
}

Interval &Interval::operator&=(const Interval &interval) {
  if (left() < interval.left() ||
     (left() == interval.left() && !leftOpen())) {
    setLeft(interval.left());
    setLeftOpen(interval.leftOpen());
  }

  if (right() > interval.right() ||
     (right() == interval.right() && !rightOpen())) {
    setRight(interval.right());
    setRightOpen(interval.rightOpen());
  }

  return *this;
}

Interval &Interval::operator+=(const TimeValue &time) {
  left() += time;
  right() += time;

  return *this;
}

Interval &Interval::operator-=(const TimeValue &time) {
  left() -= time;
  right() -= time;

  return *this;
}

Interval Interval::operator-() const {
  return Interval(-right(), -left(), rightOpen(), leftOpen());
}

std::ostream &operator<<(std::ostream &o, const Interval &interval) {
  if (interval.leftOpen()) {
    o << "]";
  }
  else {
    o << "[";
  }

  o << " " << interval.left() << ", " << interval.right() << " ";

  if (interval.rightOpen()) {
    o << "[";
  }
  else {
    o << "]";
  }

  return o;
}

Interval operator&&(bool b, const Interval &interval) {
  return interval && b;
}

Interval operator||(bool b, const Interval &interval) {
  return interval || b;
}
