/*
 * 
 *
 */

package ujf.verimag.bip.ifinder.invariant;

import bip2.ujf.verimag.bip.instance.*;
import bip2.ujf.verimag.bip.behavior.State;

/*
 * at variables
 *
 */

public class AtVariable extends Variable {

    // members
    protected State m_state;
    
    // constructor
    public AtVariable(State state, AtomInstance instance, Invariant invariant) {
        super(instance, invariant);
        m_state = state;
    }
    
    public String getName() { return m_state.getName(); }
    
    public Category getCategory() { return Category.AT; }
    
    public String getSortName() {
	return Encoding == EncodingT.INTEGER2 ? "Int" : "Bool";
    }

    public String getDeclaration() {
	// not yet finished : would be simplified with an Int2 sort...
	String declaration = super.getDeclaration();
	if (Encoding == EncodingT.INTEGER2) {
	    String accessName = getAccessName();
	    declaration += "(assert (<= 0 " + accessName + " 1))\n";
	}
	return declaration;
    }
    
    // SMT encoding
    
    public enum EncodingT {BOOLEAN, INTEGER2};
    
    public static EncodingT Encoding = EncodingT.INTEGER2;
    
}
