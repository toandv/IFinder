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

#ifndef _BIP_Engine_IntervalItf_HPP_
#define _BIP_Engine_IntervalItf_HPP_

#include <bip-engineiface-config.hpp>
using namespace bipbasetypes;
using namespace biptypes;

// used classes
#include "TimeValue.hpp"

// referenced classes
class Interval;


class IntervalItf {
 public:
  // destructor
  virtual ~IntervalItf();

  // operations
  virtual Interval &operator=(const Interval &interval) = 0;

  // getters for references
  TimeValue &left() { return mLeft; }
  const TimeValue &left() const { return mLeft; }
  TimeValue &right() { return mRight; }
  const TimeValue &right() const { return mRight; }

  // getters for attributes
  bool leftOpen() const { return mLeftOpen; }
  bool rightOpen() const { return mRightOpen; }

  // setters for references
  void setLeft(const TimeValue &left) { mLeft = left; }
  void setRight(const TimeValue &right) { mRight = right; }

  // setters for attributes
  void setLeftOpen(bool leftOpen) { mLeftOpen = leftOpen; }
  void setRightOpen(bool rightOpen) { mRightOpen = rightOpen; }

 protected:
  // protected constructors
  IntervalItf(const TimeValue &left, const TimeValue &right);
  IntervalItf(const TimeValue &left, const TimeValue &right, bool leftOpen, bool rightOpen);

  // attributes
  bool mLeftOpen;
  bool mRightOpen;

  // references
  TimeValue mLeft;
  TimeValue mRight;
};

#endif // _BIP_Engine_IntervalItf_HPP_
