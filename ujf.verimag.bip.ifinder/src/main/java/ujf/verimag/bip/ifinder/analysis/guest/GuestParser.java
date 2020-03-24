// $ANTLR 3.5.2 Guest.g 2019-07-17 16:09:58

package ujf.verimag.bip.ifinder.analysis.guest;

import ujf.verimag.bip.ifinder.invariant.*;
import ujf.verimag.bip.ifinder.analysis.GuestAnalysis;


import org.antlr.runtime.*;
import java.util.Stack;
import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.HashMap;

@SuppressWarnings("all")
public class GuestParser extends Parser {
	public static final String[] tokenNames = new String[] {
		"<invalid>", "<EOR>", "<DOWN>", "<UP>", "AND", "COMMENT", "GE", "ID", 
		"IMPLIES", "INT", "LE", "NOT", "OR", "WHITESPACE", "'('", "')'", "'*'", 
		"'+'", "'-'", "'='", "'['", "']'"
	};
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
	public Parser[] getDelegates() {
		return new Parser[] {};
	}

	// delegators


	public GuestParser(TokenStream input) {
		this(input, new RecognizerSharedState());
	}
	public GuestParser(TokenStream input, RecognizerSharedState state) {
		super(input, state);
	}

	@Override public String[] getTokenNames() { return GuestParser.tokenNames; }
	@Override public String getGrammarFileName() { return "Guest.g"; }



	// $ANTLR start "start"
	// Guest.g:20:1: start returns [Assertion assertion] : at0= disjunction EOF ;
	public final Assertion start() throws RecognitionException {
		Assertion assertion = null;


		Assertion at0 =null;

		try {
			// Guest.g:21:5: (at0= disjunction EOF )
			// Guest.g:21:7: at0= disjunction EOF
			{
			pushFollow(FOLLOW_disjunction_in_start55);
			at0=disjunction();
			state._fsp--;
			if (state.failed) return assertion;
			match(input,EOF,FOLLOW_EOF_in_start57); if (state.failed) return assertion;
			if ( state.backtracking==0 ) {assertion = at0; }
			}

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
		}
		finally {
			// do for sure before leaving
		}
		return assertion;
	}
	// $ANTLR end "start"



	// $ANTLR start "disjunction"
	// Guest.g:25:1: disjunction returns [Assertion assertion] : at0= conjunction ( OR at1= conjunction )* ;
	public final Assertion disjunction() throws RecognitionException {
		Assertion assertion = null;


		Assertion at0 =null;
		Assertion at1 =null;

		try {
			// Guest.g:26:5: (at0= conjunction ( OR at1= conjunction )* )
			// Guest.g:26:7: at0= conjunction ( OR at1= conjunction )*
			{
			pushFollow(FOLLOW_conjunction_in_disjunction100);
			at0=conjunction();
			state._fsp--;
			if (state.failed) return assertion;
			if ( state.backtracking==0 ) { assertion = at0; }
			// Guest.g:28:9: ( OR at1= conjunction )*
			loop1:
			while (true) {
				int alt1=2;
				int LA1_0 = input.LA(1);
				if ( (LA1_0==OR) ) {
					alt1=1;
				}

				switch (alt1) {
				case 1 :
					// Guest.g:28:11: OR at1= conjunction
					{
					match(input,OR,FOLLOW_OR_in_disjunction123); if (state.failed) return assertion;
					pushFollow(FOLLOW_conjunction_in_disjunction129);
					at1=conjunction();
					state._fsp--;
					if (state.failed) return assertion;
					if ( state.backtracking==0 ) { if (!(assertion instanceof ComposedAssertion &&
					                        ((ComposedAssertion) assertion).isOr())) {
					                    assertion = new ComposedAssertion(ComposedAssertion.Operator.OR);
					                    ((ComposedAssertion) assertion).add(at0);
					                }
					              ((ComposedAssertion) assertion).add(at1); }
					}
					break;

				default :
					break loop1;
				}
			}

			}

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
		}
		finally {
			// do for sure before leaving
		}
		return assertion;
	}
	// $ANTLR end "disjunction"



