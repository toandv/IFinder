/*
 *
 *
 */

package ujf.verimag.bip.ifinder.analysis;

import java.util.HashMap;
import java.util.TreeSet;

import bip2.ujf.verimag.bip.types.*;
import bip2.ujf.verimag.bip.connector.*;
import bip2.ujf.verimag.bip.component.atom.*;
import bip2.ujf.verimag.bip.instance.*;
import bip2.ujf.verimag.bip.behavior.*;

import ujf.verimag.bip.ifinder.tool.*;
import ujf.verimag.bip.ifinder.invariant.*;
import ujf.verimag.bip.ifinder.analysis.petrinet.*;
import ujf.verimag.bip.ifinder.analysis.reachability.*;

/*
 *
 *
 */

public class ControlReachabilityAnalysis extends Analysis {

    // the underlying Petri net explorer
    protected XPetriNetExplorer m_explorer;

    // reached/explored markings
    protected TreeSet<Marking> m_explored;

    // pending, yet to be explored markings
    protected TreeSet<Marking> m_pending;

    // marking extraction: full/positive
    protected String x_marking;
    
    //
    // constructor
    //
    
    protected ControlReachabilityAnalysis(HashMap<String, String> options,
					  ComponentInstance instance, Tool tool) {
	super("control-reachability", options, instance, tool);
	m_explorer = null;
	m_explored = null;
	m_pending = null;
	
	// options
	x_marking = getOption("x-marking", new String[]{"full", "positive"}, "positive");
    }

    //
    // applicability checks
    //
    
    protected boolean isApplicable(AtomInstance atom) {
        // no external ports in atoms
        AtomType type = (AtomType) atom.getDeclaration().getType();
        return (type.getExternalPortDeclarations().size() == 0) ? true : false;
    }
    
    protected boolean isApplicable(ConnectorInstance connector) {
        // no broadcasts
        ConnectorType type = connector.getDeclaration().getType();
        ConnectorInteractionDefinition definition = type.getInteractionDefinition();
        return Helper.IsSynchro(definition);
    }
    
    //
    // assertion generation
    //

    protected Assertion generate() {
	
	// generic initialization
	initializeNet();
	generateAtVariables();

	// create the reachable assertion
	ComposedAssertion reachable
	    = new ComposedAssertion(ComposedAssertion.Operator.OR);
	
	// setup the Petri net explorer
	m_explorer = new XPetriNetExplorer(m_instance, m_net);
	m_explorer.initializeControl();
	
	// compute the reachable markings
	computeReachable();
	m_tool.information("reached " + m_explored.size() +
			   " markings", 3);
	
	// extract assertions
	for(Marking m : m_explored)
	    reachable.add(extract(m));
	
	// return
	return reachable;
    }

    //
    // reachable marking computation
    // 
    
    protected void computeReachable() {
	MarkingComparator comparator = new MarkingComparator();

	// initialize explored
	m_explored = new TreeSet<Marking>(comparator);

	// initialize pending
	m_pending = new TreeSet<Marking>(comparator);

	// main reachability loop
	m_pending.add( m_explorer.getInitialMarking() );
	while (!m_pending.isEmpty()) {
	    Marking m = m_pending.pollFirst();
	    // add to explored set
	    m_explored.add(m);
	    // explore sucessor for all fireable transiions
	    for(XTransition t : m_explorer.getFireable(m)) {
		Marking m_succ = m_explorer.getSuccessorMarking(m, t);
		if ( m_explored.contains(m_succ) ||
		     m_pending.contains(m_succ)) continue;
		m_pending.add(m_succ);
	    }
	}
    }

    //
    // extraction
    //
    
    protected AtBooleanAssertion extract(Marking marking) {
	int n = m_net.getStateCount();
	AtBooleanAssertion assertion =
	    new AtBooleanAssertion(AtBooleanAssertion.Operator.AND);
	for(int i = 1; i <= n; i++) {
	    AtVariable at_state_i = a_map.get(i);
	    boolean i_bit = marking.isBit(i);
	    if (x_marking.equals("full") || i_bit)
		assertion.add(at_state_i, i_bit);
	}
	return assertion;
    }
    
}

