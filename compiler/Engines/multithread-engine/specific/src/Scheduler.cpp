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

#include "Scheduler.hpp"
#include <Interval.hpp>

Scheduler::Scheduler(const TimeValue &time) :
  mTime(time),
  mPlannedTime(TimeValue::MAX),
  mMinCriterion(TimeValue::MAX),
  mNbMinCriterion(0),
  mAsap(false),
  mFirstEnabled(false) {
  setTime(time);
}

Scheduler::Scheduler(const TimeValue &time, bool asap, bool firstEnabled) :
  mTime(time),
  mPlannedTime(TimeValue::MAX),
  mMinCriterion(TimeValue::MAX),
  mNbMinCriterion(0),
  mAsap(asap),
  mFirstEnabled(firstEnabled) {
  setTime(time);
}

Scheduler::~Scheduler() {
}

bool Scheduler::choose(const Interval &interval, bool hasPriority) {
  bool ret = false;

  if (!interval.empty()) {
    TimeValue plannedTime = plan(interval);
    TimeValue criterion = schedulingCriterion(interval, plannedTime);

    if (criterion < mMinCriterion || (criterion == mMinCriterion && hasPriority)) {
      // found a new minimum for the criterion
      mMinCriterion = criterion;
      mNbMinCriterion = 1;

      ret = true;
    }
    else if (criterion == mMinCriterion) {
      ++mNbMinCriterion;

      // random choice between whose which minimize the criterion
      if ((unsigned int) (mNbMinCriterion * (rand() / (RAND_MAX + 1.0))) == mNbMinCriterion - 1) {
        ret = true;
      }
    }

    if (ret) {
      mPlannedTime = plannedTime;
    }
  }

  return ret;
}

/**
 * \brief Chose an execution time meeting a given timing constraint.
 *
 * The choice is made according to the selected scheduling policy,
 * i.e. ASAP or random.
 *
 * \param interval is the target interval.
 * 
 * \return a time value meeting the timing constraint.
 */
TimeValue Scheduler::plan(const Interval &interval) const {
  TimeValue ret;

  if (asap()) {
    ret = interval.next(time());
  }
  else {
    ret = interval.random(TimeValue(1, NANOSECOND));
  }

  return ret;
}

/**
 * \brief Criterion the scheduler should minimize.
 *
 * In the case of firstEnabled scheduling policy,
 * the criterion is the next time value that belong
 * to the target interval from the current time,
 * otherwise it is the planned execution time.
 *
 * \param interval is the validity interval of the target
 * interaction / internal port / external port to schedule.
 * \param planned is the planned time for execution of the
 * target interaction / internal port / external.
 * 
 * \return the time value defining the scheduling
 * criterion to be minimized.
 */
TimeValue Scheduler::schedulingCriterion(const Interval &interval, const TimeValue &planned) const {
  TimeValue ret = TimeValue::MAX;

  if (firstEnabled()) {
    ret = interval.next(time());
  }
  else {
    ret = planned;
  }

  return ret;
}
