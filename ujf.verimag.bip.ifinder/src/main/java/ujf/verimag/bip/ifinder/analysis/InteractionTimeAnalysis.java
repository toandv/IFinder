/*
 *
 *
 */

package ujf.verimag.bip.ifinder.analysis;

import java.util.HashMap;
import java.util.ArrayList;

import bip2.ujf.verimag.bip.port.*;
import bip2.ujf.verimag.bip.types.*;
import bip2.ujf.verimag.bip.connector.*;
import bip2.ujf.verimag.bip.component.*;
import bip2.ujf.verimag.bip.component.atom.*;
import bip2.ujf.verimag.bip.instance.*;

import ujf.verimag.bip.ifinder.tool.*;
import ujf.verimag.bip.ifinder.invariant.*;

/*
 *
 *
 */

public class InteractionTimeAnalysis extends Analysis {

    //
    // constructor
    //
    
    protected InteractionTimeAnalysis(HashMap<String, String> options,
				      ComponentInstance instance, Tool tool) {
	super("interaction-time", options, instance, tool);
	
	assert(instance instanceof CompoundInstance);
    }

    //
    // applicability checks
    //
    
    protected boolean isApplicable(AtomInstance atom) {
	// no external ports in atoms
	AtomType type = (AtomType) atom.getDeclaration().getType();
	return (type.getExternalPortDeclarations().size() == 0) ? true : false;
    }

    protected boolean isApplicable(CompoundInstance compound) {
	// inner ports (of subcomponents or connectors) are exclusively
	// used either in connectors or exported but not both
	for(ExportedPortInstance exportedPort : compound.getExportedPorts()) 
	    for(PortInstance innerPort_x : exportedPort.getReferencedPorts() )
		for(ConnectorInstance connector : compound.getSubConnectorInstances())
		    for(PortInstance innerPort_y : connector.getPortParamInstances())
			if (innerPort_x == innerPort_y) 
			    return false;
	
	return true;
    }
    
    protected boolean isApplicable(ConnectorInstance connector) {
	// no broadcasts
	ConnectorType type = connector.getDeclaration().getType();
	ConnectorInteractionDefinition definition = type.getInteractionDefinition();
	return Helper.IsSynchro(definition);
    }

    //
    // assertion generation (non-recursive)
    //
    
    protected Assertion generate() {
	// generate history clocks 
	generateHistoryClocks((CompoundInstance) m_instance);
	
	// generate min assertions
	ComposedAssertion assertion =
	    new ComposedAssertion(ComposedAssertion.Operator.AND);
	generateMinAssertions(assertion, (CompoundInstance)m_instance);

	// return
	return assertion;
    }

    // generate the history clocks
    
    protected void generateHistoryClocks(CompoundInstance compound) {
	// for this compound
	generateHistoryClocksForConnectors(compound);
	generateHistoryClocksForExportedPorts(compound);
	// for sub-components
	for(CompoundInstance subCompound : compound.getSubCompoundInstances())
	    generateHistoryClocksForExportedPorts(subCompound);
	for(AtomInstance subAtom : compound.getSubAtomInstances())
	    generateHistoryClocksForExportedPorts(subAtom);
    }

    protected void generateHistoryClocksForExportedPorts(ComponentInstance component) {
	// create the history clocks for exported ports
	for(ExportedPortInstance exportedPort : component.getExportedPorts()) {
	    HistoryClockVariable h_exportedPort =
		new HistoryClockVariable(exportedPort, component, m_invariant);
	    m_invariant.addVariable(h_exportedPort);
	}
    }
    
    protected void generateHistoryClocksForConnectors(CompoundInstance compound) {
	// create history clocks for connector instances
	for(ConnectorInstance connector : compound.getSubConnectorInstances()) {
	    HistoryClockVariable h_connector =
		new HistoryClockVariable(connector, compound, m_invariant);
	    m_invariant.addVariable(h_connector);
	}
    }

    // generate the min assertions
    
