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

#include <Launcher.hpp>

#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>

#include <stack>
#include <queue>

#include <Component.hpp>
#include <Compound.hpp>

#include <Atom.hpp>
#include <PortValue.hpp>

#include <Port.hpp>
#include <AtomInternalPort.hpp>
#include <CompoundExportPort.hpp>
#include <ConnectorExportPort.hpp>

#include <Executer.hpp>

#include <Logger.hpp>

#include <GlobalClock.hpp>
#include <RealTimeClock.hpp>
#include <SimulationClock.hpp>

#include <BipCallBack.hpp>

#include <engine-version.hpp>

// global method that instanciates the system found in the generated code
Component* deploy(int argc, char **argv);

// constructors
Launcher::Launcher(int argc, char **argv, Component &root) :
  LauncherItf(argc, argv, root),
  mLogger(NULL),
  mExecuter(NULL),
  mPlatformClock(NULL),
  mInitializeCallBack(NULL),
  mInitializeThreadCallBack(NULL) {
  /* implement your constructor here */
}

Launcher::Launcher(int argc, char **argv, Component &root, BipCallBack &initializeCallBack, BipCallBack &initializeThreadCallBack) :
  LauncherItf(argc, argv, root),
  mLogger(NULL),
  mExecuter(NULL),
  mPlatformClock(NULL),
  mInitializeCallBack(&initializeCallBack),
  mInitializeThreadCallBack(&initializeThreadCallBack) {
  /* implement your constructor here */
}

// destructor
Launcher::~Launcher() {
  if (mLogger != NULL) {
    delete mLogger;
  }

  if (mExecuter != NULL) {
    delete mExecuter;
  }

  if (mPlatformClock != NULL) {
    delete mPlatformClock;
  }
}

