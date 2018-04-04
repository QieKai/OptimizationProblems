// QUBO formulation for chromatic sum problem
// usage: $./chromatic_sum_qubo k < graph.alist < graph.d.out
// k is set to be same as graph size n if not given by argument
#include <iostream>
#include <fstream>
#include <utility>
#include <list>
#include <sstream>
#include <string>
#include <stdlib.h>

using namespace std;
void read_graph(const int n, list <pair<int,int> > &adjacent_list);
void makeQuboMatrix(double **Q, int n, int k, list <pair<int,int> > adjacent_list);
void printMatrix(double **Q, const int n);

int main(int argc, char *argv[])
{
    int n=0,k=0;
    double **Q;
    list <pair<int,int> > adjacent_list;
    cin>>n;
    if (argc < 2) cout << "Correct usage: " << argv[0] <<" <filename>" << endl;
    else if(argc == 2) k = (int)strtol (argv[1],NULL,10);
    else k = n;
    read_graph(n,adjacent_list);
    const int N = n*k;
    Q = new double*[N];
    for (int i=0; i<N; i++)
    {
        Q[i] = new double[N];
        for (int j=0; j<N; j++) Q[i][j] = 0;
    }

    makeQuboMatrix(Q,n,k,adjacent_list);
    cout<< n*k<<" "<<k<<endl;
    printMatrix(Q,N);
    for (int i=0; i<N; i++)
        delete [] Q[i];
    delete [] Q;
    return 0;
}

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

void makeQuboMatrix(double **Q, int n, int k, list <pair<int,int> > adjacent_list)
{
    const int penalty = n;

    //f(x) = sum j*x[i,j]
    for (int i=0; i<n; i++)
    {
        for(int j=0; j<k; j++)
        {
            int idx = i*k+j;
            Q[idx][idx] = Q[idx][idx] + (j+1);
        }
    }
    //p1(x) = sum(i) (1- sum(j)x[i,j])^2
    for (int i=0; i<n; i++){
        for(int j=0; j<k; j++){
            for(int l=0; l<k; l++){
                if(j!=l){
                    int idx1 = i*k+j;
                    int idx2 = i*k+l;
                    Q[idx1][idx2] += penalty;
                }
                else {
                    int idx = i*k+j;
                    Q[idx][idx] = Q[idx][idx] - penalty;
                }
            }
        }
    }

    //p2(x) = sum(E(G))sum(j) x[u,j]*x[v,j]
    list<pair<int,int> >::const_iterator iterator;

    for (iterator = adjacent_list.begin(); iterator != adjacent_list.end(); ++iterator) {
        int u = (*iterator).first;
        int v = (*iterator).second;
        for(int j=0; j<k; j++) {
            Q[u*k+j][v*k+j] += penalty;
        }
    }
}

void printMatrix(double **Q, const int n)
{
    for (int i=0; i<n; i++){
        for (int j=0; j<n; j++) cout<< Q[i][j]<<" ";
        cout <<endl;
    }
}
