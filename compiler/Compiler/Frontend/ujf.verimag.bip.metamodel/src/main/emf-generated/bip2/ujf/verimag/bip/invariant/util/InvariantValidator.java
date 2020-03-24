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

package bip2.ujf.verimag.bip.invariant.util;

import bip2.ujf.verimag.bip.invariant.*;

import bip2.ujf.verimag.bip.time.util.TimeValidator;

import java.util.Map;

import org.eclipse.emf.common.util.Diagnostic;
import org.eclipse.emf.common.util.DiagnosticChain;
import org.eclipse.emf.common.util.ResourceLocator;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EPackage;

import org.eclipse.emf.ecore.util.EObjectValidator;

/**
 * <!-- begin-user-doc -->
 * The <b>Validator</b> for the model.
 * <!-- end-user-doc -->
 * @see bip2.ujf.verimag.bip.invariant.InvariantPackage
 * @generated
 */
public class InvariantValidator extends EObjectValidator {
    /**
     * The cached model package
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public static final InvariantValidator INSTANCE = new InvariantValidator();

    /**
     * A constant for the {@link org.eclipse.emf.common.util.Diagnostic#getSource() source} of diagnostic {@link org.eclipse.emf.common.util.Diagnostic#getCode() codes} from this package.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see org.eclipse.emf.common.util.Diagnostic#getSource()
     * @see org.eclipse.emf.common.util.Diagnostic#getCode()
     * @generated
     */
    public static final String DIAGNOSTIC_SOURCE = "bip2.ujf.verimag.bip.invariant";

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
     * The cached base package validator.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected TimeValidator timeValidator;