	// $ANTLR start "conjunction"
	// Guest.g:37:1: conjunction returns [Assertion assertion] : at0= implication ( AND at1= implication )* ;
	public final Assertion conjunction() throws RecognitionException {
		Assertion assertion = null;


		Assertion at0 =null;
		Assertion at1 =null;

		try {
			// Guest.g:38:5: (at0= implication ( AND at1= implication )* )
			// Guest.g:38:7: at0= implication ( AND at1= implication )*
			{
			pushFollow(FOLLOW_implication_in_conjunction171);
			at0=implication();
			state._fsp--;
			if (state.failed) return assertion;
			if ( state.backtracking==0 ) { assertion = at0; }
			// Guest.g:40:9: ( AND at1= implication )*
			loop2:
			while (true) {
				int alt2=2;
				int LA2_0 = input.LA(1);
				if ( (LA2_0==AND) ) {
					alt2=1;
				}

				switch (alt2) {
				case 1 :
					// Guest.g:40:11: AND at1= implication
					{
					match(input,AND,FOLLOW_AND_in_conjunction193); if (state.failed) return assertion;
					pushFollow(FOLLOW_implication_in_conjunction199);
					at1=implication();
					state._fsp--;
					if (state.failed) return assertion;
					if ( state.backtracking==0 ) { if (!(assertion instanceof ComposedAssertion &&
					                        ((ComposedAssertion) assertion).isAnd())) {
					                    assertion = new ComposedAssertion(ComposedAssertion.Operator.AND);
					                    ((ComposedAssertion) assertion).add(at0);
					                }
					              ((ComposedAssertion) assertion).add(at1); }
					}
					break;

				default :
					break loop2;
				}
			}

			}

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
		}
		finally {
			// do for sure before leaving
		}
		return assertion;
	}
	// $ANTLR end "conjunction"



	// $ANTLR start "implication"
	// Guest.g:49:1: implication returns [Assertion assertion] : at0= equivalence ( IMPLIES at1= equivalence )? ;
	public final Assertion implication() throws RecognitionException {
		Assertion assertion = null;


		Assertion at0 =null;
		Assertion at1 =null;

		try {
			// Guest.g:50:5: (at0= equivalence ( IMPLIES at1= equivalence )? )
			// Guest.g:50:7: at0= equivalence ( IMPLIES at1= equivalence )?
			{
			pushFollow(FOLLOW_equivalence_in_implication241);
			at0=equivalence();
			state._fsp--;
			if (state.failed) return assertion;
			if ( state.backtracking==0 ) { assertion = at0; }
			// Guest.g:52:9: ( IMPLIES at1= equivalence )?
			int alt3=2;
			int LA3_0 = input.LA(1);
			if ( (LA3_0==IMPLIES) ) {
				alt3=1;
			}
			switch (alt3) {
				case 1 :
					// Guest.g:52:11: IMPLIES at1= equivalence
					{
					match(input,IMPLIES,FOLLOW_IMPLIES_in_implication263); if (state.failed) return assertion;
					pushFollow(FOLLOW_equivalence_in_implication269);
					at1=equivalence();
					state._fsp--;
					if (state.failed) return assertion;
					if ( state.backtracking==0 ) { assertion = new ComposedAssertion(ComposedAssertion.Operator.IMPLIES);
					              ((ComposedAssertion) assertion).add(at0);   
					              ((ComposedAssertion) assertion).add(at1); }
					}
					break;

			}

			}

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
		}
		finally {
			// do for sure before leaving
		}
		return assertion;
	}
	// $ANTLR end "implication"



