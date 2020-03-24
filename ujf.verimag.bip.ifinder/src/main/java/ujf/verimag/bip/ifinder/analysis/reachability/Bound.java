/*
 * 
 *
 */

package ujf.verimag.bip.ifinder.analysis.reachability;

/*
 *
 *
 */

public class Bound {
    
    public final static int INFINIT = Integer.MAX_VALUE;
    
    public enum SignT {LT, LE};

    //
    
    public static int Make(SignT sign, int value) {
	return (sign == SignT.LE ? 1 : 0) + (value << 1);
    }
    
    public static SignT Sign(int bound) {
	return ((bound & 1) == 1) ? SignT.LE : SignT.LT;
    }
    
    public static int Value(int bound) {
	return bound >> 1;
    }
    
    //
    
    public static int Add(int bound1, int bound2) {
	return (bound1 & bound2 & 1) | ((bound1 & ~1) + (bound2 & ~1));
    }
    
    public static int Complement(int bound) {
	return Make(((bound & 1) == 1) ? SignT.LT : SignT.LE,
		    -(bound >> 1));
    }

    public static int Min(int bound1, int bound2) {
	return (bound1 < bound2) ? bound1 : bound2;
    }
    
    public static int Max(int bound1, int bound2) {
	return (bound1 < bound2) ? bound2 : bound1;
    }
    
    //
    
    public static String ToString(int bound) {
	return bound == INFINIT ? "<oo" :
	    (((bound & 1) == 1) ? "<=" : "<") + (bound >> 1); 
    }
    
}
