/*
 *
 *
 */

package ujf.verimag.bip.ifinder.analysis.reachability;

import java.util.HashMap;
import java.util.ArrayList;

import bip2.ujf.verimag.bip.instance.*;
import bip2.ujf.verimag.bip.types.AtomType;
import bip2.ujf.verimag.bip.behavior.*;
import bip2.ujf.verimag.bip.actionlang.*;
import bip2.ujf.verimag.bip.time.*;

import ujf.verimag.bip.ifinder.analysis.*;
import ujf.verimag.bip.ifinder.analysis.petrinet.*;


/*
 *
 *
 */

public class XTransitionInfo {

    // members
        
    private XTransition m_transition;
    
    private Marking m_pre, m_post;
    
    private Urgency m_urgency;
    private TConstraints m_guard;
    private TSets m_action;
    
    // constructor
    
    public XTransitionInfo(XTransition transition) {
	m_transition = transition;
	m_pre = m_post = null;
	m_urgency = Urgency.LAZY;
	m_guard = null;
	m_action = null;
    }
    
    public void initializeControl(XPetriNet net) {
	// reminder - states are indexed from 1 to n
 	int n = net.getStateCount();
	
	// define the pre/post markings
	m_pre = new Marking(n+1);
	m_post = new Marking(n+1);
	for(AtomInstance atom : net.getAtoms()) {
	    Transition t = m_transition.getSupport().get(atom);
	    if (t == null) continue; 
	    for(State state : Helper.GetStates(atom)) {
		int index = net.getStateIndex(atom, state);
		if (t.getSources().contains(state))
		    m_pre.setBit(index);
		if (t.getDestinations().contains(state))
		    m_post.setBit(index);
	    }
	}
    }
    
    public void initializeTiming(XPetriNet net) {
	// initialize urgency
	int max_u = Urgency.LAZY_VALUE;
	for(AtomInstance atom : m_transition.getSupport().keySet()) {
	    Transition t = m_transition.getSupport().get(atom);
	    int t_u = t.getUrgency().getValue();
	    if (max_u < t_u) max_u = t_u;
	}
	m_urgency = Urgency.get(max_u);
	// initialize guard, if any
	initializeTimingGuard(net);
	// initialize clock (re)sets, if any
	initializeTimingSets(net);
    }

    // helper
    // initialize timing constraints

    private void initializeTimingGuard(XPetriNet net) {
	// book-keeping transition constraints per atoms
	HashMap<AtomInstance, ArrayList<BinaryOpExpression>> map =
	    new HashMap<AtomInstance, ArrayList<BinaryOpExpression>>();
	
	for(AtomInstance atom : m_transition.getSupport().keySet()) {
	    Transition t = m_transition.getSupport().get(atom);
	    ArrayList<BinaryOpExpression> t_constraints =  Helper.GetTConstraints(t);
	    if (t_constraints == null) continue;
	    map.put(atom, t_constraints);
	}
	
	// encoding
	ArrayList<Integer> X = new ArrayList<Integer>();
	ArrayList<Integer> Y = new ArrayList<Integer>();
	ArrayList<Integer> B = new ArrayList<Integer>();
	Encode(map, X, Y, B, net);

	// if no constraints, return
	if (X.size() == 0) return;
	
	// transform to raw int vectors and create the guard
	int size = X.size();
	int[] x = new int[size];
	int[] y = new int[size];
	int[] b = new int[size];
	for(int k = 0; k < size; k++) {
	    x[k] = X.get(k);
	    y[k] = Y.get(k);
	    b[k] = B.get(k);
	}
	m_guard = new TConstraints(x, y, b);
    }

    // helper
    // initialize timing (re)sets
    
