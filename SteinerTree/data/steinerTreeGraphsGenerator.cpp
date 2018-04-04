#include <iostream>
#include <sstream>
#include <random>
#include <map>

using namespace std;

int main() {
int node_size=0;
    cin>>node_size;
    vector <pair<int,int> > adjacent_list;
    map<pair<int,int>,int> weight;

    mt19937 rng;
    rng.seed(random_device()());
    uniform_int_distribution<mt19937::result_type> dist(1,10);

    string line;
    std::getline(cin, line);
    int lineCnt=0;
    for(int i=0;i<node_size;i++)
    {
        std::getline(cin, line);
        istringstream iss(line);
        int a;
        while (iss >> a)
        {
            adjacent_list.push_back(make_pair(lineCnt,a));
            if(weight.find(make_pair(lineCnt,a)) == weight.end() && weight.find(make_pair(a,lineCnt)) == weight.end()){
                int w = dist(rng);
                weight[make_pair(lineCnt,a)] = w;
                weight[make_pair(a,lineCnt)] = w;
            }
        }
        lineCnt++;
    }

    cout<<node_size<<endl;

    for(int i=0, lineNum=0;i<adjacent_list.size();i++)
    {
        if(adjacent_list[i].first == lineNum){
            cout<<adjacent_list[i].second<<" ";
        }else{
            cout<<endl;
            cout<<adjacent_list[i].second<<" ";
            lineNum++;
        }
    }
    cout<<endl;

    for(int i=0, lineNum=0;i<adjacent_list.size();i++)
    {
        if(adjacent_list[i].first == lineNum){
            cout<<weight[adjacent_list[i]]<<" ";
        }else{
            cout<<endl;
            cout<<weight[adjacent_list[i]]<<" ";
            lineNum++;
        }
    }
    cout<<endl;

    uniform_int_distribution<mt19937::result_type> setURdm(0,node_size-1);
    const int setUSize = node_size/2+1;
    vector<int> setU;

    for(int i=0;i<setUSize;i++)
    {
        int x = setURdm(rng);
        while(find(setU.begin(), setU.end(), x) != setU.end())x = setURdm(rng);
        setU.push_back(x);
        cout<<x<<" ";
    }

    return 0;
}

