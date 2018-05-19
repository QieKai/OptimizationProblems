// QUBO formulation for Steiner Tree
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cmath>
#include <map>
#include <algorithm>
//#define DEBUG

using namespace std;
void print_matrix(double **Q, int n);
void read_graph(int n, vector <pair<int,int> > &adjacent_list, vector <int> &setU, map<pair<int,int>,int> &weight);
const long generate_qubo(double **&Q, int node_size, vector<pair<int, int> > adjacent_list,
                         vector <int> setU, map<pair<int,int>,int> weight);

int main(int argc, char *argv[])
{
    int node_size=0;
    double **Q;
    vector <pair<int,int> > adjacent_list;
    vector <int> setU;
    map<pair<int,int>,int> weight;

    cin>>node_size;

    read_graph(node_size,adjacent_list, setU, weight);

    const long N = generate_qubo(Q, node_size, adjacent_list, setU, weight);
    cout<<N<<endl;
    print_matrix(Q,N);
    for (int i=0; i<N; i++)
        delete [] Q[i];
    delete [] Q;
    return 0;
}

//void read_graph(const int n, vector <pair<int,int> > &adjacent_list, vector <int> &setU, map<pair<int,int>,int> &weight)
//{
//    string line;
//    int lineCnt=-1;
//    for(int i=0;i<n+1;i++)
//    {
//        std::getline(cin, line);
//        istringstream iss(line);
//        int a;
//        while (iss >> a)adjacent_list.push_back(make_pair(lineCnt,a));
//        lineCnt++;
//    }
//    lineCnt=0;
//    for(int i=0;i<n;i++)
//    {
//        std::getline(cin, line);
//        istringstream iss(line);
//        int a;
//        while (iss >> a) weight[adjacent_list[lineCnt++]] = a;
//    }
//    std::getline(cin, line);
//    istringstream iss(line);
//    int a;
//    while (iss >> a) setU.push_back(a);
//#ifdef DEBUG
//    for(std::map<pair<int,int>,int>::iterator iti=weight.begin(); iti!=weight.end(); ++iti)
//    {
//        cout << iti->first.first << " "<< iti->first.second << " -> " << iti->second<<endl;
//    }
//#endif
//}

