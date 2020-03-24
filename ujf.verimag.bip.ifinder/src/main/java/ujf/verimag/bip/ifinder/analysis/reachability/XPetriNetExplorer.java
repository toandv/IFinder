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
import bip2.ujf.verimag.bip.time.*;

import ujf.verimag.bip.ifinder.analysis.*;
import ujf.verimag.bip.ifinder.analysis.petrinet.*;

/*
 *
 *
 */

public class XPetriNetExplorer {

    // underlying component instance
    ComponentInstance m_instance;
    
    // underlying Petri net
    XPetriNet m_net;

    // additional transition information
    HashMap<XTransition, XTransitionInfo> t_info;
    
    // additional state information
    HashMap<AtomInstance, HashMap<State, StateInfo>> s_info;
    
    // initial marking
    Marking m_initial;
    
    // initial and extrapolation zone
    Zone z_initial, z_extrapolation;
    
    //
    // constructor
    //
    
    public XPetriNetExplorer(ComponentInstance instance, XPetriNet net) {
	m_instance = instance;
	m_net = net;
	t_info = null;
	s_info = null;
	m_initial = null;
	z_initial = z_extrapolation = null;
    }
    
    public void initializeControl() {
	// create & initialize state info
	s_info = new HashMap<AtomInstance, HashMap<State, StateInfo>>();
	for(AtomInstance atom : m_net.getAtoms()) {
	    s_info.put(atom, new HashMap<State, StateInfo>());
	    for(State state : Helper.GetStates(atom)) {
		s_info.get(atom).put(state, new StateInfo(atom, state));
		s_info.get(atom).get(state).initializeControl(m_net);
	    }
	}
	
	// create & initialize transition info
	t_info = new HashMap<XTransition, XTransitionInfo>();
	for(XTransition xt : m_net.getITransitions()) {
	    t_info.put(xt, new XTransitionInfo(xt));
	    t_info.get(xt).initializeControl(m_net);
	}
	for(ExportedPortInstance port : m_instance.getExportedPorts()) {
	    for(XTransition xt : m_net.getVTransitions(port)) {
		t_info.put(xt, new XTransitionInfo(xt));
		t_info.get(xt).initializeControl(m_net);
	    }
	}

	// create & initialize initial marking
      	int n = m_net.getStateCount(); // states are indexed from 1 to n
	m_initial = new Marking(n+1);
	for(AtomInstance atom : m_net.getAtoms())
	    for(State state : Helper.GetInitStates(atom)) {
		int index = m_net.getStateIndex(atom, state);
		m_initial.setBit(index);	
	    }
    }
    
    public void initializeTiming() {
	// initialize timing of state info
	for(AtomInstance atom : m_net.getAtoms())
	    for(State state : Helper.GetStates(atom))
		s_info.get(atom).get(state).initializeTiming(m_net);

	// initialize timing of transition info
	for(XTransition xtransition : t_info.keySet())
	    t_info.get(xtransition).initializeTiming(m_net);

	// not yet implemented : timing for initial transition e.g., clock
	// not yet implemented : sets to non-zero values are ignored
	
	// create initial zone
	initializeTiming_initialZone();

	// create extrapolation zone
	initializeTiming_extrapolationZone();
    }

    private void initializeTiming_initialZone() {
	int n = m_net.getClockCount(), // clocks are indexed from 1 to n
	    m = m_net.getAtomClockCount(); // atom clocks from 1 to m
	
	// create default zone with all clocks inactive
	z_initial = new Zone(n+1, false);
	
	// set all atom clocks to 0 and init history clock, if tracked
	int __m = m + (m_net.trackInit() ? 1 : 0);
	int[] x = new int[__m], v = new int[__m];
	for(int i = 0; i < m; i++) {
	    x[i] = i+1;
	    v[i] = 0;
	}
	if (m_net.trackInit()) {
	    x[m] = m_net.getInitClockIndex();
	    v[m] = 0;
	}
	z_initial.set(x, v);
	
	// note: the above shall satifies tpcs of initial marking
    }

