/**
 * Copyright Verimag laboratory.
 * 
 * contributors:
 *  Jacques Combaz (jacques.combaz@univ-grenoble-alpes.fr)
 * 
 * This software is a computer program whose purpose is to generate
 * executable code from BIP models.
 * 
 * This software is governed by the CeCILL-B license under French law and
 * abiding by the rules of distribution of free software.  You can  use, 
 * modify and/ or redistribute the software under the terms of the CeCILL-B
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info".
 * 
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or 
 * data to be ensured and,  more generally, to use and operate it in the 
 * same conditions as regards security.
 * 
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */

package bip2.ujf.verimag.bip.time.util;

import bip2.ujf.verimag.bip.actionlang.BinaryOpExpression;
import bip2.ujf.verimag.bip.actionlang.BinaryOperators;
import bip2.ujf.verimag.bip.actionlang.DirectBooleanExpression;
import bip2.ujf.verimag.bip.actionlang.DirectClockDeclarationReferenceExpression;
import bip2.ujf.verimag.bip.actionlang.DirectDataDeclarationReferenceExpression;
import bip2.ujf.verimag.bip.actionlang.DirectFloatExpression;
import bip2.ujf.verimag.bip.actionlang.DirectIntegerExpression;
import bip2.ujf.verimag.bip.actionlang.DirectStringExpression;
import bip2.ujf.verimag.bip.actionlang.FunctionCallExpression;
import bip2.ujf.verimag.bip.actionlang.SubDataDeclarationReferenceExpression;
import bip2.ujf.verimag.bip.actionlang.UnaryOpExpression;
import bip2.ujf.verimag.bip.actionlang.UnaryOperators;
import bip2.ujf.verimag.bip.actionlang.ValuedExpression;
import bip2.ujf.verimag.bip.behavior.Transition;
import bip2.ujf.verimag.bip.time.*;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.emf.common.util.Diagnostic;
import org.eclipse.emf.common.util.DiagnosticChain;
import org.eclipse.emf.common.util.ResourceLocator;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EPackage;

import org.eclipse.emf.ecore.util.EObjectValidator;

import ujf.verimag.bip.error.message.ErrorCodeEnum;

/**
 * <!-- begin-user-doc -->
 * The <b>Validator</b> for the model.
 * <!-- end-user-doc -->
 * @see bip2.ujf.verimag.bip.time.TimePackage
 * @generated
 */
public class TimeValidator extends EObjectValidator {
    /**
     * The cached model package
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public static final TimeValidator INSTANCE = new TimeValidator();

    /**
     * A constant for the {@link org.eclipse.emf.common.util.Diagnostic#getSource() source} of diagnostic {@link org.eclipse.emf.common.util.Diagnostic#getCode() codes} from this package.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.eclipse.emf.common.util.Diagnostic#getSource()
     * @see org.eclipse.emf.common.util.Diagnostic#getCode()
     * @generated
     */
    public static final String DIAGNOSTIC_SOURCE = "bip2.ujf.verimag.bip.time";

