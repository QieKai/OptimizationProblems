# Degree-Constrained Minimum Spanning Tree

- **lp_DCMST** - A solver using Integer Programming model
- **qubo_formula** - A implementation of Alex's QUBO formulation in his work
[Improved QUBO Formulations for D-Wave Quantum Computing](https://www.google.co.nz/url?sa=t&rct=j&q=&esrc=s&source=web&cd=1&cad=rja&uact=8&ved=2ahUKEwi38eichMvdAhWMWrwKHZKCBSoQFjAAegQIAhAB&url=https%3A%2F%2Fwww.researchgate.net%2Fpublication%2F322634540_Improved_QUBO_Formulations_for_D-Wave_Quantum_Computing&usg=AOvVaw0VsMUbJ2OT07Kvt6YyC94Q).
It takes an adjacent list of graph as input and output a QUBO matrix which can later be solved by a QUBO solver.
- **post_proc** - take the output of QUBO solver and transfer it into a solution to the original problem.