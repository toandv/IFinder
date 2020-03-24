/*
 *
 *
 */

package ujf.verimag.bip.ifinder.invariant;

import java.util.Set;
import java.util.HashMap;
import java.util.Arrays;

/*
 * linear assertions involving data- variables
 * 
 */

public class DataLinearAssertion extends BasicAssertion {

    public enum Operator {LE, EQ, GE};
    
    // members
    protected HashMap<DataVariable,Integer> m_term;
    protected Operator m_operator;
    protected int m_bound;
    
    public DataLinearAssertion(Operator operator) {
	m_term = new HashMap<DataVariable,Integer>();
	m_operator = operator;
	m_bound = 0;
    }
    
    public void add(DataVariable variable, Integer coefficient) {
	m_term.put(variable, coefficient);
    }
    
    public void setBound(int bound) {
	m_bound = bound;
    }
    
    public String toString() {
	// not yet implemented : did not work for boolean encoding
	
	String result = "(";
	int size = m_term.size();
	DataVariable variables[] = m_term.keySet().toArray( new DataVariable[0] );
	Arrays.sort(variables, new VariableComparator());

	// prepare left and right terms
	// such that to have positive coeffs on both sides
	Assertion.Indent += 2;
	String left = "", right = "";
	int left_count = 0, right_count = 0;
	for(int i = 0; i < size; i++) {
	    DataVariable var_i = variables[i];
	    int c = m_term.get(var_i);
	    if (c > 0) {
		if (left_count != 0) left += "\n" + blankIndent();
		if (c != 1) left += "(* " + c + " ";
		left += var_i.getAccessName();
		if (c != 1) left += ")";
		left_count++;
	    }
	    if (c < 0) {
		if (right_count != 0) right += "\n" + blankIndent();
		if (c != -1) right += "(* " + (-c) + " ";
		right += var_i.getAccessName();
		if (c != -1) right += ")";
		right_count++;
	    }
	}
	if (m_bound > 0) {
	    if (right_count != 0) right += "\n" + blankIndent();
	    right += m_bound;
	    right_count++;
	}
	if (m_bound < 0) {
	    if (left_count != 0) left += "\n" + blankIndent();
	    left += (-m_bound);
	    left_count++;
	}
	Assertion.Indent -= 2;
	
	// the result
	result += (m_operator == Operator.EQ) ? "=" :
	    (m_operator == Operator.LE) ? "<=" : ">=";
	result += " ";

	result += left_count == 0 ? "0" :
	    (left_count == 1 ? left : "(+ " + left + ")");

	if (left_count <= 1 || right_count <= 1)
	    result += " ";
	else {
	    Assertion.Indent++;
	    result += "\n" + blankIndent();
	    Assertion.Indent--;
	}

	result += right_count == 0 ? "0" :
	    (right_count == 1) ? right : "(+ " + right + ")";

	// end
	result += ")";
	return result;
    }
}
