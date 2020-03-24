/*
 *
 *
 */

package ujf.verimag.bip.ifinder.analysis.petrinet;

import java.util.HashMap;
import java.util.ArrayList;

import bip2.ujf.verimag.bip.instance.*;
import bip2.ujf.verimag.bip.behavior.*;

/*
 *
 *
 */

public class XTransition {

    // the component instance where the transition is defined
    protected ComponentInstance m_instance;

    // for visible transitions, their exported port
    protected ExportedPortInstance m_port;
    
    // the set of contained transitions
    protected HashMap<AtomInstance, Transition> m_support;

    // constructor from a transition in atomic component

    protected XTransition(AtomInstance atom, Transition transition) {
	m_instance = atom;
	m_port = null;
	m_support = new HashMap<AtomInstance,Transition>();
	m_support.put(atom, transition);
    }
    
    // constructor from a set of syncronized transitions

    protected XTransition(CompoundInstance compound,
			  ArrayList<XTransition> xtransitionSet) {
	m_instance = compound;
	m_port = null;
	m_support = new HashMap<AtomInstance, Transition>();
	for(XTransition xtransition : xtransitionSet) {
	    for(AtomInstance atom : xtransition.getSupport().keySet()) {
		assert( m_support.get(atom) == null );
		m_support.put(atom, xtransition.getSupport().get(atom));
	    }
	}
    }

    // replica constructor (need at copy from subcomponents ...)

    protected XTransition(CompoundInstance compound, XTransition xtransition) {
	m_instance = compound;
	m_port = null;
	m_support = new HashMap<AtomInstance, Transition>();
	for(AtomInstance atom : xtransition.getSupport().keySet()) 
	    m_support.put(atom, xtransition.getSupport().get(atom));
    }

    // access / update
    
    public HashMap<AtomInstance, Transition> getSupport() { return m_support; }
    
    public ExportedPortInstance getExportedPort() { return m_port; }
    
    public void setExportedPort(ExportedPortInstance port) { m_port = port; }

    // dump
    
    public void dump() {
	System.out.print("["); 
	if (m_port != null)
	    System.out.print(m_port.getDeclaration().getName());
	System.out.print("] ");
	for(AtomInstance atom : m_support.keySet()) {
	    Transition t = m_support.get(atom);
	    for(State s : t.getSources())
		System.out.print(atom.getDeclaration().getName() +
				 "." + s.getName() + " ");
	}
	System.out.print(" -> ");
	for(AtomInstance atom : m_support.keySet()) {
	    Transition t = m_support.get(atom);
	    for(State s : t.getDestinations())
		System.out.print(atom.getDeclaration().getName() +
				 "." + s.getName() + " ");
	}
	System.out.println();
    }
}
