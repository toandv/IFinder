/*
 *
 *
 */

package ujf.verimag.bip.ifinder.analysis;

import java.io.*;
import java.util.HashMap;

import bip2.ujf.verimag.bip.instance.*;
import bip2.ujf.verimag.bip.connector.*;
import bip2.ujf.verimag.bip.behavior.*;
import bip2.ujf.verimag.bip.time.*;
import bip2.ujf.verimag.bip.data.*;

import ujf.verimag.bip.ifinder.tool.*;
import ujf.verimag.bip.ifinder.invariant.*;

import org.antlr.runtime.*;
import org.antlr.runtime.tree.*;
import ujf.verimag.bip.ifinder.analysis.guest.*;

/*
 *
 *
 */

public class GuestAnalysis extends Analysis {
    
    // filename
    protected String x_filename;
    
    // constructor
    
    protected GuestAnalysis(HashMap<String, String> options,
			    ComponentInstance instance, Tool tool) {
	super("guest", options, instance, tool);

	// options
	x_filename = getOption("x-filename", null, null);
    }
    
    //
    // assertion generation
    // 
    
    protected Assertion generate() {
	if (x_filename == null)
	    return null;
	
	m_tool.information("parse '" + x_filename + "' ...", 3);

	Assertion assertion = null;
	Context = this;
	try {
	    InputStream istream = new FileInputStream(x_filename);
	    GuestLexer lexer = new GuestLexer(new ANTLRInputStream(istream));
	    GuestParser parser = new GuestParser(new CommonTokenStream(lexer));
	    assertion = parser.start();
	}
	catch (IOException e) {
	    m_tool.error("unexpected io exception");
	    m_tool.abort();
	}
	catch (Exception e) {
	    m_tool.error("unexpected parsing exception");
	    m_tool.abort(); 
	}
	Context = null;
	return assertion;
    }

    //
    // static helpers, called from the guest lexer/parser
    
    static GuestAnalysis Context;
    
    public static Variable LookupVariable(String identifier) {
	Variable variable
	    = LookupVariable_rec(identifier.split("\\."), 0, Context.m_instance);
	if (variable == null)
	    ParseError("unknown variable '" + identifier + "'");
	return variable;
    }
    
    public static Variable LookupVariable_rec(String[] path, int index,
					      ComponentInstance instance) {
	
	// recurse while not at the end of the path
	if (index  < path.length-1 && instance instanceof CompoundInstance) {
	    CompoundInstance compound = (CompoundInstance) instance;
	    for(AtomInstance subAtom : compound.getSubAtomInstances())
		if (subAtom.getDeclaration().getName().equals(path[index]))
		    return LookupVariable_rec(path, index+1, subAtom);
	    for(CompoundInstance subCompound : compound.getSubCompoundInstances())
		if (subCompound.getDeclaration().getName().equals(path[index]))
		    return LookupVariable_rec(path, index+1, subCompound);
	}

	// at the end of the path...
	if (index == path.length-1) {
	    // check if not already declared within the invariant...
	    Variable variable = null;
	    variable = Context.m_invariant.findVariable(instance, Variable.Category.AT,
							path[index]);
	    if (variable != null) return variable;
	    variable = Context.m_invariant.findVariable(instance, Variable.Category.CLOCK,
							path[index]);
	    if (variable != null) return variable;
	    variable = Context.m_invariant.findVariable(instance, Variable.Category.DATA,
							path[index]);
	    if (variable != null) return variable;
	    
	    // name encountered for the first time...
	    if (instance instanceof AtomInstance) {
		AtomInstance atom = (AtomInstance) instance;		
		// could denote an atom state -> at variable
		for(State state : Helper.GetStates(atom)) 
		    if (state.getName().equals(path[index])) {
			variable = new AtVariable(state, atom, Context.m_invariant);
			Context.m_invariant.addVariable(variable);
			return variable;
		    }
		// could denote an atom clock -> clock cariable
		for(ClockDeclaration clock : Helper.GetClocks(atom))
		    if (clock.getName().equals(path[index])) {
			variable = new GenuineClockVariable(clock, atom, Context.m_invariant);
			Context.m_invariant.addVariable(variable);
			return variable;
		    }
		// could denote an atom data -> data variable
		for(DataDeclaration data : Helper.GetData(atom))
		    if (data.getName().equals(path[index])) {
			variable = new DataVariable(data, atom, Context.m_invariant);
			Context.m_invariant.addVariable(variable);
			return variable;
		    }
	    }

	    if (instance instanceof CompoundInstance) {
		CompoundInstance compound = (CompoundInstance) instance;
		// could denote a connector -> history clock variable
		for(ConnectorInstance connector : compound.getSubConnectorInstances())
		    if (connector.getDeclaration().getName().equals(path[index])) {
			variable = new HistoryClockVariable(connector, compound, Context.m_invariant);
			Context.m_invariant.addVariable(variable);
			return variable;
		    }
	    } 
	    
	    // could denote an exported port -> history clock variable
	    for(ExportedPortInstance port : instance.getExportedPorts())
		if (port.getDeclaration().getName().equals(path[index])) {
		    variable = new HistoryClockVariable(port, instance, Context.m_invariant);
		    Context.m_invariant.addVariable(variable);
		    return variable;
		}
	}
	
	return null;
    }
    
