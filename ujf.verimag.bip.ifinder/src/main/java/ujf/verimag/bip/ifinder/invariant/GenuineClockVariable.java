/*
 * 
 *
 */

package ujf.verimag.bip.ifinder.invariant;

import bip2.ujf.verimag.bip.instance.*;
import bip2.ujf.verimag.bip.time.ClockDeclaration;

/*
 * genuine clock variables, from the model
 *
 */

public class GenuineClockVariable extends ClockVariable {

    // members
    ClockDeclaration m_clock;

    // constructor
    public GenuineClockVariable(ClockDeclaration clock,
				AtomInstance instance, Invariant invariant) {
	super(instance, invariant);
	m_clock = clock;
    }

    public String getName() { return m_clock.getName(); }
    
}