    private void initializeTimingSets(XPetriNet net) {
	int size = 0;

	// book-keeping transitions (re)sets
	HashMap<AtomInstance, ArrayList<AssignmentExpression>> map =
	    new HashMap<AtomInstance, ArrayList<AssignmentExpression>>();
	for(AtomInstance atom : m_transition.getSupport().keySet()) {
	    Transition t = m_transition.getSupport().get(atom);
	    map.put(atom, Helper.GetTSets(t));
	    size += map.get(atom).size();
	}

	ExportedPortInstance port = m_transition.getExportedPort();
	if (port != null && net.trackHistory(port))
	    size += 1;
	
	// if no resets, returns
	if (size == 0) return;

	// otherwise, initialize 
	int[] x = new int[size];
	int[] v = new int[size];
	int k = 0;
	
	for(AtomInstance atom : map.keySet())
	    for(AssignmentExpression assign : map.get(atom)) {
		DirectClockDeclarationReferenceExpression lhs =
		    (DirectClockDeclarationReferenceExpression) assign.getLhs();
		DirectIntegerExpression rhs =
		    (DirectIntegerExpression) assign.getRhs();
		ClockDeclaration clock = lhs.getClockDeclaration();
		x[k] = net.getAtomClockIndex(atom, clock);
		v[k] = rhs.getValue();
		k++;
	    }
	if (port != null && net.trackHistory(port)) {
	    x[k] = net.getHistoryClockIndex(port);
	    v[k] = 0;
	    k++;
	}
	
	assert(k == size); // just in case
	m_action = new TSets(x, v);
    }

    //
    // accessors
    //

    public Marking getPre() { return m_pre; }

    public Marking getPost() { return m_post; }

    public Urgency getUrgency() { return m_urgency; }

    public TConstraints getGuard() { return m_guard; }

    public TSets getAction() { return m_action; }

    
    // static helper
    // for encoding timed constraints
    
    static void Encode(HashMap<AtomInstance, ArrayList<BinaryOpExpression>> map,
		       ArrayList<Integer> X, ArrayList<Integer> Y, ArrayList<Integer> B,
		       XPetriNet net) {

	// go expression by expression
	for(AtomInstance atom : map.keySet())
	    for(BinaryOpExpression binary : map.get(atom)) {
		BinaryOperators operator = binary.getOperator();
		ValuedExpression left = binary.getLeft();
		ValuedExpression right = binary.getRight();
		// reverse orientation, if needed
		if (operator == BinaryOperators.GREATER_THAN ||
		    operator == BinaryOperators.GREATER_THAN_OR_EQUAL) {
		    ValuedExpression tmp;
		    tmp = left; left = right; right = tmp;
		    operator = (operator == BinaryOperators.GREATER_THAN) ?
			BinaryOperators.LESS_THAN : BinaryOperators.LESS_THAN_OR_EQUAL;
		}
		// detect clock index and value positions
		boolean positive =
		    (left instanceof DirectClockDeclarationReferenceExpression) &&
		    (right instanceof DirectIntegerExpression);
		boolean negative =
		    (right instanceof DirectClockDeclarationReferenceExpression) &&
		    (left instanceof DirectIntegerExpression);
		assert((positive && !negative) || (negative && !positive));
		DirectClockDeclarationReferenceExpression clockRef =
		    (DirectClockDeclarationReferenceExpression) (positive ? left : right);
		DirectIntegerExpression intExp =
		    (DirectIntegerExpression) (positive ? right : left);
		ClockDeclaration clock = clockRef.getClockDeclaration();
		int index = net.getAtomClockIndex(atom, clock);
		int value = intExp.getValue();
		// setup the constraint(s)
		switch (operator) {
		case LESS_THAN:
		    X.add(positive ? index : 0);
		    Y.add(positive ? 0 : index);
		    B.add(Bound.Make(Bound.SignT.LT, positive ? value : -value));
		    break;    
		case LESS_THAN_OR_EQUAL:
		    X.add(positive ? index : 0);
		    Y.add(positive ? 0 : index);
		    B.add(Bound.Make(Bound.SignT.LE, positive ? value : -value));
		    break;
		case EQUAL:
		    X.add(index);
		    Y.add(0);
		    B.add(Bound.Make(Bound.SignT.LE, value));
		    X.add(0);
		    Y.add(index);
		    B.add(Bound.Make(Bound.SignT.LE, -value));
		default:
		    // this might never happen
		    assert(false);
		}
	    }
    }
}
