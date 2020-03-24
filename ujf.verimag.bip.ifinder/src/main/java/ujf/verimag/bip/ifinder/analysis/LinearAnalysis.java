/*
 *
 *
 */

package ujf.verimag.bip.ifinder.analysis;

import java.io.*;
import java.util.HashMap;
import java.lang.String;
import java.lang.Process;
    
import bip2.ujf.verimag.bip.types.*;
import bip2.ujf.verimag.bip.connector.*;
import bip2.ujf.verimag.bip.component.atom.*;
import bip2.ujf.verimag.bip.instance.*;
import bip2.ujf.verimag.bip.behavior.*;

import ujf.verimag.bip.ifinder.tool.*;
import ujf.verimag.bip.ifinder.invariant.*;
import ujf.verimag.bip.ifinder.analysis.petrinet.*;

/*
 *
 *
 */

public class LinearAnalysis extends Analysis {

    // external 'simplify' handle
    Process m_simplify;

    // simplify solved form, decode 
    HashMap<Integer, HashMap<Integer, Integer>> m_solution;

    // number of linear assertions
    int m_count;

    //
    // constructor
    // 
    
    protected LinearAnalysis(HashMap<String, String> options,
			     ComponentInstance instance, Tool tool) {
	super("linear", options, instance, tool);

	m_simplify = null;
	m_solution = null;
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
	
	try {
	    // setup simplify handles
	    initializeSimplify();
	    // encode transition flow constraints
	    encodeAll();
	    // parse back the simplify solved form
	    parseSolution();
	    // extract linear invariants 
	    assertion = extractAll();
	}
	catch (IOException e) {
	    m_tool.error("unexpected io exception");
	    m_tool.abort();
	}
	
	m_tool.information("extract " + m_count + " linear constraints", 3);
	
	return assertion;
    }
    
    private void initializeSimplify()
	throws IOException {
	// warning: ensure that cli-lib/simplify.bin have exec permissions
	m_simplify = new ProcessBuilder("simplify.bin", "").start();
    }

    //
    // encode linear invariants constraints
    //

    private void encodeAll()
	throws IOException {
	// encode transition flows
	for(XTransition xtransition : m_net.getITransitions())
	    encodeTransitionFlow(xtransition);
	for(ExportedPortInstance port : m_instance.getExportedPorts())
	    for(XTransition xtransition : m_net.getVTransitions(port))
		encodeTransitionFlow(xtransition);
	// close the streams, to let simplify work
	m_simplify.getOutputStream().close();	
    }

    private void encodeTransitionFlow(XTransition xtransition)
	throws IOException {
	String clause = "[0";
	for(AtomInstance atom : xtransition.getSupport().keySet()) {
	    Transition t = xtransition.getSupport().get(atom);
	    for(State s : t.getSources()) {
		clause += "+x" + m_net.getStateIndex(atom, s);
	    }
	    for(State s : t.getDestinations()) {
		clause += "-x" + m_net.getStateIndex(atom, s);
	    }
	}
	clause += "=0]";
	// System.out.println(clause);
	// add the clause to simplify
	m_simplify.getOutputStream().write(clause.getBytes());
	
    }

    //
    // parse simplify output
    //
    
    void parseSolution()
	throws IOException {
	InputStream is = m_simplify.getInputStream();
	BufferedReader br = new BufferedReader(new InputStreamReader(is));
	// setup solution map
	m_solution = new HashMap<Integer, HashMap<Integer, Integer>>();
	// parse solution line by line
	for(;;) { 
	    String line = br.readLine();
	    if (line == null) break;
	    parseSolutionLine(line);
	}
    }
    
    void parseSolutionLine(String line) {
	// parse solution line [xi = 0+\sum_k ak*xk] into
	// a m_solution entry [ i -> [k -> ak]_k ]
	
	String parts[] = line.split("=");
	
	// left part is a variable "[+xi"
	int left_var = Integer.parseInt(parts[0].substring(3));
	m_solution.put(left_var, new HashMap<Integer, Integer>());
	
	// right part is a linear term "0+\sum_k ak*xk]",
	// break it into monomial tokens
	String token = ""; 
	for(int k = 1; k < parts[1].length()-1; k++) {
	    token += parts[1].charAt(k);
	    char next = parts[1].charAt(k+1);
	    if (next == '+' || next == '-' || next == ']') {
		// we get a new monomial inside token, handle it
		int sign = (token.charAt(0) == '+') ? 1 : -1;
		int star_index = token.indexOf('*');
		int coefficient = (star_index > 0)
		    ? Integer.parseInt(token.substring(1, star_index))
		    : 1;
		int right_var = (star_index > 0) 
		    ? Integer.parseInt(token.substring(star_index + 2))
		    : Integer.parseInt(token.substring(2));
		m_solution.get(left_var).put(right_var, sign * coefficient);
		// cleanup the token, prepare for next
		token = "";
	    }    
	}
    }

    //
    // extract linear invariants
    //
    
    private ComposedAssertion extractAll() {
	
	// create the main assertion
	ComposedAssertion allLinear
	    = new ComposedAssertion(ComposedAssertion.Operator.AND);
	
	// reminder: states are indexed from 1 to n
	int n = m_net.getStateCount();
	
	// generate a solution basis (inside valuation)
	int valuation[] = new int[n+1]; 
	for(int i = 1; i <= n; i++) {
	    if (m_solution.get(i) != null) continue;
	    // otherwise, i is an index of a free variable index,
	    // fix it to 1, all other to 0 
	    for(int k = 1; k <= n; k++)
		valuation[k] = 0;
	    valuation[i] = 1;
	    // compute dependent variables 
	    for(int k : m_solution.keySet()) 
		if (m_solution.get(k).containsKey(i))
		    valuation[k] = m_solution.get(k).get(i);
	    // extract invariant for current valuation
	    allLinear.add(extract(valuation));
	    m_count++;
	}
	
	// return
	return allLinear;
    }
    
    private AtLinearAssertion extract(int[] valuation) {
	// create the linear assertion
	AtLinearAssertion assertion =
	    new AtLinearAssertion(AtLinearAssertion.Operator.EQ);
	// 
	int n = m_net.getStateCount(), bound = 0;
	// fill it
	for(AtomInstance atom : m_net.getAtoms())
	    for(State state : Helper.GetStates(atom)) {
		int i = m_net.getStateIndex(atom, state);
		if (valuation[i] == 0) continue; 
		// add the monomial to the invariant
		AtVariable at_state_i = a_map.get(i);
		assertion.add(at_state_i, valuation[i]);
		// if initial, update the bound
		if (Helper.GetInitStates(atom).contains(state)) 
		    bound += valuation[i];
	    }
	assertion.setBound(bound);
	// return 
	return assertion;
    }
    
}
