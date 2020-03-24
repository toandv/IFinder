/*
 *
 *
 */

package ujf.verimag.bip.ifinder.analysis;

import java.util.List;
import java.util.ArrayList;

import bip2.ujf.verimag.bip.instance.*;
import bip2.ujf.verimag.bip.types.*;
import bip2.ujf.verimag.bip.connector.*;
import bip2.ujf.verimag.bip.behavior.*;
import bip2.ujf.verimag.bip.component.atom.*;
import bip2.ujf.verimag.bip.actionlang.*;
import bip2.ujf.verimag.bip.invariant.*;
import bip2.ujf.verimag.bip.time.*;
    
/*
 *
 *
 */

public class Helper {
    
    // only static members - various helper functions
    
    // check for synchronizations 
    
    public static boolean IsSynchro(ConnectorInteractionDefinition definition) {
	if (definition.isQuoted())
	    return false;
	boolean synchro = true;
 	if (definition instanceof ConnectorInteractionNestedDefinition) {
	    ConnectorInteractionNestedDefinition nested =
		(ConnectorInteractionNestedDefinition) definition;
	    for(ConnectorInteractionDefinition subDefinition :
		    nested.getSubInteractions()) {
		synchro &= IsSynchro(subDefinition);
	    }
	}
	return synchro;
    }
    
    // retrieve the subcomponent instance exporting some (inner) port
    
    public static ComponentInstance GetComponent
	(CompoundInstance compound, ExportedPortInstance innerPort) {
	for(AtomInstance subAtom : compound.getSubAtomInstances())
	    for(ExportedPortInstance port : subAtom.getExportedPorts())
		if (port == innerPort)
		    return subAtom;
	for(CompoundInstance subCompound : compound.getSubCompoundInstances())
	    for(ExportedPortInstance port : subCompound.getExportedPorts())
		if (port == innerPort)
		    return subCompound;
	return null;
    }


    // retrieve the connector instance exporting some (inner) port
    
    public static ConnectorInstance GetConnector
	(CompoundInstance compound, ConnectorPortInstance innerPort) {
	for(ConnectorInstance connector : compound.getSubConnectorInstances())
	    if (connector.getExportedPort() == innerPort)
		return connector;
	return null;
    }


    // retrieve the set of subcomponent' ports synchronized by some
    // connector
    
    public static ArrayList<ExportedPortInstance> GetPorts
	(CompoundInstance compound, ConnectorInstance connector) {
	ArrayList<ExportedPortInstance> list =
	    new ArrayList<ExportedPortInstance>();
	for(PortInstance port : connector.getPortParamInstances()) {
	    if (port instanceof ConnectorPortInstance) {
		ConnectorInstance subConnector =
		    GetConnector(compound, (ConnectorPortInstance)port);
		assert(subConnector != null);
		list.addAll(GetPorts(compound, subConnector));
	    }
	    else {
		assert(port instanceof ExportedPortInstance);
		list.add((ExportedPortInstance) port);
	    }
	}
	return list;
    }


    // retrieve the set of states of an atom instance
    
    public static List<State> GetStates(AtomInstance atom) {
	AtomType type = (AtomType) atom.getDeclaration().getType();
	return type.getBehavior().getStates();
    }
    
    // retrieve the set of initial states of an atom instance
    
    public static List<State> GetInitStates(AtomInstance atom) {
	AtomType type = (AtomType) atom.getDeclaration().getType();
	return type.getBehavior().getInitStates();
    }

    // retrieve the set of clocks

    public static List<ClockDeclaration> GetClocks(AtomInstance atom) {
	AtomType type = (AtomType) atom.getDeclaration().getType();
	return type.getClockDeclarations();
    } 

    // retrieve the set of data variables

    public static List<AtomInternalDataDeclaration> GetData(AtomInstance atom) {
	AtomType type = (AtomType) atom.getDeclaration().getType();
	return type.getInternalDataDeclarations();
    }
    
    
    // check if an atom is automaton
    
