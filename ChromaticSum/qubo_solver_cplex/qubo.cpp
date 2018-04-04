// qubo solver takes a matrix with size n and k as input and generates a optimal solution
// Usage:  $./qubo < matrix.txt
#include <ilcplex/ilocplex.h>
ILOSTLBEGIN
void read_qubo(int &n, double **&Q);
int main (int argc, char **argv)
{
    int n=0, k=0;
    double **Q;
    cin >> n >>k;
    read_qubo(n,Q);

    IloEnv   env;
    try {
        IloModel model(env);
        IloNumVarArray var(env);
        for (int i = 0; i < n; ++i) var.add(IloNumVar(env,0,1,ILOBOOL));
        IloExpr expr(env);
        for (int i=0; i<n; i++)
            for (int j=0; j< n; j++)
                expr += var[i]*var[j]*Q[i][j];
        IloObjective obj = IloMinimize(env,expr);
        model.add(obj);
        IloCplex cplex(model);
        cplex.setOut(env.getNullStream());

        // Optimize the problem and obtain solution.
        if ( !cplex.solve() ) {
            env.error() << "Failed to optimize LP" << endl;
            throw(-1);
        }

        IloNumArray vals(env);
        cplex.getValues(vals, var);
        // obtain sum of colorings
        int sl = 0;
        for (int i = 0; i < n/k; i++)
          for (int j = 0; j < k; j++) sl = sl + (int)vals[i*k+j] * (j+1);
        cout <<"('n=', "<<n <<", ' k=', "<<k<<")"<< endl;
        cout<<"newresult: [[";
        for (int i = 0; i < n; i++)
        {
          if(i+1!=n) cout <<(int)vals[i] <<", ";
          else cout <<(int)vals[i] <<"]]"<<endl;;
        }
    }
    catch (IloException& e) {
        cerr << "Concert exception caught: " << e << endl;
    }
    catch (...) {
        cerr << "Unknown exception caught" << endl;
    }

    env.end();
    for (int i=0; i<n; i++)
        delete [] Q[i];
    delete [] Q;
    return 0;
}  // END main

void read_qubo(int &n, double **&Q) {
  // Start of: initializing Q
  Q = new double*[n];
  for (int i=0; i<n; i++)
  {
      Q[i] = new double[n];
      for (int j=0; j<n; j++)
         Q[i][j] = 0;
  }
  for (int i=0; i<n; i++)
  {
    for(int j=0;j<n;j++)
          cin >> Q[i][j];
  }
}
