/*
 * 
 *
 */

package ujf.verimag.bip.ifinder.invariant;

import bip2.ujf.verimag.bip.instance.*;

import bip2.ujf.verimag.bip.port.*;
import bip2.ujf.verimag.bip.connector.*;

/*
 * genuine clock variables, from the model
 *
 */

public class HistoryClockVariable extends ClockVariable {

    public enum Category { PORT, CONNECTOR, INIT };

    protected Category m_category;
    protected ExportedPortInstance m_port;
    protected ConnectorInstance m_connector;
    
    // constructor
    public HistoryClockVariable(ExportedPortInstance port,
				ComponentInstance instance, Invariant invariant) {
	super(instance, invariant);
	m_category = Category.PORT;
	m_port = port;
	m_connector = null;
    }

    public HistoryClockVariable(ConnectorInstance connector,
				ComponentInstance instance, Invariant invariant) {
	super(instance, invariant);
	m_category = Category.CONNECTOR;
	m_port = null;
	m_connector = connector;
    }

    public HistoryClockVariable(ComponentInstance instance, Invariant invariant) {
	super(instance, invariant);
	m_category = Category.INIT;
	m_port = null;
	m_connector = null;
    }
    
    public String getName() {
	String name = null;
	switch (m_category) {
	case PORT:
	    name = m_port.getDeclaration().getName(); break;
	case CONNECTOR:
	    name = m_connector.getDeclaration().getName(); break;
	case INIT:
	    name = "t0"; break; 
	}
	return name;
    }

    public String getAccessName() {
	return (m_category != Category.INIT) ? super.getAccessName() : getName();
    }
    
}

