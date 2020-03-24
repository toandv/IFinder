/*
 *
 *
 */

package ujf.verimag.bip.ifinder.analysis;

import java.util.List;
import java.util.HashMap;
import java.util.Arrays;

import bip2.ujf.verimag.bip.instance.*;
import bip2.ujf.verimag.bip.behavior.*;

import ujf.verimag.bip.ifinder.tool.*; 
import ujf.verimag.bip.ifinder.invariant.*;

import ujf.verimag.bip.ifinder.analysis.petrinet.*;

/*
 *
 *
 */

public abstract class Analysis {

    // analysis title
    protected String m_title;

    // analysis options
    protected HashMap<String, String> m_options;
    
    // instance under analysis
    protected ComponentInstance m_instance;
    
    // context tool
    protected Tool m_tool;
    
    // the invariant generated
    protected Invariant m_invariant;

    // the underlying Petri net
    protected XPetriNet m_net;

    // at_* variable map
    protected HashMap<Integer, AtVariable> a_map;
    
    //
    // constructor
    //
    
    protected Analysis(String title,
		       HashMap<String, String> options,
		       ComponentInstance instance,
		       Tool tool) {
	m_title = title;
	m_options = options;
	m_instance = instance;
	m_tool = tool;
	m_invariant = null;
	m_net = null;
	a_map = null;
    }
    
    // members access
    
    protected ComponentInstance getInstance() { return m_instance; }
    
    protected Invariant getInvariant() { return m_invariant; }

    //
    // generic execution scheme
    //
    
    protected void execute() {

	// check applicability
	m_tool.information("check if '" + m_title +
			   "' is applicable ... ", 1);
	boolean ok = isApplicable();
	m_tool.information((ok ? "yes" : "no"), 2);
	
	if (!ok) return;

	// initialize invariant - provide support for variables
	m_invariant = new Invariant(m_title, this.getInstance(), m_tool);
	
	m_tool.information("generate invariant by '" + m_title +
			   "' ... " );
	Assertion assertion = generate();
	m_tool.information("generate invariant by '" + m_title + "' "
			   + (assertion != null ? "done" : "failed")  );
	
	// set invariant assertion
	m_invariant.setAssertion(assertion);
    }

    //
    // generic applicability check
    //
    
    protected boolean isApplicable() {
	if (m_instance instanceof AtomInstance) 
	    return isApplicable((AtomInstance) m_instance);
	
	if (m_instance instanceof CompoundInstance)
	    return isApplicable_rec((CompoundInstance) m_instance);

	return true;
    }

    protected boolean isApplicable_rec(CompoundInstance compound) {
	boolean ok = true;
	for(AtomInstance atom : compound.getSubAtomInstances() )
	    ok &= isApplicable(atom);
	for(CompoundInstance subCompound : compound.getSubCompoundInstances() )
	    ok &= isApplicable_rec(subCompound);
	for(ConnectorInstance connector : compound.getSubConnectorInstances() ) 
	    ok &= isApplicable(connector);
	ok &= isApplicable(compound); 
	return ok;
    }
    
    protected boolean isApplicable(AtomInstance atom) {
	// override in subclasses, as needed
	return true;
    }
    
    protected boolean isApplicable(ConnectorInstance connector) {
	// override in subclasses, as needed
	return true;
    }
    
    protected boolean isApplicable(CompoundInstance compound) {
	// override in subclasses, as needed
	return true;
    }

    //
    // invariant assertion generation
    //
    
    protected abstract Assertion generate();
    
    // helper
    // initializet the underlying Petri net
    
    protected void initializeNet() {
        m_tool.information("construct the Petri net ... ", 1);
        m_net = new XPetriNet(m_instance, null);
        m_tool.information("|P|=" + m_net.getStateCount() + ", " +
                           "|T|=" + m_net.getTransitionCount() + " done", 2);
	// m_net.dump();
    }
    
    // helper
    // generate at_* variables for the net and map them
    
    protected void generateAtVariables() {
 	assert(m_net != null);
	a_map = new HashMap<Integer, AtVariable>();
	
        // generate at variables
        for(AtomInstance atom : m_net.getAtoms()) 
            for(State state : Helper.GetStates(atom)) {
                AtVariable at_state = new AtVariable(state, atom, m_invariant);
                m_invariant.addVariable(at_state);
		// map then using net indexes
		a_map.put(m_net.getStateIndex(atom, state), at_state);
            }
    }
    
    // helper
    // get option value

    protected String getOption(String option,
			       String allowedValues[],
			       String defaultValue) {
	String x_value = m_options.get(option);
	
	if ((x_value == null) ||
	    (x_value != null && allowedValues != null &&
	     !Arrays.asList(allowedValues).contains(x_value)))
	    x_value = defaultValue;
	
	return x_value;
    }
    
    //
    // the single entry point for running analysis
    //
    
    public static Invariant Generate
	(String x_analysis, HashMap<String, String> options,
	 ComponentInstance instance, Tool tool) {
	
	List<String> available =
	    Arrays.asList("trap",
			  "linear",
			  "control-reachability",
			  "zone-reachability",
			  "guest",
			  "atom-control",
			  "propagation",
			  "interaction-time");
	
	if (!available.contains(x_analysis)) {
	    tool.error("unknown analysis '" + x_analysis + "'");
	    tool.abort();
	}
	
	// compatibility checks
	if ((instance instanceof AtomInstance) &&
	    (x_analysis.equals("interaction-time"))) {
	    tool.error("mismatch '" + x_analysis + "' on atomic component type");
	    tool.abort();
	}
	if ((instance instanceof CompoundInstance) &&
	    (x_analysis.equals("atom-control"))) {
	    tool.error("mismatch '" + x_analysis + "' on compound component type");
	    tool.abort();
	}

	// running
	Analysis analysis = null;
	Invariant invariant = null;
	
	if (x_analysis.equals("trap")) 
	    analysis = new TrapAnalysis(options, instance, tool);
	
	if (x_analysis.equals("linear")) 
	    analysis = new LinearAnalysis(options, instance, tool);
	
	if (x_analysis.equals("control-reachability"))
	    analysis = new ControlReachabilityAnalysis(options, instance, tool);

	if (x_analysis.equals("zone-reachability"))
	    analysis = new ZoneReachabilityAnalysis(options, instance, tool);

	if (x_analysis.equals("guest"))
	    analysis = new GuestAnalysis(options, instance, tool);
	
	if (x_analysis.equals("atom-control")) 
	    analysis = new AtomControlAnalysis(options, instance, tool);
	
	if (x_analysis.equals("propagation"))
	    analysis = new PropagationAnalysis(options, instance, tool);
	
	if (x_analysis.equals("interaction-time"))
	    analysis = new InteractionTimeAnalysis(options, instance, tool);
	
	if (analysis != null) {
	    analysis.execute();
	    invariant = analysis.getInvariant();
	}
	
	return invariant;
    } 
    
}
