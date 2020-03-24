/*
 *
 *
 */

package ujf.verimag.bip.ifinder.invariant;

/*
 * base class for invariant assertions
 *
 */

public abstract class Assertion {
    
    public Assertion() {}
    
    public abstract String toString();

    public String blankIndent() {
	int k = Blank.length() / 4;
	if (k < Indent)
	    for(int j = k; j < Indent; j++)
		Blank += "    ";
	else if (k > Indent)
	    Blank = Blank.substring(0, 4 * Indent);
	return Blank;
    }
    
    public static int Indent = 0;
    
    private static String Blank = "";
    
}



