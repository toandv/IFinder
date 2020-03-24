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

package bip2.ujf.verimag.bip.time;

import bip2.ujf.verimag.bip.actionlang.ActionlangPackage;
import bip2.ujf.verimag.bip.annotation.AnnotationPackage;

import bip2.ujf.verimag.bip.types.TypesPackage;
import org.eclipse.emf.ecore.EAttribute;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EEnum;
import org.eclipse.emf.ecore.EPackage;
import org.eclipse.emf.ecore.EReference;

/**
 * <!-- begin-user-doc -->
 * The <b>Package</b> for the model.
 * It contains accessors for the meta objects to represent
 * <ul>
 *   <li>each class,</li>
 *   <li>each feature of each class,</li>
 *   <li>each enum,</li>
 *   <li>and each data type</li>
 * </ul>
 * <!-- end-user-doc -->
 * @see bip2.ujf.verimag.bip.time.TimeFactory
 * @model kind="package"
 * @generated
 */
public interface TimePackage extends EPackage {
    /**
     * The package name.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    String eNAME = "time";

    /**
     * The package namespace URI.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    String eNS_URI = "http://bip2/ujf/verimag/bip/time/1.0";

    /**
     * The package namespace name.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    String eNS_PREFIX = "bip2.ujf.verimag.bip.time";

    /**
     * The singleton instance of the package.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    TimePackage eINSTANCE = bip2.ujf.verimag.bip.time.impl.TimePackageImpl
            .init();

    /**
     * The meta object id for the '{@link bip2.ujf.verimag.bip.time.impl.ClockDeclarationImpl <em>Clock Declaration</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see bip2.ujf.verimag.bip.time.impl.ClockDeclarationImpl
     * @see bip2.ujf.verimag.bip.time.impl.TimePackageImpl#getClockDeclaration()
     * @generated
     */
    int CLOCK_DECLARATION = 0;

    /**
     * The feature id for the '<em><b>Bip Annotations</b></em>' map.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CLOCK_DECLARATION__BIP_ANNOTATIONS = AnnotationPackage.ANNOTATED_ELEMENT__BIP_ANNOTATIONS;

    /**
     * The feature id for the '<em><b>Unit</b></em>' reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CLOCK_DECLARATION__UNIT = AnnotationPackage.ANNOTATED_ELEMENT_FEATURE_COUNT + 0;

    /**
     * The feature id for the '<em><b>Name</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CLOCK_DECLARATION__NAME = AnnotationPackage.ANNOTATED_ELEMENT_FEATURE_COUNT + 1;

    /**
     * The number of structural features of the '<em>Clock Declaration</em>' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int CLOCK_DECLARATION_FEATURE_COUNT = AnnotationPackage.ANNOTATED_ELEMENT_FEATURE_COUNT + 2;

    /**
     * The meta object id for the '{@link bip2.ujf.verimag.bip.time.UrgentElement <em>Urgent Element</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see bip2.ujf.verimag.bip.time.UrgentElement
     * @see bip2.ujf.verimag.bip.time.impl.TimePackageImpl#getUrgentElement()
     * @generated
     */
    int URGENT_ELEMENT = 1;

    /**
     * The feature id for the '<em><b>Urgency</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int URGENT_ELEMENT__URGENCY = 0;

    /**
     * The number of structural features of the '<em>Urgent Element</em>' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int URGENT_ELEMENT_FEATURE_COUNT = 1;

    /**
     * The meta object id for the '{@link bip2.ujf.verimag.bip.time.impl.TimeImpl <em>Time</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see bip2.ujf.verimag.bip.time.impl.TimeImpl
     * @see bip2.ujf.verimag.bip.time.impl.TimePackageImpl#getTime()
     * @generated
     */
    int TIME = 2;

    /**
     * The feature id for the '<em><b>Bip Annotations</b></em>' map.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int TIME__BIP_ANNOTATIONS = TypesPackage.TYPE__BIP_ANNOTATIONS;

    /**
     * The feature id for the '<em><b>Name</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int TIME__NAME = TypesPackage.TYPE__NAME;

    /**
     * The feature id for the '<em><b>Native</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int TIME__NATIVE = TypesPackage.TYPE__NATIVE;

    /**
     * The feature id for the '<em><b>Super Types</b></em>' reference list.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int TIME__SUPER_TYPES = TypesPackage.TYPE__SUPER_TYPES;

    /**
     * The feature id for the '<em><b>Castable Types</b></em>' reference list.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int TIME__CASTABLE_TYPES = TypesPackage.TYPE__CASTABLE_TYPES;

    /**
     * The number of structural features of the '<em>Time</em>' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int TIME_FEATURE_COUNT = TypesPackage.TYPE_FEATURE_COUNT + 0;

    /**
     * The meta object id for the '{@link bip2.ujf.verimag.bip.time.Guarded <em>Guarded</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see bip2.ujf.verimag.bip.time.Guarded
     * @see bip2.ujf.verimag.bip.time.impl.TimePackageImpl#getGuarded()
     * @generated
     */
    int GUARDED = 3;