const long generate_qubo(double **&Q, int node_size, vector<pair<int, int> > adjacent_list,
                         vector <int> setU, map<pair<int,int>,int> weight)
{
    // set nominated root as 0
    int nominated_root = setU[0];

    // index for each variable in Q matrix
    int cnt=0;

    // set V/U
    vector <int> setV_U;
    for(int i=0;i<node_size;i++){
        if(std::find(setU.begin(), setU.end(), i) == setU.end()){
            setV_U.push_back(i);
        }
    }

    // e variables
    map<pair<int,int>,int> edge2matrix;
    for (vector<pair<int,int> >::const_iterator iterator = adjacent_list.begin();
         iterator != adjacent_list.end(); ++iterator) {
        if ((*iterator).second == nominated_root) continue;
        edge2matrix[*iterator] = cnt;
        cnt++;
    }

    // x variables
    std::map<pair<int,int>,int> x2matrix;
    for(int u=0; u<node_size-1; u++)
    {
        if (u==nominated_root) continue;
        for(int v=u+1; v<node_size;v++)
        {
            if (v==nominated_root) continue;
            x2matrix[make_pair(u,v)] = cnt;
            cnt++;
        }
    }

    // variables needed: N = (2|E| - Deg_G(v_0)) + C(|V|-1, 2)
    const long N = edge2matrix.size() + x2matrix.size();

    // initialize Q matrix
    Q = new double*[N];
    for (int i=0; i<N; i++)
    {
        Q[i] = new double[N];
        for (int j=0; j<N; j++) Q[i][j] = 0;
    }

#ifdef DEBUG
    /*********** debug ************/
    for(map<pair<int,int>,int>::iterator it=edge2matrix.begin(); it!=edge2matrix.end(); ++it)
    {
        printf("e%d,%d ", it->first.first+1, it->first.second+1);
    }
    for(map<pair<int,int>,int>::iterator it=x2matrix.begin(); it!=x2matrix.end(); ++it)
    {
        printf("x%d,%d ", it->first.first+1, it->first.second+1);
    }
    printf("\n");
    /*********** debug ************/
#endif

    /********* F_{I,1} *********/
    for(int u=0; u<node_size-2; u++)
    {
        if (u == nominated_root) continue;
        for(int v=1; v<node_size-1; v++)
        {
            if (v == nominated_root) continue;
            for(int w=2; w<node_size; w++)
            {
                if (w == nominated_root) continue;
                if (x2matrix.count(make_pair(u,v)) == 0 || x2matrix.count(make_pair(u,w)) == 0
                    || x2matrix.count(make_pair(v,w)) == 0) continue;
                int x_uv = x2matrix[make_pair(u,v)];
                int x_uw = x2matrix[make_pair(u,w)];
                int x_vw = x2matrix[make_pair(v,w)];
                Q[x_uw][x_uw]++;
                Q[x_uv][x_vw]++;
                Q[x_uv][x_uw]--;
                Q[x_uw][x_vw]--;
            }
        }
    }
#ifdef DEBUG
    printf("\n********* F_{I,1} *********\n");
    print_matrix(Q, N);
#endif

    /********* F_{I,2} *********/
    // = ∑ e_{u,v}^2 -e_{u,v} * x_{u,v} + e_{v,u} * x_{u,v}
    int u,v;
    for (vector<pair<int,int> >::const_iterator iterator = adjacent_list.begin();
         iterator != adjacent_list.end(); ++iterator) {
        u = (*iterator).first;
        v = (*iterator).second;
        if(u==nominated_root || v==nominated_root)continue;
        if (u<v)
        {
            // e_{u,v}^2
            int idx1 = edge2matrix[*iterator];
            Q[idx1][idx1]++;
            // -e_{u,v} * x_{u,v}
            int idx2 = x2matrix[*iterator];
            Q[idx1][idx2]--;
            // e_{v,u} * x_{u,v}
            int idx3 = edge2matrix[make_pair(v,u)];
            Q[idx3][idx2]++;
        }
    }
#ifdef DEBUG
    printf("\n********* F_{I,2} *********\n");
    print_matrix(Q, N);
#endif

    /********* F_{I,3} *********/
    for(int i=0; i<setU.size(); i++)
    {
        int v = setU[i];
        if (v == nominated_root) continue;
        for(map<pair<int,int>,int>::iterator iti=edge2matrix.begin(); iti!=edge2matrix.end(); ++iti)
        {
            if(iti->first.second != v) continue;
            int idx1 = iti->second;
            for(map<pair<int,int>,int>::iterator itj=edge2matrix.begin(); itj!=edge2matrix.end(); ++itj)
            {
                if(itj->first.second != v) continue;
                if(iti->second!=itj->second)
                {
                    int idx2 = itj->second;
                    Q[idx1][idx2]++;
                }
                else
                {
                    int idx = iti->second;
                    Q[idx][idx]--;
                }
            }
        }
    }
#ifdef DEBUG
    printf("\n********* F_{I,3} *********\n");
    print_matrix(Q, N);
#endif

    /********* F_{I,4} *********/
    //
    for(int i = 0; i<setV_U.size(); i++){
        v=setV_U[i];
        for(map<pair<int,int>,int>::iterator iti=edge2matrix.begin(); iti!=edge2matrix.end(); ++iti){
            if (iti->first.second != v ) continue;
            for(map<pair<int,int>,int>::iterator itj=edge2matrix.begin(); itj!=edge2matrix.end(); ++itj) {
                if (itj->first.second != v || iti->first.first >= itj->first.first) continue;
                Q[iti->second][itj->second]+=node_size;
            }
        }
    }

#ifdef DEBUG
    printf("\n********* F_{I,4} *********\n");
    print_matrix(Q, N);
#endif

    /********* F_{I,5} *********/
    //
    for(int i = 0; i<setV_U.size(); i++){
        v=setV_U[i];
        for(map<pair<int,int>,int>::iterator iti=edge2matrix.begin(); iti!=edge2matrix.end(); ++iti){
            if (iti->first.first != v ) continue;
            Q[iti->second][iti->second]++;
        }

        for(map<pair<int,int>,int>::iterator iti=edge2matrix.begin(); iti!=edge2matrix.end(); ++iti){
            if (iti->first.first != v ) continue;
            for(map<pair<int,int>,int>::iterator itj=edge2matrix.begin(); itj!=edge2matrix.end(); ++itj){
                if (itj->first.second != v ) continue;
                Q[iti->second][itj->second]--;
            }
        }
    }

#ifdef DEBUG
    printf("\n********* F_{I,5} *********\n");
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

    /********* O_I *********/
    for(map<pair<int,int>,int>::iterator it=edge2matrix.begin(); it!=edge2matrix.end(); ++it)
    {
        u = it->first.first;
        v = it->first.second;
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
#endif
}