// QUBO formulation for Degree-Constrained Minimum Spanning Tree (∆-spanning tree)
// Variable dConst represents the degree constraint of ∆-spanning tree problem
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cmath>
#include <map>
//#define DEBUG

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

//void read_graph(const int n, vector <pair<int,int> > &adjacent_list, map<pair<int,int>,int> &weight)
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
//#ifdef DEBUG
//    for(std::map<pair<int,int>,int>::iterator iti=weight.begin(); iti!=weight.end(); ++iti)
//    {
//        cout << iti->first.first << " "<< iti->first.second << " -> " << iti->second<<endl;
//    }
//#endif
//}
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
    for(std::map<pair<int,int>,int>::iterator iti=weight.begin(); iti!=weight.end(); ++iti)
    {
        cout << iti->first.first << " "<< iti->first.second << " -> " << iti->second<<endl;
    }
#endif
}

const long generate_qubo(double **&Q, int node_size, int degree_constraint, vector<pair<int, int> > adjacent_list,
                         map<pair<int,int>,int> weight)
{
    // set nominated root as 0
    int nominated_root = 0;

    // index for each variable in Q matrix
    int cnt=0;
    std::map<pair<int,int>,int> edge2matrix;
    vector<pair<int,int> >::const_iterator iterator;
    for (iterator = adjacent_list.begin(); iterator != adjacent_list.end(); ++iterator) {
        if ((*iterator).second == nominated_root) continue;
        edge2matrix[*iterator] = cnt;
        cnt++;
    }

    std::map<pair<int,int>,int> x2matrix;
    for(int u=1; u<node_size-1; u++)
    {
        for(int v=u+1; v<node_size;v++)
        {
            x2matrix[make_pair(u,v)] = cnt;
            cnt++;
        }
    }

    // variables needed: N = (2|E| - Deg_G(v_0)) + C(|V|-1, 2) + |V|(k+1), where k = [log_2(∆)]
    int k = (int)(log(degree_constraint)/log(2));
    const long N = edge2matrix.size() + x2matrix.size() + node_size*(k+1);

    // initialize Q matrix
    Q = new double*[N];
    for (int i=0; i<N; i++)
    {
        Q[i] = new double[N];
        for (int j=0; j<N; j++) Q[i][j] = 0;
    }

#ifdef DEBUG
    /*********** debug ************/
    for(std::map<pair<int,int>,int>::iterator it=edge2matrix.begin(); it!=edge2matrix.end(); ++it)
    {
        printf("e%d,%d ", it->first.first+1, it->first.second+1);
    }
    for(std::map<pair<int,int>,int>::iterator it=x2matrix.begin(); it!=x2matrix.end(); ++it)
    {
        printf("x%d,%d ", it->first.first+1, it->first.second+1);
    }
    for(int i=0; i<node_size;i++){
        for(int j=0;j<k+1;j++){
            printf("z%d,%d ", i+1, j);
        }
    }
    printf("\n");
    /*********** debug ************/
#endif

    /********* F_{I,1} *********/
//    for(int u=1; u<node_size-3; u++)
//    {
//        for(int v=2; v<node_size-2; v++)
//        {
//            for(int w=3; w<node_size-1; w++)
//            {
//                if (x2matrix.count(make_pair(u,v)) == 0 || x2matrix.count(make_pair(u,w)) == 0
//                    || x2matrix.count(make_pair(v,w)) == 0) continue;
//                int x_uv = x2matrix[make_pair(u,v)];
//                int x_uw = x2matrix[make_pair(u,w)];
//                int x_vw = x2matrix[make_pair(v,w)];
//                Q[x_uw][x_uw]++;
//                Q[x_uv][x_vw]++;
//                Q[x_uv][x_uw]--;
//                Q[x_uw][x_vw]--;
//            }
//        }
//    }
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

    /********* F_{I,2} *********/
    // = ∑ e_{u,v}^2 -e_{u,v} * x_{u,v} + e_{v,u} * x_{u,v}
    int u,v;
    for (iterator = adjacent_list.begin(); iterator != adjacent_list.end(); ++iterator) {
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
    for(v=0; v<node_size; v++)
    {
        if (v == nominated_root) continue;
        for(std::map<pair<int,int>,int>::iterator iti=edge2matrix.begin(); iti!=edge2matrix.end(); ++iti)
        {
            if(iti->first.second == v){
                for(std::map<pair<int,int>,int>::iterator itj=edge2matrix.begin(); itj!=edge2matrix.end(); ++itj)
                {
                    if(itj->first.second == v){
                        if(iti->second!=itj->second)
                        {
                            int idx1 = iti->second;
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

        // ( ∑ (e_{u,v} + e_{v,u} ) + ∑ e_{v0,v}) ^2
        for(std::map<pair<int,int>,int>::iterator iti=edge2matrix.begin(); iti!=edge2matrix.end(); ++iti)
        {
            bool b1 = (iti->first.second == v) || (iti->first.first == v);
            if(b1)
            {
                int idx1 = iti->second;
                for(std::map<pair<int,int>,int>::iterator itj=edge2matrix.begin(); itj!=edge2matrix.end(); ++itj)
                {
                    bool b2 = (itj->first.second == v) || (itj->first.first == v);
                    if(b2)
                    {
                        int idx2 = itj->second;
                        Q[idx1][idx2]++;
                    }
                }
            }
        }

        // - 2 (Z_v ) * ( ∑ (e_{u,v} + e_{v,u} ) )
        for(int i=0;i<k+1;i++)
        {
            if(i == k) coeff1 = degree_constraint+1-(int)pow(2,k);
            else coeff1 = (int)pow(2,i);
            int idx1 = cnt + v*(k+1) + i;
            for(std::map<pair<int,int>,int>::iterator iti=edge2matrix.begin(); iti!=edge2matrix.end(); ++iti) {
                bool b1 = (iti->first.second == v) || (iti->first.first == v);
                if (b1) {
                    int idx2 = iti->second;
                    Q[idx1][idx2] = Q[idx1][idx2] - coeff1;
                    Q[idx2][idx1] = Q[idx2][idx1] - coeff1;
                }
            }
        }
    }

    // (Z_v0 - ∑ e_{v0,u} )^2
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
    // (∑ e_{v0,u} ) ^2
    for(std::map<pair<int,int>,int>::iterator iti=edge2matrix.begin(); iti!=edge2matrix.end(); ++iti)
    {
        if(iti->first.first == nominated_root){
            int idx1 = iti->second;
            for(std::map<pair<int,int>,int>::iterator itj=edge2matrix.begin(); itj!=edge2matrix.end(); ++itj)
            {
                if(itj->first.first == nominated_root){
                    int idx2 = itj->second;
                    Q[idx1][idx2]++;
                }
            }
        }
    }
    // - 2 * Z_v * (∑ e_{v0,u} )
    for(std::map<pair<int,int>,int>::iterator it=edge2matrix.begin(); it!=edge2matrix.end(); ++it)
    {
        if (it->first.first == nominated_root)
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
    for(std::map<pair<int,int>,int>::iterator it=weight.begin(); it!=weight.end(); ++it)
        if (maxWeight < it->second) {maxWeight = it->second;}
    int P_I = (node_size-1) * maxWeight +1;
    for (int i=0; i<N; i++)
    {
        for (int j=0; j<N; j++)
            Q[i][j] = Q[i][j] * P_I;
    }

    /********* O_I *********/
    for(std::map<pair<int,int>,int>::iterator it=edge2matrix.begin(); it!=edge2matrix.end(); ++it)
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

void print_matrix(double **Q, const int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            printf("%d ", (int) Q[i][j]);
        printf("\n");
    }
}