/*
 *
 *
 */

package ujf.verimag.bip.ifinder.analysis;

import java.util.HashMap;

import bip2.ujf.verimag.bip.types.*;
import bip2.ujf.verimag.bip.connector.*;
import bip2.ujf.verimag.bip.component.atom.*;
import bip2.ujf.verimag.bip.instance.*;
import bip2.ujf.verimag.bip.behavior.*;

import ujf.verimag.bip.ifinder.tool.*;
import ujf.verimag.bip.ifinder.invariant.*;
import ujf.verimag.bip.ifinder.analysis.petrinet.*;

import org.sat4j.core.VecInt;
import org.sat4j.specs.*;
import org.sat4j.minisat.*;
import org.sat4j.minisat.orders.*;
import org.sat4j.minisat.core.*;

/*
 *
 *
 */

public class TrapAnalysis extends Analysis {
        
    // sat4j solver
    ISolver m_solver;

    // trap count, limit
    int m_count, m_limit;
    
    //
    // constructor
    //
    
    protected TrapAnalysis(HashMap<String, String> options,
			   ComponentInstance instance,  Tool tool) {
	super("trap", options, instance, tool);

	String x_limit = getOption("x-trap-limit", null, null);
	if (x_limit != null) m_limit = Integer.parseInt(x_limit); 
	
	m_solver = null;
	m_count = 0;
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
	Assertion assertion = null;
	
	// generic initialization
	initializeNet();
	generateAtVariables();
	
	// setup the solver
	initializeSolver();
	
	try {
	    // encode trap constraints
	    encodeAll();
	    // extract trap invariants from minimal solutions
	    assertion = extractAllMinTraps();
	} catch (TimeoutException e) {
	    m_tool.information("unexpected sat4j solver timeout", 3);
	} catch (ContradictionException e) {
	    m_tool.error("unexpected sat4j solver contradiction");
	    m_tool.abort();
	}
	
	m_tool.information("extract " + m_count + " traps", 3);
	
	return assertion;
    }
    
    private void initializeSolver() {
	// creates the light solver
	m_solver = SolverFactory.newLight();
	
	// fix the phase order, that is,
	// always assign first 0, then 1 to literals variables 
	// mandatory for getting traps increasingly wrt set inclusion
	((Solver<DataStructureFactory>)m_solver).getOrder()
	    .setPhaseSelectionStrategy(new UserFixedPhaseSelectionStrategy());
	
	// System.out.println(m_solver.toString());
	
	// initialize the number of variables (not mandatory...)
	m_solver.newVar(m_net.getStateCount());
    }
    
    //
    // encoding trap constraints
    //
    
    private void encodeAll()
	throws ContradictionException {
	// not containing entire components
	for(AtomInstance atom : m_net.getAtoms())
	    encodeNotSubset(atom);
	// initially marked
	encodeInitiallyMarked();
	// transition flows
	for(XTransition xtransition : m_net.getITransitions())
	    encodeTransitionFlow(xtransition);
	for(ExportedPortInstance port : m_instance.getExportedPorts())
	    for(XTransition xtransition : m_net.getVTransitions(port))
		encodeTransitionFlow(xtransition);
    }
    
    private void encodeTransitionFlow(XTransition xtransition)
	throws ContradictionException {
	int length, clause[], k;
	
	// determine the clause length
	length = 1;
	for(AtomInstance atom : xtransition.getSupport().keySet()) {
	    Transition t = xtransition.getSupport().get(atom);
	    length += t.getDestinations().size();
	}
	
	// create the clause 
	clause = new int[length];
	
	// common, fill with the positive literals (all destinations)
	k = 0;
	for(AtomInstance atom : xtransition.getSupport().keySet()) {
	    Transition t = xtransition.getSupport().get(atom);
	    for(State s : t.getDestinations()) 
		clause[k++] = m_net.getStateIndex(atom, s);   
	}
	
	// specific, add one negative literal (some source)
	for(AtomInstance atom : xtransition.getSupport().keySet()) {
	    Transition t = xtransition.getSupport().get(atom);
	    for(State s : t.getSources()) {
		clause[k] = -m_net.getStateIndex(atom, s);
		m_solver.addClause(new VecInt(clause));
	    }
	}
    }

    private void encodeInitiallyMarked()
	throws ContradictionException {
	int length, clause[], k;
	
	// determine the clause length
	length = 0;
	for(AtomInstance atom : m_net.getAtoms())
	    length += Helper.GetInitStates(atom).size();
	
	// create the clause and fill it
	clause = new int[length];
	k = 0;
	for(AtomInstance atom : m_net.getAtoms()) 
	    for(State s : Helper.GetInitStates(atom)) 
		clause[k++] = m_net.getStateIndex(atom, s);
	
	// add the clause
	m_solver.addClause(new VecInt(clause));
    }
    
    private void encodeNotSubset(AtomInstance atom)
	throws ContradictionException {
	int length, clause[], k;
	
	// determine the clause length
	length = Helper.GetStates(atom).size();
	
	// create the clause ans fill it, 
	clause = new int[length];
	k = 0;
	for(State s : Helper.GetStates(atom))
	    clause[k++] = -m_net.getStateIndex(atom, s);
	
	// add the clause to the solver
	m_solver.addClause(new VecInt(clause));
    }
    
    private void encodeNotSubset(int[] solution)
	throws ContradictionException {
	int length, clause[];
	
	// get the positive part of the solution
	length = 0;
	for(int i = 0; i < solution.length; i++)
	    if (solution[i] > 0) length++;
	
	// create the clause and fill it
	clause = new int[length];
	for(int i = 0, k = 0; i < solution.length; i++)
	    if (solution[i] > 0) clause[k++] = -solution[i];
	
	// add the clause
	m_solver.addClause(new VecInt(clause));
    }

    //
    // extraction of trap invariants
    //
    
    private ComposedAssertion extractAllMinTraps()
	throws TimeoutException, ContradictionException {
	
	ComposedAssertion allTraps
	    = new ComposedAssertion(ComposedAssertion.Operator.AND);
	
	IProblem problem = m_solver;
	while (problem.isSatisfiable()) {
	    // get a (hopefully minimal) solution from the solver
	    int[] solution = problem.model();
	    // extract the trap invariant from it
	    allTraps.add( extractTrap(solution)) ;
	    // add constraint to eliminate supersets
	    encodeNotSubset(solution);
	    // count the number on traps, stop on some user-defined limit...
	    if (++m_count == m_limit) break;
	}
	
	return allTraps;
    }
    
    private AtBooleanAssertion extractTrap(int[] solution) {
	int n = m_net.getStateCount();
	
	// convert the sat4j solution into a boolean valuation
	// that is, mark indexes for all positive literals
	int val[] = new int[n+1];
	for(int i = 0; i <= n; i++) val[i] = 0;
	for(int i = 0; i < solution.length; i++) 
	    if (solution[i] > 0) val[ solution[i] ] = 1;
	
	// construct the trap from the valuation
	AtBooleanAssertion trap =
	    new AtBooleanAssertion(AtBooleanAssertion.Operator.OR);
	
	// map back the positive literals into corresponding variables
	for(int i = 1; i <= n; i++) {
	    if (val[i] == 0) continue; 
	    AtVariable at_state_i = a_map.get(i);
	    trap.add(at_state_i, true);
	}

	// return
	return trap;
    }    

}
