% https://en.wikipedia.org/wiki/Boolean_satisfiability_problem
% ∀x ∀y ∃z (x ∨ y ∨ z) ∧ (¬x ∨ ¬y ∨ ¬z)

%@forall
x | nx.
%@forall
y | ny.
%@exists
z | nz.
%@constraints
{x;y;z;nx;ny;nz}.
:- nx, ny, nz.
:- x, y, z.


%% Result: COHERENT