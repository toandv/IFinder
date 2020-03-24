/*
 * 
 *
 */

package ujf.verimag.bip.ifinder.tool;

import java.lang.String;
import java.io.*;
import java.util.List;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.TreeSet;
import java.util.TreeMap;

import org.apache.commons.cli.*;

import bip2.ujf.verimag.bip.types.*;
import bip2.ujf.verimag.bip.component.*;
import bip2.ujf.verimag.bip.instance.*;

import ujf.verimag.bip.ifinder.analysis.*;
import ujf.verimag.bip.ifinder.invariant.*;

import com.microsoft.z3.*;

/*
 *
 *
 */

public class IChecker extends Tool {
    
    // the system component instance
    protected ComponentInstance m_root;

    // access names within the root instance
    protected HashMap<ComponentInstance,
	String> m_access;

    // generated invariants
    protected HashMap<ComponentInstance,
	ArrayList<Invariant>> m_invariants;

    // the property
    protected String m_property;
    
    //
    // constructor
    //

    protected IChecker() {
	super("ichecker");
	m_root = null;
	m_access = null;
	m_invariants = new HashMap<ComponentInstance,
	    ArrayList<Invariant>>();
	m_property = null;
    }
    
    // specific ichecker cli options

    protected void addSpecificToolOptions(Options options) {
	Option root = new Option
	    ("r", "root", true,
	     "apply to component type, required");
	root.setRequired(true);
	options.addOption(root);
	
	Option invariantSpec = new Option
	    ("i", "invariant-specification", true,
	     "compute invariants according to specification, required");
	invariantSpec.setRequired(true);
	options.addOption(invariantSpec);
	
	Option property = new Option
	    ("s", "property", true,
	     "prove property, required");
	property.setRequired(true);
	options.addOption(property);
	
    }
    
    // specific ichecker run

    protected void runSpecific() {
	
	// instantiate root component
	if (m_command.hasOption("root")) {
	    ComponentType type =
		m_model.locateComponentType(m_command.getOptionValue("root"));
	    m_root = m_model.createComponentInstance(type, "root");
	}

	// initialize access and invariant structure
	if (m_root != null) {
	    m_access = createAccess(m_root);
	    createInvariants_rec(m_root);
	}
	
	// generate invariants
	if (m_command.hasOption("invariant-specification")) 
	    processInvariantSpecification(m_command.getOptionValue("invariant-specification"));

	// load/parse the property
	if (m_command.hasOption("property")) 
	    loadProperty(m_command.getOptionValue("property"));
	
	// generate the SMT script
	String x_smt =
	    getMixedName(m_command.getOptionValue("invariant-specification"),
			 m_command.getOptionValue("property"));
	generateSMT(x_smt);
	
	// run the SMT solver [z3]
	check(x_smt);
    }
    
    // helper
    // process the invariant specification file

    protected void processInvariantSpecification(String x_specification) {

	information("process invariant specification '" + x_specification + "' ...");
	
        try {
            FileReader fileReader = new FileReader(x_specification);
            BufferedReader bufferedReader = new BufferedReader(fileReader);
	    String line = null;
	
            while((line = bufferedReader.readLine()) != null) {
		line.trim();
                if (line.length() > 0 && !line.startsWith("#")) {
		    information("process line '" + line + "' ...");
		    String args[] = line.split(" ");
		    processInvariantLine(args);
		}   
	    }
	    
	    bufferedReader.close();
        }
        catch(FileNotFoundException ex) {
            error("unable to open file '" + x_specification + "'");
	    abort();
        }
        catch(IOException ex) {
            error("error reading file '"  + x_specification + "'");
	    abort();
        }

	information("process invariant specification '" + x_specification + "' done");
    }
    
    // helper
    // process one invariant line of the invariant specification

    protected void processInvariantLine(String args[]) {
	String x_path = "*";
	
	// parse invariant line options
	CommandLine command = parseInvariantLineOptions(args); 
	
	// generate invariant
	Invariant invariant = generate(command); 
	
	// get instantiation path
	if (command.hasOption("instantiation-path"))
	    x_path = command.getOptionValue("instantiation-path");
	
	// add the invariant to the database
	if (invariant != null) 
	    addInvariant(invariant, x_path);
    }
    
    protected CommandLine parseInvariantLineOptions(String args[]) {
	CommandLine command = null;
	
	// setup invariant line options ~ as for ifinder
	Options options = new Options();
	addGenericInvariantOptions(options);
	
	// specific invariant line options: instantiation
	Option instantiation = new Option
	    ("ip", "instantiation-path", true, "instantiate according to path");
	options.addOption(instantiation);
	
	// parse arguments
	CommandLineParser parser = new DefaultParser();
	try {
            command = parser.parse(options, args);
        } catch (ParseException e) {
	    error(e.getMessage() != null ?
		  e.getMessage() : "invariant line parse exception");
	    abort();
        }
	
	return command;
    }

    protected void addInvariant(Invariant invariant, String path) {
	// the path is defined by the grammar
	
	//      path ::= sequence | sequence:path
	//  sequence ::=        * |   compid.sequence 

	// not yet finished : handle only the * paths

	assert(path.equals("*"));
	
	for(ComponentInstance instance : m_invariants.keySet()) {
	    if (instance.getDeclaration().getType() ==
		invariant.getInstance().getDeclaration().getType()) {
		m_invariants.get(instance).add(invariant);
		information("invariant recorded for '" + m_access.get(instance) + "'");
	    }
	}
	
    }

