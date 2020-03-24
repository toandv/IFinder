/*
 *
 *
 */

package ujf.verimag.bip.ifinder.invariant;

import java.util.List;
import java.util.ArrayList;


/*
 * min assertion: clock = min(clock_1, ..., clock_n)
 *
 */

public class ClockMinAssertion extends BasicAssertion {

    protected ClockVariable m_clock;
    protected List<ClockVariable> m_arguments;
    
    public ClockMinAssertion(ClockVariable clock) {
	m_clock = clock;
	m_arguments = new ArrayList<ClockVariable>();
    }
    
    public void addArgument(ClockVariable arg) {
	m_arguments.add(arg);
    }
    
    public String toString() {
	int size = m_arguments.size();
	String result = "(= " + m_clock.getAccessName() + " "; 
	// the min term
	if (size == 0)
	    result += "?";
	else if (size == 1) 
	    result += m_arguments.get(0).getAccessName();
	else { // recursive, as the min function has 2 arguments ...
	    Assertion.Indent++;
	    result += "\n" + blankIndent();
	    result += toString_rec(0, size);
	    Assertion.Indent--;
	}
	result += ")";
	return result;
    }

    // produce the min expression for [i, j)
    private String toString_rec(int i, int j) {
	String result = "";
	if (j == i+1) 
	    result += m_arguments.get(i).getAccessName();
	else {
	    int k = (i + j) / 2;
	    result += "(min ";
	    Assertion.Indent++;
	    result += toString_rec(i, k);
	    result += "\n" + blankIndent();
	    result += toString_rec(k, j);
	    Assertion.Indent--;
	    result += ")";
	}
	return result;
    }
}