    public static Assertion MakeAssertion(HashMap<Variable,Boolean> map, String operator) {
	// operator \in {and, or, [none]}
	// all variables are AtVariable
	AtBooleanAssertion assertion = null;
	if (operator.equals("and") || operator.equals("none"))
	    assertion = new AtBooleanAssertion(AtBooleanAssertion.Operator.AND);
	if (operator.equals("or"))
	    assertion = new AtBooleanAssertion(AtBooleanAssertion.Operator.OR);
	if (assertion == null)
	    ParseError("malformed boolean assertion");
	
	for(Variable variable : map.keySet()) {
	    if (variable instanceof AtVariable)
		assertion.add((AtVariable)variable, map.get(variable));
	    else
		ParseError("malformed boolean assertion");
	}
	return assertion;
    }
    
    public static Assertion MakeAssertion(HashMap<Variable,Integer> map, String operator,
					  int bound) {
	// operator \in {<=, =, >=} 
	// all variables are AtVariable or
	// all variables are ClockVariable and, clock (difference) constraint
	// all variables are DataVariables -- 
	
	if (!(operator.equals("<=") || operator.equals("=") ||
	      operator.equals(">="))) return null;
	
	int n_at = 0, n_clock = 0, n_data = 0; 
	for(Variable variable : map.keySet()) {
	    if (variable instanceof AtVariable) n_at++;
	    if (variable instanceof ClockVariable) n_clock++;
	    if (variable instanceof DataVariable) n_data++;
	}
	
	if (n_at == map.size()) {
	    // all variables are at variables...
	    AtLinearAssertion.Operator op = operator.equals("<=") ?
		AtLinearAssertion.Operator.LE : operator.equals(">=") ?
		AtLinearAssertion.Operator.GE : AtLinearAssertion.Operator.EQ;
	    AtLinearAssertion assertion	= new AtLinearAssertion(op);
	    for(Variable variable : map.keySet()) 
		assertion.add((AtVariable) variable, map.get(variable));
	    assertion.setBound(bound);
	    return assertion;
	}
	
	if (n_clock == map.size()) {
	    // all variables are clock variables ...
	    ClockBoundAssertion.Operator op = operator.equals("=") ?
		ClockBoundAssertion.Operator.EQ : ClockBoundAssertion.Operator.LE;
	    ClockBoundAssertion assertion = null;
	    ClockVariable c1 = null, c2 = null;
	    int a1 = 0, a2 = 0;
	    if (n_clock > 2)
		ParseError("malfomed clock assertion, too many clocks");
	    for(Variable variable : map.keySet()) {
		if (c1 == null) {
		    c1 = (ClockVariable)variable;
		    a1 = map.get(variable);
		    continue;
		}
		if (c2 == null) {
		    c2 = (ClockVariable)variable;
		    a2 = map.get(variable);
		    continue;
		}
	    }
	    if (c1 != null && c2 != null && a1 == -1 && a2 == 1) {
		// swap c1 and c2
		ClockVariable tmp = c1; c1 = c2; c2 = tmp;
		a1 = 1; a2 = -1;
	    }
	    if (c1 != null && c2 != null && a1 == 1 && a2 == -1) {
		assertion = operator.equals(">=") ?
		    new ClockBoundAssertion(c2, c1, op, -bound):
		    new ClockBoundAssertion(c1, c2, op,  bound);
		return assertion;
	    }
	    else if (c1 != null && a1 == 1) {
		assertion = operator.equals(">=") ?
		    new ClockBoundAssertion(c1, null, op, -bound):
		    new ClockBoundAssertion(c1, null, op,  bound);
		return assertion;
	    }
	    else
		ParseError("malformed clock assertion, unexpected form");
	}

	if (n_data == map.size()) {
	    // all variables are data variables...
	    DataLinearAssertion.Operator op = operator.equals("<=") ?
		DataLinearAssertion.Operator.LE : operator.equals(">=") ?
		DataLinearAssertion.Operator.GE : DataLinearAssertion.Operator.EQ;
	    DataLinearAssertion assertion = new DataLinearAssertion(op);
	    for(Variable variable : map.keySet())
		if (variable.getSortName().equals("Int") ||
		    variable.getSortName().equals("Real"))
		    assertion.add((DataVariable) variable, map.get(variable));
		else
		    ParseError("malformed data assertion, unexpected '" +
			       variable.getSortName() + "' variable");
	    assertion.setBound(bound);
	    return assertion;
	}
	
	ParseError("unsupported linear assertion");
	
	return null;
    }

    public static void ParseError(String message) {
	Context.m_tool.error(message);
	Context.m_tool.abort();
    }
    
}
