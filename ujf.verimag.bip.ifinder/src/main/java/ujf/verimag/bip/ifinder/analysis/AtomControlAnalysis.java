/*
 *
 *
 */

package ujf.verimag.bip.ifinder.analysis;

import java.util.HashMap;

import bip2.ujf.verimag.bip.types.*;
import bip2.ujf.verimag.bip.instance.*;
import bip2.ujf.verimag.bip.behavior.*;

import ujf.verimag.bip.ifinder.tool.*;
import ujf.verimag.bip.ifinder.invariant.*;


/*
 *
 *
 */

public class AtomControlAnalysis extends Analysis {

    // the atom type
    AtomType m_type;
    
    // the atom behavior
    PetriNet m_behavior;

    //
    // constructor
    //
    
    protected AtomControlAnalysis(HashMap<String, String> options,
				  ComponentInstance instance, Tool tool) {
	super("atom-control", options, instance,  tool);

	assert(instance instanceof AtomInstance);
	m_type = (AtomType) ((AtomInstance)instance).getDeclaration().getType();
	m_behavior = m_type.getBehavior();
    }

    //
    // assertion generation
    //
    
    protected Assertion generate() {
	// generate all at variables
	generateAtVariables();
	
	// generate invariant assertion
	ComposedAssertion control
	    = new ComposedAssertion(ComposedAssertion.Operator.AND);
	
	// add the big or assertion
	control.add(mkOneOfStates()) ;
	
	// if automaton, generate exclusive assertions
	if (Helper.IsAutomaton(m_behavior)) 
	    for(State s : m_behavior.getStates()) 
		control.add(mkOneStateExclusive(s));

	// return
	return control;
    }

    // generate at_* variables (override from Analysis !!!)
    
    protected void generateAtVariables() {
	AtomInstance atom = (AtomInstance)m_instance;
        // warning: no check for replicated vars
        for(State s : m_behavior.getStates()) {
            AtVariable at_s = new AtVariable(s, atom, m_invariant);
            m_invariant.addVariable( at_s );
        }
    }    

    // generate big or assertions
    
    protected AtBooleanAssertion mkOneOfStates() {
	AtomInstance atom = (AtomInstance)m_instance;
	AtBooleanAssertion assertion = new AtBooleanAssertion
	    (AtBooleanAssertion.Operator.OR);
	for(State s : m_behavior.getStates()) {
	    AtVariable at_s = (AtVariable) m_invariant.findVariable
		(atom, Variable.Category.AT, s.getName());
	    assert(at_s != null);
	    assertion.add(at_s, true);
	}
	return assertion;
    }

    // generate exclusive assertions
    
    protected ComposedAssertion mkOneStateExclusive(State s) {
	AtomInstance atom = (AtomInstance)m_instance;
	
	AtVariable at_s = (AtVariable) m_invariant.findVariable
	    (atom, Variable.Category.AT, s.getName());
	assert(at_s != null);
	
	AtBooleanAssertion left = new AtBooleanAssertion
	    (AtBooleanAssertion.Operator.OR);
	left.add(at_s, true);
	
	AtBooleanAssertion right = new AtBooleanAssertion
	    (AtBooleanAssertion.Operator.AND);
	for(State r : m_behavior.getStates())
	    if (r != s) {
		AtVariable at_r = (AtVariable) m_invariant.findVariable
		    (atom, Variable.Category.AT, r.getName());
		assert(at_r != null);
		right.add(at_r, false);
	    }

	ComposedAssertion assertion = new ComposedAssertion
	    (ComposedAssertion.Operator.EQUIV);
	assertion.add(left);
	assertion.add(right);

	return assertion;
    }

    
    
}
