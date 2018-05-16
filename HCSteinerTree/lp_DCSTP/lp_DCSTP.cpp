#include <ilcplex/ilocplex.h>
#include <iostream>
#include <fstream>
#include <list>
#include <sstream>
#include <map>
#include <vector>
//#define DEBUG
ILOSTLBEGIN
using namespace std;

bool find_all_sol=false;
void read_graph(int n, vector <pair<int,int> > &adjacent_list, vector <int> &setU, map<pair<int,int>,int> &weight);

int main (int argc, char **argv)
{
    int node_size=0, hop_constraint=0;
    vector <pair<int,int> > adjacent_list;
    map<pair<int,int>,int> weight;
    vector <int> setU;

    if (argc < 2) cout << "Correct usage: " << argv[0] <<" <Hop constraint>" << endl;
    else if(argc == 3){
        string param1 = string(argv[1]);
        string param2 = string(argv[2]);
        if(param1 == "-a")hop_constraint = (int)strtol (argv[2], nullptr,10);
        else if (param2 == "-a")hop_constraint = (int)strtol (argv[1], nullptr,10);
        find_all_sol = true;
    }
    else hop_constraint = (int)strtol (argv[1], nullptr,10);

    cin>>node_size;

    read_graph(node_size,adjacent_list,setU,weight);
    int nominated_root = setU[0];

    int cnt=0;

    map<pair<int,int>,int> edge2matrix;

    for (vector<pair<int,int> >::const_iterator iterator = adjacent_list.begin();
         iterator != adjacent_list.end(); ++iterator) {
        if ((*iterator).second == nominated_root) continue;
        edge2matrix[*iterator] = cnt;
        cnt++;
    }

#ifdef DEBUG
    for(std::map<pair<int,int>,int>::iterator iti=edge2matrix.begin(); iti!=edge2matrix.end(); ++iti)
    {
        cout<<iti->first.first+1<<" "<<iti->first.second+1<<" "<<iti->second<<endl;
    }
#endif

    IloEnv   env;
    try {
        IloModel model(env);

        IloNumVarArray var(env);
        IloRangeArray con(env);

        //e_{u,v} variables
        for(std::map<pair<int,int>,int>::iterator iti=edge2matrix.begin(); iti!=edge2matrix.end(); ++iti)
        {
            char buffer [10];
            sprintf (buffer, "e_%d,%d", iti->first.first+1, iti->first.second+1);
            var.add(IloNumVar(env,0,1,ILOINT,buffer));
        }

        // d_v variables
        for(int i=0; i < node_size; i++){
            char buffer [10];
            sprintf (buffer, "d_%d", i+1);
            if (i == nominated_root){ // d_0 = 0
                var.add(IloNumVar(env,0,0,ILOINT, buffer));
            }
            else if(find(setU.begin(), setU.end(), i) != setU.end()){  // d_v >= 0 where v \in U
                var.add(IloNumVar(env,1,hop_constraint,ILOINT, buffer));
            }
            else // d_v >= -1
            {
                var.add(IloNumVar(env,-1,hop_constraint,ILOINT, buffer));
            }
        }

        IloExpr expr(env);

        // objective function  F((e,d)=∑ c_{u,v} e_{u,v}
        for(std::map<pair<int,int>,int>::iterator it=edge2matrix.begin(); it!=edge2matrix.end(); ++it)
        {
            int u = it->first.first;
            int v = it->first.second;
            expr += weight[make_pair(u,v)]*var[it->second];
        }

        IloObjective obj = IloMinimize(env,expr);

        IloExprArray expr_constrs2 = IloExprArray(env,node_size-1);
        IloExprArray expr_constrs3 = IloExprArray(env,node_size-1);
        IloExprArray expr_constrs4 = IloExprArray(env,node_size-1);

        int i = 0;
        for(int v=0; v<node_size; v++) {
            if(v==nominated_root) continue;
            expr_constrs2[i] = IloExpr(env);
            // get ∑e_{u,v}
            for(std::map<pair<int,int>,int>::iterator iti=edge2matrix.begin(); iti!=edge2matrix.end(); ++iti)
            {
                if (iti->first.second == v) {
                    expr_constrs2[i] += var[iti->second];
                }
            }

            // ∑e_{u,v} <= 1
            char cont_name2 [10];
            sprintf (cont_name2, "c2_%d", i+1);
            con.add(IloRange(env, 0,expr_constrs2[i], 1,cont_name2));

//            // (H+1) * ∑e_{u,v} >= d_v+1
            char cont_name3[10];
            sprintf (cont_name3, "c3_%d", i+1);
            expr_constrs3[i] = expr_constrs2[i]*(hop_constraint+1)-var[edge2matrix.size()+v]-1;
            con.add(IloRange(env, 0,expr_constrs3[i],IloInfinity ,cont_name3));

            // 2* ∑e_{u,v} >= d_v + 1
            char cont_name4[10];
            sprintf (cont_name4, "c4_%d", i+1);
            expr_constrs4[i] = var[edge2matrix.size()+v]+1 - 2 *expr_constrs2[i];
            con.add(IloRange(env, 0,expr_constrs4[i],IloInfinity ,cont_name4));
            
            i++;
        }
         // 1 - H*(1-e_{u,v}) <=  d_v - d_u <= 1 + H*(1-e_{u,v})
        IloExprArray expr_constrs5_1 = IloExprArray(env,edge2matrix.size());
        IloExprArray expr_constrs5_2 = IloExprArray(env,edge2matrix.size());
        i=0;
        for(std::map<pair<int,int>,int>::iterator iti=edge2matrix.begin(); iti!=edge2matrix.end(); ++iti)
        {
            int u = iti->first.first;
            int v = iti->first.second;

            expr_constrs5_1[i] = var[edge2matrix.size()+v] - var[edge2matrix.size()+u] + (hop_constraint+2)*(1-var[iti->second]);
            char cont_name5_1[10];
            sprintf (cont_name5_1, "c51_%d", i+1);
            con.add(IloRange(env, 1,expr_constrs5_1[i],IloInfinity ,cont_name5_1));

            expr_constrs5_2[i] = var[edge2matrix.size()+u] - var[edge2matrix.size()+v] + hop_constraint*(1-var[iti->second]) + 1;
            char cont_name5_2[10];
            sprintf (cont_name5_2, "c52_%d", i+1);
            con.add(IloRange(env, 0,expr_constrs5_2[i],IloInfinity ,cont_name5_2));
            i++;
        }

//        IloExprArray expr_constrs8 = IloExprArray(env,node_size);
//
//        for(int v=0; v<node_size; v++) {
//            expr_constrs8[v] = IloExpr(env);
//            for(std::map<pair<int,int>,int>::iterator iti=edge2matrix.begin(); iti!=edge2matrix.end(); ++iti)
//            {
//                if ((*iti).first.second == v || (*iti).first.first == v) {
//                    expr_constrs8[v] += var[(*iti).second];
//                }
//            }
//            char buffer [10];
//            sprintf (buffer, "c8_%d", v+1);
//            con.add(IloRange(env, 1, expr_constrs8[v], hop_constraint, buffer));
//        }

        model.add(obj);
        model.add(con);
        IloCplex cplex(model);
        cplex.exportModel("lpe.lp");

        //do not display any cplex output on the screen
        cplex.setOut(env.getNullStream());
        cplex.setWarning(env.getNullStream());
        cplex.setError(env.getNullStream());

        if(find_all_sol){
            cplex.setParam(IloCplex::SolnPoolGap,0);
                cplex.setParam(IloCplex::SolnPoolIntensity,4);
                cplex.setParam(IloCplex::PopulateLim,2100000000);
                cplex.setParam(IloCplex::Param::OptimalityTarget,3);
                if ( !cplex.populate() ) {
                    env.error() << "Failed to optimize" << endl;
                    throw(-1);
                }

                IloNumArray vals(env);
                int numsol = cplex.getSolnPoolNsolns();
                env.out() << "Solution status = " << cplex.getStatus() << endl;
                if(numsol<1) return 0;
                int minsol=(int)cplex.getObjValue(0);
                for (int i = 0; i < numsol; i++)
                    if(minsol>(int)cplex.getObjValue(i))
                        minsol=cplex.getObjValue(i);
                env.out() << "Solution value = " << minsol << endl;
                for (int i = 0; i < numsol; i++)
                {
                if((int)cplex.getObjValue(i) == minsol){
                    cplex.getValues(vals, var,i);
                    //env.out() << "Values = "<<endl << vals << endl;
//                    env.out() <<"["<<i<< "] Variables = ["; // << vals << endl;
//                    for (int j = 0; j<n-1; j++) env.out() << vals[j] << ',';
//                    env.out() << vals[n-1] << ']' << endl;
                    env.out() << "Variables = " << vals << endl;
                }
            }
        } else {
            // Optimize the problem and obtain solution.
            if ( !cplex.solve() ) {
                env.error() << "Failed to optimize" << endl;
                throw(-1);
            }
            IloNumArray vals(env);
            //env.out() << "Solution status = " << cplex.getStatus() << endl;
            env.out() << "Solution value  = " << cplex.getObjValue() << endl;
            cplex.getValues(vals, var);
            //env.out() << "Variables = " << vals << endl;
            //env.out() <<"Variables = ["; // << vals << endl;
//            for (int j = 0; j<n-1; j++) env.out() << vals[j] << ',';
//            env.out() << vals[n-1] << ']' << endl;
        }
    }
    catch (IloException& e) {
        std::cerr << "Concert exception caught: " << e << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown exception caught" << std::endl;
    }

    env.end();

    return 0;
}  // END main

