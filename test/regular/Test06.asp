% Test with iterations

%@exists
b :- a.
e | f.
%@exists
h :- b.
a.
%@constraint
:- b, h.

