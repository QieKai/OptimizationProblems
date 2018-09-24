# Finding the Chromatic Sums of Graphs 
Starting from a BIP (binary integer programming) formulation, we develop a D-Wave feasible QUBO (quadratic unconstrained binary optimisation) formulation of the chromatic sum problem. Our construction requires *nk* qubits for a graph of *n* vertices and upper bound of *k* colours.
- **lp_solver_cplex** - A solver using Binary Integer Programming model
- **chromatic_sum_qubo.cpp** - QUBO formulation implementation. It takes an adjacent list input and output a QUBO matrix which can later be solved by a QUBO solver.  
- **qubo_solver_cplex** - A QUBO solver that takes a QUBO size and QUBO matrix as input.  