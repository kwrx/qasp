%@exists
a | b.                      % P1 = {a} {b}
%@forall
c | d :- a.                 % P2 U {a} = {a,c} {a,d}        -> {a,c} U C -> INCOHERENT  |   Non tutti gli AS(P2) U {a} sono coerenti con un AS(P1)
d :- b.                     %                               -> {a,d} U C -> COHERENT    |   
                            % P2 U {b} = {b,d}              -> {b,d} U C -> COHERENT    |   Un AS(P1) = {b} è coerente con tutti gli AS(P2) U {b}
%@constraints
:- c, not d.                % C

% Result: { a(2), b(2) } is COHERENT.

% SE esiste un modello @exists tale per cui in tutti i modelli di @forall risulta valido applicando il constraint.