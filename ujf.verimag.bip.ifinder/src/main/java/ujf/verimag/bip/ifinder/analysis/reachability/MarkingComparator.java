/*
 *
 *
 */

package ujf.verimag.bip.ifinder.analysis.reachability;

import java.util.*;

/*
 *
 *
 */

public class MarkingComparator
    implements Comparator<Marking> {

    public int compare(Marking a, Marking b) {
	int cmp = 0;
	cmp = a.m_bytes.length - b.m_bytes.length;
	for(int i = 0; cmp == 0 && i < a.m_bytes.length; i++) 
	    cmp = a.m_bytes[i] - b.m_bytes[i];
	return cmp;
    }
    
}
