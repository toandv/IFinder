/*
 *
 *
 */

package ujf.verimag.bip.ifinder.analysis.reachability;

/*
 *
 *
 */

class TConstraints {
    
    public int[] x, y, b;
    
    public TConstraints(int n) {
	x = new int[n];
	y = new int[n];
	b = new int[n];
    }
    
    public TConstraints(int[] ax, int[] ay, int[] ab) {
	x = ax;
	y = ay;
	b = ab;
    }
    
    public int size() {
	return x.length;
    }

    public int frontSize() {
	// no of constraints of the form x <= c
	int s = 0;
	for(int i = 0; i < x.length; i++)
	    if (y[i] == 0 && Bound.Sign(b[i]) == Bound.SignT.LE) s++;
	return s;
    }
    
}
