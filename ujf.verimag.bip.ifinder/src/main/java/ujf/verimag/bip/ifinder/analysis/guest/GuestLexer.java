// $ANTLR 3.5.2 Guest.g 2019-07-17 16:09:58

package ujf.verimag.bip.ifinder.analysis.guest;


import org.antlr.runtime.*;
import java.util.Stack;
import java.util.List;
import java.util.ArrayList;

@SuppressWarnings("all")
public class GuestLexer extends Lexer {
	public static final int EOF=-1;
	public static final int T__14=14;
	public static final int T__15=15;
	public static final int T__16=16;
	public static final int T__17=17;
	public static final int T__18=18;
	public static final int T__19=19;
	public static final int T__20=20;
	public static final int T__21=21;
	public static final int AND=4;
	public static final int COMMENT=5;
	public static final int GE=6;
	public static final int ID=7;
	public static final int IMPLIES=8;
	public static final int INT=9;
	public static final int LE=10;
	public static final int NOT=11;
	public static final int OR=12;
	public static final int WHITESPACE=13;

	// delegates
	// delegators
	public Lexer[] getDelegates() {
		return new Lexer[] {};
	}

	public GuestLexer() {} 
	public GuestLexer(CharStream input) {
		this(input, new RecognizerSharedState());
	}
	public GuestLexer(CharStream input, RecognizerSharedState state) {
		super(input,state);
	}
	@Override public String getGrammarFileName() { return "Guest.g"; }

