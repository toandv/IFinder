/*
 *
 *
 */

package ujf.verimag.bip.ifinder.analysis;

import java.util.Map;
import java.util.ArrayList;
import java.util.ListIterator;
import java.util.HashMap;
import java.util.TreeMap;

import bip2.ujf.verimag.bip.types.*;
import bip2.ujf.verimag.bip.connector.*;
import bip2.ujf.verimag.bip.component.atom.*;
import bip2.ujf.verimag.bip.instance.*;
import bip2.ujf.verimag.bip.behavior.*;
import bip2.ujf.verimag.bip.invariant.*;
import bip2.ujf.verimag.bip.time.ClockDeclaration;

import ujf.verimag.bip.ifinder.tool.*;
import ujf.verimag.bip.ifinder.invariant.*;
import ujf.verimag.bip.ifinder.analysis.petrinet.*;
import ujf.verimag.bip.ifinder.analysis.reachability.*;

/*
 *
 *
 */

public class ZoneReachabilityAnalysis extends Analysis {
    
    // clock map
    protected HashMap<Integer, ClockVariable> c_map;
    
    // Petri net explorer
    protected XPetriNetExplorer m_explorer;
    
    // explored symbolic states
    protected TreeMap<Marking, ArrayList<Zone>> m_explored;

    // pending, yet to be explored symbolic states
    protected TreeMap<Marking, ArrayList<Zone>> m_pending;

    // history clock specification
    protected String x_history;

    // marking extract mode: full/positive
    protected String x_marking;

    // zone extraction mode: full/core
    protected String x_zone;
        
    
    //
    
    protected ZoneReachabilityAnalysis(HashMap<String, String> options,
				       ComponentInstance instance, Tool tool) {
	super("zone-reachability", options, instance, tool);
	c_map = null;
	m_explorer = null;
	m_explored = null;
	m_pending = null;
	
	// options
	x_history = getOption("x-history", null, null);
	x_marking = getOption("x-marking", new String[]{"full", "positive"}, "positive");
	x_zone = getOption("x-zone", new String[]{"full", "core"}, "core");
    }

    //
    // applicability tests
    //
    
    protected boolean isApplicable(AtomInstance atom) {
	boolean ok = true;
        AtomType type = (AtomType) atom.getDeclaration().getType();
	PetriNet behavior = type.getBehavior();
	// no external ports in atoms
	ok &= (type.getExternalPortDeclarations().size() == 0) ? true : false;
 	// no data
	ok &= (type.getInternalDataDeclarations().size() == 0) ? true : false;
	// not yet implemented : all clocks have the same units
	// not yet implemented : check initial transition
	// pure time transitions guards and resets
	for(Transition transition : behavior.getTransitions())
	    ok &= Helper.IsTimePure(transition);
	// pure time state tpcs
	for(AtomInvariant invariant : type.getInvariants()) 
	    ok &= Helper.IsTimePureConstraint(invariant.getGuard());
	// return
	return ok;
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

	// adapted initialization, see below
	initializeNet();
	generateAtVariables();
	generateClockVariables();
	
	// create the 'reachable' assertion
	ComposedAssertion reachable
	    = new ComposedAssertion(ComposedAssertion.Operator.OR);
	
	// setup the timed Petri net explorer
	m_explorer = new XPetriNetExplorer(m_instance, m_net);
	m_explorer.initializeControl();
	m_explorer.initializeTiming();
	
	// compute the reachable symbolic states
	computeReachable();
	m_tool.information("reached " + getCount(m_explored) +
			   " symbolic states", 3);
	
	// extract assertions
	for(Marking m : m_explored.keySet())
	    reachable.add(extract(m, m_explored.get(m)));
	
	// return
	return reachable;
    }

    // override from Analysis such that to add history clocks

    protected void initializeNet() {

        // construct the underlying Petri net
        m_tool.information("construct the Petri net ... ", 1);
        m_net = new XPetriNet(m_instance, x_history);
        m_tool.information("|P|=" + m_net.getStateCount() + ", " +
                           "|T|=" + m_net.getTransitionCount() + ", " +
			   "|C|=" + m_net.getClockCount() + " done", 2);
	
	// m_net.dump();
    }

    // generate clock variables and their map
    