    public static boolean IsAutomaton(PetriNet behavior) {
	boolean result = true;
	if (behavior.getInitStates().size() != 1)
	    result = false;
	for(Transition t : behavior.getTransitions()) {
	    if (t.getSources().size() != 1 ||
		t.getDestinations().size() != 1) {
		result = false;
		break;
	    }
	}
	return result;
    }

    // get the list of invariants of a state
    
    public static ArrayList<AtomInvariant> GetInvariants(AtomInstance atom, State state) {
	AtomType type = (AtomType) atom.getDeclaration().getType();
	ArrayList<AtomInvariant> list = new ArrayList<AtomInvariant>();
	for(AtomInvariant invariant : type.getInvariants())
	    if (invariant.getSources().contains(state))
		list.add(invariant);
	return list;
    }
    
    // checks related to timing
    
    public static boolean IsTimePure(Transition transition) {
	boolean pure = true;
	
	// conjunctive guard of clock constraints
	if (transition.getGuard() != null)
	    pure &= IsTimePureConstraint(transition.getGuard());
	
	// all actions are clock resets
	for(Expression expr : transition.getActions()) 
	    pure &= IsTimePureSet(expr);
	
	return pure;
    }
    
    public static boolean IsTimePureConstraint(ValuedExpression expression) {
	if (! (expression instanceof BinaryOpExpression))
	    return false;

	BinaryOpExpression binary = (BinaryOpExpression) expression;
	BinaryOperators operator = binary.getOperator();
	ValuedExpression left = binary.getLeft();
	ValuedExpression right = binary.getRight();
	
	if (operator == BinaryOperators.LOGICAL_AND) {
	    return IsTimePureConstraint(left) && IsTimePureConstraint(right);
	}
	
	if (! (operator == BinaryOperators.EQUAL ||
	       // operator == BinaryOperators.LESS_THAN ||
	       // operator == BinaryOperators.GREATER_THAN ||
	       operator == BinaryOperators.LESS_THAN_OR_EQUAL ||
	       operator == BinaryOperators.GREATER_THAN_OR_EQUAL))
	    return false;

	if ((left instanceof DirectClockDeclarationReferenceExpression) &&
	    (right instanceof DirectIntegerExpression))
	    return true;

	if ((right instanceof DirectClockDeclarationReferenceExpression) &&
	    (left instanceof DirectIntegerExpression))
	    return true;

	return false;
    }
    
    public static boolean IsTimePureSet(Expression expression) {
	if (! (expression instanceof AssignmentExpression))
	    return false;
	
	Expression lhs = ((AssignmentExpression) expression).getLhs();
	if (! (lhs instanceof DirectClockDeclarationReferenceExpression))
	    return false;
	
	Expression rhs = ((AssignmentExpression) expression).getRhs();
	if (! (rhs instanceof DirectIntegerExpression) )
	    return false;
	
	int value = ((DirectIntegerExpression) rhs).getValue();
	if (value < 0)
	    return false;
	
	return true;
    }

    public static ArrayList<BinaryOpExpression> GetTConstraints(Transition transition) {
	// assume transition is time pure
	if (transition.getGuard() == null)
	    return null;
	return GetTConstraints(transition.getGuard());
    }

    
    public static ArrayList<BinaryOpExpression> GetTConstraints(ValuedExpression expression) {
	ArrayList<BinaryOpExpression> list =
	    new ArrayList<BinaryOpExpression>();
	
	BinaryOpExpression binary = (BinaryOpExpression) expression;
	BinaryOperators operator = binary.getOperator();
	ValuedExpression left = binary.getLeft();
	ValuedExpression right = binary.getRight();
	
	if (operator == BinaryOperators.LOGICAL_AND) {
	    list.addAll(GetTConstraints(left));
	    list.addAll(GetTConstraints(right));
	}
	else
	    list.add(binary);
	return list;
    }
    
    public static ArrayList<AssignmentExpression> GetTSets(Transition transition) {
	ArrayList<AssignmentExpression> list
	    = new ArrayList<AssignmentExpression>();
	for(Expression expr : transition.getActions()) {
	    AssignmentExpression assign = (AssignmentExpression) expr;
	    list.add(assign);
	}
	return list;
    }
    
}
