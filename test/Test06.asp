% Test with iterations

%@exists
b :- a.
e | f.
%@exists
h :- b.
a.
%@constraints
{b;h}.
:- b, h.

