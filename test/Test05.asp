% https://en.wikipedia.org/wiki/Boolean_satisfiability_problem
% ∀x ∀y ∀z (x ∨ y ∨ z) ∧ (¬x ∨ ¬y ∨ ¬z)

%@forall
x | nx.
%@forall
y | ny.
%@forall
z | nz.
%@constraints
{x;y;z;nx;ny;nz}.
:- nx, ny, nz.
:- x, y, z.


%% Result: INCOHERENT