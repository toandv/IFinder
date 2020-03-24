/*
 *
 *
 */

package ujf.verimag.bip.ifinder.invariant;

import bip2.ujf.verimag.bip.actionlang.ValuedExpression;

/*
 * encapsulate BIP boolean expressions as assertions
 *
 */

public class DataAssertion extends BasicAssertion {

    protected ValuedExpression m_expression;
    
    public DataAssertion(ValuedExpression expression) {
	m_expression = expression;
    }
    
    public String toString() {
	// not yet implemented
	return null;
    }

}
