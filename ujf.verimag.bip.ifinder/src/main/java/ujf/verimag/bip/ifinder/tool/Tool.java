/*
 *
 *
 */

package ujf.verimag.bip.ifinder.tool;

import java.io.*;
import java.util.HashMap;

import org.apache.commons.cli.*;

import bip2.ujf.verimag.bip.types.*;
import bip2.ujf.verimag.bip.component.*;
import bip2.ujf.verimag.bip.instance.*;

import ujf.verimag.bip.ifinder.analysis.*;
import ujf.verimag.bip.ifinder.invariant.*;

/*
 *
 *
 */

abstract public class Tool {

    // tool name
    protected String m_name;
    
    // command line options
    protected Options m_options;
    protected CommandLine m_command;

    // working variables
    protected boolean m_verbose;

    // model helper
    protected Model m_model;

    //
    // constructor
    //
    
    protected Tool(String name) {
	m_name = name;
	m_options = null;
	m_command = null;
	m_verbose = false;
	m_model = new Model(this);
	initializeOptions();
    }
    
    // initialize command line options

    protected void initializeOptions() {
	m_options = new Options();
	addGenericToolOptions(m_options);
	addSpecificToolOptions(m_options);
    }	
    
    protected void addGenericToolOptions(Options options) {
	Option help = new Option
	    ("h", "help", false, "print this message, optional");
	options.addOption(help);
	
	Option verbose = new Option
	    ("v", "verbose", false, "be extra verbose, optional");
	options.addOption(verbose);
   	
	Option package_ = new Option
	    ("p", "package", true, "load package, required");
        package_.setRequired(true);
        options.addOption(package_);
	
	// not yet finished : encodings, etc
    }

    abstract protected void addSpecificToolOptions(Options options);
    
    protected void addGenericInvariantOptions(Options options) {
	Option atom = new Option
	    ("at", "atom-type", true,
	     "compute invariant on atom type, required*");
	options.addOption(atom);
	
	Option compound = new Option
	    ("ct", "compound-type", true,
	     "compute invariant on compound type, required*");
	options.addOption(compound);
	
	Option analysis = new Option
	    ("a", "analysis", true,
	     "execute the given analysis, required");
	analysis.setRequired(true);
	options.addOption(analysis);
	
	// not yet finished : add analysis related options
	// not yet finished : such options must be named x-a, x-b, ...
	
	Option trap_limit = new Option
	    ("xtl", "x-trap-limit", true,
	     "limit the number of extracted traps, optional");
	options.addOption(trap_limit);
	
	Option marking_extraction = new Option
	    ("xm", "x-marking", true,
	     "select /full/positive/ extraction for markings");
	options.addOption(marking_extraction);
	
	Option zone_extraction = new Option
	    ("xz", "x-zone", true,
	     "select /full/core/ extraction for zones");
	options.addOption(zone_extraction);
	
	Option history = new Option
	    ("xh", "x-history", true,
	     "record history of exported ports");
	options.addOption(history);

	Option xfilename = new Option
	    ("xf", "x-filename", true,
	     "input guest filename");
	options.addOption(xfilename);
	    
    }
    
    protected void run(String args[]) {
	// parse command line arguments
	m_command = parseCommandLineArgs(args);
	
	// process common tool options
	if (m_command.hasOption("help")) {  printCommandLineUsage(); return;  }
	if (m_command.hasOption("verbose"))  { m_verbose = true; }
	
	// load package
	if (m_command.hasOption("package"))
	    m_model.loadPackage(m_command.getOptionValue("package"));
	
	// specific run
	runSpecific();
    }

    abstract protected void runSpecific();

    // helper
    // generate invariant according to command line options 