void read_graph(const int n, vector <pair<int,int> > &adjacent_list, vector <int> &setU, map<pair<int,int>,int> &weight)
{
    vector <pair<int,int> > adjacent_tmp;
    map <pair<int,int>,int> adjacent;
    string line;
    int lineCnt=-1;
    for(int i=0;i<n+1;i++)
    {
        std::getline(cin, line);
        istringstream iss(line);
        int a;
        while (iss >> a)
        {
            adjacent[make_pair(lineCnt,a)] = 1;
            adjacent_tmp.push_back(make_pair(lineCnt,a));
        }
        lineCnt++;
    }
    
    lineCnt=0;
    for(int i=0;i<n;i++)
    {
        std::getline(cin, line);
        istringstream iss(line);
        int a;
        while (iss >> a) weight[adjacent_tmp[lineCnt++]] = a;
    }
    for(map <pair<int,int>,int>::iterator it=adjacent.begin(); it!=adjacent.end(); ++it) {
        adjacent_list.push_back(it->first);
    }
    std::getline(cin, line);
    istringstream iss(line);
    int a;
    while (iss >> a) setU.push_back(a);
    
#ifdef DEBUG
    for(std::map<pair<int,int>,int>::iterator iti=weight.begin(); iti!=weight.end(); ++iti)
    {
        cout << iti->first.first+1 << " "<< iti->first.second+1 << " -> " << iti->second<<endl;
    }
#endif
}