    private void initializeTiming_extrapolationZone() {
	int n = m_net.getClockCount(), // clocks are indexed from 1 to n
	    m = m_net.getAtomClockCount(); // atom clocks from 1 to m

	// compute max bounds for every clock...
	int[] maxbound = new int[m+1];
	for(int i = 1;  i <= m; i++)
	    maxbound[i] = Bound.Make(Bound.SignT.LE, 0);
	for(XTransition xt : t_info.keySet()) {
	    XTransitionInfo xti = t_info.get(xt);
	    if (xti.getGuard() != null)
		initializeTiming_extrapolationZone_constraints
		    (maxbound, xti.getGuard());
	}
	for(AtomInstance atom : m_net.getAtoms())
	    for(State state : Helper.GetStates(atom)) {
		StateInfo si = s_info.get(atom).get(state);
		if (si.getTpc() != null)
		    initializeTiming_extrapolationZone_constraints
			(maxbound, si.getTpc());
	    }

	// compute the max maxbound
	int max_maxbound = Bound.Make(Bound.SignT.LE, 0);
	for(int i = 1; i <= m; i++)
	    if (max_maxbound < maxbound[i])
		max_maxbound = maxbound[i];
	
	// create default zone with all clocks active
	// - define atom clocks with their computed bound
	// - define history et/ou init clocks with the max bound
	z_extrapolation = new Zone(n+1, true);
	
	int[] x = new int[n], y = new int[n], b = new int[n], o = new int[n];
	for(int i = 0; i < n; i++) {
	    x[i] = i+1;
	    y[i] = 0;
	    b[i] = (1 <= i+1 && i+1 <= m) ? maxbound[i+1] : max_maxbound;
	    o[i] = Bound.Make(Bound.SignT.LE, 0);
	}
	boolean consistent = false;
	consistent = z_extrapolation.intersects(x, y, b); //  x <= b
	consistent = z_extrapolation.intersects(y, x, o); // -x <= 0
	assert(consistent);
    }

    private void initializeTiming_extrapolationZone_constraints
	(int[] maxbound, TConstraints constraints) {
	// update max bounds wrt constraints 
	for(int j = 0; j < constraints.size(); j++) {
	    int x_j = constraints.x[j];
	    int y_j = constraints.y[j];
	    int b_j = constraints.b[j];
	    int v_j = Bound.Value(b_j);
	    if (y_j == 0 && maxbound[x_j] < Bound.Make(Bound.SignT.LE,  v_j))
		maxbound[x_j] = Bound.Make(Bound.SignT.LE,  v_j);
	    if (x_j == 0 && maxbound[y_j] < Bound.Make(Bound.SignT.LE, -v_j))
		maxbound[y_j] = Bound.Make(Bound.SignT.LE, -v_j);
	}
    }

    
    //
    // untimed/control explorer interface
    //
    
    public Marking getInitialMarking() { return m_initial; }
    
    public ArrayList<XTransition> getFireable(Marking marking) {
	ArrayList<XTransition> list = new ArrayList<XTransition>();
	// sequential selection (simple but not efficient...)
	for(XTransition xt : t_info.keySet()) {
	    XTransitionInfo xti = t_info.get(xt);
	    if (marking.enables(xti.getPre())) 
		list.add(xt);
	}
	return list;
    }
    
    public Marking getSuccessorMarking(Marking marking, XTransition xt) {
	XTransitionInfo xti = t_info.get(xt);
	Marking succ = new Marking(marking);
	succ.execute(xti.getPre(), xti.getPost());
	return succ;
    }

    //
    // timed explorer interface
    //

    public Zone getInitialZone() {
	return z_initial;
    }

    public Zone getSuccessorZone(Zone z, XTransition xt) {
	XTransitionInfo xti = t_info.get(xt);
	// get a copy of the zone
	boolean consistent = true;
	Zone z_succ = new Zone(z);
	// intersect with the transition guard, if any
	if (xti.getGuard() != null) {
	    TConstraints guard = xti.getGuard();
	    consistent = z_succ.intersects(guard.x, guard.y, guard.b);
	    if (!consistent) return null;
	}
	// apply the clock resets, if any
	if (xti.getAction() != null) {
	    TSets action = xti.getAction();
	    z_succ.set(action.x, action.v);
	}
	// intersect with tpcs of destination states
	for(AtomInstance atom : xt.getSupport().keySet()) {
	    Transition transition = xt.getSupport().get(atom);
	    for(State state : transition.getDestinations()) {
		StateInfo si = s_info.get(atom).get(state);
		if (si.getTpc() != null) {
		    TConstraints tpc = si.getTpc();
		    consistent = z_succ.intersects(tpc.x, tpc.y, tpc.b);
		    if (!consistent) return null;
		}
	    }
	}
	// return
	return z_succ;
    }

