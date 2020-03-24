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

public class IFinder extends Tool {

    // constructor

    protected IFinder() {
	super("ifinder");
    }

    // specific ifinder cli options

    protected void addSpecificToolOptions(Options options) {
	addGenericInvariantOptions(options);

	Option output = new Option
	    ("o", "output", true, "output invariant to file, optional");
	options.addOption(output);
    }
    
    // specific ifinder run

    protected void runSpecific() {
	PrintStream output = null;
	Invariant invariant = null;
	
	// generate invariant
	invariant = generate(m_command);
	
	// output 
	output = System.out;
	if (m_command.hasOption("output")) {
	    String x_output = m_command.getOptionValue("output");
	    output = openPrintStream(x_output);
	}
     	if (invariant != null) {
	    output.print( invariant.header() );
	    output.print( invariant.toString() );
	    output.print( invariant.footer() );
	}
	if (output != System.out)
	    output.close();
	
	information("done");
    }
    
    // main 

    public static void main(String args[]) {
	IFinder ifinder = new IFinder();
	ifinder.run(args);
    }

}