    /**
     * A constant with a fixed name that can be used as the base value for additional hand written constants.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    private static final int GENERATED_DIAGNOSTIC_CODE_COUNT = 0;

    /**
     * A constant with a fixed name that can be used as the base value for additional hand written constants in a derived class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected static final int DIAGNOSTIC_CODE_COUNT = GENERATED_DIAGNOSTIC_CODE_COUNT;

    /**
     * Creates an instance of the switch.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public TimeValidator() {
        super();
    }

    /**
     * Returns the package of this validator switch.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    protected EPackage getEPackage() {
        return TimePackage.eINSTANCE;
    }

    /**
     * Calls <code>validateXXX</code> for the corresponding classifier of the model.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    protected boolean validate(int classifierID, Object value,
            DiagnosticChain diagnostics, Map<Object, Object> context) {
        switch (classifierID) {
        case TimePackage.CLOCK_DECLARATION:
            return validateClockDeclaration((ClockDeclaration) value,
                    diagnostics, context);
        case TimePackage.URGENT_ELEMENT:
            return validateUrgentElement((UrgentElement) value, diagnostics,
                    context);
        case TimePackage.TIME:
            return validateTime((Time) value, diagnostics, context);
        case TimePackage.GUARDED:
            return validateGuarded((Guarded) value, diagnostics, context);
        case TimePackage.GUARDED_UNTIMED:
            return validateGuardedUntimed((GuardedUntimed) value, diagnostics,
                    context);
        case TimePackage.RESUMED:
            return validateResumed((Resumed) value, diagnostics, context);
        case TimePackage.RESUME:
            return validateResume((Resume) value, diagnostics, context);
        case TimePackage.URGENCY:
            return validateUrgency((Urgency) value, diagnostics, context);
        default:
            return true;
        }
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public boolean validateClockDeclaration(ClockDeclaration clockDeclaration,
            DiagnosticChain diagnostics, Map<Object, Object> context) {
        return validate_EveryDefaultConstraint((EObject) clockDeclaration,
                diagnostics, context);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public boolean validateUrgentElement(UrgentElement urgentElement,
            DiagnosticChain diagnostics, Map<Object, Object> context) {
        return validate_EveryDefaultConstraint((EObject) urgentElement,
                diagnostics, context);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public boolean validateTime(Time time, DiagnosticChain diagnostics,
            Map<Object, Object> context) {
        return validate_EveryDefaultConstraint((EObject) time, diagnostics,
                context);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public boolean validateGuarded(Guarded guarded,
            DiagnosticChain diagnostics, Map<Object, Object> context) {
        if (!validate_NoCircularContainment((EObject) guarded, diagnostics,
                context))
            return false;
        boolean result = validate_EveryMultiplicityConforms((EObject) guarded,
                diagnostics, context);
        if (result || diagnostics != null)
            result &= validate_EveryDataValueConforms((EObject) guarded,
                    diagnostics, context);
        if (result || diagnostics != null)
            result &= validate_EveryReferenceIsContained((EObject) guarded,
                    diagnostics, context);
        if (result || diagnostics != null)
            result &= validate_EveryBidirectionalReferenceIsPaired(
                    (EObject) guarded, diagnostics, context);
        if (result || diagnostics != null)
            result &= validate_EveryProxyResolves((EObject) guarded,
                    diagnostics, context);
        if (result || diagnostics != null)
            result &= validate_UniqueID((EObject) guarded, diagnostics, context);
        if (result || diagnostics != null)
            result &= validate_EveryKeyUnique((EObject) guarded, diagnostics,
                    context);
        if (result || diagnostics != null)
            result &= validate_EveryMapEntryUnique((EObject) guarded,
                    diagnostics, context);
        if (result || diagnostics != null)
            result &= validateGuarded_nestedComparisonOnClocks(guarded,
                    diagnostics, context);
        if (result || diagnostics != null)
            result &= validateGuarded_clocksOnOneSideOfLogicalOr(guarded,
                    diagnostics, context);
        if (result || diagnostics != null)
            result &= validateGuarded_invalidNotEqualOnClocks(guarded,
                    diagnostics, context);
        if (result || diagnostics != null)
            result &= validateGuarded_noClocksInLogicalNot(guarded,
                    diagnostics, context);
        if (result || diagnostics != null)
            result &= validateGuarded_invalidUseOfMultiplicationOrDivisionOnClocks(
                    guarded, diagnostics, context);
        if (result || diagnostics != null)
            result &= validateGuarded_comparisonOfMoreThanTwoClocks(guarded,
                    diagnostics, context);
        if (result || diagnostics != null)
            result &= validateGuarded_comparisonOfClocksHavingWrongSign(
                    guarded, diagnostics, context);
        return result;
    }

    /**
     * Validates the nestedComparisonOnClocks constraint of '<em>Guarded</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated NOT
     */
    public boolean validateGuarded_nestedComparisonOnClocks(Guarded guarded,
            DiagnosticChain diagnostics, Map<Object, Object> context) {

        Set<BinaryOpExpression> invalidComparisons = new HashSet<BinaryOpExpression>();

        List<BinaryOperators> allowedComparisonOperators = Arrays.asList(
                BinaryOperators.EQUAL, BinaryOperators.GREATER_THAN,
                BinaryOperators.GREATER_THAN_OR_EQUAL,
                BinaryOperators.LESS_THAN, BinaryOperators.LESS_THAN_OR_EQUAL);

        if (guarded.getGuard() != null) {
            for (DirectClockDeclarationReferenceExpression ref : getAllClockReferences(guarded
                    .getGuard())) {
                boolean alreadyInAComparison = false;

                for (ValuedExpression upper : getUpperExpressions(ref)) {
                    if (upper instanceof BinaryOpExpression) {
                        BinaryOpExpression binaryExpr = (BinaryOpExpression) upper;

                        if (allowedComparisonOperators.contains(binaryExpr
                                .getOperator())) {
                            if (alreadyInAComparison) {
                                invalidComparisons.add(binaryExpr);
                                break;
                            }

                            alreadyInAComparison = true;
                        }
                    }
                }
            }
        }

        if (!invalidComparisons.isEmpty()) {
            if (diagnostics != null) {
                for (BinaryOpExpression binaryExpr : invalidComparisons) {
                    diagnostics.add(createDiagnostic(Diagnostic.ERROR,
                            DIAGNOSTIC_SOURCE, 0,
                            "_UI_GenericConstraint_diagnostic",
                            new Object[] { "nestedComparisonOnClocks",
                                    getObjectLabel(binaryExpr, context) },
                            new Object[] { binaryExpr,
                                    ErrorCodeEnum.nestedComparisonOnClocks },
                            context));
                }
            }

            return false;
        }
        return true;

    }

