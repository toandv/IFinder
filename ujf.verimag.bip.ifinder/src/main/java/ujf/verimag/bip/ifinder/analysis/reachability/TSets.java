/*
 *
 *
 */

package ujf.verimag.bip.ifinder.analysis.reachability;

/*
 *
 *
 */

class TSets {
    
    public int[] x, v;

    public TSets(int n) {
	x = new int[n];
	v = new int[n];
    }
    
    public TSets(int[] ax, int[] av) {
	x = ax;
	v = av;
    }
           
    public int size() {
	return x.length;
    }
    
}
