/*
 *
 *
 */

package ujf.verimag.bip.ifinder.analysis;

import java.util.HashMap;

import bip2.ujf.verimag.bip.instance.*;

import ujf.verimag.bip.ifinder.tool.*;
import ujf.verimag.bip.ifinder.invariant.*;


/*
 *
 *
 */

public class PropagationAnalysis extends Analysis {

    //
    //
    
    protected PropagationAnalysis(HashMap<String, String> options,
				  ComponentInstance instance, Tool tool) {
	super("propagation", options, instance, tool);
	
	assert(instance instanceof AtomInstance);
    }
    
    //
    // assertion generation
    // 
    
    protected Assertion generate() {
	// not yet implemented
	return null;
    }
    
}
