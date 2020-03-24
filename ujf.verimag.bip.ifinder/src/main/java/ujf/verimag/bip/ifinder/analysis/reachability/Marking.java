/*
 *
 *
 */

package ujf.verimag.bip.ifinder.analysis.reachability;

/*
 *
 *
 */

public class Marking {

    // a bit vector
    protected byte[] m_bytes;
    
    // constructors
    
    public Marking(int nbits) {
	int len = (nbits / 8)  + ((nbits % 8 != 0) ? 1 : 0);
	m_bytes = new byte[len];
	for(int k = 0; k < len; k++)
	    m_bytes[k] = 0;
    }
    
    public Marking(Marking marking) {
	m_bytes = new byte[marking.m_bytes.length];
	for(int k = 0; k < marking.m_bytes.length; k++)
	    m_bytes[k] = marking.m_bytes[k];
    }
    
    // basic bits operations
    
    public boolean isBit(int i) {
	return (m_bytes[i/8] & (0x1 << (i%8))) != 0;
    }

    public void setBit(int i) {
	m_bytes[i/8] |=  ((0x1) << (i%8));
    }

    public void resetBit(int i) {
	m_bytes[i/8] &= ~((0x1) << (i%8));
    }

    // global operations

    public boolean enables(Marking pre) {
	boolean ok = true;
	for(int k = 0; ok && k < m_bytes.length; k++)
	    ok = ((pre.m_bytes[k] & m_bytes[k]) == pre.m_bytes[k]);
	return ok;
    }

    public void execute(Marking pre, Marking post) {
	for(int k = 0; k < m_bytes.length; k++) {
	    m_bytes[k] &= ~pre.m_bytes[k];
	    m_bytes[k] |= post.m_bytes[k];
	}
    }

    // dump
    
    public void dump() {
	for(int k = 0; k < m_bytes.length; k++)
	    for(int j = 0;  j < 8; j++) {
		boolean bit = ((m_bytes[k] & (0x1 << j)) != 0);
		System.out.print( bit ? "1" : "0");
	    }
	System.out.println("");
    }
}
