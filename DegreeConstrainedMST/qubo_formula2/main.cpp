// QUBO formulation for Degree-Constrained Minimum Spanning Tree (∆-spanning tree)
// Variable dConst represents the degree constraint of ∆-spanning tree problem
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cmath>
#include <map>
//#define DEBUG

#define FIRST 0
#define MIDDLE 1
#define LAST 2

using namespace std;
void print_matrix(double **Q, int n);
void read_graph(int n, vector <pair<int,int> > &adjacent_list, map<pair<int,int>,int> &weight);
const long generate_qubo(double **&Q, int node_size, int degree_constraint, vector<pair<int, int> > adjacent_list,
                         map<pair<int,int>,int> weight);

int main(int argc, char *argv[])
{
    int node_size=0,degree_constraint=0;
    double **Q;
    vector <pair<int,int> > adjacent_list;
    map<pair<int,int>,int> weight;
    if (argc != 2) cout << "Correct usage: " << argv[0] <<" <Degree constraint>" << endl;
    else degree_constraint = (int)strtol (argv[1], nullptr,10);

    cin>>node_size;

    read_graph(node_size,adjacent_list, weight);

    const long N = generate_qubo(Q, node_size, degree_constraint, adjacent_list, weight);
    cout<<N<<endl;
    print_matrix(Q,N);
    for (int i=0; i<N; i++)
        delete [] Q[i];
    delete [] Q;
    return 0;
}

void read_graph(const int n, vector <pair<int,int> > &adjacent_list, map<pair<int,int>,int> &weight)
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

#ifdef DEBUG
    //    for(std::map<pair<int,int>,int>::iterator iti=weight.begin(); iti!=weight.end(); ++iti)
//    {
//        cout << iti->first.first << " "<< iti->first.second << " -> " << iti->second<<endl;
//    }
#endif
}

