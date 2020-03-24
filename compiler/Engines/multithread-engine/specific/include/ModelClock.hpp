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

#ifndef _BIP_Engine_ModelClock_HPP_
#define _BIP_Engine_ModelClock_HPP_

#include <GlobalClock.hpp>
#include <TimeValue.hpp>

/** \brief Model is used to represent the global time elapsed in the model.
 * It should only give values meeting the semantics of the model.
 *
 */
class ModelClock : public GlobalClock {
 public:
  ModelClock();
  ModelClock(GlobalClock &platformClock);
  virtual ~ModelClock();

  virtual TimeValue time() const { return mTime; }
  virtual bool wait(const TimeValue &newTime);
  void update();

  void setPlatformClock(GlobalClock &platformClock) { assert(mPlatformClock == NULL); mPlatformClock = &platformClock; }

 protected:
  GlobalClock &platformClock() const { assert(mPlatformClock != NULL); return *mPlatformClock; }
  void setTime(const TimeValue &time);

  GlobalClock *mPlatformClock;
  TimeValue mTime;
};

inline
bool ModelClock::wait(const TimeValue &newTime) {
  if (newTime > time()) {
    // new time
    setTime(newTime);

    // wait for platform clock to reach target time value
    return platformClock().wait(time());
  }
  else {
    return false;
  }
}

inline
void ModelClock::update() {
  setTime(platformClock().time());
}

inline
void ModelClock::setTime(const TimeValue &time) {
  assert(mTime <= time);
  
  mTime = time;
}

#endif // _BIP_Engine_ModelClock_HPP_