    public ArrayList<Zone> getTimeElapseZoneList(Zone z, Marking m,
						 ArrayList<XTransition> xt_fireable) {
	//
	// three exclusive situations:
	//
	// 1. exists fireable t :                eager /\ invisible /\ guard=true
	// 2. exists fireable t : (eager \/ delayable) /\ invisible
	// 3. forall fireable t :                 lazy \/   visible
	//
	// given moreover I = tpc(m)
	//
	// 1. return { Z }
	// 2. return { Z } \cup { (Z /\ C).fwd /\ C /\ I  |  C ... }
	//     where C ranges over combinations of tpc constraints 
	//     -- one for every invisible eager,
	//     -- all for every invisible delayable amogst the upper front
	// 3. return { Z.fwd /\ I }
	//

	// detect the situation
	
	int situation = 3;
	for(XTransition xt : xt_fireable) {
	    Urgency xt_u = t_info.get(xt).getUrgency();
	    if (xt_u != Urgency.LAZY &&	xt.getExportedPort() == null) {
		situation = 2;
		if (xt_u == Urgency.EAGER && t_info.get(xt).getGuard() == null) {
		    situation = 1;
		    break;
		}
	    }
	}
	
	ArrayList<Zone> list = new ArrayList<Zone>();
	
	// compute according to the three situations
	if (situation == 1) 
	    list.add( new Zone(z) );
	if (situation == 2) {
	    list.add( new Zone(z) );
	    list.addAll( getTimeElapseWithTpcAndUrgency(z, m, xt_fireable));
	}
	if (situation == 3)
	    list.add( getTimeElapseWithTpc(z, m) );

	// not yet implemented : check & remove inclusions in the list
	
	// return
	return list;
    }

    // helper
    // time elapse - situation 3
    
    private Zone getTimeElapseWithTpc(Zone z, Marking m) {
	boolean consistent = true;
	Zone z_elapse = new Zone(z);
	
	// make the time progress...
	z_elapse.forward();
	// intersect with the state invariant in m
	for(AtomInstance atom : m_net.getAtoms()) {
	    if (!consistent) break;
	    for(State state : Helper.GetStates(atom)) {
		if (!consistent) break;
		int i = m_net.getStateIndex(atom, state);
		if (m.isBit(i)) {
		    TConstraints tpc_i = s_info.get(atom).get(state).getTpc();
		    if (tpc_i != null)
			consistent = z_elapse.intersects(tpc_i.x, tpc_i.y, tpc_i.b);
		}
	    }
	}
	// shall be consistent, in principle...
	assert(consistent);
	
	// extrapolate, done only here...
	z_elapse.extrapolate(z_extrapolation);

	// return
	return z_elapse;
    }

    // helper
    // time elapse - situation 2

    private ArrayList<Zone> getTimeElapseWithTpcAndUrgency
	(Zone z, Marking m, ArrayList<XTransition> xt_fireable) {

	int size = 0; // pre-compute the no of negated constraints...
	
	// construct the (short) list of non-lazy invisible fireable
	ArrayList<XTransition> xt_u_fireable = new ArrayList<XTransition>();
	for(XTransition xt : xt_fireable) {
	    Urgency xt_u = t_info.get(xt).getUrgency();
	    if (xt_u != Urgency.LAZY && xt.getExportedPort() == null) {
		xt_u_fireable.add(xt);
		if (xt_u == Urgency.EAGER)
		    size += 1;
		if (xt_u == Urgency.DELAYABLE) {
		    TConstraints xt_guard = t_info.get(xt).getGuard();
		    size += (xt_guard == null) ? 0 : xt_guard.frontSize();
		}
	    }
	}
	
	// prepare to collect the result
	ArrayList<Zone> list = new ArrayList<Zone>();

	// create the place of (negated) constraints
	int[] x = new int[size], y = new int[size], b = new int[size];
	
	// go recursively and collect all...
	collectTimeElapseWithTpcAndUrgency_rec(z, m, 0, xt_u_fireable, 0, x, y, b, list);
	
	// return
	return list;
    }
    
    private void collectTimeElapseWithTpcAndUrgency_rec
	(Zone z, Marking m,
	 int xt_u_count, ArrayList<XTransition> xt_u_fireable,
	 int c_count, int[] x, int[] y, int[] b,
	 ArrayList<Zone> list) {
	
	// terminal case
	if (xt_u_count == xt_u_fireable.size()) 
	    collectTimeElapseWithTpcAndUrgency_end(z, m, x, y, b, list);
	
	// non-terminal - either eager or delayable
	else {
	    // handle the transition xt_u_count
	    XTransition xt = xt_u_fireable.get(xt_u_count);
	    Urgency xt_u = t_info.get(xt).getUrgency();
	    if (xt_u == Urgency.EAGER)
		collectTimeElapseWithTpcAndUrgency_eager
		    (z, m, xt_u_count, xt_u_fireable, c_count, x, y, b, list);
	    else if (xt_u == Urgency.DELAYABLE) 
		collectTimeElapseWithTpcAndUrgency_delayable
		    (z, m, xt_u_count, xt_u_fireable, c_count, x, y, b, list);   
	}
    }
    