    protected void generateMinAssertions(ComposedAssertion top,
					 CompoundInstance compound) {

	// 1. collect the inner ports that are exported
	
	ArrayList<PortInstance> innerExported = new ArrayList<PortInstance>();
	for(ExportedPortInstance exportedPort : compound.getExportedPorts()) 
	    for(PortInstance innerPort : exportedPort.getReferencedPorts())
		if (!innerExported.contains(innerPort))
		    innerExported.add(innerPort);
	

	// 2. generate assertions for the inner ports that are not exported 
	
	// 2.a - for exported ports of connectors
	for(ConnectorInstance connector : compound.getSubConnectorInstances()) 
	    if (connector.getExportedPort() != null)
		if (!innerExported.contains(connector.getExportedPort())) {
		    HistoryClockVariable h_connector =
			findHistoryClock(compound, connector);
		    ClockMinAssertion assertion =
			mkMinAssertionForInnerPort(compound, connector.getExportedPort(),
						   h_connector);
		    top.add(assertion);
		}
    
	// 2.b - for exported ports of atom sub-components
	for(AtomInstance subAtom : compound.getSubAtomInstances()) 
	    for(ExportedPortInstance exportedPort : subAtom.getExportedPorts())
		if (!innerExported.contains(exportedPort)) {
		    HistoryClockVariable h_exportedPort =
			findHistoryClock(subAtom, exportedPort);
		    ClockMinAssertion assertion =
			mkMinAssertionForInnerPort(compound, exportedPort, h_exportedPort);
		    top.add(assertion);
		}
	
	// 2.c - for exported ports of compound sub-components
	for(CompoundInstance subCompound : compound.getSubCompoundInstances()) 
	    for(ExportedPortInstance exportedPort : subCompound.getExportedPorts())
		if (!innerExported.contains(exportedPort)) {
		    HistoryClockVariable h_exportedPort =
			findHistoryClock(subCompound, exportedPort);
		    ClockMinAssertion assertion =
			mkMinAssertionForInnerPort(compound, exportedPort, h_exportedPort);
		    top.add(assertion);
		}
	
 	// 3. generate assertions for the exported ports of this compound
	
	for(ExportedPortInstance exportedPort : compound.getExportedPorts()) {
	    HistoryClockVariable h_exportedPort =
		findHistoryClock(compound, exportedPort);
	    ClockMinAssertion assertion =
		mkMinAssertionForExportedPort(compound, exportedPort, h_exportedPort);
	    top.add(assertion);
	}
	
    }

    // generate the min assertion for an inner port
    
    protected ClockMinAssertion mkMinAssertionForInnerPort
	(CompoundInstance compound, PortInstance port, HistoryClockVariable h_port) {

	// create the assertion
	ClockMinAssertion assertion = new ClockMinAssertion(h_port);
	
	// go through connectors within compound and find bindings of port
	for(ConnectorInstance connector : compound.getSubConnectorInstances()) 
	    for(PortInstance innerPort : connector.getPortParamInstances())
		if (port == innerPort) {
		    HistoryClockVariable h_connector =
			findHistoryClock(compound, connector);
		    assertion.addArgument(h_connector);
		}
	
	// return
	return assertion;
    }

    // generate the min assertion for an exported port 
    
    protected ClockMinAssertion mkMinAssertionForExportedPort
	(CompoundInstance compound, ExportedPortInstance exportedPort,
	 HistoryClockVariable h_exportedPort) {

	// create the assertion
	ClockMinAssertion assertion = new ClockMinAssertion(h_exportedPort);
	
	// go through referenced ports, and find their bindings...
	for(PortInstance innerPort : exportedPort.getReferencedPorts()) {
	    // a. - through connectors exported ports
	    for(ConnectorInstance connector : compound.getSubConnectorInstances())
		if (connector.getExportedPort() == innerPort) {
		    HistoryClockVariable h_inner =
			findHistoryClock(compound, connector);
		    assertion.addArgument(h_inner);
		}
	    // b. - through sub-atom exported ports
	    for(AtomInstance subAtom : compound.getSubAtomInstances())
		if (subAtom.getExportedPorts().contains(innerPort)) {
		    HistoryClockVariable h_inner =
			findHistoryClock(subAtom, (ExportedPortInstance)innerPort);
		    assertion.addArgument(h_inner);
		}
	    // c. - through sub-compound exported ports
	    for(CompoundInstance subCompound : compound.getSubCompoundInstances())
		if (subCompound.getExportedPorts().contains(innerPort)) {
		    HistoryClockVariable h_inner =
			findHistoryClock(subCompound, (ExportedPortInstance)innerPort);
		    assertion.addArgument(h_inner);
		}   
	}

	// return
	return assertion;
    }

    //
    // helpers
    //
    
    protected HistoryClockVariable findHistoryClock(CompoundInstance compound,
						    ConnectorInstance connector) {
	Variable h_connector =
	    m_invariant.findVariable(compound, Variable.Category.CLOCK,
				     connector.getDeclaration().getName());
	assert(h_connector != null);
	assert(h_connector instanceof HistoryClockVariable);
	return (HistoryClockVariable) h_connector;
    }
    
    protected HistoryClockVariable findHistoryClock(ComponentInstance component,
						    ExportedPortInstance exportedPort) {
	Variable h_exportedPort =
	    m_invariant.findVariable(component, Variable.Category.CLOCK,
				     exportedPort.getDeclaration().getName());
	assert(h_exportedPort != null);
	assert(h_exportedPort instanceof HistoryClockVariable);
	return (HistoryClockVariable) h_exportedPort;
    }
   
}
