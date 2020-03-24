/*
 *
 *
 */

package ujf.verimag.bip.ifinder.invariant;

import java.util.Set;
import java.util.HashMap;
import java.util.Arrays;

/*
 * boolean assertions involving only at-variables
 *
 */

public class AtBooleanAssertion extends BasicAssertion {
    
    public enum Operator {AND, OR};
    
    // members
    protected HashMap<AtVariable,Boolean> m_clause;
    protected Operator m_operator;
    
    public AtBooleanAssertion(Operator operator) {
	m_clause = new HashMap<AtVariable,Boolean>();
	m_operator = operator;
    }

    public void add(AtVariable variable, Boolean sign) {
	m_clause.put(variable, sign);
    }

    public String toString() {
	switch (AtVariable.Encoding) {
	case BOOLEAN:
	    return toString_boolean();
	case INTEGER2:
	    return toString_integer2();
	}
	return null;
    }
    
    private String toString_boolean() {
	String result = "";
	int size = m_clause.size();
	AtVariable variables[] = m_clause.keySet().toArray(new AtVariable[0]);
	Arrays.sort(variables, new VariableComparator());
	
	if (size == 0)
	    result += (m_operator == Operator.AND) ? "true" : "false";
	else if (size == 1) {
	    result += m_clause.get(variables[0]) ? "" : "(not ";
	    result += variables[0].getAccessName();
	    result += m_clause.get(variables[0]) ? "" : ")";
	}
	else {
	    result += (m_operator == Operator.AND) ? "(and " : "(or ";
	    Assertion.Indent++;
	    for (int i = 0; i < size; i++) {
		if (i != 0) { result += "\n" + blankIndent(); }
		boolean i_sign =  m_clause.get(variables[i]);
		result += i_sign ? "" : "(not ";
		result += variables[i].getAccessName();
		result += i_sign ? "" : ")";
	    }
	    Assertion.Indent--;
	    result += ")";
	}

	return result;
    }
    
    private String toString_integer2() {
	String result = "";
	int size = m_clause.size();
	AtVariable variables[] = m_clause.keySet().toArray(new AtVariable[0]);
	Arrays.sort(variables, new VariableComparator());
	
	if (size == 0)
	    result += (m_operator == Operator.AND) ? "true" : "false";
	else if (size == 1) {
	    result += "(= ";
	    result += m_clause.get(variables[0]) ? "" : "(- 1 ";
	    result += variables[0].getAccessName();
	    result += m_clause.get(variables[0]) ? "" : ")";
	    result += " 1)";
	}
	else {
	    result += "(>= (+ ";
	    Assertion.Indent++;
	    for (int i = 0; i < size; i++) {
		if (i != 0) { result += "\n" + blankIndent(); }
		boolean i_sign =  m_clause.get(variables[i]);
		result += i_sign ? "" : "(- 1 ";
		result += variables[i].getAccessName();
		result += i_sign ? "" : ")";
	    }
	    Assertion.Indent--;
	    result += ") ";
	    result += (m_operator == Operator.AND) ? size : "1";
	    result += ")";
	}

	return result;
    }
}
