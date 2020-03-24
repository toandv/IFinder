/*
 *
 *
 */

package ujf.verimag.bip.ifinder.invariant;

/*
 * difference assertion : x - y <= v
 *
 */

public class ClockBoundAssertion extends BasicAssertion {

    public enum Operator { EQ, LE, LT };

    protected ClockVariable x, y; //
    protected Operator operator; // operator
    protected int value; // 
    
    public ClockBoundAssertion
	(ClockVariable _x, ClockVariable _y, Operator _operator, int _value) {
	x = _x; y = _y; operator = _operator; value = _value;
    }
    
    public String toString() {
	String result = "(";
	result += (operator == Operator.EQ) ? "=" :
	    (operator == Operator.LE) ? "<=" : "<";
	result += " ";
	if (x != null && y != null) {
	    if (value < 0) result += "(+ ";
	    result += x.getAccessName();
	    if (value < 0) result += " " + (-value) + ")";
	    result += " ";
	    if (value > 0) result += "(+ ";
	    result += y.getAccessName();
	    if (value > 0) result += " " + value + ")";
	}
	if (x != null && y == null) {
	    result += x.getAccessName();
	    result += " ";
	    result += value;
	}
	if (x == null && y != null) {
	    result += (-value);
	    result += " ";
	    result += y.getAccessName();
	}
	result += ")";
	return result;
    }
}