	// $ANTLR start "T__14"
	public final void mT__14() throws RecognitionException {
		try {
			int _type = T__14;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// Guest.g:6:7: ( '(' )
			// Guest.g:6:9: '('
			{
			match('('); 
			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "T__14"

	// $ANTLR start "T__15"
	public final void mT__15() throws RecognitionException {
		try {
			int _type = T__15;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// Guest.g:7:7: ( ')' )
			// Guest.g:7:9: ')'
			{
			match(')'); 
			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "T__15"

	// $ANTLR start "T__16"
	public final void mT__16() throws RecognitionException {
		try {
			int _type = T__16;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// Guest.g:8:7: ( '*' )
			// Guest.g:8:9: '*'
			{
			match('*'); 
			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "T__16"

	// $ANTLR start "T__17"
	public final void mT__17() throws RecognitionException {
		try {
			int _type = T__17;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// Guest.g:9:7: ( '+' )
			// Guest.g:9:9: '+'
			{
			match('+'); 
			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "T__17"

	// $ANTLR start "T__18"
	public final void mT__18() throws RecognitionException {
		try {
			int _type = T__18;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// Guest.g:10:7: ( '-' )
			// Guest.g:10:9: '-'
			{
			match('-'); 
			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "T__18"

	// $ANTLR start "T__19"
	public final void mT__19() throws RecognitionException {
		try {
			int _type = T__19;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// Guest.g:11:7: ( '=' )
			// Guest.g:11:9: '='
			{
			match('='); 
			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "T__19"

	// $ANTLR start "T__20"
	public final void mT__20() throws RecognitionException {
		try {
			int _type = T__20;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// Guest.g:12:7: ( '[' )
			// Guest.g:12:9: '['
			{
			match('['); 
			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "T__20"

	// $ANTLR start "T__21"
	public final void mT__21() throws RecognitionException {
		try {
			int _type = T__21;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// Guest.g:13:7: ( ']' )
			// Guest.g:13:9: ']'
			{
			match(']'); 
			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "T__21"

	// $ANTLR start "AND"
	public final void mAND() throws RecognitionException {
		try {
			int _type = AND;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// Guest.g:126:16: ( 'and' )
			// Guest.g:126:18: 'and'
			{
			match("and"); 

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "AND"

	// $ANTLR start "OR"
	public final void mOR() throws RecognitionException {
		try {
			int _type = OR;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// Guest.g:127:16: ( 'or' )
			// Guest.g:127:18: 'or'
			{
			match("or"); 

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "OR"

	// $ANTLR start "NOT"
	public final void mNOT() throws RecognitionException {
		try {
			int _type = NOT;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// Guest.g:128:16: ( 'not' )
			// Guest.g:128:18: 'not'
			{
			match("not"); 

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "NOT"

	// $ANTLR start "ID"
	public final void mID() throws RecognitionException {
		try {
			int _type = ID;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// Guest.g:130:16: ( ( 'a' .. 'z' | 'A' .. 'Z' ) ( 'a' .. 'z' | 'A' .. 'Z' | '0' .. '9' | '_' | '.' )* )
			// Guest.g:130:18: ( 'a' .. 'z' | 'A' .. 'Z' ) ( 'a' .. 'z' | 'A' .. 'Z' | '0' .. '9' | '_' | '.' )*
			{
			if ( (input.LA(1) >= 'A' && input.LA(1) <= 'Z')||(input.LA(1) >= 'a' && input.LA(1) <= 'z') ) {
				input.consume();
			}
			else {
				MismatchedSetException mse = new MismatchedSetException(null,input);
				recover(mse);
				throw mse;
			}
			// Guest.g:130:38: ( 'a' .. 'z' | 'A' .. 'Z' | '0' .. '9' | '_' | '.' )*
			loop1:
			while (true) {
				int alt1=2;
				int LA1_0 = input.LA(1);
				if ( (LA1_0=='.'||(LA1_0 >= '0' && LA1_0 <= '9')||(LA1_0 >= 'A' && LA1_0 <= 'Z')||LA1_0=='_'||(LA1_0 >= 'a' && LA1_0 <= 'z')) ) {
					alt1=1;
				}

				switch (alt1) {
				case 1 :
					// Guest.g:
					{
					if ( input.LA(1)=='.'||(input.LA(1) >= '0' && input.LA(1) <= '9')||(input.LA(1) >= 'A' && input.LA(1) <= 'Z')||input.LA(1)=='_'||(input.LA(1) >= 'a' && input.LA(1) <= 'z') ) {
						input.consume();
					}
					else {
						MismatchedSetException mse = new MismatchedSetException(null,input);
						recover(mse);
						throw mse;
					}
					}
					break;

				default :
					break loop1;
				}
			}

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "ID"

	// $ANTLR start "INT"
	public final void mINT() throws RecognitionException {
		try {
			int _type = INT;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// Guest.g:132:16: ( ( '0' .. '9' )+ )
			// Guest.g:132:18: ( '0' .. '9' )+
			{
			// Guest.g:132:18: ( '0' .. '9' )+
			int cnt2=0;
			loop2:
			while (true) {
				int alt2=2;
				int LA2_0 = input.LA(1);
				if ( ((LA2_0 >= '0' && LA2_0 <= '9')) ) {
					alt2=1;
				}

				switch (alt2) {
				case 1 :
					// Guest.g:
					{
					if ( (input.LA(1) >= '0' && input.LA(1) <= '9') ) {
						input.consume();
					}
					else {
						MismatchedSetException mse = new MismatchedSetException(null,input);
						recover(mse);
						throw mse;
					}
					}
					break;

				default :
					if ( cnt2 >= 1 ) break loop2;
					EarlyExitException eee = new EarlyExitException(2, input);
					throw eee;
				}
				cnt2++;
			}

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "INT"

	// $ANTLR start "LE"
	public final void mLE() throws RecognitionException {
		try {
			int _type = LE;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// Guest.g:134:16: ( '<=' )
			// Guest.g:134:18: '<='
			{
			match("<="); 

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "LE"

	// $ANTLR start "GE"
	public final void mGE() throws RecognitionException {
		try {
			int _type = GE;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// Guest.g:135:16: ( '>=' )
			// Guest.g:135:18: '>='
			{
			match(">="); 

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "GE"

	// $ANTLR start "IMPLIES"
	public final void mIMPLIES() throws RecognitionException {
		try {
			int _type = IMPLIES;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// Guest.g:137:16: ( '=>' )
			// Guest.g:137:18: '=>'
			{
			match("=>"); 

			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "IMPLIES"

	// $ANTLR start "COMMENT"
	public final void mCOMMENT() throws RecognitionException {
		try {
			int _type = COMMENT;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// Guest.g:139:16: ( '#' (~ ( '\\r' | '\\n' ) )* ( '\\r' )? '\\n' )
			// Guest.g:139:18: '#' (~ ( '\\r' | '\\n' ) )* ( '\\r' )? '\\n'
			{
			match('#'); 
			// Guest.g:139:22: (~ ( '\\r' | '\\n' ) )*
			loop3:
			while (true) {
				int alt3=2;
				int LA3_0 = input.LA(1);
				if ( ((LA3_0 >= '\u0000' && LA3_0 <= '\t')||(LA3_0 >= '\u000B' && LA3_0 <= '\f')||(LA3_0 >= '\u000E' && LA3_0 <= '\uFFFF')) ) {
					alt3=1;
				}

				switch (alt3) {
				case 1 :
					// Guest.g:
					{
					if ( (input.LA(1) >= '\u0000' && input.LA(1) <= '\t')||(input.LA(1) >= '\u000B' && input.LA(1) <= '\f')||(input.LA(1) >= '\u000E' && input.LA(1) <= '\uFFFF') ) {
						input.consume();
					}
					else {
						MismatchedSetException mse = new MismatchedSetException(null,input);
						recover(mse);
						throw mse;
					}
					}
					break;

				default :
					break loop3;
				}
			}

			// Guest.g:139:36: ( '\\r' )?
			int alt4=2;
			int LA4_0 = input.LA(1);
			if ( (LA4_0=='\r') ) {
				alt4=1;
			}
			switch (alt4) {
				case 1 :
					// Guest.g:139:37: '\\r'
					{
					match('\r'); 
					}
					break;

			}

			match('\n'); 
			 _channel=HIDDEN; 
			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "COMMENT"

	// $ANTLR start "WHITESPACE"
	public final void mWHITESPACE() throws RecognitionException {
		try {
			int _type = WHITESPACE;
			int _channel = DEFAULT_TOKEN_CHANNEL;
			// Guest.g:141:16: ( ( ' ' | '\\r' | '\\t' | '\\n' )+ )
			// Guest.g:141:18: ( ' ' | '\\r' | '\\t' | '\\n' )+
			{
			// Guest.g:141:18: ( ' ' | '\\r' | '\\t' | '\\n' )+
			int cnt5=0;
			loop5:
			while (true) {
				int alt5=2;
				int LA5_0 = input.LA(1);
				if ( ((LA5_0 >= '\t' && LA5_0 <= '\n')||LA5_0=='\r'||LA5_0==' ') ) {
					alt5=1;
				}

				switch (alt5) {
				case 1 :
					// Guest.g:
					{
					if ( (input.LA(1) >= '\t' && input.LA(1) <= '\n')||input.LA(1)=='\r'||input.LA(1)==' ' ) {
						input.consume();
					}
					else {
						MismatchedSetException mse = new MismatchedSetException(null,input);
						recover(mse);
						throw mse;
					}
					}
					break;

				default :
					if ( cnt5 >= 1 ) break loop5;
					EarlyExitException eee = new EarlyExitException(5, input);
					throw eee;
				}
				cnt5++;
			}

			 _channel=HIDDEN; 
			}

			state.type = _type;
			state.channel = _channel;
		}
		finally {
			// do for sure before leaving
		}
	}
	// $ANTLR end "WHITESPACE"

	@Override
	public void mTokens() throws RecognitionException {
		// Guest.g:1:8: ( T__14 | T__15 | T__16 | T__17 | T__18 | T__19 | T__20 | T__21 | AND | OR | NOT | ID | INT | LE | GE | IMPLIES | COMMENT | WHITESPACE )
		int alt6=18;
		switch ( input.LA(1) ) {
		case '(':
			{
			alt6=1;
			}
			break;
		case ')':
			{
			alt6=2;
			}
			break;
		case '*':
			{
			alt6=3;
			}
			break;
		case '+':
			{
			alt6=4;
			}
			break;
		case '-':
			{
			alt6=5;
			}
			break;
		case '=':
			{
			int LA6_6 = input.LA(2);
			if ( (LA6_6=='>') ) {
				alt6=16;
			}

			else {
				alt6=6;
			}

			}
			break;
		case '[':
			{
			alt6=7;
			}
			break;
		case ']':
			{
			alt6=8;
			}
			break;
		case 'a':
			{
			int LA6_9 = input.LA(2);
			if ( (LA6_9=='n') ) {
				int LA6_20 = input.LA(3);
				if ( (LA6_20=='d') ) {
					int LA6_23 = input.LA(4);
					if ( (LA6_23=='.'||(LA6_23 >= '0' && LA6_23 <= '9')||(LA6_23 >= 'A' && LA6_23 <= 'Z')||LA6_23=='_'||(LA6_23 >= 'a' && LA6_23 <= 'z')) ) {
						alt6=12;
					}

					else {
						alt6=9;
					}

				}

				else {
					alt6=12;
				}

			}

			else {
				alt6=12;
			}

			}
			break;
		case 'o':
			{
			int LA6_10 = input.LA(2);
			if ( (LA6_10=='r') ) {
				int LA6_21 = input.LA(3);
				if ( (LA6_21=='.'||(LA6_21 >= '0' && LA6_21 <= '9')||(LA6_21 >= 'A' && LA6_21 <= 'Z')||LA6_21=='_'||(LA6_21 >= 'a' && LA6_21 <= 'z')) ) {
					alt6=12;
				}

				else {
					alt6=10;
				}

			}

			else {
				alt6=12;
			}

			}
			break;
		case 'n':
			{
			int LA6_11 = input.LA(2);
			if ( (LA6_11=='o') ) {
				int LA6_22 = input.LA(3);
				if ( (LA6_22=='t') ) {
					int LA6_25 = input.LA(4);
					if ( (LA6_25=='.'||(LA6_25 >= '0' && LA6_25 <= '9')||(LA6_25 >= 'A' && LA6_25 <= 'Z')||LA6_25=='_'||(LA6_25 >= 'a' && LA6_25 <= 'z')) ) {
						alt6=12;
					}

					else {
						alt6=11;
					}

				}

				else {
					alt6=12;
				}

			}

			else {
				alt6=12;
			}

			}
			break;
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
		case 'I':
		case 'J':
		case 'K':
		case 'L':
		case 'M':
		case 'N':
		case 'O':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'T':
		case 'U':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z':
			{
			alt6=12;
			}
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			{
			alt6=13;
			}
			break;
		case '<':
			{
			alt6=14;
			}
			break;
		case '>':
			{
			alt6=15;
			}
			break;
		case '#':
			{
			alt6=17;
			}
			break;
		case '\t':
		case '\n':
		case '\r':
		case ' ':
			{
			alt6=18;
			}
			break;
		default:
			NoViableAltException nvae =
				new NoViableAltException("", 6, 0, input);
			throw nvae;
		}
		switch (alt6) {
			case 1 :
				// Guest.g:1:10: T__14
				{
				mT__14(); 

				}
				break;
			case 2 :
				// Guest.g:1:16: T__15
				{
				mT__15(); 

				}
				break;
			case 3 :
				// Guest.g:1:22: T__16
				{
				mT__16(); 

				}
				break;
			case 4 :
				// Guest.g:1:28: T__17
				{
				mT__17(); 

				}
				break;
			case 5 :
				// Guest.g:1:34: T__18
				{
				mT__18(); 

				}
				break;
			case 6 :
				// Guest.g:1:40: T__19
				{
				mT__19(); 

				}
				break;
			case 7 :
				// Guest.g:1:46: T__20
				{
				mT__20(); 

				}
				break;
			case 8 :
				// Guest.g:1:52: T__21
				{
				mT__21(); 

				}
				break;
			case 9 :
				// Guest.g:1:58: AND
				{
				mAND(); 

				}
				break;
			case 10 :
				// Guest.g:1:62: OR
				{
				mOR(); 

				}
				break;
			case 11 :
				// Guest.g:1:65: NOT
				{
				mNOT(); 

				}
				break;
			case 12 :
				// Guest.g:1:69: ID
				{
				mID(); 

				}
				break;
			case 13 :
				// Guest.g:1:72: INT
				{
				mINT(); 

				}
				break;
			case 14 :
				// Guest.g:1:76: LE
				{
				mLE(); 

				}
				break;
			case 15 :
				// Guest.g:1:79: GE
				{
				mGE(); 

				}
				break;
			case 16 :
				// Guest.g:1:82: IMPLIES
				{
				mIMPLIES(); 

				}
				break;
			case 17 :
				// Guest.g:1:90: COMMENT
				{
				mCOMMENT(); 

				}
				break;
			case 18 :
				// Guest.g:1:98: WHITESPACE
				{
				mWHITESPACE(); 

				}
				break;

		}
	}



}
