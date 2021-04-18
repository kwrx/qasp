c(1..5).
d(1..5).

%@exists
a(X) :- b(X), c(X).
b(X) :- d(X).
%@foreach
d(X) :- c(X).
%@constraints
:- c(X), X > 3.