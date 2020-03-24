/*
 * 
 *
 */

package ujf.verimag.bip.ifinder.invariant;

import java.util.List;
import java.util.ArrayList;

/*
 * composed assertions
 *
 */

public class ComposedAssertion extends Assertion {

    public enum Operator {AND, OR, IMPLIES, EQUIV};
    
    // members
    protected Operator m_operator;
    protected List<Assertion> m_assertions;
    
    public ComposedAssertion(Operator operator) {
	m_operator = operator;
	m_assertions = new ArrayList<Assertion>();
    }
    
    public void add(Assertion assertion) {
	m_assertions.add(assertion);
    }
    
    public String toString() {
	String result = "";
	switch (m_operator) {
	case AND:
	case OR:
	    if (m_assertions.size() == 0)
		result += (m_operator == Operator.AND) ? "true" : "false";
	    else if (m_assertions.size() == 1)
		result += m_assertions.get(0).toString();
	    else {
		result += (m_operator == Operator.AND) ? "(and " : "(or ";
		Assertion.Indent++;
		for(int i = 0; i < m_assertions.size(); i++) {
		    if (i >= 1) { result += "\n" + blankIndent(); }
		    result += m_assertions.get(i).toString();
		}
		Assertion.Indent--;
		result += ")";
	    }
	    break;
	    
	case IMPLIES:
	case EQUIV:
	    result += (m_operator == Operator.IMPLIES) ? "(=> " : "(= ";
	    if (m_assertions.size() == 2) {
		Assertion.Indent++;
		result += m_assertions.get(0).toString();
		result += "\n" + blankIndent();
		result += m_assertions.get(1).toString();
		Assertion.Indent--;
	    }
	    else
		result += "?";
	    result += ")";
	    break;
	}
	
	return result;
    }

    public boolean isAnd() {
	return m_operator == Operator.AND;
    }
    public boolean isOr() {
	return m_operator == Operator.OR;
    }
    
}