    protected void createInvariants_rec(ComponentInstance instance) {
	// add invariant list for this instance
	m_invariants.put(instance, new ArrayList<Invariant>());
	
	// go recursively to sub-instances...
	if (instance instanceof CompoundInstance) {
	    CompoundInstance compound = (CompoundInstance) instance;
	    for (AtomInstance subInstance : compound.getSubAtomInstances()) {
		createInvariants_rec(subInstance);
	    }
	    for(CompoundInstance subInstance : compound.getSubCompoundInstances()) {
		createInvariants_rec(subInstance);
	    }
	}
    }

    // helper
    // load/parse the property from the file
    
    protected void loadProperty(String x_property) {
	// not yet finished : so far, assume the property is expressed
	// not yet finished : as a valid SMT term in the checker context
	// not yet finished : parse the file, type check wrt the root, etc...
	
	information("load property '" + x_property + "' ... ", 1);

	m_property = "";
        try {
            FileReader fileReader = new FileReader(x_property);
            BufferedReader bufferedReader = new BufferedReader(fileReader);
	    String line = null;
            while((line = bufferedReader.readLine()) != null) {
		line.trim();
                if (line.length() > 0 && !line.startsWith("#")) {
		    m_property += line;
		}   
	    }
	    bufferedReader.close();
        }
        catch(FileNotFoundException ex) {
            error("unable to open file '" + x_property + "'");
	    abort();
        }
        catch(IOException ex) {
            error("error reading file '"  + x_property + "'");
	    abort();
        }

	information("done", 2);
    }
    
    // helper
    // generation of the SMT script
    
    protected void generateSMT(String x_smt) {
	information("generate SMT script '" + x_smt + "' ... ", 1);
	PrintStream output = openPrintStream(x_smt + ".smt");
	
	// preamble
	output.print("\n\n; invariant composition [" +
		     m_root.getDeclaration().getType().getName() + "]\n\n");
	output.print("(define-fun min ((x Real) (y Real)) Real\n" +
		     "    (ite (< x y) x y))\n\n");
	// declare variables
	declareVariables(output);
	// assert invariants
	assertInvariants(output);
	// assert the negation of property
	assertNegatedProperty(output);
	// postamble
	
	output.close();
	information("done", 2);
    }
    
    // declare invariant variables, avoid replication

    protected void declareVariables(PrintStream output) {
	TreeSet<String> declared = new TreeSet<String>();
	Invariant.Flat = true;
	for(ComponentInstance instance : m_invariants.keySet()) {
	    Invariant.Access = m_access.get(instance);
	    for(Invariant invariant : m_invariants.get(instance)) 
		for(Variable variable : invariant.getVariables()) {
		    String varname = variable.getAccessName();
		    if (declared.contains(varname)) continue;
		    output.print( variable.getDeclaration() );
		    declared.add(varname);
		}
	    Invariant.Access = "";
	}
	Invariant.Flat = false;
    }
    
    // generate invariant assert statements...
    
    protected void assertInvariants(PrintStream output) {
	Invariant.Flat = true;	
	for(ComponentInstance instance : m_invariants.keySet()) {
	    ArrayList<Invariant> invariants = m_invariants.get(instance);
	    Invariant.Access = m_access.get(instance);
	    for (Invariant invariant : invariants) {
		output.print(invariant.header());
		output.print("(assert " + invariant.toString() + ")");
		output.print(invariant.footer());
	    }
	    Invariant.Access = "";
	}

	Invariant.Flat = false;
    }
    
    // assert the negation of property 
    
    protected void assertNegatedProperty(PrintStream output) {
	output.print("\n\n; the negation of the property \n\n");
	output.print("(assert (not " + m_property +"))\n\n");
    }

    // helper
    // check for satisfiability and get model, if any...

    protected void check(String x_smt) {
	information("run SMT solver ... ");
	
	HashMap<String, String> config = new HashMap<String, String>();
	config.put("model", "true");
	
	// create z3 context and solver
	Context context = new Context(config);
	Solver solver = context.mkSolver();
	
	// load assertions 
	solver.fromFile(x_smt + ".smt");
	
	// check for satisfiability
	Status status = solver.check();
	com.microsoft.z3.Model model = null;
	
	switch (status) {
	case UNSATISFIABLE:
	    System.out.println("valid");
	    break;
	case SATISFIABLE:
	    System.out.println("invalid");
	    dumpModel( solver.getModel(), x_smt );
	    break;
	case UNKNOWN:
	    System.out.println("unknown");
	    break;
	}
    }
    
    protected void dumpModel(com.microsoft.z3.Model model, String x_smt) {
	// not yet finished : unparse the z3 model and 
	// not yet finished : get back variable values for every component...

	// use the map to order vmodel variables by name...
	TreeMap<String, String> x_map = new TreeMap<String, String>();
	information("dump model ... ", 1);
	PrintStream output = openPrintStream(x_smt + ".model");
	for (FuncDecl constDecl : model.getConstDecls()) {
	    String name = constDecl.getName().toString();
	    String value = model.getConstInterp(constDecl).toString();
	    if (!value.equals("0"))
		x_map.put(name, value);
	}
	for(String name : x_map.keySet()) 
	    output.println(name + " = " + x_map.get(name));
	output.println("* = 0");
	output.close();
	information("done", 2);
    }
    
    // helper
    // get the smt mixed file name invariant-property

    protected String getMixedName(String x_is, String x_property) {
	String y_is = x_is.endsWith(".inv") ?
	    x_is.substring(0, x_is.length() - 4) : x_is;
	String y_property = x_property.endsWith(".pro") ?
	    x_property.substring(0, x_property.length() - 4) : x_property;
	return y_is + "-" + y_property;
    }
    
    
    // main 

    public static void main(String args[]) {
	IChecker ichecker = new IChecker();
	ichecker.run(args);
    }
  
    
}