    /**
     * Return the path leading to an expression in the expression tree.
     * @param expressions the target expressions
     * @return a list of expression representing the path
     * @generated NOT
     */
    private List<ValuedExpression> getUpperExpressions(
            ValuedExpression expression) {
        List<ValuedExpression> ret = new ArrayList<ValuedExpression>();

        while (expression.eContainer() != null) {
            if (!(expression.eContainer() instanceof ValuedExpression))
                break;

            expression = (ValuedExpression) expression.eContainer();
            ret.add(expression);
        }

        return ret;
    }

    /**
     * 
     * @param 
     * @return 
     * @generated NOT
     */
    private int getSign(DirectClockDeclarationReferenceExpression clockRef,
            ValuedExpression expression) {
        int sign = 1;

        for (ValuedExpression upper : getUpperExpressions(clockRef)) {
            if (upper instanceof UnaryOpExpression) {
                UnaryOpExpression unaryExpr = (UnaryOpExpression) upper;

                if (unaryExpr.getOperator() == UnaryOperators.MINUS) {
                    sign = -sign;
                } else if (unaryExpr.getOperator() != UnaryOperators.PLUS) {
                    // no change of sign
                } else {
                    sign = 0;
                }
            } else if (upper instanceof BinaryOpExpression) {
                BinaryOpExpression binaryExpr = (BinaryOpExpression) upper;

                if (binaryExpr.getOperator() == BinaryOperators.SUBTRACTION) {
                    if (getAllClockReferences(binaryExpr.getRight()).contains(
                            clockRef)) {
                        sign = -sign;
                    }
                } else if (binaryExpr.getOperator() != BinaryOperators.ADDITION) {
                    // no change of sign
                } else {
                    sign = 0;
                }
            }

            if (upper == expression) {
                break;
            }
        }

        return sign;
    }