    private void collectTimeElapseWithTpcAndUrgency_end
	(Zone z, Marking m,
	 int[] x, int[] y, int[] b,
	 ArrayList<Zone> list) {
	
	// restrict z according to the current constraints
	boolean consistent = true;
	Zone z_restrict = new Zone(z);
	consistent = z_restrict.intersects(x, y, b);
	if (!consistent) return;
	
	// potential time elapse 
	// perform regular time elapse wrt the marking tpcs 
	Zone z_elapse = getTimeElapseWithTpc(z_restrict, m);
	
	// restrict again wrt the current constraints, 
	consistent = z_elapse.intersects(x, y, b);
	assert(consistent);
		
	// record the obtained zone
	list.add(z_elapse);
    }
    
    private void collectTimeElapseWithTpcAndUrgency_eager
	(Zone z, Marking m,
	 int xt_u_count, ArrayList<XTransition> xt_u_fireable,
	 int c_count, int[] x, int[] y, int[] b,
	 ArrayList<Zone> list) {

	// extract transition guard
	XTransition xt = xt_u_fireable.get(xt_u_count);
	XTransitionInfo xti = t_info.get(xt);
	TConstraints xt_guard = xti.getGuard();

	// for any elementary constraint...
	for(int j = 0; j < xt_guard.size(); j++) {
	    // build its negation
	    if (xt_guard.x[j] == 0 &&
		Bound.Sign(xt_guard.b[j]) == Bound.SignT.LE) {
		// special case: negate x >= k into x <= k
		x[c_count] = xt_guard.y[j];
		y[c_count] = xt_guard.x[j];
		b[c_count] = Bound.Make(Bound.SignT.LE, -Bound.Value(xt_guard.b[j]));
	    }
	    else {
		// otherwise, standard negation
		x[c_count] = xt_guard.y[j];
		y[c_count] = xt_guard.x[j];
		b[c_count] = Bound.Complement(xt_guard.b[j]);
	    }
	    // recursion
	    collectTimeElapseWithTpcAndUrgency_rec
		(z, m, xt_u_count+1, xt_u_fireable, c_count+1, x, y, b, list);
	}	
    }
    
    private void collectTimeElapseWithTpcAndUrgency_delayable
	(Zone z, Marking m,
	 int xt_u_count, ArrayList<XTransition> xt_u_fireable,
	 int c_count, int[] x, int[] y, int[] b,
	 ArrayList<Zone> list) {
	
	// extract transition guard
	XTransition xt = xt_u_fireable.get(xt_u_count);
	XTransitionInfo xti = t_info.get(xt);
	TConstraints xt_guard = xti.getGuard();
	int size = (xt_guard == null) ? 0 : xt_guard.frontSize();
	
	if (size == 0) {
	    // nothing to be done, just recurse and then return
	    collectTimeElapseWithTpcAndUrgency_rec
		(z, m, xt_u_count+1, xt_u_fireable, c_count, x, y, b, list);
	    return;
	}
	  
	// there are upper front constraints
	// iterate here for all potential combinations 
	for(int v = 0; v < (1 << size); v++) {
	    // go through upper front constraints and combine them
	    for(int j = 0, k = 0; j < xt_guard.size(); j++) {
		if (!(xt_guard.y[j] == 0 &&
		      Bound.Sign(xt_guard.b[j]) == Bound.SignT.LE))
		    continue;
		// an upper front constraint here ...
		if ((v & (1 << k)) != 0) {
		    // collect as it is
		    x[c_count + k] = xt_guard.x[j];
		    y[c_count + k] = xt_guard.y[j];
		    b[c_count + k] = xt_guard.b[j];
		}
		else {
		    // or collect the negation
		    x[c_count + k] = xt_guard.y[j];
		    y[c_count + k] = xt_guard.x[j];
		    b[c_count + k] = Bound.Complement(xt_guard.b[j]);
		}
		k++;
	    }
	    // recursion
	    collectTimeElapseWithTpcAndUrgency_rec
		(z, m, xt_u_count+1, xt_u_fireable, c_count+size, x, y, b, list);
	}
    }

}