    /**
     * The feature id for the '<em><b>Guard</b></em>' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int GUARDED__GUARD = 0;

    /**
     * The number of structural features of the '<em>Guarded</em>' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int GUARDED_FEATURE_COUNT = 1;

    /**
     * The meta object id for the '{@link bip2.ujf.verimag.bip.time.impl.GuardedUntimedImpl <em>Guarded Untimed</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see bip2.ujf.verimag.bip.time.impl.GuardedUntimedImpl
     * @see bip2.ujf.verimag.bip.time.impl.TimePackageImpl#getGuardedUntimed()
     * @generated
     */
    int GUARDED_UNTIMED = 4;

    /**
     * The feature id for the '<em><b>Guard</b></em>' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int GUARDED_UNTIMED__GUARD = GUARDED__GUARD;

    /**
     * The number of structural features of the '<em>Guarded Untimed</em>' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int GUARDED_UNTIMED_FEATURE_COUNT = GUARDED_FEATURE_COUNT + 0;

    /**
     * The meta object id for the '{@link bip2.ujf.verimag.bip.time.impl.ResumedImpl <em>Resumed</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see bip2.ujf.verimag.bip.time.impl.ResumedImpl
     * @see bip2.ujf.verimag.bip.time.impl.TimePackageImpl#getResumed()
     * @generated
     */
    int RESUMED = 5;

    /**
     * The feature id for the '<em><b>Resume</b></em>' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int RESUMED__RESUME = 0;

    /**
     * The number of structural features of the '<em>Resumed</em>' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int RESUMED_FEATURE_COUNT = 1;

    /**
     * The meta object id for the '{@link bip2.ujf.verimag.bip.time.impl.ResumeImpl <em>Resume</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see bip2.ujf.verimag.bip.time.impl.ResumeImpl
     * @see bip2.ujf.verimag.bip.time.impl.TimePackageImpl#getResume()
     * @generated
     */
    int RESUME = 6;

    /**
     * The feature id for the '<em><b>Guard</b></em>' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int RESUME__GUARD = GUARDED__GUARD;

    /**
     * The number of structural features of the '<em>Resume</em>' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     * @ordered
     */
    int RESUME_FEATURE_COUNT = GUARDED_FEATURE_COUNT + 0;

    /**
     * The meta object id for the '{@link bip2.ujf.verimag.bip.time.Urgency <em>Urgency</em>}' enum.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see bip2.ujf.verimag.bip.time.Urgency
     * @see bip2.ujf.verimag.bip.time.impl.TimePackageImpl#getUrgency()
     * @generated
     */
    int URGENCY = 7;

    /**
     * Returns the meta object for class '{@link bip2.ujf.verimag.bip.time.ClockDeclaration <em>Clock Declaration</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for class '<em>Clock Declaration</em>'.
     * @see bip2.ujf.verimag.bip.time.ClockDeclaration
     * @generated
     */
    EClass getClockDeclaration();

    /**
     * Returns the meta object for the reference '{@link bip2.ujf.verimag.bip.time.ClockDeclaration#getUnit <em>Unit</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the reference '<em>Unit</em>'.
     * @see bip2.ujf.verimag.bip.time.ClockDeclaration#getUnit()
     * @see #getClockDeclaration()
     * @generated
     */
    EReference getClockDeclaration_Unit();

    /**
     * Returns the meta object for the attribute '{@link bip2.ujf.verimag.bip.time.ClockDeclaration#getName <em>Name</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Name</em>'.
     * @see bip2.ujf.verimag.bip.time.ClockDeclaration#getName()
     * @see #getClockDeclaration()
     * @generated
     */
    EAttribute getClockDeclaration_Name();

    /**
     * Returns the meta object for class '{@link bip2.ujf.verimag.bip.time.UrgentElement <em>Urgent Element</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for class '<em>Urgent Element</em>'.
     * @see bip2.ujf.verimag.bip.time.UrgentElement
     * @generated
     */
    EClass getUrgentElement();

