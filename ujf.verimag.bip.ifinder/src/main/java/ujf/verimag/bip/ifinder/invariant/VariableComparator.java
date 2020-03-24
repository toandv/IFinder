/*
 *
 *
 */
package ujf.verimag.bip.ifinder.invariant;

import java.util.*;

public class VariableComparator
    implements Comparator<Variable> {
    
    public int compare(Variable a, Variable b) {
	return a.getAccessName().compareTo(b.getAccessName());
    }	    
}
