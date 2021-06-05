% Test with iterations

%@exists
b :- a.
e | f.
%@exists
h :- b.
a | d.
%@constraint
:- b, h.

