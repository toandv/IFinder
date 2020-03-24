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

import java.util.Arrays;
import java.util.Collections;
import java.util.List;

import org.eclipse.emf.common.util.Enumerator;

/**
 * <!-- begin-user-doc -->
 * A representation of the literals of the enumeration '<em><b>Unit</b></em>',
 * and utility methods for working with them.
 * <!-- end-user-doc -->
 * @see bip2.ujf.verimag.bip.time.TimePackage#getTimeUnit()
 * @model
 * @generated
 */
public enum TimeUnit implements Enumerator {
    /**
     * The '<em><b>None</b></em>' literal object.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #NONE_VALUE
     * @generated
     * @ordered
     */
    NONE(0, "none", "none"),

    /**
     * The '<em><b>Nanosecond</b></em>' literal object.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #NANOSECOND_VALUE
     * @generated
     * @ordered
     */
    NANOSECOND(1, "nanosecond", "nanosecond"),

    /**
     * The '<em><b>Microsecond</b></em>' literal object.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #MICROSECOND_VALUE
     * @generated
     * @ordered
     */
    MICROSECOND(2, "microsecond", "microsecond"),

    /**
     * The '<em><b>Millisecond</b></em>' literal object.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #MILLISECOND_VALUE
     * @generated
     * @ordered
     */
    MILLISECOND(3, "millisecond", "millisecond"),

    /**
     * The '<em><b>Second</b></em>' literal object.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #SECOND_VALUE
     * @generated
     * @ordered
     */
    SECOND(4, "second", "second"),

    /**
     * The '<em><b>Minute</b></em>' literal object.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #MINUTE_VALUE
     * @generated
     * @ordered
     */
    MINUTE(5, "minute", "minute"),

    /**
     * The '<em><b>Hour</b></em>' literal object.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #HOUR_VALUE
     * @generated
     * @ordered
     */
    HOUR(6, "hour", "hour"),

    /**
     * The '<em><b>Day</b></em>' literal object.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @see #DAY_VALUE
     * @generated
     * @ordered
     */
    DAY(7, "day", "day");

    /**
     * The '<em><b>None</b></em>' literal value.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of '<em><b>None</b></em>' literal object isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @see #NONE
     * @model name="none"
     * @generated
     * @ordered
     */
    public static final int NONE_VALUE = 0;

    /**
     * The '<em><b>Nanosecond</b></em>' literal value.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of '<em><b>Nanosecond</b></em>' literal object isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @see #NANOSECOND
     * @model name="nanosecond"
     * @generated
     * @ordered
     */
    public static final int NANOSECOND_VALUE = 1;

    /**
     * The '<em><b>Microsecond</b></em>' literal value.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of '<em><b>Microsecond</b></em>' literal object isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @see #MICROSECOND
     * @model name="microsecond"
     * @generated
     * @ordered
     */
    public static final int MICROSECOND_VALUE = 2;

    /**
     * The '<em><b>Millisecond</b></em>' literal value.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of '<em><b>Millisecond</b></em>' literal object isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @see #MILLISECOND
     * @model name="millisecond"
     * @generated
     * @ordered
     */
    public static final int MILLISECOND_VALUE = 3;

    /**
     * The '<em><b>Second</b></em>' literal value.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of '<em><b>Second</b></em>' literal object isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @see #SECOND
     * @model name="second"
     * @generated
     * @ordered
     */
    public static final int SECOND_VALUE = 4;

    /**
     * The '<em><b>Minute</b></em>' literal value.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of '<em><b>Minute</b></em>' literal object isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @see #MINUTE
     * @model name="minute"
     * @generated
     * @ordered
     */
    public static final int MINUTE_VALUE = 5;

    /**
     * The '<em><b>Hour</b></em>' literal value.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of '<em><b>Hour</b></em>' literal object isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @see #HOUR
     * @model name="hour"
     * @generated
     * @ordered
     */
    public static final int HOUR_VALUE = 6;

    /**
     * The '<em><b>Day</b></em>' literal value.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of '<em><b>Day</b></em>' literal object isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @see #DAY
     * @model name="day"
     * @generated
     * @ordered
     */
    public static final int DAY_VALUE = 7;

    /**
     * An array of all the '<em><b>Unit</b></em>' enumerators.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    private static final TimeUnit[] VALUES_ARRAY = new TimeUnit[] { NONE,
            NANOSECOND, MICROSECOND, MILLISECOND, SECOND, MINUTE, HOUR, DAY, };

    /**
     * A public read-only list of all the '<em><b>Unit</b></em>' enumerators.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public static final List<TimeUnit> VALUES = Collections
            .unmodifiableList(Arrays.asList(VALUES_ARRAY));

    /**
     * Returns the '<em><b>Unit</b></em>' literal with the specified literal value.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public static TimeUnit get(String literal) {
        for (int i = 0; i < VALUES_ARRAY.length; ++i) {
            TimeUnit result = VALUES_ARRAY[i];
            if (result.toString().equals(literal)) {
                return result;
            }
        }
        return null;
    }

    /**
     * Returns the '<em><b>Unit</b></em>' literal with the specified name.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public static TimeUnit getByName(String name) {
        for (int i = 0; i < VALUES_ARRAY.length; ++i) {
            TimeUnit result = VALUES_ARRAY[i];
            if (result.getName().equals(name)) {
                return result;
            }
        }
        return null;
    }

    /**
     * Returns the '<em><b>Unit</b></em>' literal with the specified integer value.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public static TimeUnit get(int value) {
        switch (value) {
        case NONE_VALUE:
            return NONE;
        case NANOSECOND_VALUE:
            return NANOSECOND;
        case MICROSECOND_VALUE:
            return MICROSECOND;
        case MILLISECOND_VALUE:
            return MILLISECOND;
        case SECOND_VALUE:
            return SECOND;
        case MINUTE_VALUE:
            return MINUTE;
        case HOUR_VALUE:
            return HOUR;
        case DAY_VALUE:
            return DAY;
        }
        return null;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    private final int value;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    private final String name;

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    private final String literal;

    /**
     * Only this class can construct instances.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    private TimeUnit(int value, String name, String literal) {
        this.value = value;
        this.name = name;
        this.literal = literal;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public int getValue() {
        return value;
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
    public String getLiteral() {
        return literal;
    }

    /**
     * Returns the literal value of the enumerator, which is its string representation.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    @Override
    public String toString() {
        return literal;
    }

} //TimeUnit
