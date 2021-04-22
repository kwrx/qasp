%@exists
a.
%@forall
b.
%@exists
c.
%@constraints
:-     b, not c,     a.
:- not b,     c,     a.
:-     b,     c, not a.
:-     b, not c, not a.
