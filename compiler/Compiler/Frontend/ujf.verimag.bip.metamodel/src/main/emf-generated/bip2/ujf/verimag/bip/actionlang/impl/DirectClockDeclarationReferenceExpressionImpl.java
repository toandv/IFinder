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

package bip2.ujf.verimag.bip.actionlang.impl;

import bip2.ujf.verimag.bip.actionlang.ActionlangPackage;
import bip2.ujf.verimag.bip.actionlang.DirectClockDeclarationReferenceExpression;
import bip2.ujf.verimag.bip.actionlang.Expression;
import bip2.ujf.verimag.bip.data.DataDeclaration;

import bip2.ujf.verimag.bip.time.ClockDeclaration;
import bip2.ujf.verimag.bip.types.Type;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.util.BasicEList;
import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.ENotificationImpl;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Direct Clock Declaration Reference Expression</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link bip2.ujf.verimag.bip.actionlang.impl.DirectClockDeclarationReferenceExpressionImpl#getClockDeclaration <em>Clock Declaration</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class DirectClockDeclarationReferenceExpressionImpl extends
        ValuedExpressionImpl implements
        DirectClockDeclarationReferenceExpression {
    /**
     * The cached value of the '{@link #getClockDeclaration() <em>Clock Declaration</em>}' reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getClockDeclaration()
     * @generated
     * @ordered
     */
    protected ClockDeclaration clockDeclaration;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected DirectClockDeclarationReferenceExpressionImpl() {
        super();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    protected EClass eStaticClass() {
        return ActionlangPackage.Literals.DIRECT_CLOCK_DECLARATION_REFERENCE_EXPRESSION;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public ClockDeclaration getClockDeclaration() {
        if (clockDeclaration != null && ((EObject) clockDeclaration).eIsProxy()) {
            InternalEObject oldClockDeclaration = (InternalEObject) clockDeclaration;
            clockDeclaration = (ClockDeclaration) eResolveProxy(oldClockDeclaration);
            if (clockDeclaration != oldClockDeclaration) {
                if (eNotificationRequired())
                    eNotify(new ENotificationImpl(
                            this,
                            Notification.RESOLVE,
                            ActionlangPackage.DIRECT_CLOCK_DECLARATION_REFERENCE_EXPRESSION__CLOCK_DECLARATION,
                            oldClockDeclaration, clockDeclaration));
            }
        }
        return clockDeclaration;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public ClockDeclaration basicGetClockDeclaration() {
        return clockDeclaration;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setClockDeclaration(ClockDeclaration newClockDeclaration) {
        ClockDeclaration oldClockDeclaration = clockDeclaration;
        clockDeclaration = newClockDeclaration;
        if (eNotificationRequired())
            eNotify(new ENotificationImpl(
                    this,
                    Notification.SET,
                    ActionlangPackage.DIRECT_CLOCK_DECLARATION_REFERENCE_EXPRESSION__CLOCK_DECLARATION,
                    oldClockDeclaration, clockDeclaration));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    public Object eGet(int featureID, boolean resolve, boolean coreType) {
        switch (featureID) {
        case ActionlangPackage.DIRECT_CLOCK_DECLARATION_REFERENCE_EXPRESSION__CLOCK_DECLARATION:
            if (resolve)
                return getClockDeclaration();
            return basicGetClockDeclaration();
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
        case ActionlangPackage.DIRECT_CLOCK_DECLARATION_REFERENCE_EXPRESSION__CLOCK_DECLARATION:
            setClockDeclaration((ClockDeclaration) newValue);
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
        case ActionlangPackage.DIRECT_CLOCK_DECLARATION_REFERENCE_EXPRESSION__CLOCK_DECLARATION:
            setClockDeclaration((ClockDeclaration) null);
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
        case ActionlangPackage.DIRECT_CLOCK_DECLARATION_REFERENCE_EXPRESSION__CLOCK_DECLARATION:
            return clockDeclaration != null;
        }
        return super.eIsSet(featureID);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated NOT
     */
    public Type getType() {
        return getClockDeclaration().getUnit();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated NOT
     */
    public boolean isConst() {
        return false;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated NOT
     */
    public EList<DataDeclaration> getReadVariables() {
        return new BasicEList<DataDeclaration>();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated NOT
     */
    public EList<DataDeclaration> getMayReadVariables() {
        return new BasicEList<DataDeclaration>();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated NOT
     */
    public EList<DataDeclaration> getWriteVariables() {
        return new BasicEList<DataDeclaration>();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated NOT
     */
    public EList<DataDeclaration> getMayWriteVariables() {
        return new BasicEList<DataDeclaration>();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated NOT
     */
    public EList<Expression> getMayReadUninitializedVariables(
            EList<DataDeclaration> uninitialized) {
        return new BasicEList<Expression>();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @Override
     * @generated NOT
     */
    public EList<ClockDeclaration> getReadClocks() {
        EList<ClockDeclaration> ret = new BasicEList<ClockDeclaration>();

        ret.add(getClockDeclaration());

        return ret;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @Override
     * @generated NOT
     */
    public EList<ClockDeclaration> getMayReadClocks() {
        EList<ClockDeclaration> ret = new BasicEList<ClockDeclaration>();

        ret.add(getClockDeclaration());

        return ret;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @Override
     * @generated NOT
     */
    public EList<ClockDeclaration> getWriteClocks() {
        return new BasicEList<ClockDeclaration>();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @Override
     * @generated NOT
     */
    public EList<ClockDeclaration> getMayWriteClocks() {
        return new BasicEList<ClockDeclaration>();
    }

} //DirectClockDeclarationReferenceExpressionImpl
