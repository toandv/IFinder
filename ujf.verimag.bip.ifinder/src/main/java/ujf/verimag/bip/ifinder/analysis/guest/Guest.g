grammar Guest;

options {
    backtrack=true;
}

@parser::header {
package ujf.verimag.bip.ifinder.analysis.guest;

import ujf.verimag.bip.ifinder.invariant.*;
import ujf.verimag.bip.ifinder.analysis.GuestAnalysis;
}

@lexer::header{
package ujf.verimag.bip.ifinder.analysis.guest;
}

// parser

start returns [Assertion assertion]
    : at0 = disjunction EOF
        {$assertion = $at0.assertion; }
    ;
        
disjunction returns [Assertion assertion]
    : at0 = conjunction 
        { $assertion = $at0.assertion; }
        ( OR at1 = conjunction
            { if (!($assertion instanceof ComposedAssertion &&
                        ((ComposedAssertion) $assertion).isOr())) {
                    $assertion = new ComposedAssertion(ComposedAssertion.Operator.OR);
                    ((ComposedAssertion) $assertion).add($at0.assertion);
                }
              ((ComposedAssertion) $assertion).add($at1.assertion); } )*
    ;

conjunction returns [Assertion assertion]
    : at0 = implication
        { $assertion = $at0.assertion; }
        ( AND at1 = implication
            { if (!($assertion instanceof ComposedAssertion &&
                        ((ComposedAssertion) $assertion).isAnd())) {
                    $assertion = new ComposedAssertion(ComposedAssertion.Operator.AND);
                    ((ComposedAssertion) $assertion).add($at0.assertion);
                }
              ((ComposedAssertion) $assertion).add($at1.assertion); } )*
    ;

implication returns [Assertion assertion]
    : at0 = equivalence
        { $assertion = $at0.assertion; }
        ( IMPLIES at1 = equivalence
            { $assertion = new ComposedAssertion(ComposedAssertion.Operator.IMPLIES);
              ((ComposedAssertion) $assertion).add($at0.assertion);   
              ((ComposedAssertion) $assertion).add($at1.assertion); } )?
    ;

equivalence returns [Assertion assertion]
    : at0 = atomic
        { $assertion = $at0.assertion; }
        ( '=' at1 = atomic
            { $assertion = new ComposedAssertion(ComposedAssertion.Operator.EQUIV);
              ((ComposedAssertion) $assertion).add($at0.assertion);   
              ((ComposedAssertion) $assertion).add($at1.assertion); } )?
    ;
    
// atomic assertions - enforce paranthesis to avoid conflicts...

atomic returns [Assertion assertion]
    : '[' ba = boolean_assertion ']'
        { $assertion = GuestAnalysis.MakeAssertion($ba.map, $ba.operator); }
    | '[' la = linear_assertion ']'
        { $assertion = GuestAnalysis.MakeAssertion($la.map, $la.operator, $la.bound); }
    | '(' at0 = disjunction ')'
        { $assertion = $at0.assertion; }
    ;

boolean_assertion returns [HashMap<Variable, Boolean> map, String operator]
    : b0 = boolean_term
        { $map = new HashMap<Variable,Boolean>();
          $map.put($b0.variable, $b0.sign);
          $operator = "none"; }
        ( op = (OR|AND)
                { if ( $operator.equals("none")) $operator = $op.text;
                  if (!$operator.equals($op.text))  
                      GuestAnalysis.ParseError("unsupported mixed and/or boolean assertions"); }
            b1 = boolean_term
            { $map.put($b1.variable, $b1.sign); } )*
    ;
    

boolean_term returns [Variable variable, boolean sign]
    : { $sign = true; }
        ( NOT { $sign = false; } )?
        id = ID { $variable = GuestAnalysis.LookupVariable($id.text); }
    ;

linear_assertion returns [HashMap<Variable, Integer> map, String operator, int bound]
    : t0 = linear_term
        { $map = new HashMap<Variable, Integer>();
          $map.put($t0.variable, $t0.coefficient); }
        ( s1 = ('+'|'-') t1 = linear_term
            { int coeff = $t1.coefficient;
              if ($s1.text.equals("-")) coeff *= -1;
              $map.put($t1.variable, coeff); } )*
        op = (LE|'='|GE)
        { $operator = $op.text; }
        b = INT
        { $bound = Integer.parseInt($b.text); }
        ( s2 = ('+'|'-') t2 = linear_term
            { int coeff = $t2.coefficient;
              if ($s2.text.equals("+")) coeff *= -1;
              $map.put($t2.variable, coeff); } )*
    ;

linear_term returns [Variable variable, int coefficient]
    : { $coefficient = 1; }
        ( intc = INT '*' { $coefficient = Integer.parseInt($intc.text); } )?
        id = ID { $variable = GuestAnalysis.LookupVariable($id.text); }
    ;



// lexer

AND            : 'and'  ;
OR             : 'or'   ;
NOT            : 'not'  ;

ID             : ('a'..'z'|'A'..'Z') ('a'..'z'|'A'..'Z'|'0'..'9'|'_'|'.')* ;

INT            : ('0'..'9')+ ;

LE             : '<='   ;
GE             : '>='   ;

IMPLIES        : '=>'   ;

COMMENT        : '#' ~('\r'|'\n')* ('\r')? '\n' { $channel=HIDDEN; }   ;

WHITESPACE     : (' '|'\r'|'\t'|'\n')+          { $channel=HIDDEN; }   ;