    protected void generateClockVariables() {
	assert(m_net != null);
	c_map = new HashMap<Integer, ClockVariable>();
	
	// generate atom clock variables 
	for(AtomInstance atom : m_net.getAtoms())
	    for(ClockDeclaration clock : Helper.GetClocks(atom)) {
		GenuineClockVariable a_clock =
		    new GenuineClockVariable(clock, atom, m_invariant);
		m_invariant.addVariable(a_clock);
		c_map.put(m_net.getAtomClockIndex(atom, clock), a_clock);
	    }
	// generate history clock variables, if any
	for(ExportedPortInstance port : m_instance.getExportedPorts())
	    if (m_net.trackHistory(port)) {
		HistoryClockVariable h_clock =
		    new HistoryClockVariable(port, m_instance, m_invariant);
		m_invariant.addVariable(h_clock);
		c_map.put(m_net.getHistoryClockIndex(port), h_clock);
	    }
	// generate init history clock, if any
	if (m_net.trackInit()) {
	    HistoryClockVariable h_clock =
		new HistoryClockVariable(m_instance, m_invariant);
	    m_invariant.addVariable(h_clock);
	    c_map.put(m_net.getInitClockIndex(), h_clock);
	}
    }

    //
    // reachable symbolic states computation
    //

    protected void computeReachable() {
	MarkingComparator comparator = new MarkingComparator();

	// initialize explored
	m_explored = new TreeMap<Marking, ArrayList<Zone>>(comparator);

	// initialize pending
	m_pending = new TreeMap<Marking, ArrayList<Zone>>(comparator);

	// main reachability loop
	Marking m0 = m_explorer.getInitialMarking();
	Zone z0 = m_explorer.getInitialZone();
	m_pending.put(m0, new ArrayList<Zone>());
	m_pending.get(m0).add(z0);
	
	while (!m_pending.isEmpty()) {
	    
	    // get the first marking from pending ...
	    Marking m = m_pending.firstKey();
	    Zone z = m_pending.get(m).remove(0); 

	    // if the m list becomes empty, remove it from pending
	    if (m_pending.get(m).size() == 0)
		m_pending.remove(m);

	    // compute fireable transitions from marking m
	    ArrayList<XTransition> xt_fireable = m_explorer.getFireable(m);
	    
	    // compute the time progress of z according to the state tpcs
	    // and urgencies/guards of fireable transitions (enabled or
	    // not) in the current marking...
	    ArrayList<Zone> z_elapse_list =
		m_explorer.getTimeElapseZoneList(z, m, xt_fireable);
	    
	    // store the newly obtained symbolic states as explored [no
	    // need to add to the pending as they are immediately
	    // considered for exploration by the discrete transitions]
	    for(Zone z_elapse : z_elapse_list) {
		if (checkIncluded(m_explored, m, z_elapse))
		    continue;
		// the symbolic state (m, z_elapse) is new, store it
		insert(m_explored, m, z_elapse);
	    }

	    // compute the discrete successors for all zones in zl_elapse_list
	    // add all new ones (neither pending nor explored) as pending ...
	    for (XTransition xt : xt_fireable) {
		Marking m_succ = m_explorer.getSuccessorMarking(m, xt);
		for(Zone z_elapse : z_elapse_list) {
		    Zone z_succ = m_explorer.getSuccessorZone(z_elapse, xt);
		    if (z_succ == null) continue;
		    if (checkIncluded(m_explored, m_succ, z_succ) ||
			checkIncluded(m_pending, m_succ, z_succ))
			continue;
		    // the successor (m_succ, z_succ) is new so add it to pending
 		    insert(m_pending, m_succ, z_succ);
		}
	    }   
	}
    }
    
    protected boolean checkIncluded(TreeMap<Marking, ArrayList<Zone>> store,
				    Marking marking, Zone zone) {
	ArrayList<Zone> zoneList = store.get(marking);
	if (zoneList == null) return false;
	boolean included = false;
	for(Zone z : zoneList)
	    if (z.includes(zone)) {
		included = true; break;
	    }
	return included;
    }

    protected void insert(TreeMap<Marking, ArrayList<Zone>> store,
			  Marking marking, Zone zone) {
	if (store.get(marking) == null)
	    store.put(marking, new ArrayList<Zone>());
	// apriori, we know the zone is not included int the current list,
	// however, we will check for the reverse inclusion to simplify the
	// store....
	ArrayList<Zone> zoneList = store.get(marking);
	ListIterator<Zone> iterator = zoneList.listIterator();
	while (iterator.hasNext()) {
	    Zone z = iterator.next();
	    if (zone.includes(z))
		iterator.remove();
	}
	zoneList.add(zone);
    }

