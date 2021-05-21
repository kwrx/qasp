%@exists
a | na.
b | nb.
%@forall
c | nc.
d | nd.
e | ne.
%@constraints
:- c, not e.
:- d, not e.
:- not c, not d, e.