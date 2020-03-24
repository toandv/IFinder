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

#include <TimeValue.hpp>
#include <limits.h>

// constructors
TimeValue::TimeValue() :
  TimeValueItf() {
  /* implement your constructor here */
}

TimeValue::TimeValue(int value, TimeUnit unit) {
  switch (unit) {
    case NONE:
      setTime((long long) value*1000*1000*1000);
      break;

    case NANOSECOND:
      setTime((long long) value);
      break;

    case MICROSECOND:
      setTime((long long) value*1000);
      break;

    case MILLISECOND:
      setTime((long long) value*1000*1000);
      break;

    case SECOND:
      setTime((long long) value*1000*1000*1000);
      break;

    case MINUTE:
      setTime((long long) value*1000*1000*1000*60);
      break;

    case HOUR:
      setTime((long long) value*1000*1000*1000*60*60);
      break;

    case DAY:
      setTime((long long) value*1000*1000*1000*60*60*24);
      break;

    default:
      assert(false);
  }
}

TimeValue::TimeValue(double value, TimeUnit unit) {
  switch (unit) {
    case NONE:
      setTime((long long) value*1000*1000*1000);
      break;

    case NANOSECOND:
      setTime((long long) value);
      break;

    case MICROSECOND:
      setTime((long long) value*1000);
      break;

    case MILLISECOND:
      setTime((long long) value*1000*1000);
      break;

    case SECOND:
      setTime((long long) value*1000*1000*1000);
      break;

    case MINUTE:
      setTime((long long) value*1000*1000*1000*60);
      break;

    case HOUR:
      setTime((long long) value*1000*1000*1000*60*60);
      break;

    case DAY:
      setTime((long long) value*1000*1000*1000*60*60*24);
      break;

    default:
      assert(false);
  }
}

TimeValue::TimeValue(long long value, TimeUnit unit) {
  switch (unit) {
    case NONE:
      setTime(value*1000*1000*1000);
      break;

    case NANOSECOND:
      setTime(value);
      break;

    case MICROSECOND:
      setTime(value*1000);
      break;

    case MILLISECOND:
      setTime(value*1000*1000);
      break;

    case SECOND:
      setTime(value*1000*1000*1000);
      break;

    case MINUTE:
      setTime(value*1000*1000*1000*60);
      break;

    case HOUR:
      setTime(value*1000*1000*1000*60*60);
      break;

    case DAY:
      setTime(value*1000*1000*1000*60*60*24);
      break;

    default:
      assert(false);
  }
}

TimeValue::TimeValue(const TimeValue &value) :
  mTime(value.time()) {
}

// destructor
TimeValue::~TimeValue() {
  /* implement your destructor here */
}

// constants
const TimeValue TimeValue::ZERO(0);
const TimeValue TimeValue::MIN(LLONG_MIN);
const TimeValue TimeValue::MAX(LLONG_MAX);

std::ostream &operator<<(std::ostream &o, const TimeValue &time) {
  if (time == TimeValue::MIN) {
    o << "-INFTY";
  }
  else if (time == TimeValue::MAX) {
    o << "+INFTY";
  }
  else {
    TimeValue oneNs = TimeValue((long long) 1, NANOSECOND);
    long long nsTime = time / oneNs;

    long long usTime, msTime, sTime, minTime, hTime;
    long long dDisp = 0;
    int nsDisp = 0, usDisp = 0, msDisp = 0, sDisp = 0, minDisp = 0, hDisp = 0;

    if (nsTime < 0) {
      nsTime = -nsTime;
      o << "-";
    }

    nsDisp = nsTime % 1000;  // get nanoseconds 0-999
    usTime = nsTime / 1000;

     usDisp = usTime % 1000; // get microseconds 0-999
     msTime = usTime / 1000;

     msDisp = msTime % 1000; // get milliseconds 0-999
      sTime = msTime / 1000;

      sDisp = sTime % 60;    // get seconds 0-59
    minTime = sTime / 60;    

    minDisp = minTime % 60;  // get minutes
      hTime = minTime / 60;

      hDisp = hTime % 24;    // get hours
      dDisp = hTime / 24;    // get days

    if   (dDisp != 0) o <<   dDisp << "day";
    if   (hDisp != 0) o <<   hDisp <<  "hr";
    if (minDisp != 0) o << minDisp << "min";
    if   (sDisp != 0) o <<   sDisp <<   "s";
    if  (msDisp != 0) o <<  msDisp <<  "ms";
    if  (usDisp != 0) o <<  usDisp <<  "us";
    if  (nsDisp != 0) o <<  nsDisp <<  "ns";

    if (dDisp == 0 &&
        hDisp == 0 &&
      minDisp == 0 &&
        sDisp == 0 &&
       msDisp == 0 &&
       usDisp == 0 &&
       nsDisp == 0) o << 0;
  }

  return o;
}

// constructors
AtomicTimeValue::AtomicTimeValue() {
  mTime.store(TimeValue::ZERO.time());
}

// destructor
AtomicTimeValue::~AtomicTimeValue() {
}