    protected int getCount(TreeMap<Marking, ArrayList<Zone>> store) {
	int count = 0;
	for(Marking m : store.keySet())
	    count += store.get(m).size();
	return count;
    }
    
    //
    // assertion extraction from symbolic states
    //
    
    protected ComposedAssertion extract(Marking marking, ArrayList<Zone> zonelist) {
	ComposedAssertion zoneDisjunction = new
	    ComposedAssertion(ComposedAssertion.Operator.OR);
	for(Zone zone : zonelist)
	    zoneDisjunction.add(extract(zone));
	
	ComposedAssertion assertion = new
	    ComposedAssertion(ComposedAssertion.Operator.AND);
	assertion.add(extract(marking));
	assertion.add(zoneDisjunction);
	return assertion;
    }
    
    protected ComposedAssertion extract(Zone zone) {
	ComposedAssertion bounds = null; 
	if (x_zone.equals("full"))
	    bounds = extractFull(zone);
	else if (x_zone.equals("core"))
	    bounds = extractCore(zone);
	return bounds;
    }
    
    // extract all the bounds from the zone

    protected ComposedAssertion extractFull(Zone zone) {
	ComposedAssertion bounds = new
	    ComposedAssertion(ComposedAssertion.Operator.AND);
	int n = zone.size();
	for(int i = 0; i <= n; i++) 
	    if (zone.active(i)) 
		for(int j = 0; j <= n; j++) {
		    if (i == j || !zone.active(j)) continue;
		    if (zone.get(i,j) == Bound.INFINIT) continue;
		    bounds.add(extract(zone, i, j));
		}
	    else 
		bounds.add(new ClockBoundAssertion
			   (c_map.get(i), null, ClockBoundAssertion.Operator.EQ, -1));
	
	return bounds;
    }
    
    // extract all the relevant bounds from the zone

    protected ComposedAssertion extractCore(Zone zone) {
	ComposedAssertion bounds = new
	    ComposedAssertion(ComposedAssertion.Operator.AND);
	int n = zone.size();
	
	// compute linearly dependent active clocks,
	// for any clock i find the lowest index j<i such that
	// c_i is dependent on c_j and record it within dep
	ArrayList<Integer> dep = new ArrayList<Integer>();
	for(int i = 1; i <= n; i++) {
	    if (!zone.active(i)) continue;
	    for(int j = 0; j < i; j++) {
		if (!zone.active(j)) continue;
		if (!zone.dependent(i, j)) continue;
		// dependency found, add the equality bound
		dep.add(i);
		ClockVariable c_i = (i == 0) ? null : c_map.get(i);
		ClockVariable c_j = (j == 0) ? null : c_map.get(j);
		int b_ij = zone.get(i, j);
		ClockBoundAssertion bound = new ClockBoundAssertion
		    (c_i, c_j, ClockBoundAssertion.Operator.EQ, Bound.Value(b_ij));
		bounds.add(bound);
		break;
	    }
	}
	
	// add non-derived bounds between non-dependent active clocks
	for(int i = 0; i <= n; i++) {
	    if (!zone.active(i) || dep.contains(i)) continue;
	    for(int j = 0; j <= n; j++) {
		if (i == j) continue;
		if (!zone.active(j) || dep.contains(j)) continue;
		if (zone.get(i, j) == Bound.INFINIT) continue;
		if (zone.derived(i, j)) continue;
		bounds.add(extract(zone, i, j));
	    }
	}

	// add =-1 bound for inactive clocks
	for(int i = 1; i <= n; i++)
	    if (!zone.active(i)) {
		ClockBoundAssertion bound = new ClockBoundAssertion
		    (c_map.get(i), null, ClockBoundAssertion.Operator.EQ, -1);
		bounds.add(bound);
	    }
	
	return bounds;
    }
    
    // extract one clock bound assertion from the dbm

    ClockBoundAssertion extract(Zone zone, int i, int j) {
	int b_ij = zone.get(i, j);
	assert(b_ij != Bound.INFINIT);
	ClockVariable c_i = (i == 0) ? null : c_map.get(i);
	ClockVariable c_j = (j == 0) ? null : c_map.get(j);
	ClockBoundAssertion bound = new ClockBoundAssertion
	    (c_i, c_j, (Bound.Sign(b_ij) == Bound.SignT.LE ?
			ClockBoundAssertion.Operator.LE : ClockBoundAssertion.Operator.LT),
	     Bound.Value(b_ij));
	return bound;
    }
    
    // unfortunately, replicated from control reachability... 

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
