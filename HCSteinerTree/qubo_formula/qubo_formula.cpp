// QUBO formulation for Steiner Tree
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cmath>
#include <map>
#include <algorithm>
//#define DEBUG

#define FIRST 0
#define MIDDLE 1
#define LAST 2

using namespace std;
void print_matrix(double **Q, int n);
void read_graph(int n, vector <pair<int,int> > &adjacent_list, vector <int> &setU, map<pair<int,int>,int> &weight);
const long generate_qubo(double **&Q, int node_size, vector<pair<int, int> > adjacent_list,
                         vector <int> setU, map<pair<int,int>,int> weight, int hop_constraint);
void print_matrix_latex(double **Q, const int n);

int main(int argc, char *argv[])
{
    int node_size=0,hop_constraint=0;
    double **Q;
    vector <pair<int,int> > adjacent_list;
    vector <int> setU;
    map<pair<int,int>,int> weight;
    if (argc != 2) cout << "Correct usage: " << argv[0] <<" <Hop constraint>" << endl;
    else hop_constraint = (int)strtol (argv[1], nullptr,10);
    cin>>node_size;

    read_graph(node_size,adjacent_list, setU, weight);

    const long N = generate_qubo(Q, node_size, adjacent_list, setU, weight, hop_constraint);
    cout<<N<<endl;
    print_matrix(Q,N);
    //print_matrix_latex(Q,N);
    for (int i=0; i<N; i++)
        delete [] Q[i];
    delete [] Q;
    return 0;
}

