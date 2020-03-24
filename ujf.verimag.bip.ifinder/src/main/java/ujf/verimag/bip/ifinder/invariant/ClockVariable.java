/*
 * 
 *
 */

package ujf.verimag.bip.ifinder.invariant;

import bip2.ujf.verimag.bip.instance.*;

/*
 * clock variables
 *
 */

public abstract class ClockVariable extends Variable {

    public enum EncodingType {INTEGER, REAL};

    // constructor
    public ClockVariable(ComponentInstance instance, Invariant invariant) {
	super(instance, invariant);
    }

    public Category getCategory() { return Category.CLOCK; }

    public String getSortName() { return "Real"; }
    
     public String getDeclaration() {
	 // clocks belong to {-1} \cup [0,oo)
	 String declaration = super.getDeclaration();
	 String accessName = getAccessName();
	 declaration +=
	     "(assert (or (= -1 " + accessName + ")\n" +
	     "            (<= 0 " + accessName + ")))\n";
	 return declaration;
     }
    
    //

    public static EncodingType Encoding = EncodingType.REAL;
    
}
