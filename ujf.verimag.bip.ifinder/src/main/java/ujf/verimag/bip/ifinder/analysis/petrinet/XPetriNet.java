package ujf.verimag.bip.ifinder.analysis.petrinet;

/*
 *
 *
 */

import java.util.HashMap;
import java.util.ArrayList;
import java.util.Arrays;

import bip2.ujf.verimag.bip.instance.*;
import bip2.ujf.verimag.bip.types.AtomType;
import bip2.ujf.verimag.bip.behavior.*;
import bip2.ujf.verimag.bip.time.ClockDeclaration;

import ujf.verimag.bip.ifinder.analysis.*;

/*
 *
 *
 */

public class XPetriNet {

    // component instance
    protected ComponentInstance m_component;
    
    // the set of atomic components 
    protected ArrayList<AtomInstance> m_atoms; 
    
    // visible transitions on exported ports (for composition)
    protected HashMap<ExportedPortInstance, ArrayList<XTransition>> v_transitions;
    
    // invisible transitions (completed herein)
    protected ArrayList<XTransition> i_transitions;
    
    // atom state indexing 
    protected HashMap<AtomInstance, HashMap<State, Integer>> s_index;
    
    // atom clock indexing
    protected HashMap<AtomInstance, HashMap<ClockDeclaration, Integer>> c_index;
    
    // history clock indexing for exported ports, if tracked
    protected HashMap<ExportedPortInstance, Integer> h_index;

    // init history clock index, if tracked
    protected int h_init;
    
    //
    // constructor
    //

    public XPetriNet(ComponentInstance component, String x_history) {
	m_component = component;
	m_atoms = new ArrayList<AtomInstance>();
	v_transitions = new HashMap<ExportedPortInstance, ArrayList<XTransition>>();
	for(ExportedPortInstance export : m_component.getExportedPorts())
	    v_transitions.put( export, new ArrayList<XTransition>());
	i_transitions = new ArrayList<XTransition>();
	if (component instanceof AtomInstance)
	    initializeAtom();
	if (component instanceof CompoundInstance)
	    initializeCompound();
	for(ExportedPortInstance exportedPort : m_component.getExportedPorts())
	    for(XTransition xtransition : v_transitions.get(exportedPort))
		xtransition.setExportedPort(exportedPort);
	// indexes
	s_index = new HashMap<AtomInstance, HashMap<State, Integer>>();
	initializeSIndex();
	c_index = new HashMap<AtomInstance, HashMap<ClockDeclaration, Integer>>();
	initializeCIndex();
	h_index = new HashMap<ExportedPortInstance, Integer>();
	initializeHIndex(x_history);
	h_init = -1;
	initializeHInit(x_history);
    }
    
    // initialize the Petri net of an atom

    private void initializeAtom() {
	AtomInstance atom = (AtomInstance) m_component;
	AtomType type = (AtomType) atom.getDeclaration().getType();
	
	// define atoms
	m_atoms.add(atom);
	
	// define transitions
	for(Transition t : type.getBehavior().getTransitions()) {
	    boolean exported = false;
	    for(ExportedPortInstance exportedPort : atom.getExportedPorts())
		for(PortInstance innerPort : exportedPort.getReferencedPorts()) 
		    if (innerPort.getDeclaration() == t.getTriggerPort()) {
			// if exported, create a visible transition
			XTransition new_t = new XTransition(atom, t);
			v_transitions.get(exportedPort).add(new_t);
			exported = true;
		    }
	    // create an invisible transition
	    if (!exported) 
		i_transitions.add(new XTransition(atom, t));
	}
    }
    
    // initialize the Petri net of a compound, recursive

