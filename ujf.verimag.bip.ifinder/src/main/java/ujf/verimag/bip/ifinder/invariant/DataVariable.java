/*
 * 
 *
 */

package ujf.verimag.bip.ifinder.invariant;

import java.util.HashMap;

import bip2.ujf.verimag.bip.instance.*;
import bip2.ujf.verimag.bip.data.DataDeclaration;
import bip2.ujf.verimag.bip.data.DataType;

/*
 * data variables in BIP
 *
 */

public class DataVariable extends Variable {

    // members
    protected DataDeclaration m_data;

    // constructor
    public DataVariable(DataDeclaration data,
			AtomInstance instance, Invariant invariant) {
	super(instance, invariant);
	m_data = data;
    }
    
    public String getName() { return m_data.getName(); }

    public Category getCategory() { return Category.DATA; }

    public String getSortName() {
	String typename = m_data.getDataType().getName();
	String sortname = Encoding.get(typename);
	return (sortname != null) ? sortname : typename;
    }

    // SMT encoding

    private static HashMap<String, String> Encoding =
	new HashMap<String,String>() {{
	    put("int", "Int");
	}};
    
}
