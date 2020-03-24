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

#ifndef _BIP_Engine_TimeValue_HPP_
#define _BIP_Engine_TimeValue_HPP_

// inherited classes
#include <TimeValueItf.hpp>

#include <ostream>

class Interval;

class TimeValue : public TimeValueItf {
 public:
  // constructors
  TimeValue();
  TimeValue(int value, TimeUnit unit);
  TimeValue(double value, TimeUnit unit);
  TimeValue(long long value, TimeUnit unit);
  TimeValue(const TimeValue &value);

  // destructor
  virtual ~TimeValue();

  // operations
  virtual TimeValue &operator=(const TimeValue &value) { setTime(value.time()); return *this; }
  virtual bool operator<(const TimeValue &value) const { return time() < value.time(); }
  virtual bool operator>(const TimeValue &value) const { return time() > value.time(); }
  virtual bool operator<=(const TimeValue &value) const { return time() <= value.time(); }
  virtual bool operator>=(const TimeValue &value) const { return time() >= value.time(); }
  virtual bool operator==(const TimeValue &value) const { return time() == value.time(); }
  virtual bool operator!=(const TimeValue &value) const { return time() != value.time(); }
  virtual TimeValue &operator+=(const TimeValue &value);
  virtual TimeValue &operator-=(const TimeValue &value);

  TimeValue operator+(const TimeValue &value) const { TimeValue ret = *this; ret += value; return ret; }
  TimeValue operator-(const TimeValue &value) const { TimeValue ret = *this; ret -= value; return ret; }

  TimeValue operator-() const;
  TimeValue operator*(const long long &factor) const { TimeValue ret = *this; ret.mTime = ret.mTime * factor; return ret; }
  TimeValue operator*(int factor) const { TimeValue ret = *this; ret.mTime = ret.mTime * factor; return ret; }
  TimeValue operator*(const double &factor) const { TimeValue ret = *this; ret.mTime = (long long) (factor * ret.mTime); return ret; }
  long long operator/(const TimeValue &value) const { return time() / value.time(); }
  TimeValue operator/(int factor) const { TimeValue ret = *this; ret.mTime = ret.mTime / factor; return ret; }
  TimeValue operator/(const double &factor) const { TimeValue ret = *this; ret.mTime = ret.mTime / factor; return ret; }

  // marshalling operation
  static size_t size() { return sizeof(long long); }
  void marshalling(char *b) { long long *ptr = (long long *) b; *ptr = time(); }
  void unmarshalling(const char *b) { const long long *ptr = (const long long *) b; setTime(*ptr); }

  // constants
  static const TimeValue ZERO, MIN, MAX;

  friend std::ostream &operator<<(std::ostream &o, const TimeValue &time);
  friend TimeValue operator*(int factor, const TimeValue &time);
  friend TimeValue operator*(double factor, const TimeValue &time);

 protected:
  TimeValue(long long time) : TimeValueItf(), mTime(time) { }

  const long long &time() const { return mTime; }
  long long &time() { return mTime; }
  void setTime(long long time) { mTime = time; }

  long long mTime;
};

inline TimeValue &TimeValue::operator+=(const TimeValue &value) {
  assert(!(*this == MAX && value == MIN));
  assert(!(*this == MIN && value == MAX));
  
  if (*this != MAX && *this != MIN) {
    if (value == MAX) {
      *this = MAX;
    }
    else if (value == MIN) {
      *this = MIN;
    }
    else {
      time() += value.time();
    }
  }

  return *this;
}

inline TimeValue &TimeValue::operator-=(const TimeValue &value) {
  TimeValue negValue = -value;

  return *this += negValue;
}

inline TimeValue TimeValue::operator-() const {
  TimeValue ret;

  if (*this == MIN) {
    ret = MAX;
  }
  else if (*this == MAX) {
    ret = MIN;
  }
  else {
    ret = TimeValue(-time());
  }

  return ret;
}

inline TimeValue operator*(int factor, const TimeValue &time) {
  TimeValue ret = time;
  return time * factor;
}
inline TimeValue operator*(double factor, const TimeValue &time) {
  TimeValue ret = time;
  return time * factor;
}

#endif // _BIP_Engine_TimeValue_HPP_