const long generate_qubo(double **&Q, int node_size, vector<pair<int, int> > adjacent_list,
                         vector <int> setU, map<pair<int,int>,int> weight, int hop_constraint)
{
    // set nominated root as 0
    int nominated_root = setU[0];

    map<vector<int>,int> edge2matrix;

    // set V/U
    vector <int> setV_U;
    for(int i=0;i<node_size;i++){
        if(std::find(setU.begin(), setU.end(), i) == setU.end()){
            setV_U.push_back(i);
        }
    }

    // init variables e_uv,i, and index of each variable in Q matrix
    int cnt=0;
    for (vector<pair<int,int> >::iterator iterator = adjacent_list.begin(); iterator != adjacent_list.end(); ++iterator) {
        if ((*iterator).second == nominated_root) continue;
        // variables x_{v0,u}_1
        if ((*iterator).first == nominated_root) {
            int myints[] = {iterator->first,iterator->second,1};
            vector<int> var(myints, myints + sizeof(myints) / sizeof(int) );
            edge2matrix[var] = cnt;
            cnt++;
        } else // variables x_{u,v}_i, where 2 <= i <= hop_constraint
        {
            for(int i=2;i<=hop_constraint;i++)
            {
                int myints[] = {iterator->first,iterator->second,i};
                vector<int> var(myints, myints + sizeof(myints) / sizeof(int) );
                edge2matrix[var] = cnt;
                cnt++;
            }
        }
    }

    // variables needed: N = 2(H-1)(|E| - Deg_G(v_0)) + Deg_G(v_0)
    const long N = edge2matrix.size();

    // initialize Q matrix
    Q = new double*[N];
    for (int i=0; i<N; i++)
    {
        Q[i] = new double[N];
        for (int j=0; j<N; j++) Q[i][j] = 0;
    }

#ifdef DEBUG
    /*********** debug ************/
    for(map<vector<int>,int>::iterator it=edge2matrix.begin(); it!=edge2matrix.end(); ++it)
    {
        printf("e%d,%d ", it->first[FIRST]+1, it->first[MIDDLE]+1);
    }
    printf("\n");
    /*********** debug ************/
#endif

    /********* F_{I,1} *********/
    for(int i=0; i<setU.size(); i++)
    {
        int v = setU[i];
        if (v == nominated_root) continue;
        for(map<vector<int>,int>::iterator iti=edge2matrix.begin(); iti!=edge2matrix.end(); ++iti)
        {
            if(iti->first[MIDDLE] != v) continue;
            int idx1 = iti->second;
            for(map<vector<int>,int>::iterator itj=edge2matrix.begin(); itj!=edge2matrix.end(); ++itj)
            {
                if(itj->first[MIDDLE] != v) continue;
                if(iti->second!=itj->second)
                {
                    int idx2 = itj->second;
                    Q[idx1][idx2]=Q[idx1][idx2] + node_size;
                }
                else
                {
                    Q[idx1][idx1]=Q[idx1][idx1] - node_size;
                }
            }
        }
    }
#ifdef DEBUG
    printf("\n********* F_{I,1} *********\n");
    print_matrix(Q, N);
#endif

    /********* F_{I,2} *********/
    int u,v;
    for(int i = 0; i<setV_U.size(); i++){
        v=setV_U[i];
        for(map<vector<int>,int>::iterator iti=edge2matrix.begin(); iti!=edge2matrix.end(); ++iti){
            if (iti->first[MIDDLE] != v ) continue;
            for(map<vector<int>,int>::iterator itj=edge2matrix.begin(); itj!=edge2matrix.end(); ++itj) {
                if (itj->first[MIDDLE] != v || iti->first[FIRST] >= itj->first[FIRST]) continue;
                Q[iti->second][itj->second]=Q[iti->second][itj->second] + node_size;
            }
        }
    }

#ifdef DEBUG
    printf("\n********* F_{I,2} *********\n");
    print_matrix(Q, N);
#endif

    /********* F_{I,3} *********/
    for(v=0; v<node_size; v++){
        if (v == nominated_root) continue;
        for(map<vector<int>,int>::iterator iti=edge2matrix.begin(); iti!=edge2matrix.end(); ++iti)
        {
            if(iti->first[MIDDLE] != v) continue;
            if(iti->first[LAST] < 2) continue;
            u=iti->first[FIRST];
            int idx1 = iti->second;
            //bool hasParent=false;
            Q[idx1][idx1]++;
            for(map<vector<int>,int>::iterator itj=edge2matrix.begin(); itj!=edge2matrix.end(); ++itj)
            {
                if(itj->first[MIDDLE] != u) continue;
                if(itj->first[LAST] != (iti->first[LAST]-1)) continue;
                int idx2 = itj->second;
                Q[idx1][idx2]--;
                //hasParent = true;
            }
//            if (hasParent) {
//                Q[idx1][idx1]++;
//            }
        }
    }
#ifdef DEBUG
    printf("\n********* F_{I,3} *********\n");
    print_matrix(Q, N);
#endif


    /********* P_I *********/
    int maxWeight = 0;
    for(map<pair<int,int>,int>::iterator it=weight.begin(); it!=weight.end(); ++it)
        if (maxWeight < it->second) {maxWeight = it->second;}
    int P_I = (node_size-1) * maxWeight +1;
    for (int i=0; i<N; i++)
    {
        for (int j=0; j<N; j++)
            Q[i][j] = Q[i][j] * P_I;
    }
#ifdef DEBUG
    printf("\n********* P_I *********\n");
    print_matrix(Q, N);
    printf("\n");
#endif
    /********* O_I *********/
    for(map<vector<int>,int>::iterator it=edge2matrix.begin(); it!=edge2matrix.end(); ++it)
    {
        u = it->first[FIRST];
        v = it->first[MIDDLE];
        Q[it->second][it->second] = Q[it->second][it->second] + weight[make_pair(u,v)];
    }
#ifdef DEBUG
    printf("\n********* O_I *********\n");
    print_matrix(Q, N);
    printf("\n");
#endif

    return N;
}

void print_matrix(double **Q, const int n)
{
    for (int i=0; i<n; i++)
    {
        for (int j=0; j<n; j++)
            printf("%4d ", (int)Q[i][j]);
        printf("\n");
    }
}

void print_matrix_latex(double **Q, const int n)
{
    printf("\n");
    for (int i=0; i<n; i++)
    {
        for (int j=0; j<n; j++)
        {
            if (j<i)
            {
                printf("&    ");
            }
            else
            {
                if (i!=j)
                    printf("&%4d", (int)(Q[i][j]+Q[j][i]));
                else
                    printf("&%4d", (int)Q[i][j]);
            }

        }

        printf("\n");
    }
}


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
        cout << iti->first.first << " "<< iti->first.second << " -> " << iti->second<<endl;
    }

    for(vector <int>::iterator iti=setU.begin(); iti!=setU.end(); ++iti)
    {
        cout << *iti <<endl;
    }
#endif
}
