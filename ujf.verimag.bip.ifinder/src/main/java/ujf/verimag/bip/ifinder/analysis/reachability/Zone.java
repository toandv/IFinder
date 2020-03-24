/*
 *
 *
 */

package ujf.verimag.bip.ifinder.analysis.reachability;

/*
 * fixed-size dbms 
 *
 */

public class Zone {

    private int[][] dbm;
    
    //
    // constructors
    //
    
    public Zone(int n, boolean x_active) {
	// all clocks inactive
	dbm = new int[n][];
	for(int i = 0; i < n; i++) {
	    dbm[i] = new int[n];
	    for(int j = 0; j < n; j++)
		dbm[i][j] = Bound.INFINIT;
	}
	// mark clock 0 active
	dbm[0][0] = 0;
	// mark all other clocks as indicated
	for(int i = 1; i < n; i++)
	    dbm[i][i] = x_active ? 0 : -1;
    }
    
    public Zone(Zone z) {
	// copy zone z
	int n = z.dbm.length;
	dbm = new int[n][];
	for(int i = 0; i < n; i++) {
	    dbm[i] = new int[n];
	    for(int j = 0; j < n; j++)
		dbm[i][j] = z.dbm[i][j];
	}
    } 
    
    //
    // zone operations
    //
    
    public boolean intersects(int[] x, int[] y, int[] b) {
	assert(x.length == y.length && y.length == b.length);
	for(int k = 0; k < x.length; k++) {
	    int i = x[k], j = y[k], c = b[k];
	    assert(0 <= i && i < dbm.length && dbm[i][i] == 0);
	    assert(0 <= j && j < dbm.length && dbm[j][j] == 0);
	    assert(i != j);
	    // add constraint
	    dbm[i][j] = Bound.Min(dbm[i][j], c);
	}
	boolean consistent = normalize();
	return consistent;
    }
    
    public void set(int[] x, int[] v) {
	assert(x.length == v.length);
	for(int k = 0; k < x.length; k++) {
	    int i = x[k], c = v[k];
	    assert(1 <= i && i < dbm.length);
	    assert(0 <= c);
	    // clear i constraints
	    for(int j = 1; j < dbm.length; j++)
		dbm[i][j] = dbm[j][i] = Bound.INFINIT;
	    // mark i as active
	    dbm[i][i] = 0;
	    // add =c constraint
	    dbm[i][0] = Bound.Make(Bound.SignT.LE,  c);
	    dbm[0][i] = Bound.Make(Bound.SignT.LE, -c);
	}
	// normalize
	boolean consistent = normalize();
	assert(consistent);
    }
    
    public void kill(int[] x) {
	for(int k = 0; k < x.length; k++) {
	    int i = x[k];
	    assert(1 <= i && i < dbm.length);
	    // clear i constraints
	    for(int j = 1; j < dbm.length; j++)
		dbm[i][j] = dbm[j][i] = Bound.INFINIT;
	    // mark i as inactive
	    dbm[i][i] = -1;
	}
    }
    
    public void forward() {
	for(int i = 1; i < dbm.length; i++)
	    dbm[i][0] = Bound.INFINIT;
	// no need to normalize
    }
    
    public void extrapolate(Zone z) {
	assert(dbm.length == z.dbm.length);
	for(int i = 0; i < dbm.length; i++)
	    for(int j = 0; j < dbm.length; j++) {
		if (i == j) continue;
		if (z.dbm[i][j] < Bound.INFINIT &&
		    z.dbm[i][j] < dbm[i][j]) {
		    dbm[i][j] = Bound.INFINIT;
		    continue;
		}
		// not yet finished : clarify 
		// if (z.dbm[j][i] < Bound.INFINIT &&
		//     dbm[i][j] < Bound.Complement(z.dbm[j][i])) {
		//     dbm[i][j] = Bound.Complement(z.dbm[j][i]);
		//     continue;
		// }
	    }
	// not yet finished : normalize ?
    }
    
    private boolean normalize() {
	// Floyd-Warshall
	for(int k = 0; k < dbm.length; k++)
	    for(int i = 0; i < dbm.length; i++) 
		if (i != k && dbm[i][k] < Bound.INFINIT) { 
		    if (dbm[k][i] < Bound.INFINIT && 
			Bound.Add(dbm[i][k],dbm[k][i]) < Bound.Make(Bound.SignT.LE,0))
			return false;		    
		    for (int j = 0; j < dbm.length; j++)
			if (i != j && j != k && dbm[k][j] < Bound.INFINIT &&
			    Bound.Add(dbm[i][k], dbm[k][j]) < dbm[i][j])
			    dbm[i][j] = Bound.Add(dbm[i][k], dbm[k][j]);
		}
	return true;
    }
    
    //
    //  
    //
    
    public boolean equals(Zone z) {
	assert(dbm.length == z.dbm.length);
	boolean eq = true;
	for(int i = 0; eq && i < dbm.length; i++)
	    for(int j = 0; eq && j < dbm.length; j++)
		eq = (dbm[i][j] == z.dbm[i][j]);
	return eq;
    }
    
    public boolean includes(Zone z) {
	assert(dbm.length == z.dbm.length);
	boolean inc = true;
	for(int i = 0; inc && i < dbm.length; i++)
	    inc = (dbm[i][i] == z.dbm[i][i]);
	for(int i = 0; inc && i < dbm.length; i++) {
	    if (dbm[i][i] == -1) continue;
	    for(int j = 0; inc && j < dbm.length; j++) {
		if (i == j || dbm[j][j] == -1) continue;
		inc = (dbm[i][j] >= z.dbm[i][j]);
	    }
	}
	return inc;
    }
    
    //
    //
    //
    
    public int size() { return dbm.length - 1; }
    
    public int get(int i, int j) { return dbm[i][j]; }
    
    public boolean active(int i) { return dbm[i][i] == 0; }
    
    public boolean derived(int i, int j) {
	assert(i != j && active(i) && active(j));
	boolean der = false;
	for(int k = 0; !der && k < dbm.length; k++) {
	    if (!active(k)) continue;
	    if (i != k && dbm[i][k] != Bound.INFINIT && !dependent(i, k) &&
		k != j && dbm[k][j] != Bound.INFINIT && !dependent(j, k) &&
		Bound.Add(dbm[i][k],dbm[k][j]) <= dbm[i][j])
		der = true;
	}
	return der;
    }
    
    public boolean dependent(int i, int j) {
	assert(i != j && active(i) && active(j));
	if (dbm[i][j] == Bound.INFINIT ||
	    dbm[j][i] == Bound.INFINIT)
	    return false;
	if (Bound.Sign(dbm[i][j]) == Bound.SignT.LT ||
	    Bound.Sign(dbm[j][i]) == Bound.SignT.LT)
	    return false;
	if (Bound.Value(dbm[i][j]) + Bound.Value(dbm[j][i]) > 0)
	    return false;
	return true;
    }
    
    //
    // dump
    //
    
    public void dump() {
	for(int i = 0; i < dbm.length; i++) {
	    for(int j = 0; j < dbm.length; j++) {
		if (i == j)
		    System.out.print( active(i) ? "*  " : "-   ");
		else 
		    System.out.print(Bound.ToString(dbm[i][j]));
		System.out.print(" ");
	    }
	    System.out.println();
	}
    }
    
    
}
