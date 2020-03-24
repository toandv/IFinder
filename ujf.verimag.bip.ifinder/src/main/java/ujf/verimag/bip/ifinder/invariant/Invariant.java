/*
 *
 *
 */

package ujf.verimag.bip.ifinder.invariant;

import java.util.List;
import java.util.ArrayList;
import java.util.HashMap;

import bip2.ujf.verimag.bip.instance.*;

import ujf.verimag.bip.ifinder.tool.*;
    
/*
 *
 *
 */

public class Invariant {

    // analysis name/title
    protected String m_title;

    // base component instance + access paths for sub-components
    protected ComponentInstance m_instance;
    protected HashMap<ComponentInstance, String> m_access;

    // the running tool
    protected Tool m_tool;

    // support variables + assertion
    protected HashMap<ComponentInstance, List<Variable>> m_support;
    protected Assertion m_assertion;

    // constructor
    public Invariant(String title,
		     ComponentInstance instance,
		     Tool tool) {
	m_title = title;
	m_instance = instance;
	m_access = tool.createAccess(instance);
	m_tool = tool;
	m_support = new HashMap<ComponentInstance, List<Variable>>();
	m_assertion = null;
    }

    public ComponentInstance getInstance() { return m_instance; }

    public HashMap<ComponentInstance, String> getAccess() { return m_access; }

    public List<Variable> getVariables() {
	List<Variable> variables = new ArrayList<Variable>();
	for(ComponentInstance instance : m_support.keySet())
	    variables.addAll(m_support.get(instance));
	return variables;
    }
    
    public void addVariable(Variable variable) {
	List<Variable> instanceVariables =
	    m_support.get( variable.getInstance() );
	if (instanceVariables == null) {
	    instanceVariables = new ArrayList<Variable>();
	    m_support.put( variable.getInstance(),
			   instanceVariables );
	}
	// warning: no check for replicated vars
	instanceVariables.add(variable);
    }
    
    public Variable findVariable(ComponentInstance instance,
				 Variable.Category category,
				 String name) {
	List<Variable> instanceVariables = m_support.get(instance);
	if ( instanceVariables == null )
	    return null;
	
	Variable result = null;
	for(Variable variable : instanceVariables)
	    if (variable.getCategory() == category &&
		variable.getName().equals(name) ) {
		result = variable;
		break;
	    }
	return result;
    }
    
    public void setAssertion(Assertion assertion) {
	m_assertion = assertion;
    }

    public String toString() {
       	return (m_assertion != null) ? m_assertion.toString() : "true";
    }
    
    public String header() {
	return "\n; invariant " + m_title + " [" +
	    m_instance.getDeclaration().getType().getName() + " " +
	    Invariant.Access + "]\n\n";
    }

    public String footer() {
	return "\n\n;       end " + m_title + " [" +
	    m_instance.getDeclaration().getType().getName() + " " +
	    Invariant.Access + "]\n\n";
    }
    
    // outside invariant access
    // must be set by the tool before printing and/or exporting
    public static String Access = "";

    // flatten variable names 
    public static boolean Flat = false;
    
}

