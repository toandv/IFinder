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

#ifndef _BIP_Engine_Thread_HPP_
#define _BIP_Engine_Thread_HPP_

// inherited classes
#include "Atom.hpp"
#include "AtomInternalPort.hpp"
#include "PortValue.hpp"

class Job;
template<class T> class ReadyQueue;
class BipCallBack;

class Thread {
 public:
  // constructors
  Thread(ReadyQueue<Job> &jobs);

  // destructor
  virtual ~Thread();

  // operations
  void start();
  void join() { mThread->join(); }

  // getter / setters
  ReadyQueue<Job> &jobs() { return mJobs; }
  bool hasJob() const { return mJob != NULL; }
  Job &job() const { return *mJob; }
  Job *jobPtr() const { return mJob; }
  bool hasInitializeCallBack() const { return mInitializeCallBack != NULL; }
  BipCallBack &initializeCallBack() { return *mInitializeCallBack; }
  void setInitializeCallBack(BipCallBack &initializeCallBack) { mInitializeCallBack = &initializeCallBack; }

 protected:
  // protected operations
  void execute();

  // protected getter / setters
  void setJob(Job &job) { mJob = &job; }

  ReadyQueue<Job> &mJobs;
  thread *mThread;
  Job *mJob;
  BipCallBack *mInitializeCallBack;
};

#endif // _BIP_Engine_Thread_HPP_
