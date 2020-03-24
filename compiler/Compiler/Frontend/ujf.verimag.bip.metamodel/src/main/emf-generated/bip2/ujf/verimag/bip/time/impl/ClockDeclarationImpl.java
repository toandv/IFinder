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

package bip2.ujf.verimag.bip.time.impl;

import bip2.ujf.verimag.bip.annotation.impl.AnnotatedElementImpl;

import bip2.ujf.verimag.bip.time.ClockDeclaration;
import bip2.ujf.verimag.bip.time.Time;
import bip2.ujf.verimag.bip.time.TimePackage;
import bip2.ujf.verimag.bip.time.TimeUnit;

import org.eclipse.emf.common.notify.Notification;

import org.eclipse.emf.ecore.EClass;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Clock Declaration</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link bip2.ujf.verimag.bip.time.impl.ClockDeclarationImpl#getUnit <em>Unit</em>}</li>
 *   <li>{@link bip2.ujf.verimag.bip.time.impl.ClockDeclarationImpl#getName <em>Name</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class ClockDeclarationImpl extends AnnotatedElementImpl implements
        ClockDeclaration {
    /**
     * The cached value of the '{@link #getUnit() <em>Unit</em>}' reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getUnit()
     * @generated
     * @ordered
     */
    protected Time unit;

    /**
     * The default value of the '{@link #getName() <em>Name</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getName()
     * @generated
     * @ordered
     */
    protected static final String NAME_EDEFAULT = null;

    /**
     * The cached value of the '{@link #getName() <em>Name</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getName()
     * @generated
     * @ordered
     */
    protected String name = NAME_EDEFAULT;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected ClockDeclarationImpl() {
        super();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    protected EClass eStaticClass() {
        return TimePackage.Literals.CLOCK_DECLARATION;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Time getUnit() {
        if (unit != null && ((EObject) unit).eIsProxy()) {
            InternalEObject oldUnit = (InternalEObject) unit;
            unit = (Time) eResolveProxy(oldUnit);
            if (unit != oldUnit) {
                if (eNotificationRequired())
                    eNotify(new ENotificationImpl(this, Notification.RESOLVE,
                            TimePackage.CLOCK_DECLARATION__UNIT, oldUnit, unit));
            }
        }
        return unit;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Time basicGetUnit() {
        return unit;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setUnit(Time newUnit) {
        Time oldUnit = unit;
        unit = newUnit;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET,
                    TimePackage.CLOCK_DECLARATION__UNIT, oldUnit, unit));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public String getName() {
        return name;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setName(String newName) {
        String oldName = name;
        name = newName;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET,
                    TimePackage.CLOCK_DECLARATION__NAME, oldName, name));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    public Object eGet(int featureID, boolean resolve, boolean coreType) {
        switch (featureID) {
        case TimePackage.CLOCK_DECLARATION__UNIT:
            if (resolve)
                return getUnit();
            return basicGetUnit();
        case TimePackage.CLOCK_DECLARATION__NAME:
            return getName();
        }
        return super.eGet(featureID, resolve, coreType);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    public void eSet(int featureID, Object newValue) {
        switch (featureID) {
        case TimePackage.CLOCK_DECLARATION__UNIT:
            setUnit((Time) newValue);
            return;
        case TimePackage.CLOCK_DECLARATION__NAME:
            setName((String) newValue);
            return;
        }
        super.eSet(featureID, newValue);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    public void eUnset(int featureID) {
        switch (featureID) {
        case TimePackage.CLOCK_DECLARATION__UNIT:
            setUnit((Time) null);
            return;
        case TimePackage.CLOCK_DECLARATION__NAME:
            setName(NAME_EDEFAULT);
            return;
        }
        super.eUnset(featureID);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    public boolean eIsSet(int featureID) {
        switch (featureID) {
        case TimePackage.CLOCK_DECLARATION__UNIT:
            return unit != null;
        case TimePackage.CLOCK_DECLARATION__NAME:
            return NAME_EDEFAULT == null ? name != null : !NAME_EDEFAULT
                    .equals(name);
        }
        return super.eIsSet(featureID);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    public String toString() {
        if (eIsProxy())
            return super.toString();

        StringBuffer result = new StringBuffer(super.toString());
        result.append(" (name: ");
        result.append(name);
        result.append(')');
        return result.toString();
    }

} //ClockDeclarationImpl
