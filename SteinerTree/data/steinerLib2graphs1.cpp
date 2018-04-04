#include <iostream>
#include <sstream>
#include <map>
#include <vector>
using namespace std;

int main() {
    map<pair<int,int>,int> weight;
    vector <pair<int,int> > adjacent_list;
    vector <int> setU;
    string line;
    while (getline(cin, line))
    {
        if(line.length() <= 0) continue;
        if(line[0] == 'E' && line[1] == ' '){
            istringstream iss(line.erase(0,2));
            int u,v,w;
            iss >> u;
            iss >> v;
            iss >> w;
            u--;v--;
            adjacent_list.push_back(make_pair(u,v));
            adjacent_list.push_back(make_pair(v,u));
            weight[make_pair(u,v)] = w;
            weight[make_pair(v,u)] = w;
        }
        else if (line[0] == 'T' && line[1] == ' ')
        {
            istringstream iss(line.erase(0,2));
            int v;
            iss >> v;
            v--;
            setU.push_back(v);
        }
    }

    int node_size=0;
    for (vector<pair<int,int> >::const_iterator iterator = adjacent_list.begin();
         iterator != adjacent_list.end(); ++iterator)
    {
        if(node_size < (iterator->first))
            node_size = iterator->first;
        else if (node_size < (iterator->second))
            node_size = iterator->second;
    }
    node_size++;
    cout << node_size <<endl;
    for(int i=0; i<node_size; i++){
        for (vector<pair<int,int> >::const_iterator iterator = adjacent_list.begin();
             iterator != adjacent_list.end(); ++iterator) {
            if(iterator->first == i) {
                cout <<iterator->second<<" ";
            }
        }
        cout<<endl;
    }

    for(int i=0; i<node_size; i++){
        for (vector<pair<int,int> >::const_iterator iterator = adjacent_list.begin();
             iterator != adjacent_list.end(); ++iterator) {
            if(iterator->first == i) {
                cout <<weight[*iterator]<<" ";
            }
        }
        cout<<endl;
    }

    for (vector <int>::const_iterator iterator = setU.begin();
         iterator != setU.end(); ++iterator) {
        cout<<*iterator<<" ";
    }

    return 0;
}