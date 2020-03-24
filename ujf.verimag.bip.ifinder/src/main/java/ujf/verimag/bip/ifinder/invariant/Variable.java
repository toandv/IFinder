/*
 *
 *
 */

package ujf.verimag.bip.ifinder.invariant;

import bip2.ujf.verimag.bip.instance.*;

/*
 *
 *
 */

public abstract class Variable {
    
    public enum Category { AT, DATA, CLOCK };
    
    // members
    protected Invariant m_invariant;
    protected ComponentInstance m_instance;
    
    // constructor
    Variable(ComponentInstance instance,
	     Invariant invariant) {
        m_instance = instance;
	m_invariant = invariant;
    }
    
    public ComponentInstance getInstance() { return m_instance; }
    
    public abstract String getName();
    
    public abstract Category getCategory();
    
    public boolean equals(Variable variable) {
        return this.getInstance() == variable.getInstance() &&
            this.getCategory() == variable.getCategory() &&
            this.getName().equals(variable.getName());
    }

    public String getAccessName() {
	String accessName = Invariant.Access + 
	    m_invariant.getAccess().get(m_instance) +
	    getName();
	if (Invariant.Flat)
	    accessName = accessName.replace('.', '_');
	return accessName;
    }
    
    public abstract String getSortName();

    public String getDeclaration() {
	return "(declare-const " +
	    getAccessName() + " " + getSortName() + ")\n";
    }
    
}

// for variables within an invariant
// for a component instance, the pair (category, name) must be unique