	// $ANTLR start "equivalence"
	// Guest.g:58:1: equivalence returns [Assertion assertion] : at0= atomic ( '=' at1= atomic )? ;
	public final Assertion equivalence() throws RecognitionException {
		Assertion assertion = null;


		Assertion at0 =null;
		Assertion at1 =null;

		try {
			// Guest.g:59:5: (at0= atomic ( '=' at1= atomic )? )
			// Guest.g:59:7: at0= atomic ( '=' at1= atomic )?
			{
			pushFollow(FOLLOW_atomic_in_equivalence311);
			at0=atomic();
			state._fsp--;
			if (state.failed) return assertion;
			if ( state.backtracking==0 ) { assertion = at0; }
			// Guest.g:61:9: ( '=' at1= atomic )?
			int alt4=2;
			int LA4_0 = input.LA(1);
			if ( (LA4_0==19) ) {
				alt4=1;
			}
			switch (alt4) {
				case 1 :
					// Guest.g:61:11: '=' at1= atomic
					{
					match(input,19,FOLLOW_19_in_equivalence333); if (state.failed) return assertion;
					pushFollow(FOLLOW_atomic_in_equivalence339);
					at1=atomic();
					state._fsp--;
					if (state.failed) return assertion;
					if ( state.backtracking==0 ) { assertion = new ComposedAssertion(ComposedAssertion.Operator.EQUIV);
					              ((ComposedAssertion) assertion).add(at0);   
					              ((ComposedAssertion) assertion).add(at1); }
					}
					break;

			}

			}

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
		}
		finally {
			// do for sure before leaving
		}
		return assertion;
	}
	// $ANTLR end "equivalence"



