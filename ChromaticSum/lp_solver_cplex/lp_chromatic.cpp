// BIP solver for Chromatic Sum problem
// Usage: $./CSP_BIP_Solver
#include <ilcplex/ilocplex.h>
#include <iostream>
#include <fstream>
#include <list>
#include <sstream>
ILOSTLBEGIN
void read_graph(const int n, list <pair<int,int> > &adjacent_list);
int main (int argc, char **argv)
{
    int n=0,k=0;
    list <pair<int,int> > adjacent_list;
    cin>>n;
    if (argc == 2) k = (int)strtol (argv[1],NULL,10);
    else k=n;
    read_graph(n,adjacent_list);
    const int N = n*k;
    IloEnv   env;
    try {
        IloModel model(env);
        IloNumVarArray var(env);
        IloRangeArray con(env);

        for (int i = 0; i < N; ++i)
        {
            char buffer [10];
            sprintf (buffer, "x%d", i+1);
            var.add(IloNumVar(env,0,1,ILOBOOL,buffer));
        }

        IloExpr expr(env);
        //f(x) = sum j*x[i,j]
        for (int i=0; i<n; i++)
            for (int j=0; j<k; j++)
            {
                int idx = i*k+j;
                expr += var[idx]*(j+1);
            }
        IloObjective obj = IloMinimize(env,expr);

        IloExprArray expr_constrs = IloExprArray(env,n);
        for (int i=0; i<n; i++){
            expr_constrs[i] = IloExpr(env);
            for (int j=0; j<k; j++){
                int idx = i*k+j;
                expr_constrs[i] += var[idx];
            }
            char buffer [10];
            sprintf (buffer, "c%d", i+1);
            con.add(IloRange(env, 1,expr_constrs[i], 1,buffer));
        }

        //p2(x) = sum(E(G))sum(j) x[u,j]+x[v,j]<=1
        list<pair<int,int> >::const_iterator iterator;

        for (iterator = adjacent_list.begin(); iterator != adjacent_list.end(); ++iterator) {
            int u = (*iterator).first;
            int v = (*iterator).second;
            for(int j=0; j<k; j++) con.add(var[u*k+j]+var[v*k+j]<=1);

        }

        model.add(obj);
        model.add(con);
        IloCplex cplex(model);
        cplex.exportModel("lpe.lp");
        cplex.setOut(env.getNullStream());
        // Optimize the problem and obtain solution.
        if ( !cplex.solve() ) {
            env.error() << "Failed to optimize LP" << endl;
            throw(-1);
        }

        IloNumArray vals(env);
        //env.out() << "Solution status = " << cplex.getStatus() << endl;
        //env.out() << "Solution value  = " << cplex.getObjValue() << endl;
        //cplex.getValues(vals, var);
        //env.out() << "Values = " << vals << endl;
        cout<<cplex.getObjValue();
    }
    catch (IloException& e) {
        cerr << "Concert exception caught: " << e << endl;
    }
    catch (...) {
        cerr << "Unknown exception caught" << endl;
    }
    env.end();
    return 0;
}  // END main

void read_graph(const int n, list <pair<int,int> > &adjacent_list)
{
  string line;
  int lineCnt=-1;
  for(int i=0;i<n+1;i++)
  {
    std::getline(cin, line);
    istringstream iss(line);
    int a;
    while (iss >> a)adjacent_list.push_back(make_pair(lineCnt,a));
    lineCnt++;
  }
}