    /**
     * Returns the meta object for the attribute '{@link bip2.ujf.verimag.bip.time.UrgentElement#getUrgency <em>Urgency</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the attribute '<em>Urgency</em>'.
     * @see bip2.ujf.verimag.bip.time.UrgentElement#getUrgency()
     * @see #getUrgentElement()
     * @generated
     */
    EAttribute getUrgentElement_Urgency();

    /**
     * Returns the meta object for class '{@link bip2.ujf.verimag.bip.time.Time <em>Time</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for class '<em>Time</em>'.
     * @see bip2.ujf.verimag.bip.time.Time
     * @generated
     */
    EClass getTime();

    /**
     * Returns the meta object for class '{@link bip2.ujf.verimag.bip.time.Guarded <em>Guarded</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for class '<em>Guarded</em>'.
     * @see bip2.ujf.verimag.bip.time.Guarded
     * @generated
     */
    EClass getGuarded();

    /**
     * Returns the meta object for the containment reference '{@link bip2.ujf.verimag.bip.time.Guarded#getGuard <em>Guard</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference '<em>Guard</em>'.
     * @see bip2.ujf.verimag.bip.time.Guarded#getGuard()
     * @see #getGuarded()
     * @generated
     */
    EReference getGuarded_Guard();

    /**
     * Returns the meta object for class '{@link bip2.ujf.verimag.bip.time.GuardedUntimed <em>Guarded Untimed</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for class '<em>Guarded Untimed</em>'.
     * @see bip2.ujf.verimag.bip.time.GuardedUntimed
     * @generated
     */
    EClass getGuardedUntimed();

    /**
     * Returns the meta object for class '{@link bip2.ujf.verimag.bip.time.Resumed <em>Resumed</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for class '<em>Resumed</em>'.
     * @see bip2.ujf.verimag.bip.time.Resumed
     * @generated
     */
    EClass getResumed();

    /**
     * Returns the meta object for the containment reference '{@link bip2.ujf.verimag.bip.time.Resumed#getResume <em>Resume</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for the containment reference '<em>Resume</em>'.
     * @see bip2.ujf.verimag.bip.time.Resumed#getResume()
     * @see #getResumed()
     * @generated
     */
    EReference getResumed_Resume();

    /**
     * Returns the meta object for class '{@link bip2.ujf.verimag.bip.time.Resume <em>Resume</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for class '<em>Resume</em>'.
     * @see bip2.ujf.verimag.bip.time.Resume
     * @generated
     */
    EClass getResume();

    /**
     * Returns the meta object for enum '{@link bip2.ujf.verimag.bip.time.Urgency <em>Urgency</em>}'.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the meta object for enum '<em>Urgency</em>'.
     * @see bip2.ujf.verimag.bip.time.Urgency
     * @generated
     */
    EEnum getUrgency();

    /**
     * Returns the factory that creates the instances of the model.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @return the factory that creates the instances of the model.
     * @generated
     */
    TimeFactory getTimeFactory();

    /**
     * <!-- begin-user-doc -->
     * Defines literals for the meta objects that represent
     * <ul>
     *   <li>each class,</li>
     *   <li>each feature of each class,</li>
     *   <li>each enum,</li>
     *   <li>and each data type</li>
     * </ul>
     * <!-- end-user-doc -->
     * @generated
     */
    interface Literals {
        /**
         * The meta object literal for the '{@link bip2.ujf.verimag.bip.time.impl.ClockDeclarationImpl <em>Clock Declaration</em>}' class.
         * <!-- begin-user-doc -->
         * <!-- end-user-doc -->
         * @see bip2.ujf.verimag.bip.time.impl.ClockDeclarationImpl
         * @see bip2.ujf.verimag.bip.time.impl.TimePackageImpl#getClockDeclaration()
         * @generated
         */
        EClass CLOCK_DECLARATION = eINSTANCE.getClockDeclaration();

        /**
         * The meta object literal for the '<em><b>Unit</b></em>' reference feature.
         * <!-- begin-user-doc -->
         * <!-- end-user-doc -->
         * @generated
         */
        EReference CLOCK_DECLARATION__UNIT = eINSTANCE
                .getClockDeclaration_Unit();

        /**
         * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
         * <!-- begin-user-doc -->
         * <!-- end-user-doc -->
         * @generated
         */
        EAttribute CLOCK_DECLARATION__NAME = eINSTANCE
                .getClockDeclaration_Name();