    private void initializeCompound() {
	CompoundInstance compound = (CompoundInstance) m_component;
	
	HashMap<ComponentInstance, XPetriNet> subNets =
	    new HashMap<ComponentInstance, XPetriNet>();
	
	// compute the Petri nets of all subcomponents
	for(AtomInstance subAtom : compound.getSubAtomInstances()) 
	    subNets.put(subAtom,  new XPetriNet(subAtom, null));
	for(CompoundInstance subCompound : compound.getSubCompoundInstances())
	    subNets.put(subCompound, new XPetriNet(subCompound, null));
	
	// define atoms
	for(ComponentInstance subcomponent : subNets.keySet())
	    m_atoms.addAll(subNets.get(subcomponent).getAtoms());
	
	// copy invisible transitions from sub-components
	for(ComponentInstance subcomponent : subNets.keySet())
	    i_transitions.addAll(subNets.get(subcomponent).getITransitions());
	
	// create invisible transitions for top connectors
	for(ConnectorInstance connector : compound.getSubConnectorInstances())
	    if (connector.getExportedPort() == null) {
		ArrayList<XTransition> list =
		    synchronize(compound, subNets, connector);
		i_transitions.addAll(list);
	    }
	
	// define exported transitions for every exported port
	for(ExportedPortInstance exportedPort : compound.getExportedPorts())
	    for(PortInstance innerPort : exportedPort.getReferencedPorts()) {
		// if we export a connector, compute the synchronized transitions
		if (innerPort instanceof ConnectorPortInstance) {
		    ConnectorInstance connector =
			Helper.GetConnector(compound, (ConnectorPortInstance)innerPort);
		    assert(connector != null);
		    ArrayList<XTransition> list =
			synchronize(compound, subNets, connector);
		    v_transitions.get(exportedPort).addAll(list);
		}
		// if we export a subcomponent port, copy the subcomponent transitions
		if (innerPort instanceof ExportedPortInstance) {
		    ComponentInstance subComponent =
			Helper.GetComponent(compound, (ExportedPortInstance)innerPort);
		    assert(subComponent != null);
		    ArrayList<XTransition> list = new ArrayList<XTransition>();
		    for(XTransition xtransition : subNets.get(subComponent).getVTransitions((ExportedPortInstance)innerPort))
			list.add(new XTransition(compound, xtransition));
		    v_transitions.get(exportedPort).addAll(list);
		}
	    }

    }
    
    // define the set of PN transitions for a connector instance
    
    private ArrayList<XTransition> synchronize
	(CompoundInstance compound, HashMap<ComponentInstance, XPetriNet> subNets,
	 ConnectorInstance connector) {
	
	// compute the synchronization map 
	HashMap<ComponentInstance, ExportedPortInstance> syncMap =
	    new HashMap<ComponentInstance, ExportedPortInstance>();
	for(ExportedPortInstance port : Helper.GetPorts(compound, connector)) {
	    ComponentInstance subComponent =
		Helper.GetComponent(compound, port);
	    assert(subComponent != null);
	    syncMap.put(subComponent, port);
	}
	// call the helper 
	return synchronize(compound, subNets, syncMap);	
    }
    
    private ArrayList<XTransition> synchronize
	(CompoundInstance compound, HashMap<ComponentInstance, XPetriNet> subNets,
	 HashMap<ComponentInstance, ExportedPortInstance> syncMap) {
	// the result list
	ArrayList<XTransition> list = new ArrayList<XTransition>(); 
	// the currently synchronized/selected set
	ArrayList<XTransition> selection = new ArrayList<XTransition>();
	// recursively, by decreasing the syncMap
	synchronize_rec(compound, subNets, syncMap, selection, list);
	// return
	return list;
    }

    private void synchronize_rec(CompoundInstance compound,
				 HashMap<ComponentInstance, XPetriNet> subNets,
				 HashMap<ComponentInstance, ExportedPortInstance> syncMap,
				 ArrayList<XTransition> selection,
				 ArrayList<XTransition> list) {
	if (syncMap.keySet().size() == 0) {
	    // terminal case: create a new transition from the current selection
	    XTransition new_t = new XTransition(compound, selection);
	    list.add(new_t);
	}
	else {
	    // recursive case: expand the current selection for some component...
	    ComponentInstance subComponent = syncMap.keySet().iterator().next();
	    ExportedPortInstance exportedPort = syncMap.get(subComponent);
	    syncMap.remove(subComponent);
	    for(XTransition subTransition : subNets.get(subComponent).getVTransitions(exportedPort)) {
		selection.add(subTransition);
		synchronize_rec(compound, subNets, syncMap, selection, list);
		selection.remove(subTransition);
	    }
	    syncMap.put(subComponent, exportedPort);
	}	
    }

    //
    // indexing
    //
    
    // create state index

    private void initializeSIndex() {
	int index = 1; // !!! skip the zero index !!!
	for(AtomInstance atom : m_atoms) {
	    s_index.put(atom, new HashMap<State, Integer>());
	    for(State state : Helper.GetStates(atom))
		s_index.get(atom).put(state, index++);
	}
    }

    // create clock index