const long generate_qubo(double **&Q, int node_size, int degree_constraint, vector<pair<int, int> > adjacent_list,
                         map<pair<int,int>,int> weight)
{
    // set nominated root as 0
    int nominated_root = 0;

    map<vector<int>,int> edge2matrix;

    // init variables e_uv,i, and index of each variable in Q matrix
    int cnt=0;
    for (vector<pair<int,int> >::iterator iterator = adjacent_list.begin(); iterator != adjacent_list.end(); ++iterator) {
        if ((*iterator).second == nominated_root) continue;
        if ((*iterator).first == nominated_root) {
            int myints[] = {iterator->first,iterator->second,1};
            vector<int> var(myints, myints + sizeof(myints) / sizeof(int) );
            edge2matrix[var] = cnt;
            cnt++;
        } else
        {
            for(int i=2;i<node_size;i++)
            {
                int myints[] = {iterator->first,iterator->second,i};
                vector<int> var(myints, myints + sizeof(myints) / sizeof(int) );
                edge2matrix[var] = cnt;
                cnt++;
            }
        }
    }

    // variables needed in total: N = (2|E| - 2*Deg_G(v_0)) * (|V|-2) + Deg_G(v_0) + |V|(k+1), where k = [log_2(∆)]
    int k = (int)(log(degree_constraint)/log(2));
    const long N = edge2matrix.size() + node_size * (k+1);

    // initialize Q matrix
    Q = new double*[N];
    for (int i=0; i<N; i++)
    {
        Q[i] = new double[N];
        for (int j=0; j<N; j++) Q[i][j] = 0;
    }

#ifdef DEBUG
    for(map<vector<int>,int>::iterator it=edge2matrix.begin(); it!=edge2matrix.end(); ++it)
    {
        printf("e%d,%d,%d -> %d |", it->first[FIRST]+1, it->first[MIDDLE]+1, it->first[LAST], it->second);
    }
    printf("\n");
    for(map<vector<int>,int>::iterator it=edge2matrix.begin(); it!=edge2matrix.end(); ++it)
    {
        printf("e%d,%d,%d ", it->first[FIRST]+1, it->first[MIDDLE]+1, it->first[LAST]);
    }

    for(int i=0; i<node_size;i++){
        for(int j=0;j<k+1;j++){
            printf("z%d,%d   ", i+1, j);
        }
    }
    printf("\n");
#endif

    /********* F_{I,1} *********/
    int u,v;
    for(v=0; v<node_size; v++)
    {
        if (v == nominated_root) continue;
        for(map<vector<int>,int>::iterator iti=edge2matrix.begin(); iti!=edge2matrix.end(); ++iti)
        {
            if(iti->first[MIDDLE] == v){
                for(map<vector<int>,int>::iterator itj=edge2matrix.begin(); itj!=edge2matrix.end(); ++itj)
                {
                    if(itj->first[MIDDLE] == v){
                        if(iti->second!=itj->second)
                        {
                            int idx1 = iti->second;
                            int idx2 = itj->second;
                            Q[idx1][idx2]=Q[idx1][idx2] + node_size*node_size;
                        }
                        else
                        {
                            int idx = iti->second;
                            Q[idx][idx]=Q[idx][idx] - node_size*node_size;
                        }
                    }
                }
            }
        }
    }
#ifdef DEBUG
    printf("\n********* F_{I,1} *********\n");
    print_matrix(Q, N);
#endif
    /********* F_{I,2} *********/
    for(v=0; v<node_size; v++){
        if (v == nominated_root) continue;
        for(map<vector<int>,int>::iterator iti=edge2matrix.begin(); iti!=edge2matrix.end(); ++iti)
        {
            if(iti->first[MIDDLE] != v) continue;
            for(map<vector<int>,int>::iterator itj=edge2matrix.begin(); itj!=edge2matrix.end(); ++itj)
            {
                if(itj->first[FIRST] != v) continue;
                if(itj->first[LAST] > iti->first[LAST]) continue;
                int idx1 = iti->second;
                int idx2 = itj->second;
                Q[idx1][idx2]++;
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
            Q[idx1][idx1]++;
            for(map<vector<int>,int>::iterator itj=edge2matrix.begin(); itj!=edge2matrix.end(); ++itj)
            {
                if(itj->first[MIDDLE] != u) continue;
                if(itj->first[LAST] != (iti->first[LAST]-1)) continue;
                int idx2 = itj->second;
                Q[idx1][idx2]--;
            }
        }
    }
#ifdef DEBUG
    printf("\n********* F_{I,3} *********\n");
    print_matrix(Q, N);
#endif
    /********* F_{I,4} *********/
    //
    int coeff1 = 0, coeff2 = 0;
    for(v=0; v<node_size; v++)
    {
        if (v == nominated_root) continue;
        // Z_v ^2
        for(int i=0;i<k+1;i++)
        {
            if(i == k) coeff1 = degree_constraint+1-(int)pow(2,k);
            else coeff1 = (int)pow(2,i);
            int idx1 = cnt + v*(k+1) + i;
            for(int j=0;j<k+1;j++)
            {
                if(j == k) coeff2 = degree_constraint+1-(int)pow(2,k);
                else coeff2 = (int)pow(2,j);

                int idx2 = cnt + v*(k+1) + j;
                Q[idx1][idx2] = Q[idx1][idx2] + coeff1 * coeff2;
            }
        }

        // ( ∑ (e_{u,v,i} + e_{v,u,i} ) + ∑ e_{v0,v,i}) ^2
        for(map<vector<int>,int>::iterator iti=edge2matrix.begin(); iti!=edge2matrix.end(); ++iti)
        {
            bool b1 = (iti->first[MIDDLE] == v) || (iti->first[FIRST] == v);
            if(b1)
            {
                int idx1 = iti->second;
                for(map<vector<int>,int>::iterator itj=edge2matrix.begin(); itj!=edge2matrix.end(); ++itj)
                {
                    bool b2 = (itj->first[MIDDLE] == v) || (itj->first[FIRST] == v);
                    if(b2)
                    {
                        int idx2 = itj->second;
                        Q[idx1][idx2]++;
                    }
                }
            }
        }

        // - 2 (Z_v ) * ( ∑ (e_{u,v,i} + e_{v,u,i} ) )
        for(int i=0;i<k+1;i++)
        {
            if(i == k) coeff1 = degree_constraint+1-(int)pow(2,k);
            else coeff1 = (int)pow(2,i);
            int idx1 = cnt + v*(k+1) + i;
            for(map<vector<int>,int>::iterator iti=edge2matrix.begin(); iti!=edge2matrix.end(); ++iti) {
                bool b1 = (iti->first[MIDDLE] == v) || (iti->first[FIRST] == v);
                if (b1) {
                    int idx2 = iti->second;
                    Q[idx1][idx2] = Q[idx1][idx2] - coeff1;
                    Q[idx2][idx1] = Q[idx2][idx1] - coeff1;
                }
            }
        }
    }

    // (Z_v0 - ∑ e_{v0,u,i} )^2
    // Z_v0 ^2
    for(int i=0;i<k+1;i++)
    {
        if(i == k) coeff1 = degree_constraint+1-(int)pow(2,k);
        else coeff1 = (int)pow(2,i);
        for(int j=0;j<k+1;j++)
        {
            if(j == k) coeff2 = degree_constraint+1-(int)pow(2,k);
            else coeff2 = (int)pow(2,j);
            Q[cnt+i][cnt+j] = Q[cnt+i][cnt+j] + coeff1 * coeff2;
        }
    }
    // (∑ e_{v0,u,i} ) ^2
    for(map<vector<int>,int>::iterator iti=edge2matrix.begin(); iti!=edge2matrix.end(); ++iti)
    {
        if(iti->first[FIRST] == nominated_root){
            int idx1 = iti->second;
            for(std::map<vector<int>,int>::iterator itj=edge2matrix.begin(); itj!=edge2matrix.end(); ++itj)
            {
                if(itj->first[FIRST] == nominated_root){
                    int idx2 = itj->second;
                    Q[idx1][idx2]++;
                }
            }
        }
    }
    // - 2 * Z_v * (∑ e_{v0,u} )
    for(map<vector<int>,int>::iterator it=edge2matrix.begin(); it!=edge2matrix.end(); ++it)
    {
        if (it->first[FIRST] == nominated_root)
        {
            int coeff=0;
            for(int i=0;i<k+1;i++)
            {
                if(i == k) coeff = degree_constraint+1-(int)pow(2,k);
                else coeff = (int)pow(2,i);
                Q[cnt+i][it->second] = Q[cnt+i][it->second] - coeff;
                Q[it->second][cnt+i] = Q[it->second][cnt+i] - coeff;
            }
        }
    }
#ifdef DEBUG
    printf("\n********* F_{I,4} *********\n");
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

void print_matrix(double **Q, const int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            printf("%6d ", (int) Q[i][j]);
        printf("\n");
    }
}