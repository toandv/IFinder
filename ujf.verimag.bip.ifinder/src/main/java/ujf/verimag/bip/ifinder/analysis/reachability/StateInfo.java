/*
 *
 *
 */

package ujf.verimag.bip.ifinder.analysis.reachability;

import java.util.ArrayList;
import java.util.HashMap;

import bip2.ujf.verimag.bip.instance.*;
import bip2.ujf.verimag.bip.behavior.*;
import bip2.ujf.verimag.bip.invariant.*;
import bip2.ujf.verimag.bip.actionlang.*;

import ujf.verimag.bip.ifinder.analysis.*;
import ujf.verimag.bip.ifinder.analysis.petrinet.*;

/*
 *
 *
 */

public class StateInfo {
    
    private AtomInstance m_atom;
    private State m_state;
    private TConstraints m_tpc;
    
    public StateInfo(AtomInstance atom, State state) {
	m_atom = atom;
	m_state = state;
	m_tpc = null;
    }

    public void initializeControl(XPetriNet net) {
	// empty
    }

    public void initializeTiming(XPetriNet net) {
	// obtain invariants of the state
	ArrayList<AtomInvariant> invariants = Helper.GetInvariants(m_atom, m_state);

	// extract the constraints from invariants
	ArrayList<BinaryOpExpression> constraints =
	    new ArrayList<BinaryOpExpression>();
	for(AtomInvariant invariant : invariants)
	    constraints.addAll(Helper.GetTConstraints(invariant.getGuard()));

	// prepare enconding
	HashMap<AtomInstance, ArrayList<BinaryOpExpression>> map =
	    new HashMap<AtomInstance, ArrayList<BinaryOpExpression>>();
	map.put(m_atom, constraints);

	ArrayList<Integer> X = new ArrayList<Integer>();
	ArrayList<Integer> Y = new ArrayList<Integer>();
	ArrayList<Integer> B = new ArrayList<Integer>();
	XTransitionInfo.Encode(map, X, Y, B, net);

	// if no constraints, return
	if (X.size() == 0) return;

	// transform to raw int vectors and initialize tpc
	int size = X.size();
	int[] x = new int[size];
	int[] y = new int[size];
	int[] b = new int[size];
	for(int k = 0; k < size; k++) {
	    x[k] = X.get(k);
	    y[k] = Y.get(k);
	    b[k] = B.get(k);
	}
	m_tpc = new TConstraints(x, y, b);
    }

    //
    // accessors
    //
    
    public TConstraints getTpc() { return m_tpc; }
    
}