int Launcher::initialize() {
  bool verbose = true; // default value
  bool help = false;
  bool invalidOption = false;
  bool version = false;
  bool randomSeed = true;
  unsigned int seed = 0;
  unsigned int limit = 0;
  unsigned int threads = 8;

  // real-time scheduling
  bool realtime = false;
  bool asap = false;
  bool first_enabled = false;
  bool disable_time_safety = false;
  bool profiling = false;

  // semantics options
  bool relaxed = false;
  // TimeValue granularity = TimeValue::ZERO;

  int ret = EXIT_SUCCESS;

  string bipExecutableName = "<bip_executable>";

  // set the name of the bip executable
  if (mArgc > 0) {
    bipExecutableName = string(mArgv[0]);
  }

  // parse options
  for (int i = 1 ; i < mArgc ; ++i) {
    string option = mArgv[i];

    if (option == "-v" || option == "--verbose") {
      verbose = true;
    }
    else if (option == "-s" || option == "--silent") {
      verbose = false;
    }
    else if (option == "-h" || option == "--help") {
      help = true;
    }
    else if (option == "-V" || option == "--version") {
      version = true;
    }
    else if (option == "-a" || option == "--asap") {
      asap = true;
    }
    else if (option == "-f" || option == "--first-enabled") {
      first_enabled = true;
    }
    else if (option == "--relaxed") {
      relaxed = true;
    }
    else if (option == "--real-time") {
      realtime = true;
    }
    else if (option == "--disable-time-safety") {
      disable_time_safety = true;
    }
    else if (option == "--profile") {
      profiling = true;
    }
    else if (option == "--seed") {
      randomSeed = false;

      // look for an integer argument
      if (i < mArgc - 1) {
        ++i;
        std::string argument = mArgv[i];
        std::stringstream iss(argument);
        iss >> seed;

        if (iss.fail()) {
          invalidOption = true;
          ret = EXIT_FAILURE;

          cout << bipExecutableName << ": invalid arguement for '" << option << "', waiting for an interger" << endl;
          cout << "Try `" << bipExecutableName << " --help' for more information." << endl;

          break;
        }
      }
      else {
        invalidOption = true;
        ret = EXIT_FAILURE;

        cout << bipExecutableName << ": missing integer argument for '" << option << "'" << endl;
        cout << "Try `" << bipExecutableName << " --help' for more information." << endl;

        break;
      }
    }
    else if (option == "-l" || option == "--limit") {
      // look for an integer argument
      if (i < mArgc - 1) {
        ++i;
        std::string argument = mArgv[i];
        std::stringstream iss(argument);
        iss >> limit;

        if (iss.fail() || limit <= 0) {
          invalidOption = true;
          ret = EXIT_FAILURE;

          cout << bipExecutableName << ": invalid arguement for '" << option << "', waiting for a positive interger" << endl;
          cout << "Try `" << bipExecutableName << " --help' for more information." << endl;

          break;
        }
      }
      else {
        invalidOption = true;
        ret = EXIT_FAILURE;

        cout << bipExecutableName << ": missing positive integer argument for '" << option << "'" << endl;
        cout << "Try `" << bipExecutableName << " --help' for more information." << endl;

        break;
      }
    }
    else if (option == "--threads") {
      // look for an integer argument
      if (i < mArgc - 1) {
        ++i;
	    std::string argument = mArgv[i];
        std::stringstream iss(argument);
        iss >> threads;

        if (iss.fail() || threads <= 0) {
          invalidOption = true;
          ret = EXIT_FAILURE;

          cout << bipExecutableName << ": invalid arguement for '" << option << "', waiting for a positive interger" << endl;
          cout << "Try `" << bipExecutableName << " --help' for more information." << endl;

          break;
        }
      }
      else {
        invalidOption = true;
        ret = EXIT_FAILURE;

        cout << bipExecutableName << ": missing non-negative integer argument for '" << option << "'" << endl;
        cout << "Try `" << bipExecutableName << " --help' for more information." << endl;

        break;
      }
    }
    else if (hasInitializeCallBack() || hasInitializeThreadCallBack()) {
      cout << bipExecutableName << ": ignored option -- '" << option << "'" << endl;
      cout << "Try `" << bipExecutableName << " --help' for more information." << endl;
    }
    else {
      invalidOption = true;
      ret = EXIT_FAILURE;

      cout << bipExecutableName << ": invalid option -- '" << option << "'" << endl;
      cout << "Try `" << bipExecutableName << " --help' for more information." << endl;

      break;
    }
  }

  if (disable_time_safety && !realtime) {
    invalidOption = true;
    ret = EXIT_FAILURE;

    cout << bipExecutableName << ": '--disabled-time-safety' should be used in conjunction with '" << "--real-time" << "'" << endl;
    cout << "Try `" << bipExecutableName << " --help' for more information." << endl;
  }

  if (help) {
    printHelp(bipExecutableName);
  }

  if (version) {
    cout << "Reference engine. Version: " ENGINE_VERSION << endl;
    return EXIT_SUCCESS;
  }

  if (!invalidOption && !help) {
    if (realtime) {
      //Job::realTimeClock().start();
    }

    RealTimeClock::start();
    SimulationClock::initialize();

    mLogger = new Logger(cout, verbose, limit);

    mExecuter = new Executer(dynamic_cast<Compound &>(root()), realtime, asap, first_enabled, relaxed, disable_time_safety, profiling, *mLogger, threads, randomSeed, seed);

    if (hasInitializeCallBack()) {
      // set argc / argv parameters of initialize call back
      initializeCallBack().setArgc(mArgc);
      initializeCallBack().setArgv(mArgv);

      mExecuter->setInitializeCallBack(initializeCallBack());
    }
    
    if (hasInitializeThreadCallBack()) {
      // set argc / argv parameters of initialize thread call back
      initializeThreadCallBack().setArgc(mArgc);
      initializeThreadCallBack().setArgv(mArgv);
      
      mExecuter->setInitializeThreadCallBack(initializeThreadCallBack());
    }

    BipError &error = mExecuter->initialize();

    if (error.type() != NO_ERROR) {
      ret = error.type();
    }
  }

  return ret;
}

int Launcher::launch() {
  int ret = EXIT_SUCCESS;

  if (mExecuter != NULL) {  
    BipError &error = mExecuter->run();
  
    if (error.type() != NO_ERROR) {
      ret = error.type();
    }
  }

  return ret;
}

void Launcher::printHelp(const string &bipExecutableName) {
   cout << "Usage: " << bipExecutableName << " [options]" << endl;
   cout << endl;
   cout << "BIP Engine general options:" << endl;
   cout << "                    (i.e. executes interactions in parallel, if obs. equivalent)" << endl;
   cout << " --asap             after chosing an interaction, execute it ASAP" << endl;
   cout << " --first-enabled    execute the interaction which is the earliest enabled" << endl; 
   cout << " -h, --help         display this help and exit" << endl;
   cout << " -l, --limit LIMIT  limits the execution to LIMIT interactions" << endl;
   cout << " --profile          profile execution times" << endl;
   cout << " --real-time        execute in real-time (simulated time by default)" << endl;
   cout << " --seed SEED        set the seed for random to SEED" << endl;
   cout << " --threads NB       set the number of threads (by default, use the maximal HW parallelism or 8)" << endl;
   cout << " -s, --silent       disables the display of the sequence of enabled/chosen interactions" << endl;
   cout << " -v, --verbose      enables the display of the sequence of enabled/chosen interactions (default)" << endl;
   cout << " -V, --version      displays engine version and exits" << endl;
   cout << "BIP Engine semantics options (WARNING: modify the official semantics of BIP!):" << endl;
   cout << " --disable-time-safety    disable strict checking of time-safety (not compatible with simulated time)" << endl;
   cout << " --relaxed                produce warning in case of time-safety violation (error by default)" << endl;
}