    protected Invariant generate(CommandLine command) {
	String analysis = null;
	ComponentInstance instance = null;
	HashMap<String, String> options = null;
	Invariant invariant = null;
	
	// enforce selection of atom or compound type
	if ( ( command.hasOption("atom-type") &&
	       command.hasOption("compound-type")) ||
	     (!command.hasOption("atom-type") &&
	      !command.hasOption("compound-type")) ) {
	    error("Missing alternative: -at,--atom-type xor -ct,--compound-type");
	    abort();
	}

	// atom type 
	if (command.hasOption("atom-type")) {
	    AtomType atomType =
		m_model.locateAtomType(command.getOptionValue("atom-type"));
	    instance = m_model.createComponentInstance(atomType, "b");
	}
	
	// compound type
	if (command.hasOption("compound-type")) {
	    CompoundType compoundType =
		m_model.locateCompoundType(command.getOptionValue("compound-type"));
	    instance = m_model.createComponentInstance(compoundType, "b");
	}
	
	// identify analysis
	if (command.hasOption("analysis")) 
	    analysis = command.getOptionValue("analysis");
	
	// identify analysis options, if any
	options = new HashMap<String, String>();
	for(Option opt : command.getOptions()) 
	    if (opt.getLongOpt().startsWith("x-"))
		options.put(opt.getLongOpt(), opt.getValue());
	
	// run analysis
	invariant = Analysis.Generate(analysis, options, instance, this);
	
	// return
	return invariant;
    }

    // helper
    // parse command line arguments

    protected CommandLine parseCommandLineArgs(String args[]) {
	CommandLine command = null;
	CommandLineParser parser = new DefaultParser();
	try {
            command = parser.parse(m_options, args);
        } catch (ParseException e) {
	    error(e.getMessage() != null ?
		  e.getMessage() : "command line parse exception");
	    printCommandLineUsage();
	    abort();
        }
	return command;
    }

    // helper
    // print help/usage

    protected void printCommandLineUsage() {
        HelpFormatter formatter = new HelpFormatter();
	formatter.setOptionComparator(null);
	System.out.println("");
	formatter.printHelp(m_name + ".sh", "\noptions\n", m_options, "", true);
    }

    // helper
    // open output stream, System.out if fail

    protected PrintStream openPrintStream(String x_output) {
	File outputFile = new File(x_output);
	PrintStream output = null;
	try {
	    output =  new PrintStream(outputFile);
	}
	catch (FileNotFoundException e) {
	    information(e.getMessage());
	    output = System.out;
	}
	return output;
    }

    // helper
    // create access paths table for all sub-instances 

    public HashMap<ComponentInstance, String> createAccess(ComponentInstance instance) {
	HashMap<ComponentInstance, String> access = new
	    HashMap<ComponentInstance, String>();
	createAccess_rec(access, instance, "");
	return access;
    }

    private void createAccess_rec(HashMap<ComponentInstance,String> access,
				  ComponentInstance instance,
				  String path) {
	// add this instance path
	access.put(instance, path);
	
	// go recursively to sub-instances
	if (instance instanceof CompoundInstance) {
	    CompoundInstance compound = (CompoundInstance) instance;
	    for (AtomInstance subInstance : compound.getSubAtomInstances()) {
		String subPath = path + subInstance.getDeclaration().getName() + ".";
		createAccess_rec(access, subInstance, subPath);
	    }
	    for(CompoundInstance subInstance : compound.getSubCompoundInstances()) {
		String subPath = path + subInstance.getDeclaration().getName() + ".";
		createAccess_rec(access, subInstance, subPath);
	    }
	}
    }
    
    // helper
    // trace information

    public void information(String message) {
	if (m_verbose) 
	    System.out.println("[" + m_name + "] " + message);
    }
    
    public void information(String message, int flags) {
	if (m_verbose) {
	    if ((flags & 1) == 1) System.out.print("[" + m_name + "] ");
	    System.out.print(message);
	    if ((flags & 2) == 2) System.out.println();
	}
    }

    // helper
    // trace error

    public void error(String message) {
	System.err.println("[-error-] " + message + "!");
    }
    
    // abort execution

    public void abort() {
	information("abort");
	System.exit(-1);
    }

}
