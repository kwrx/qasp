%Run with options: A=2 a=1 c=1 E=2 e=1 w=2 out=PrintAspQ 
%@forall
x_1 | nx_1.
x_2 | nx_2.
%@exists
x_3 | nx_3.
x_4 | nx_4.
x_5 | nx_5.
x_6 | nx_6.
x_7 | nx_7.
%@constraint
:- not x_3, not x_4, x_5.
:- x_3, not x_5.
:- x_4, not x_5.
:- not x_5.
