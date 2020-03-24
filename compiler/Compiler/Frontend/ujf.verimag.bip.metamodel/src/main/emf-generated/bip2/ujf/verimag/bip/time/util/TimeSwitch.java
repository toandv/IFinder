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

import bip2.ujf.verimag.bip.actionlang.Expression;
import bip2.ujf.verimag.bip.annotation.AnnotatedElement;

import bip2.ujf.verimag.bip.time.*;

import bip2.ujf.verimag.bip.types.Type;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EPackage;

import org.eclipse.emf.ecore.util.Switch;

/**
 * <!-- begin-user-doc -->
 * The <b>Switch</b> for the model's inheritance hierarchy.
 * It supports the call {@link #doSwitch(EObject) doSwitch(object)}
 * to invoke the <code>caseXXX</code> method for each class of the model,
 * starting with the actual class of the object
 * and proceeding up the inheritance hierarchy
 * until a non-null result is returned,
 * which is the result of the switch.
 * <!-- end-user-doc -->
 * @see bip2.ujf.verimag.bip.time.TimePackage
 * @generated
 */
public class TimeSwitch<T> extends Switch<T> {
    /**
     * The cached model package
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected static TimePackage modelPackage;

    /**
     * Creates an instance of the switch.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public TimeSwitch() {
        if (modelPackage == null) {
            modelPackage = TimePackage.eINSTANCE;
        }
    }

    /**
     * Checks whether this is a switch for the given package.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @parameter ePackage the package in question.
     * @return whether this is a switch for the given package.
     * @generated
     */
    @Override
    protected boolean isSwitchFor(EPackage ePackage) {
        return ePackage == modelPackage;
    }

    /**
     * Calls <code>caseXXX</code> for each class of the model until one returns a non null result; it yields that result.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the first non-null result returned by a <code>caseXXX</code> call.
     * @generated
     */
    @Override
    protected T doSwitch(int classifierID, EObject theEObject) {
        switch (classifierID) {
        case TimePackage.CLOCK_DECLARATION: {
            ClockDeclaration clockDeclaration = (ClockDeclaration) theEObject;
            T result = caseClockDeclaration(clockDeclaration);
            if (result == null)
                result = caseAnnotatedElement(clockDeclaration);
            if (result == null)
                result = defaultCase(theEObject);
            return result;
        }
        case TimePackage.URGENT_ELEMENT: {
            UrgentElement urgentElement = (UrgentElement) theEObject;
            T result = caseUrgentElement(urgentElement);
            if (result == null)
                result = defaultCase(theEObject);
            return result;
        }
        case TimePackage.TIME: {
            Time time = (Time) theEObject;
            T result = caseTime(time);
            if (result == null)
                result = caseType(time);
            if (result == null)
                result = caseAnnotatedElement(time);
            if (result == null)
                result = defaultCase(theEObject);
            return result;
        }
        case TimePackage.GUARDED: {
            Guarded guarded = (Guarded) theEObject;
            T result = caseGuarded(guarded);
            if (result == null)
                result = defaultCase(theEObject);
            return result;
        }
        case TimePackage.GUARDED_UNTIMED: {
            GuardedUntimed guardedUntimed = (GuardedUntimed) theEObject;
            T result = caseGuardedUntimed(guardedUntimed);
            if (result == null)
                result = caseGuarded(guardedUntimed);
            if (result == null)
                result = defaultCase(theEObject);
            return result;
        }
        case TimePackage.RESUMED: {
            Resumed resumed = (Resumed) theEObject;
            T result = caseResumed(resumed);
            if (result == null)
                result = defaultCase(theEObject);
            return result;
        }
        case TimePackage.RESUME: {
            Resume resume = (Resume) theEObject;
            T result = caseResume(resume);
            if (result == null)
                result = caseGuarded(resume);
            if (result == null)
                result = defaultCase(theEObject);
            return result;
        }
        default:
            return defaultCase(theEObject);
        }
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Clock Declaration</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Clock Declaration</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseClockDeclaration(ClockDeclaration object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Urgent Element</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Urgent Element</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseUrgentElement(UrgentElement object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Time</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Time</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseTime(Time object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Guarded</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Guarded</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseGuarded(Guarded object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Guarded Untimed</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Guarded Untimed</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseGuardedUntimed(GuardedUntimed object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Resumed</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Resumed</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseResumed(Resumed object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Resume</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Resume</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseResume(Resume object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Annotated Element</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Annotated Element</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseAnnotatedElement(AnnotatedElement object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>Type</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>Type</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
     * @generated
     */
    public T caseType(Type object) {
        return null;
    }

    /**
     * Returns the result of interpreting the object as an instance of '<em>EObject</em>'.
     * <!-- begin-user-doc -->
     * This implementation returns null;
     * returning a non-null result will terminate the switch, but this is the last case anyway.
     * <!-- end-user-doc -->
     * @param object the target of the switch.
     * @return the result of interpreting the object as an instance of '<em>EObject</em>'.
     * @see #doSwitch(org.eclipse.emf.ecore.EObject)
     * @generated
     */
    @Override
    public T defaultCase(EObject object) {
        return null;
    }

} //TimeSwitch
