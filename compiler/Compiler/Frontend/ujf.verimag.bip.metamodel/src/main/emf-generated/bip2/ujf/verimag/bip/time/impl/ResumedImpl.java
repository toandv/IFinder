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

import bip2.ujf.verimag.bip.time.Resume;
import bip2.ujf.verimag.bip.time.Resumed;
import bip2.ujf.verimag.bip.time.TimePackage;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.ENotificationImpl;

import ujf.verimag.bip.metamodel.AnnotatedEObjectImpl;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Resumed</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link bip2.ujf.verimag.bip.time.impl.ResumedImpl#getResume <em>Resume</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class ResumedImpl extends AnnotatedEObjectImpl implements Resumed {
    /**
     * The cached value of the '{@link #getResume() <em>Resume</em>}' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #getResume()
     * @generated
     * @ordered
     */
    protected Resume resume;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    protected ResumedImpl() {
        super();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    protected EClass eStaticClass() {
        return TimePackage.Literals.RESUMED;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public Resume getResume() {
        return resume;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public NotificationChain basicSetResume(Resume newResume,
            NotificationChain msgs) {
        Resume oldResume = resume;
        resume = newResume;
        if (eNotificationRequired()) {
            ENotificationImpl notification = new ENotificationImpl(this,
                    Notification.SET, TimePackage.RESUMED__RESUME, oldResume,
                    newResume);
            if (msgs == null)
                msgs = notification;
            else
                msgs.add(notification);
        }
        return msgs;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public void setResume(Resume newResume) {
        if (newResume != resume) {
            NotificationChain msgs = null;
            if (resume != null)
                msgs = ((InternalEObject) resume).eInverseRemove(this,
                        EOPPOSITE_FEATURE_BASE - TimePackage.RESUMED__RESUME,
                        null, msgs);
            if (newResume != null)
                msgs = ((InternalEObject) newResume).eInverseAdd(this,
                        EOPPOSITE_FEATURE_BASE - TimePackage.RESUMED__RESUME,
                        null, msgs);
            msgs = basicSetResume(newResume, msgs);
            if (msgs != null)
                msgs.dispatch();
        } else if (eNotificationRequired())
            eNotify(new ENotificationImpl(this, Notification.SET,
                    TimePackage.RESUMED__RESUME, newResume, newResume));
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    public NotificationChain eInverseRemove(InternalEObject otherEnd,
            int featureID, NotificationChain msgs) {
        switch (featureID) {
        case TimePackage.RESUMED__RESUME:
            return basicSetResume(null, msgs);
        }
        return super.eInverseRemove(otherEnd, featureID, msgs);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    public Object eGet(int featureID, boolean resolve, boolean coreType) {
        switch (featureID) {
        case TimePackage.RESUMED__RESUME:
            return getResume();
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
        case TimePackage.RESUMED__RESUME:
            setResume((Resume) newValue);
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
        case TimePackage.RESUMED__RESUME:
            setResume((Resume) null);
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
        case TimePackage.RESUMED__RESUME:
            return resume != null;
        }
        return super.eIsSet(featureID);
    }

} //ResumedImpl
