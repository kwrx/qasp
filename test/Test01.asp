c(1..5).
d(1..5).

%@exists
c(6).
in_a(X) | out_a(X) :- b(X), c(X).
b(X) :- d(X).
%@foreach
d(X) :- c(X).
%@constraints
:- in_a(X), X > 3.