    /**
     * Compute all the clocks referenced by an expression or its sub-expression.
     * @param expr the target expression
     * @return all referenced clocks
     * @generated NOT
     */
    private List<DirectClockDeclarationReferenceExpression> getAllClockReferences(
            ValuedExpression expr) {
        List<DirectClockDeclarationReferenceExpression> ret = new ArrayList<DirectClockDeclarationReferenceExpression>();

        if (expr instanceof UnaryOpExpression) {
            UnaryOpExpression unaryExpr = (UnaryOpExpression) expr;

            ret.addAll(getAllClockReferences(unaryExpr.getOperand()));
        } else if (expr instanceof BinaryOpExpression) {
            BinaryOpExpression binaryExpr = (BinaryOpExpression) expr;

            ret.addAll(getAllClockReferences(binaryExpr.getLeft()));
            ret.addAll(getAllClockReferences(binaryExpr.getRight()));
        } else if (expr instanceof FunctionCallExpression) {
            FunctionCallExpression functCallExpr = (FunctionCallExpression) expr;

            for (ValuedExpression param : functCallExpr.getParameters()) {
                ret.addAll(getAllClockReferences(param));
            }
        } else if (expr instanceof DirectClockDeclarationReferenceExpression) {
            ret.add((DirectClockDeclarationReferenceExpression) expr);
        } else {
            assert (expr instanceof DirectDataDeclarationReferenceExpression
                    || expr instanceof SubDataDeclarationReferenceExpression
                    || expr instanceof DirectBooleanExpression
                    || expr instanceof DirectIntegerExpression
                    || expr instanceof DirectFloatExpression || expr instanceof DirectStringExpression);
        }

        return ret;
    }

    /**
     * Validates the essai constraint of '<em>Guarded</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated NOT
     */
    public boolean validateGuarded_clocksOnOneSideOfLogicalOr(Guarded guarded,
            DiagnosticChain diagnostics, Map<Object, Object> context) {
        Set<BinaryOpExpression> invalidLogicalOr = new HashSet<BinaryOpExpression>();

        if (guarded.getGuard() != null) {
            for (DirectClockDeclarationReferenceExpression ref : getAllClockReferences(guarded
                    .getGuard())) {
                for (ValuedExpression upper : getUpperExpressions(ref)) {
                    if (upper instanceof BinaryOpExpression) {
                        BinaryOpExpression binaryExpr = (BinaryOpExpression) upper;

                        if (binaryExpr.getOperator() == BinaryOperators.LOGICAL_OR) {
                            if (!getAllClockReferences(binaryExpr.getLeft())
                                    .isEmpty()
                                    && !getAllClockReferences(
                                            binaryExpr.getRight()).isEmpty()) {
                                invalidLogicalOr.add(binaryExpr);
                            }
                        }
                    }
                }
            }
        }

        if (!invalidLogicalOr.isEmpty()) {
            if (diagnostics != null) {
                for (BinaryOpExpression binaryExpr : invalidLogicalOr) {
                    diagnostics.add(createDiagnostic(Diagnostic.ERROR,
                            DIAGNOSTIC_SOURCE, 0,
                            "_UI_GenericConstraint_diagnostic", new Object[] {
                                    "clocksOnOneSideOfLogicalOr",
                                    getObjectLabel(binaryExpr, context) },
                            new Object[] { binaryExpr,
                                    ErrorCodeEnum.clocksOnOneSideOfLogicalOr },
                            context));
                }
            }

            return false;
        }
        return true;
    }

    /**
     * Validates the invalidNotEqualOnClocks constraint of '<em>Guarded</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated NOT
     */
    public boolean validateGuarded_invalidNotEqualOnClocks(Guarded guarded,
            DiagnosticChain diagnostics, Map<Object, Object> context) {

        Set<BinaryOpExpression> invalidComparisons = new HashSet<BinaryOpExpression>();

        List<BinaryOperators> comparisonOperators = Arrays.asList(
                BinaryOperators.EQUAL, BinaryOperators.NOT_EQUAL,
                BinaryOperators.GREATER_THAN,
                BinaryOperators.GREATER_THAN_OR_EQUAL,
                BinaryOperators.LESS_THAN, BinaryOperators.LESS_THAN_OR_EQUAL);

        List<BinaryOperators> allowedComparisonOperators = Arrays.asList(
                BinaryOperators.EQUAL, BinaryOperators.GREATER_THAN,
                BinaryOperators.GREATER_THAN_OR_EQUAL,
                BinaryOperators.LESS_THAN, BinaryOperators.LESS_THAN_OR_EQUAL);

        if (guarded.getGuard() != null) {
            for (DirectClockDeclarationReferenceExpression ref : getAllClockReferences(guarded
                    .getGuard())) {
                for (ValuedExpression upper : getUpperExpressions(ref)) {
                    if (upper instanceof BinaryOpExpression) {
                        BinaryOpExpression binaryExpr = (BinaryOpExpression) upper;

                        if (comparisonOperators.contains(binaryExpr
                                .getOperator())
                                && !allowedComparisonOperators
                                        .contains(binaryExpr.getOperator())) {
                            invalidComparisons.add(binaryExpr);
                        }
                    }
                }
            }
        }

        if (!invalidComparisons.isEmpty()) {
            if (diagnostics != null) {
                for (BinaryOpExpression binaryExpr : invalidComparisons) {
                    diagnostics.add(createDiagnostic(Diagnostic.ERROR,
                            DIAGNOSTIC_SOURCE, 0,
                            "_UI_GenericConstraint_diagnostic",
                            new Object[] { "invalidNotEqualOnClocks",
                                    getObjectLabel(binaryExpr, context) },
                            new Object[] { binaryExpr,
                                    ErrorCodeEnum.invalidNotEqualOnClocks },
                            context));
                }
            }

            return false;
        }
        return true;
    }

