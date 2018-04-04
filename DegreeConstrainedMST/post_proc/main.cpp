#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <list>
//#define DEBUG
using namespace std;
void read_graph(int n, vector <pair<int,int> > &adjacent_list, map<pair<int,int>,int> &weight);
int main(int argc, char **argv) {
    int node_size=0;
    vector <pair<int,int> > adjacent_list;
    map<pair<int,int>,int> weight;

    cin>>node_size;

    read_graph(node_size,adjacent_list, weight);

    // set nominated root as 0
    int nominated_root = 0;

    int cnt=0;
    std::map<pair<int,int>,int> edge2matrix;
    vector<pair<int,int> >::const_iterator iterator;
    for (iterator = adjacent_list.begin(); iterator != adjacent_list.end(); ++iterator) {
        if ((*iterator).second == nominated_root) continue;
        edge2matrix[*iterator] = cnt;
        cnt++;
    }

    vector<int> vars;
    for(int i=0;i<cnt;i++){
        int var;
        cin >> var;
        vars.push_back(var);
#ifdef DEBUG
        cout<<var<<" ";
#endif
    }
#ifdef DEBUG
    cout<<endl;
#endif

    int sum=0;
    /********* O_I *********/
    for(std::map<pair<int,int>,int>::iterator it=edge2matrix.begin(); it!=edge2matrix.end(); ++it)
    {
        int u = it->first.first;
        int v = it->first.second;
        int x = vars[edge2matrix[make_pair(u,v)]];
        sum = sum + x * weight[make_pair(u,v)];
    }
    cout<<"optimal solution: "<<sum<<endl;

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
    for(std::map<pair<int,int>,int>::iterator iti=weight.begin(); iti!=weight.end(); ++iti)
    {
        cout << iti->first.first << " "<< iti->first.second << " -> " << iti->second<<endl;
    }
#endif
}