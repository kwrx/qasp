%@exists
a | b.                      % P1 = {a} {b}
%@forall
c | d :- a.                 % P2 U {a} = {a,c} {a,d}        -> {a,c} U C -> INCOHERENT      |   Non tutti gli AS(P2) U {a} sono coerenti con un AS(P1)
d :- b.                     %                               -> {a,d} U C -> COHERENT        |   
                            % P2 U {b} = {b,d}              -> {b,d} U C -> COHERENT        |   Un AS(P1) = {b} è coerente con tutti gli AS(P2) U {b}
%@exists
e | f | g :- b, d.          % P3 U {b,d} = {b,d,e} {b,d,f}  -> {b,d,e} U C -> INCOHERENT    | 
                            %              {b,d,g}          -> {b,d,f} U C -> COHERENT      |   Tutti gli AS(P2) sono coerenti con almeno un AS(P3) U {b,d}
                            %                               -> {b,d,g} U C -> COHERENT      |   Tutti gli AS(P2) sono coerenti con almeno un AS(P3) U {b,d}
%@forall
:- f.                       % P4 U {b,d,f} = INCOHERENT     -> {b,d,f} U C -> INCOHERENT    | 
                            % P4 U {b,d,g} = {b,d,g}        -> {b,d,g} U C -> COHERENT      |   Almeno un AS(P3) è coerente con tutti gli AS(P4) U {b,d,g}
%@constraints
{c;d;e}.
:- c, not d.                % C
:- e.

% Result: {b,d,g} {b,d,f} are COHERENT.