    /**
     * Validates the essai constraint of '<em>Guarded</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated NOT
     */
    public boolean validateGuarded_noClocksInLogicalNot(Guarded guarded,
            DiagnosticChain diagnostics, Map<Object, Object> context) {
        Set<UnaryOpExpression> invalidLogicalNot = new HashSet<UnaryOpExpression>();

        if (guarded.getGuard() != null) {
            for (DirectClockDeclarationReferenceExpression ref : getAllClockReferences(guarded
                    .getGuard())) {
                for (ValuedExpression upper : getUpperExpressions(ref)) {
                    if (upper instanceof UnaryOpExpression) {
                        UnaryOpExpression unaryExpr = (UnaryOpExpression) upper;

                        if (unaryExpr.getOperator() == UnaryOperators.LOGICAL_NOT) {
                            invalidLogicalNot.add(unaryExpr);
                        }
                    }
                }
            }
        }

        if (!invalidLogicalNot.isEmpty()) {
            if (diagnostics != null) {
                for (UnaryOpExpression unaryExpr : invalidLogicalNot) {
                    diagnostics.add(createDiagnostic(Diagnostic.ERROR,
                            DIAGNOSTIC_SOURCE, 0,
                            "_UI_GenericConstraint_diagnostic",
                            new Object[] { "noClocksInLogicalNot",
                                    getObjectLabel(unaryExpr, context) },
                            new Object[] { unaryExpr,
                                    ErrorCodeEnum.noClocksInLogicalNot },
                            context));
                }
            }

            return false;
        }
        return true;
    }

    /**
     * Validates the invalidUseOfMultiplicationOrDivisionOnClocks constraint of '<em>Guarded</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated NOT
     */
    public boolean validateGuarded_invalidUseOfMultiplicationOrDivisionOnClocks(
            Guarded guarded, DiagnosticChain diagnostics,
            Map<Object, Object> context) {

        List<ValuedExpression> faultyExpressions = new ArrayList<ValuedExpression>();

        if (guarded.getGuard() != null) {
            for (DirectClockDeclarationReferenceExpression clockRef : getAllClockReferences(guarded
                    .getGuard())) {
                List<ValuedExpression> uppers = getUpperExpressions(clockRef);

                for (ValuedExpression expr : uppers) {
                    if (expr instanceof BinaryOpExpression) {
                        BinaryOpExpression binaryExpr = (BinaryOpExpression) expr;

                        if (binaryExpr.getOperator() == BinaryOperators.MULTIPLICATION
                                || binaryExpr.getOperator() == BinaryOperators.DIVISION) {
                            faultyExpressions.add(binaryExpr);
                            break;
                        }
                    }
                }
            }
        }

        if (!faultyExpressions.isEmpty()) {
            if (diagnostics != null) {
                for (ValuedExpression binaryExpr : faultyExpressions) {
                    diagnostics
                            .add(createDiagnostic(
                                    Diagnostic.WARNING,
                                    DIAGNOSTIC_SOURCE,
                                    0,
                                    "_UI_GenericConstraint_diagnostic",
                                    new Object[] {
                                            "invalidUseOfMultiplicationOrDivisionOnClocks",
                                            getObjectLabel(binaryExpr, context) },
                                    new Object[] {
                                            binaryExpr,
                                            ErrorCodeEnum.invalidUseOfMultiplicationOrDivisionOnClocks },
                                    context));
                }
            }

            return false;
        }
        return true;
    }