        /**
         * The meta object literal for the '{@link bip2.ujf.verimag.bip.time.UrgentElement <em>Urgent Element</em>}' class.
         * <!-- begin-user-doc -->
         * <!-- end-user-doc -->
         * @see bip2.ujf.verimag.bip.time.UrgentElement
         * @see bip2.ujf.verimag.bip.time.impl.TimePackageImpl#getUrgentElement()
         * @generated
         */
        EClass URGENT_ELEMENT = eINSTANCE.getUrgentElement();

        /**
         * The meta object literal for the '<em><b>Urgency</b></em>' attribute feature.
         * <!-- begin-user-doc -->
         * <!-- end-user-doc -->
         * @generated
         */
        EAttribute URGENT_ELEMENT__URGENCY = eINSTANCE
                .getUrgentElement_Urgency();

        /**
         * The meta object literal for the '{@link bip2.ujf.verimag.bip.time.impl.TimeImpl <em>Time</em>}' class.
         * <!-- begin-user-doc -->
         * <!-- end-user-doc -->
         * @see bip2.ujf.verimag.bip.time.impl.TimeImpl
         * @see bip2.ujf.verimag.bip.time.impl.TimePackageImpl#getTime()
         * @generated
         */
        EClass TIME = eINSTANCE.getTime();

        /**
         * The meta object literal for the '{@link bip2.ujf.verimag.bip.time.Guarded <em>Guarded</em>}' class.
         * <!-- begin-user-doc -->
         * <!-- end-user-doc -->
         * @see bip2.ujf.verimag.bip.time.Guarded
         * @see bip2.ujf.verimag.bip.time.impl.TimePackageImpl#getGuarded()
         * @generated
         */
        EClass GUARDED = eINSTANCE.getGuarded();

        /**
         * The meta object literal for the '<em><b>Guard</b></em>' containment reference feature.
         * <!-- begin-user-doc -->
         * <!-- end-user-doc -->
         * @generated
         */
        EReference GUARDED__GUARD = eINSTANCE.getGuarded_Guard();

        /**
         * The meta object literal for the '{@link bip2.ujf.verimag.bip.time.impl.GuardedUntimedImpl <em>Guarded Untimed</em>}' class.
         * <!-- begin-user-doc -->
         * <!-- end-user-doc -->
         * @see bip2.ujf.verimag.bip.time.impl.GuardedUntimedImpl
         * @see bip2.ujf.verimag.bip.time.impl.TimePackageImpl#getGuardedUntimed()
         * @generated
         */
        EClass GUARDED_UNTIMED = eINSTANCE.getGuardedUntimed();

        /**
         * The meta object literal for the '{@link bip2.ujf.verimag.bip.time.impl.ResumedImpl <em>Resumed</em>}' class.
         * <!-- begin-user-doc -->
         * <!-- end-user-doc -->
         * @see bip2.ujf.verimag.bip.time.impl.ResumedImpl
         * @see bip2.ujf.verimag.bip.time.impl.TimePackageImpl#getResumed()
         * @generated
         */
        EClass RESUMED = eINSTANCE.getResumed();

        /**
         * The meta object literal for the '<em><b>Resume</b></em>' containment reference feature.
         * <!-- begin-user-doc -->
         * <!-- end-user-doc -->
         * @generated
         */
        EReference RESUMED__RESUME = eINSTANCE.getResumed_Resume();

        /**
         * The meta object literal for the '{@link bip2.ujf.verimag.bip.time.impl.ResumeImpl <em>Resume</em>}' class.
         * <!-- begin-user-doc -->
         * <!-- end-user-doc -->
         * @see bip2.ujf.verimag.bip.time.impl.ResumeImpl
         * @see bip2.ujf.verimag.bip.time.impl.TimePackageImpl#getResume()
         * @generated
         */
        EClass RESUME = eINSTANCE.getResume();

        /**
         * The meta object literal for the '{@link bip2.ujf.verimag.bip.time.Urgency <em>Urgency</em>}' enum.
         * <!-- begin-user-doc -->
         * <!-- end-user-doc -->
         * @see bip2.ujf.verimag.bip.time.Urgency
         * @see bip2.ujf.verimag.bip.time.impl.TimePackageImpl#getUrgency()
         * @generated
         */
        EEnum URGENCY = eINSTANCE.getUrgency();

    }

} //TimePackage