    /**
     * Creates an instance of the switch.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public InvariantValidator() {
        super();
        timeValidator = TimeValidator.INSTANCE;
    }

    /**
     * Returns the package of this validator switch.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    protected EPackage getEPackage() {
        return InvariantPackage.eINSTANCE;
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
        case InvariantPackage.INVARIANT:
            return validateInvariant((Invariant) value, diagnostics, context);
        case InvariantPackage.ATOM_INVARIANT:
            return validateAtomInvariant((AtomInvariant) value, diagnostics,
                    context);
        default:
            return true;
        }
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public boolean validateInvariant(Invariant invariant,
            DiagnosticChain diagnostics, Map<Object, Object> context) {
        if (!validate_NoCircularContainment((EObject) invariant, diagnostics,
                context))
            return false;
        boolean result = validate_EveryMultiplicityConforms(
                (EObject) invariant, diagnostics, context);
        if (result || diagnostics != null)
            result &= validate_EveryDataValueConforms((EObject) invariant,
                    diagnostics, context);
        if (result || diagnostics != null)
            result &= validate_EveryReferenceIsContained((EObject) invariant,
                    diagnostics, context);
        if (result || diagnostics != null)
            result &= validate_EveryBidirectionalReferenceIsPaired(
                    (EObject) invariant, diagnostics, context);
        if (result || diagnostics != null)
            result &= validate_EveryProxyResolves((EObject) invariant,
                    diagnostics, context);
        if (result || diagnostics != null)
            result &= validate_UniqueID((EObject) invariant, diagnostics,
                    context);
        if (result || diagnostics != null)
            result &= validate_EveryKeyUnique((EObject) invariant, diagnostics,
                    context);
        if (result || diagnostics != null)
            result &= validate_EveryMapEntryUnique((EObject) invariant,
                    diagnostics, context);
        if (result || diagnostics != null)
            result &= timeValidator.validateGuarded_nestedComparisonOnClocks(
                    invariant, diagnostics, context);
        if (result || diagnostics != null)
            result &= timeValidator.validateGuarded_clocksOnOneSideOfLogicalOr(
                    invariant, diagnostics, context);
        if (result || diagnostics != null)
            result &= timeValidator.validateGuarded_invalidNotEqualOnClocks(
                    invariant, diagnostics, context);
        if (result || diagnostics != null)
            result &= timeValidator.validateGuarded_noClocksInLogicalNot(
                    invariant, diagnostics, context);
        if (result || diagnostics != null)
            result &= timeValidator
                    .validateGuarded_invalidUseOfMultiplicationOrDivisionOnClocks(
                            invariant, diagnostics, context);
        if (result || diagnostics != null)
            result &= timeValidator
                    .validateGuarded_comparisonOfMoreThanTwoClocks(invariant,
                            diagnostics, context);
        if (result || diagnostics != null)
            result &= timeValidator
                    .validateGuarded_comparisonOfClocksHavingWrongSign(
                            invariant, diagnostics, context);
        return result;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public boolean validateAtomInvariant(AtomInvariant atomInvariant,
            DiagnosticChain diagnostics, Map<Object, Object> context) {
        if (!validate_NoCircularContainment((EObject) atomInvariant,
                diagnostics, context))
            return false;
        boolean result = validate_EveryMultiplicityConforms(
                (EObject) atomInvariant, diagnostics, context);
        if (result || diagnostics != null)
            result &= validate_EveryDataValueConforms((EObject) atomInvariant,
                    diagnostics, context);
        if (result || diagnostics != null)
            result &= validate_EveryReferenceIsContained(
                    (EObject) atomInvariant, diagnostics, context);
        if (result || diagnostics != null)
            result &= validate_EveryBidirectionalReferenceIsPaired(
                    (EObject) atomInvariant, diagnostics, context);
        if (result || diagnostics != null)
            result &= validate_EveryProxyResolves((EObject) atomInvariant,
                    diagnostics, context);
        if (result || diagnostics != null)
            result &= validate_UniqueID((EObject) atomInvariant, diagnostics,
                    context);
        if (result || diagnostics != null)
            result &= validate_EveryKeyUnique((EObject) atomInvariant,
                    diagnostics, context);
        if (result || diagnostics != null)
            result &= validate_EveryMapEntryUnique((EObject) atomInvariant,
                    diagnostics, context);
        if (result || diagnostics != null)
            result &= timeValidator.validateGuarded_nestedComparisonOnClocks(
                    atomInvariant, diagnostics, context);
        if (result || diagnostics != null)
            result &= timeValidator.validateGuarded_clocksOnOneSideOfLogicalOr(
                    atomInvariant, diagnostics, context);
        if (result || diagnostics != null)
            result &= timeValidator.validateGuarded_invalidNotEqualOnClocks(
                    atomInvariant, diagnostics, context);
        if (result || diagnostics != null)
            result &= timeValidator.validateGuarded_noClocksInLogicalNot(
                    atomInvariant, diagnostics, context);
        if (result || diagnostics != null)
            result &= timeValidator
                    .validateGuarded_invalidUseOfMultiplicationOrDivisionOnClocks(
                            atomInvariant, diagnostics, context);
        if (result || diagnostics != null)
            result &= timeValidator
                    .validateGuarded_comparisonOfMoreThanTwoClocks(
                            atomInvariant, diagnostics, context);
        if (result || diagnostics != null)
            result &= timeValidator
                    .validateGuarded_comparisonOfClocksHavingWrongSign(
                            atomInvariant, diagnostics, context);
        if (result || diagnostics != null)
            result &= validateAtomInvariant_fakeConstraint(atomInvariant,
                    diagnostics, context);
        return result;
    }

    /**
     * /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\
     * WARNING: this is a fake constraint to force the generation of
     * a validator for the package invariant, in order to force the
     * validation of all constraints of guarded.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated NOT
     */
    public boolean validateAtomInvariant_fakeConstraint(
            AtomInvariant atomInvariant, DiagnosticChain diagnostics,
            Map<Object, Object> context) {

        // /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\
        // WARNING: this is a fake constraint!
        // /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\ /!\

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

} //InvariantValidator