    /**
     * Validates the comparisonOfMoreThanTwoClocks constraint of '<em>Guarded</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated NOT
     */
    public boolean validateGuarded_comparisonOfMoreThanTwoClocks(
            Guarded guarded, DiagnosticChain diagnostics,
            Map<Object, Object> context) {

        List<ValuedExpression> faultyExpressions = new ArrayList<ValuedExpression>();

        List<BinaryOperators> comparisonOperators = Arrays.asList(
                BinaryOperators.EQUAL, BinaryOperators.GREATER_THAN,
                BinaryOperators.GREATER_THAN_OR_EQUAL,
                BinaryOperators.LESS_THAN, BinaryOperators.LESS_THAN_OR_EQUAL);

        if (guarded.getGuard() != null) {
            for (DirectClockDeclarationReferenceExpression clockRef : getAllClockReferences(guarded
                    .getGuard())) {
                List<ValuedExpression> uppers = getUpperExpressions(clockRef);

                for (ValuedExpression expr : uppers) {
                    if (expr instanceof BinaryOpExpression) {
                        BinaryOpExpression binaryExpr = (BinaryOpExpression) expr;

                        if (comparisonOperators.contains(binaryExpr
                                .getOperator())) {
                            List<DirectClockDeclarationReferenceExpression> clocks = getAllClockReferences(binaryExpr);

                            // check that comparison involve at most 2 clocks
                            // and generate an error only on the 3rd clock
                            if (clocks.indexOf(clockRef) == 2) {
                                faultyExpressions.add(clockRef);
                                break;
                            }
                        }
                    }
                }
            }
        }

        if (!faultyExpressions.isEmpty()) {
            if (diagnostics != null) {
                for (ValuedExpression binaryExpr : faultyExpressions) {
                    diagnostics
                            .add(createDiagnostic(
                                    Diagnostic.WARNING,
                                    DIAGNOSTIC_SOURCE,
                                    0,
                                    "_UI_GenericConstraint_diagnostic",
                                    new Object[] {
                                            "comparisonOfMoreThanTwoClocks",
                                            getObjectLabel(binaryExpr, context) },
                                    new Object[] {
                                            binaryExpr,
                                            ErrorCodeEnum.comparisonOfMoreThanTwoClocks },
                                    context));
                }
            }

            return false;
        }
        return true;
    }

