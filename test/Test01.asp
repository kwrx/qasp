c(1..5).
d(1..5).

%@exists
c(6).
in_a(X) | out_a(X) :- b(X), c(X).
b(X) :- d(X).
%@forall
d(X) :- c(X), in_a(X).
{ in_a(X) : ... }

%@constraints
:- in_a(X), X > 3.

% SE esiste un modello @exists tale per cui in tutti i modelli di forall risulta valido applicando il constraint.