	// $ANTLR start "atomic"
	// Guest.g:69:1: atomic returns [Assertion assertion] : ( '[' ba= boolean_assertion ']' | '[' la= linear_assertion ']' | '(' at0= disjunction ')' );
	public final Assertion atomic() throws RecognitionException {
		Assertion assertion = null;


		ParserRuleReturnScope ba =null;
		ParserRuleReturnScope la =null;
		Assertion at0 =null;

		try {
			// Guest.g:70:5: ( '[' ba= boolean_assertion ']' | '[' la= linear_assertion ']' | '(' at0= disjunction ')' )
			int alt5=3;
			int LA5_0 = input.LA(1);
			if ( (LA5_0==20) ) {
				switch ( input.LA(2) ) {
				case NOT:
					{
					alt5=1;
					}
					break;
				case ID:
					{
					int LA5_4 = input.LA(3);
					if ( (LA5_4==AND||LA5_4==OR||LA5_4==21) ) {
						alt5=1;
					}
					else if ( (LA5_4==GE||LA5_4==LE||(LA5_4 >= 17 && LA5_4 <= 19)) ) {
						alt5=2;
					}

					else {
						if (state.backtracking>0) {state.failed=true; return assertion;}
						int nvaeMark = input.mark();
						try {
							for (int nvaeConsume = 0; nvaeConsume < 3 - 1; nvaeConsume++) {
								input.consume();
							}
							NoViableAltException nvae =
								new NoViableAltException("", 5, 4, input);
							throw nvae;
						} finally {
							input.rewind(nvaeMark);
						}
					}

					}
					break;
				case INT:
					{
					alt5=2;
					}
					break;
				default:
					if (state.backtracking>0) {state.failed=true; return assertion;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 5, 1, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}
			}
			else if ( (LA5_0==14) ) {
				alt5=3;
			}

			else {
				if (state.backtracking>0) {state.failed=true; return assertion;}
				NoViableAltException nvae =
					new NoViableAltException("", 5, 0, input);
				throw nvae;
			}

			switch (alt5) {
				case 1 :
					// Guest.g:70:7: '[' ba= boolean_assertion ']'
					{
					match(input,20,FOLLOW_20_in_atomic383); if (state.failed) return assertion;
					pushFollow(FOLLOW_boolean_assertion_in_atomic389);
					ba=boolean_assertion();
					state._fsp--;
					if (state.failed) return assertion;
					match(input,21,FOLLOW_21_in_atomic391); if (state.failed) return assertion;
					if ( state.backtracking==0 ) { assertion = GuestAnalysis.MakeAssertion((ba!=null?((GuestParser.boolean_assertion_return)ba).map:null), (ba!=null?((GuestParser.boolean_assertion_return)ba).operator:null)); }
					}
					break;
				case 2 :
					// Guest.g:72:7: '[' la= linear_assertion ']'
					{
					match(input,20,FOLLOW_20_in_atomic409); if (state.failed) return assertion;
					pushFollow(FOLLOW_linear_assertion_in_atomic415);
					la=linear_assertion();
					state._fsp--;
					if (state.failed) return assertion;
					match(input,21,FOLLOW_21_in_atomic417); if (state.failed) return assertion;
					if ( state.backtracking==0 ) { assertion = GuestAnalysis.MakeAssertion((la!=null?((GuestParser.linear_assertion_return)la).map:null), (la!=null?((GuestParser.linear_assertion_return)la).operator:null), (la!=null?((GuestParser.linear_assertion_return)la).bound:0)); }
					}
					break;
				case 3 :
					// Guest.g:74:7: '(' at0= disjunction ')'
					{
					match(input,14,FOLLOW_14_in_atomic435); if (state.failed) return assertion;
					pushFollow(FOLLOW_disjunction_in_atomic441);
					at0=disjunction();
					state._fsp--;
					if (state.failed) return assertion;
					match(input,15,FOLLOW_15_in_atomic443); if (state.failed) return assertion;
					if ( state.backtracking==0 ) { assertion = at0; }
					}
					break;

			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
		}
		finally {
			// do for sure before leaving
		}
		return assertion;
	}
	// $ANTLR end "atomic"


	public static class boolean_assertion_return extends ParserRuleReturnScope {
		public HashMap<Variable, Boolean> map;
		public String operator;
	};


	// $ANTLR start "boolean_assertion"
	// Guest.g:78:1: boolean_assertion returns [HashMap<Variable, Boolean> map, String operator] : b0= boolean_term (op= ( OR | AND ) b1= boolean_term )* ;
	public final GuestParser.boolean_assertion_return boolean_assertion() throws RecognitionException {
		GuestParser.boolean_assertion_return retval = new GuestParser.boolean_assertion_return();
		retval.start = input.LT(1);

		Token op=null;
		ParserRuleReturnScope b0 =null;
		ParserRuleReturnScope b1 =null;

		try {
			// Guest.g:79:5: (b0= boolean_term (op= ( OR | AND ) b1= boolean_term )* )
			// Guest.g:79:7: b0= boolean_term (op= ( OR | AND ) b1= boolean_term )*
			{
			pushFollow(FOLLOW_boolean_term_in_boolean_assertion478);
			b0=boolean_term();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) { retval.map = new HashMap<Variable,Boolean>();
			          retval.map.put((b0!=null?((GuestParser.boolean_term_return)b0).variable:null), (b0!=null?((GuestParser.boolean_term_return)b0).sign:false));
			          retval.operator = "none"; }
			// Guest.g:83:9: (op= ( OR | AND ) b1= boolean_term )*
			loop6:
			while (true) {
				int alt6=2;
				int LA6_0 = input.LA(1);
				if ( (LA6_0==AND||LA6_0==OR) ) {
					alt6=1;
				}

				switch (alt6) {
				case 1 :
					// Guest.g:83:11: op= ( OR | AND ) b1= boolean_term
					{
					op=input.LT(1);
					if ( input.LA(1)==AND||input.LA(1)==OR ) {
						input.consume();
						state.errorRecovery=false;
						state.failed=false;
					}
					else {
						if (state.backtracking>0) {state.failed=true; return retval;}
						MismatchedSetException mse = new MismatchedSetException(null,input);
						throw mse;
					}
					if ( state.backtracking==0 ) { if ( retval.operator.equals("none")) retval.operator = (op!=null?op.getText():null);
					                  if (!retval.operator.equals((op!=null?op.getText():null)))  
					                      GuestAnalysis.ParseError("unsupported mixed and/or boolean assertions"); }
					pushFollow(FOLLOW_boolean_term_in_boolean_assertion544);
					b1=boolean_term();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) { retval.map.put((b1!=null?((GuestParser.boolean_term_return)b1).variable:null), (b1!=null?((GuestParser.boolean_term_return)b1).sign:false)); }
					}
					break;

				default :
					break loop6;
				}
			}

			}

			retval.stop = input.LT(-1);

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "boolean_assertion"


	public static class boolean_term_return extends ParserRuleReturnScope {
		public Variable variable;
		public boolean sign;
	};


	// $ANTLR start "boolean_term"
	// Guest.g:92:1: boolean_term returns [Variable variable, boolean sign] : ( NOT )? id= ID ;
	public final GuestParser.boolean_term_return boolean_term() throws RecognitionException {
		GuestParser.boolean_term_return retval = new GuestParser.boolean_term_return();
		retval.start = input.LT(1);

		Token id=null;

		try {
			// Guest.g:93:5: ( ( NOT )? id= ID )
			// Guest.g:93:7: ( NOT )? id= ID
			{
			if ( state.backtracking==0 ) { retval.sign = true; }
			// Guest.g:94:9: ( NOT )?
			int alt7=2;
			int LA7_0 = input.LA(1);
			if ( (LA7_0==NOT) ) {
				alt7=1;
			}
			switch (alt7) {
				case 1 :
					// Guest.g:94:11: NOT
					{
					match(input,NOT,FOLLOW_NOT_in_boolean_term599); if (state.failed) return retval;
					if ( state.backtracking==0 ) { retval.sign = false; }
					}
					break;

			}

			id=(Token)match(input,ID,FOLLOW_ID_in_boolean_term618); if (state.failed) return retval;
			if ( state.backtracking==0 ) { retval.variable = GuestAnalysis.LookupVariable((id!=null?id.getText():null)); }
			}

			retval.stop = input.LT(-1);

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "boolean_term"


	public static class linear_assertion_return extends ParserRuleReturnScope {
		public HashMap<Variable, Integer> map;
		public String operator;
		public int bound;
	};


	// $ANTLR start "linear_assertion"
	// Guest.g:98:1: linear_assertion returns [HashMap<Variable, Integer> map, String operator, int bound] : t0= linear_term (s1= ( '+' | '-' ) t1= linear_term )* op= ( LE | '=' | GE ) b= INT (s2= ( '+' | '-' ) t2= linear_term )* ;
	public final GuestParser.linear_assertion_return linear_assertion() throws RecognitionException {
		GuestParser.linear_assertion_return retval = new GuestParser.linear_assertion_return();
		retval.start = input.LT(1);

		Token s1=null;
		Token op=null;
		Token b=null;
		Token s2=null;
		ParserRuleReturnScope t0 =null;
		ParserRuleReturnScope t1 =null;
		ParserRuleReturnScope t2 =null;

		try {
			// Guest.g:99:5: (t0= linear_term (s1= ( '+' | '-' ) t1= linear_term )* op= ( LE | '=' | GE ) b= INT (s2= ( '+' | '-' ) t2= linear_term )* )
			// Guest.g:99:7: t0= linear_term (s1= ( '+' | '-' ) t1= linear_term )* op= ( LE | '=' | GE ) b= INT (s2= ( '+' | '-' ) t2= linear_term )*
			{
			pushFollow(FOLLOW_linear_term_in_linear_assertion645);
			t0=linear_term();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) { retval.map = new HashMap<Variable, Integer>();
			          retval.map.put((t0!=null?((GuestParser.linear_term_return)t0).variable:null), (t0!=null?((GuestParser.linear_term_return)t0).coefficient:0)); }
			// Guest.g:102:9: (s1= ( '+' | '-' ) t1= linear_term )*
			loop8:
			while (true) {
				int alt8=2;
				int LA8_0 = input.LA(1);
				if ( ((LA8_0 >= 17 && LA8_0 <= 18)) ) {
					alt8=1;
				}

				switch (alt8) {
				case 1 :
					// Guest.g:102:11: s1= ( '+' | '-' ) t1= linear_term
					{
					s1=input.LT(1);
					if ( (input.LA(1) >= 17 && input.LA(1) <= 18) ) {
						input.consume();
						state.errorRecovery=false;
						state.failed=false;
					}
					else {
						if (state.backtracking>0) {state.failed=true; return retval;}
						MismatchedSetException mse = new MismatchedSetException(null,input);
						throw mse;
					}
					pushFollow(FOLLOW_linear_term_in_linear_assertion681);
					t1=linear_term();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) { int coeff = (t1!=null?((GuestParser.linear_term_return)t1).coefficient:0);
					              if ((s1!=null?s1.getText():null).equals("-")) coeff *= -1;
					              retval.map.put((t1!=null?((GuestParser.linear_term_return)t1).variable:null), coeff); }
					}
					break;

				default :
					break loop8;
				}
			}

			op=input.LT(1);
			if ( input.LA(1)==GE||input.LA(1)==LE||input.LA(1)==19 ) {
				input.consume();
				state.errorRecovery=false;
				state.failed=false;
			}
			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				MismatchedSetException mse = new MismatchedSetException(null,input);
				throw mse;
			}
			if ( state.backtracking==0 ) { retval.operator = (op!=null?op.getText():null); }
			b=(Token)match(input,INT,FOLLOW_INT_in_linear_assertion742); if (state.failed) return retval;
			if ( state.backtracking==0 ) { retval.bound = Integer.parseInt((b!=null?b.getText():null)); }
			// Guest.g:110:9: (s2= ( '+' | '-' ) t2= linear_term )*
			loop9:
			while (true) {
				int alt9=2;
				int LA9_0 = input.LA(1);
				if ( ((LA9_0 >= 17 && LA9_0 <= 18)) ) {
					alt9=1;
				}

				switch (alt9) {
				case 1 :
					// Guest.g:110:11: s2= ( '+' | '-' ) t2= linear_term
					{
					s2=input.LT(1);
					if ( (input.LA(1) >= 17 && input.LA(1) <= 18) ) {
						input.consume();
						state.errorRecovery=false;
						state.failed=false;
					}
					else {
						if (state.backtracking>0) {state.failed=true; return retval;}
						MismatchedSetException mse = new MismatchedSetException(null,input);
						throw mse;
					}
					pushFollow(FOLLOW_linear_term_in_linear_assertion778);
					t2=linear_term();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) { int coeff = (t2!=null?((GuestParser.linear_term_return)t2).coefficient:0);
					              if ((s2!=null?s2.getText():null).equals("+")) coeff *= -1;
					              retval.map.put((t2!=null?((GuestParser.linear_term_return)t2).variable:null), coeff); }
					}
					break;

				default :
					break loop9;
				}
			}

			}

			retval.stop = input.LT(-1);

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "linear_assertion"


	public static class linear_term_return extends ParserRuleReturnScope {
		public Variable variable;
		public int coefficient;
	};


	// $ANTLR start "linear_term"
	// Guest.g:116:1: linear_term returns [Variable variable, int coefficient] : (intc= INT '*' )? id= ID ;
	public final GuestParser.linear_term_return linear_term() throws RecognitionException {
		GuestParser.linear_term_return retval = new GuestParser.linear_term_return();
		retval.start = input.LT(1);

		Token intc=null;
		Token id=null;

		try {
			// Guest.g:117:5: ( (intc= INT '*' )? id= ID )
			// Guest.g:117:7: (intc= INT '*' )? id= ID
			{
			if ( state.backtracking==0 ) { retval.coefficient = 1; }
			// Guest.g:118:9: (intc= INT '*' )?
			int alt10=2;
			int LA10_0 = input.LA(1);
			if ( (LA10_0==INT) ) {
				alt10=1;
			}
			switch (alt10) {
				case 1 :
					// Guest.g:118:11: intc= INT '*'
					{
					intc=(Token)match(input,INT,FOLLOW_INT_in_linear_term832); if (state.failed) return retval;
					match(input,16,FOLLOW_16_in_linear_term834); if (state.failed) return retval;
					if ( state.backtracking==0 ) { retval.coefficient = Integer.parseInt((intc!=null?intc.getText():null)); }
					}
					break;

			}

			id=(Token)match(input,ID,FOLLOW_ID_in_linear_term853); if (state.failed) return retval;
			if ( state.backtracking==0 ) { retval.variable = GuestAnalysis.LookupVariable((id!=null?id.getText():null)); }
			}

			retval.stop = input.LT(-1);

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "linear_term"

	// Delegated rules



	public static final BitSet FOLLOW_disjunction_in_start55 = new BitSet(new long[]{0x0000000000000000L});
	public static final BitSet FOLLOW_EOF_in_start57 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_conjunction_in_disjunction100 = new BitSet(new long[]{0x0000000000001002L});
	public static final BitSet FOLLOW_OR_in_disjunction123 = new BitSet(new long[]{0x0000000000104000L});
	public static final BitSet FOLLOW_conjunction_in_disjunction129 = new BitSet(new long[]{0x0000000000001002L});
	public static final BitSet FOLLOW_implication_in_conjunction171 = new BitSet(new long[]{0x0000000000000012L});
	public static final BitSet FOLLOW_AND_in_conjunction193 = new BitSet(new long[]{0x0000000000104000L});
	public static final BitSet FOLLOW_implication_in_conjunction199 = new BitSet(new long[]{0x0000000000000012L});
	public static final BitSet FOLLOW_equivalence_in_implication241 = new BitSet(new long[]{0x0000000000000102L});
	public static final BitSet FOLLOW_IMPLIES_in_implication263 = new BitSet(new long[]{0x0000000000104000L});
	public static final BitSet FOLLOW_equivalence_in_implication269 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_atomic_in_equivalence311 = new BitSet(new long[]{0x0000000000080002L});
	public static final BitSet FOLLOW_19_in_equivalence333 = new BitSet(new long[]{0x0000000000104000L});
	public static final BitSet FOLLOW_atomic_in_equivalence339 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_20_in_atomic383 = new BitSet(new long[]{0x0000000000000880L});
	public static final BitSet FOLLOW_boolean_assertion_in_atomic389 = new BitSet(new long[]{0x0000000000200000L});
	public static final BitSet FOLLOW_21_in_atomic391 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_20_in_atomic409 = new BitSet(new long[]{0x0000000000000280L});
	public static final BitSet FOLLOW_linear_assertion_in_atomic415 = new BitSet(new long[]{0x0000000000200000L});
	public static final BitSet FOLLOW_21_in_atomic417 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_14_in_atomic435 = new BitSet(new long[]{0x0000000000104000L});
	public static final BitSet FOLLOW_disjunction_in_atomic441 = new BitSet(new long[]{0x0000000000008000L});
	public static final BitSet FOLLOW_15_in_atomic443 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_boolean_term_in_boolean_assertion478 = new BitSet(new long[]{0x0000000000001012L});
	public static final BitSet FOLLOW_set_in_boolean_assertion504 = new BitSet(new long[]{0x0000000000000880L});
	public static final BitSet FOLLOW_boolean_term_in_boolean_assertion544 = new BitSet(new long[]{0x0000000000001012L});
	public static final BitSet FOLLOW_NOT_in_boolean_term599 = new BitSet(new long[]{0x0000000000000080L});
	public static final BitSet FOLLOW_ID_in_boolean_term618 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_linear_term_in_linear_assertion645 = new BitSet(new long[]{0x00000000000E0440L});
	public static final BitSet FOLLOW_set_in_linear_assertion671 = new BitSet(new long[]{0x0000000000000280L});
	public static final BitSet FOLLOW_linear_term_in_linear_assertion681 = new BitSet(new long[]{0x00000000000E0440L});
	public static final BitSet FOLLOW_set_in_linear_assertion712 = new BitSet(new long[]{0x0000000000000200L});
	public static final BitSet FOLLOW_INT_in_linear_assertion742 = new BitSet(new long[]{0x0000000000060002L});
	public static final BitSet FOLLOW_set_in_linear_assertion768 = new BitSet(new long[]{0x0000000000000280L});
	public static final BitSet FOLLOW_linear_term_in_linear_assertion778 = new BitSet(new long[]{0x0000000000060002L});
	public static final BitSet FOLLOW_INT_in_linear_term832 = new BitSet(new long[]{0x0000000000010000L});
	public static final BitSet FOLLOW_16_in_linear_term834 = new BitSet(new long[]{0x0000000000000080L});
	public static final BitSet FOLLOW_ID_in_linear_term853 = new BitSet(new long[]{0x0000000000000002L});
}