    /**
     * Validates the comparisonOfClocksHavingWrongSign constraint of '<em>Guarded</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated NOT
     */
    public boolean validateGuarded_comparisonOfClocksHavingWrongSign(
            Guarded guarded, DiagnosticChain diagnostics,
            Map<Object, Object> context) {

        List<ValuedExpression> faultyExpressions = new ArrayList<ValuedExpression>();

        List<BinaryOperators> comparisonOperators = Arrays.asList(
                BinaryOperators.EQUAL, BinaryOperators.GREATER_THAN,
                BinaryOperators.GREATER_THAN_OR_EQUAL,
                BinaryOperators.LESS_THAN, BinaryOperators.LESS_THAN_OR_EQUAL);

        if (guarded.getGuard() != null) {
            for (DirectClockDeclarationReferenceExpression clockRef : getAllClockReferences(guarded
                    .getGuard())) {
                List<ValuedExpression> uppers = getUpperExpressions(clockRef);

                for (ValuedExpression expr : uppers) {
                    if (expr instanceof BinaryOpExpression) {
                        BinaryOpExpression binaryExpr = (BinaryOpExpression) expr;

                        if (comparisonOperators.contains(binaryExpr
                                .getOperator())) {
                            List<DirectClockDeclarationReferenceExpression> clocks = getAllClockReferences(binaryExpr);

                            // check that clocks have opposite signs
                            if (clocks.size() == 2
                                    || clocks.indexOf(clockRef) == 1) {
                                int signOfClock[] = { 0, 0 };

                                for (int i = 0; i < 2; i++) {
                                    if (getAllClockReferences(
                                            binaryExpr.getLeft()).contains(
                                            clocks.get(i))) {
                                        signOfClock[i] = getSign(clocks.get(i),
                                                binaryExpr.getLeft());
                                    } else {
                                        signOfClock[i] = -getSign(
                                                clocks.get(i),
                                                binaryExpr.getRight());
                                    }

                                    assert (signOfClock[i] == 1 || signOfClock[i] == -1);
                                }

                                if (signOfClock[0] == signOfClock[1]) {
                                    faultyExpressions.add(clockRef);
                                }
                            }
                        }
                    }
                }
            }
        }

        if (!faultyExpressions.isEmpty()) {
            if (diagnostics != null) {
                for (ValuedExpression binaryExpr : faultyExpressions) {
                    diagnostics
                            .add(createDiagnostic(
                                    Diagnostic.WARNING,
                                    DIAGNOSTIC_SOURCE,
                                    0,
                                    "_UI_GenericConstraint_diagnostic",
                                    new Object[] {
                                            "comparisonOfClocksHavingWrongSign",
                                            getObjectLabel(binaryExpr, context) },
                                    new Object[] {
                                            binaryExpr,
                                            ErrorCodeEnum.comparisonOfClocksHavingWrongSign },
                                    context));
                }
            }

            return false;
        }
        return true;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public boolean validateGuardedUntimed(GuardedUntimed guardedUntimed,
            DiagnosticChain diagnostics, Map<Object, Object> context) {
        if (!validate_NoCircularContainment((EObject) guardedUntimed,
                diagnostics, context))
            return false;
        boolean result = validate_EveryMultiplicityConforms(
                (EObject) guardedUntimed, diagnostics, context);
        if (result || diagnostics != null)
            result &= validate_EveryDataValueConforms((EObject) guardedUntimed,
                    diagnostics, context);
        if (result || diagnostics != null)
            result &= validate_EveryReferenceIsContained(
                    (EObject) guardedUntimed, diagnostics, context);
        if (result || diagnostics != null)
            result &= validate_EveryBidirectionalReferenceIsPaired(
                    (EObject) guardedUntimed, diagnostics, context);
        if (result || diagnostics != null)
            result &= validate_EveryProxyResolves((EObject) guardedUntimed,
                    diagnostics, context);
        if (result || diagnostics != null)
            result &= validate_UniqueID((EObject) guardedUntimed, diagnostics,
                    context);
        if (result || diagnostics != null)
            result &= validate_EveryKeyUnique((EObject) guardedUntimed,
                    diagnostics, context);
        if (result || diagnostics != null)
            result &= validate_EveryMapEntryUnique((EObject) guardedUntimed,
                    diagnostics, context);
        if (result || diagnostics != null)
            result &= validateGuarded_nestedComparisonOnClocks(guardedUntimed,
                    diagnostics, context);
        if (result || diagnostics != null)
            result &= validateGuarded_clocksOnOneSideOfLogicalOr(
                    guardedUntimed, diagnostics, context);
        if (result || diagnostics != null)
            result &= validateGuarded_invalidNotEqualOnClocks(guardedUntimed,
                    diagnostics, context);
        if (result || diagnostics != null)
            result &= validateGuarded_noClocksInLogicalNot(guardedUntimed,
                    diagnostics, context);
        if (result || diagnostics != null)
            result &= validateGuarded_invalidUseOfMultiplicationOrDivisionOnClocks(
                    guardedUntimed, diagnostics, context);
        if (result || diagnostics != null)
            result &= validateGuarded_comparisonOfMoreThanTwoClocks(
                    guardedUntimed, diagnostics, context);
        if (result || diagnostics != null)
            result &= validateGuarded_comparisonOfClocksHavingWrongSign(
                    guardedUntimed, diagnostics, context);
        if (result || diagnostics != null)
            result &= validateGuardedUntimed_noClocksInGuard(guardedUntimed,
                    diagnostics, context);
        return result;
    }

    /**
     * Validates the noClocksInGuardsOfInternalTransitions constraint of '<em>Transition</em>'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated NOT
     */
    public boolean validateGuardedUntimed_noClocksInGuard(
            GuardedUntimed guardedUntimed, DiagnosticChain diagnostics,
            Map<Object, Object> context) {
        boolean ok = true;

        if (guardedUntimed.getGuard() != null) {
            if (!getAllClockReferences(guardedUntimed.getGuard()).isEmpty()) {
                ok = false;
            }
        }

        if (!ok) {
            if (diagnostics != null) {
                for (DirectClockDeclarationReferenceExpression ref : getAllClockReferences(guardedUntimed
                        .getGuard())) {
                    diagnostics.add(createDiagnostic(Diagnostic.ERROR,
                            DIAGNOSTIC_SOURCE, 0,
                            "_UI_GenericConstraint_diagnostic",
                            new Object[] { "noClocksInUntimedGuard",
                                    getObjectLabel(ref, context) },
                            new Object[] { ref,
                                    ErrorCodeEnum.noClocksInUntimedGuard },
                            context));
                }
            }

            return false;
        }
        return true;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public boolean validateResumed(Resumed resumed,
            DiagnosticChain diagnostics, Map<Object, Object> context) {
        return validate_EveryDefaultConstraint((EObject) resumed, diagnostics,
                context);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public boolean validateResume(Resume resume, DiagnosticChain diagnostics,
            Map<Object, Object> context) {
        if (!validate_NoCircularContainment((EObject) resume, diagnostics,
                context))
            return false;
        boolean result = validate_EveryMultiplicityConforms((EObject) resume,
                diagnostics, context);
        if (result || diagnostics != null)
            result &= validate_EveryDataValueConforms((EObject) resume,
                    diagnostics, context);
        if (result || diagnostics != null)
            result &= validate_EveryReferenceIsContained((EObject) resume,
                    diagnostics, context);
        if (result || diagnostics != null)
            result &= validate_EveryBidirectionalReferenceIsPaired(
                    (EObject) resume, diagnostics, context);
        if (result || diagnostics != null)
            result &= validate_EveryProxyResolves((EObject) resume,
                    diagnostics, context);
        if (result || diagnostics != null)
            result &= validate_UniqueID((EObject) resume, diagnostics, context);
        if (result || diagnostics != null)
            result &= validate_EveryKeyUnique((EObject) resume, diagnostics,
                    context);
        if (result || diagnostics != null)
            result &= validate_EveryMapEntryUnique((EObject) resume,
                    diagnostics, context);
        if (result || diagnostics != null)
            result &= validateGuarded_nestedComparisonOnClocks(resume,
                    diagnostics, context);
        if (result || diagnostics != null)
            result &= validateGuarded_clocksOnOneSideOfLogicalOr(resume,
                    diagnostics, context);
        if (result || diagnostics != null)
            result &= validateGuarded_invalidNotEqualOnClocks(resume,
                    diagnostics, context);
        if (result || diagnostics != null)
            result &= validateGuarded_noClocksInLogicalNot(resume, diagnostics,
                    context);
        if (result || diagnostics != null)
            result &= validateGuarded_invalidUseOfMultiplicationOrDivisionOnClocks(
                    resume, diagnostics, context);
        if (result || diagnostics != null)
            result &= validateGuarded_comparisonOfMoreThanTwoClocks(resume,
                    diagnostics, context);
        if (result || diagnostics != null)
            result &= validateGuarded_comparisonOfClocksHavingWrongSign(resume,
                    diagnostics, context);
        return result;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public boolean validateUrgency(Urgency urgency,
            DiagnosticChain diagnostics, Map<Object, Object> context) {
        return true;
    }

    /**
     * Returns the resource locator that will be used to fetch messages for this validator's diagnostics.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    public ResourceLocator getResourceLocator() {
        // TODO
        // Specialize this to return a resource locator for messages specific to this validator.
        // Ensure that you remove @generated or mark it @generated NOT
        return super.getResourceLocator();
    }

} //TimeValidator
