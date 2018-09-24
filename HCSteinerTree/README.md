# Bounded-Depth Steiner Tree problem (or Hop-Constraint Steiner Tree)
Starting from a IP (integer programming) formulation, we develop a D-Wave feasible QUBO (quadratic unconstrained binary optimisation) formulation of the bounded-depth Steiner tree problem. Our construction requires *h\*n^2* qubits for a graph of *n* vertices and depth constraint *h*.
- **lp_DCSTP** - A solver using Integer Programming model
- **qubo_formula** - QUBO formulation implementation. It takes an adjacent list input and output a QUBO matrix which can later be solved by a QUBO solver.  