    private void initializeCIndex() {
	int index = 1; // !!! skip the zero index !!!
	for(AtomInstance atom : m_atoms) {
	    c_index.put(atom, new HashMap<ClockDeclaration, Integer>());
	    for(ClockDeclaration clock : Helper.GetClocks(atom))
		c_index.get(atom).put(clock, index++);
	}
    }

    // create history clock index, if any...

    private void initializeHIndex(String x_history) {
	h_index.clear();
	
	if (x_history == null)
	    // nothing to be tracked
 	    return;
	
	int index = getAtomClockCount() + 1;
	
	if (x_history.equals("*")) {
	    // all exported ports tracked
	    for(ExportedPortInstance port: m_component.getExportedPorts())
		h_index.put(port, index++);
	}
	else {
	    // some exported ports tracked, split the string based on ':'
	    String tokens[] = x_history.split(":");
	    for(ExportedPortInstance port : m_component.getExportedPorts()) {
		boolean found = false;
		for(int i = 0; !found && i < tokens.length; i++)
		    found = tokens[i].equals(port.getDeclaration().getName());
		if (found)
		    h_index.put(port, index++);
	    }
	}
    }
    

    private void initializeHInit(String x_history) {
	if (x_history == null)
	    // nothing to be tracked
 	    return;
	
	if (x_history.equals("*") ||
	    Arrays.asList(x_history.split(":")).contains("t0")) {
	    // init tracked
	    h_init = getAtomClockCount() + getHistoryClockCount() + 1;
	}
	
	// not yet implemented : warn if t0 denotes a tracked exported port 	
    }
    
    //
    // accessors
    //
    
    public ArrayList<AtomInstance> getAtoms() {
	return m_atoms;
    }
    
    public ArrayList<XTransition> getITransitions() {
	return i_transitions;
    }
    
    public ArrayList<XTransition> getVTransitions(ExportedPortInstance export) {
	return v_transitions.get(export);
    }
    
    public int getStateCount() {
	int count = 0;
	for(AtomInstance atom : m_atoms) 
	    count += Helper.GetStates(atom).size();
	return count;
    }
    
    public int getStateIndex(AtomInstance atom, State state) {
	return s_index.get(atom).get(state); 
    }
    
    public int getClockCount() {
	return getAtomClockCount() + getHistoryClockCount() +
	    ((h_init != -1) ? 1 : 0);
    }
    
    public int getAtomClockCount() {
	int count = 0;
	for(AtomInstance atom : m_atoms)
	    count += Helper.GetClocks(atom).size();
	return count;
    }
    
    public int getAtomClockIndex(AtomInstance atom, ClockDeclaration clock) {
	return c_index.get(atom).get(clock);
    }
    
    public int getHistoryClockCount() {
	return h_index.size();
    }
    
    public int getHistoryClockIndex(ExportedPortInstance port) {
	return h_index.get(port);
    }
    
    public boolean trackHistory(ExportedPortInstance port) {
	return h_index.keySet().contains(port);
    }

    public int getInitClockIndex() {
	return h_init;
    }

    public boolean trackInit() {
	return h_init != -1;
    }
    
    public int getTransitionCount() {
	int count = i_transitions.size();
	for(ExportedPortInstance port : m_component.getExportedPorts())
	    count += v_transitions.get(port).size();
	return count;
    }

    //
    // dump
    //
    
    public void dump() {
	
	// dump transitions
	for(ExportedPortInstance port : m_component.getExportedPorts())
	    for(XTransition xtransition : v_transitions.get(port)) 
		xtransition.dump();
	for(XTransition xtransition : i_transitions)
	    xtransition.dump();
	System.out.println();
	
	// dump state index
	for(AtomInstance atom : m_atoms) 
	    for(State s : Helper.GetStates(atom)) 
		System.out.println( atom.getDeclaration().getName() +
				    "." + s.getName() + " = " + 
				    getStateIndex(atom, s));
	System.out.println();
	
	// dump atom/history clock indexes
	for(AtomInstance atom : m_atoms)
	    for(ClockDeclaration clock : Helper.GetClocks(atom)) 
		System.out.println( atom.getDeclaration().getName() +
				    "." + clock.getName() + " = " +
				    getAtomClockIndex(atom, clock));
	for(ExportedPortInstance port : m_component.getExportedPorts())
	    if (trackHistory(port))
		System.out.println( port.getDeclaration().getName() + " = " +
				    getHistoryClockIndex(port));
	System.out.println();
    }
    
}

