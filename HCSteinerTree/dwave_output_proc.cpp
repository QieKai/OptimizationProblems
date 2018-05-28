// C++ program to verify solutions from D-Wave
// Usage: ./a.out [options] graph.alist optimalSolution < dwave.d.out
// The following options are available:
// -c print as csv format
// -d print debug information
#include <iostream>
#include <fstream>
#include <list>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#define FIRST 0
#define MIDDLE 1
#define LAST 2
using namespace std;
void read_graph(const string&, vector <pair<int,int> > &adjacent_list, vector <int> &setU, map<pair<int,int>,int> &weight);
void ProcDWaveOutput(int &quboSize, map<vector<int>, int> &);
void find_and_replace(string& source, string const& find, string const& replace);
vector<string> split(const std::string &text, char sep);
void VerifySolutions(map<vector<int>, int>&, map<vector<int>,int>, vector <int>);
void CalcAccuracy(map<vector<int>, int>, map<vector<int>,int>, map<pair<int,int>,int>);
void printCSV(int min);
void printSol(map<int,int> solOccur);
void printDebug(int, int, vector<int>);

bool DEBUG=false;
bool CSV=false;

int total_run=0,chainsize=0,order=0,size=0,lqubit=0, pqubit=0,minCost=0,correct_run=0,depth_constraint=0;

int main(int argc, char *argv[])
{
    vector <pair<int,int> > adjacent_list;
    map<pair<int,int>,int> weight;
    vector <int> setU;
    if (argc < 3)
    {
        cout << "Correct usage: " << argv[0] <<" <filename> <optimalSol>" << endl;
        return 0;
    }
    else if (argc==3)
    {
        string filename = argv[1];
        read_graph(filename,adjacent_list,setU, weight);
        minCost = (int)strtol(argv[2], nullptr,10);
    }
    else if (argc==4)
    {
        string arg = string(argv[1]);
        if(arg.find('d')!=string::npos) DEBUG=true;
        if(arg.find('c')!=string::npos) CSV=true;
        string filename = argv[2];
        read_graph(filename,adjacent_list,setU, weight);
        minCost = (int)strtol(argv[3], nullptr,10);
    }

    int quboSize=0;

    map<vector<int> ,int> quboSolutions;
    ProcDWaveOutput(quboSize, quboSolutions);
    if(DEBUG) {
        for (auto it : quboSolutions) {
            cout << "[" << it.second << "]: ";
            for (int i = 0; i < quboSize; i++)cout << it.first[i] << " ";
            cout << endl;
        }
    }

    // set nominated root as first element of set U
    int nominated_root = setU[0];
    map<vector<int>,int> edge2matrix;

    // init variables e_uv,i, and index of each variable in Q matrix
    int cnt=0;
    for (auto iterator : adjacent_list) {
        if (iterator.second == nominated_root) continue;
        // variables x_{v0,u}_1
        if (iterator.first == nominated_root) {
            int myints[] = {iterator.first,iterator.second,1};
            vector<int> var(myints, myints + sizeof(myints) / sizeof(int) );
            edge2matrix[var] = cnt;
            cnt++;
        } else // variables x_{u,v}_i, where 2 <= i <= hop_constraint
        {
            for(int i=2;i<=depth_constraint;i++)
            {
                int myints[] = {iterator.first,iterator.second,i};
                vector<int> var(myints, myints + sizeof(myints) / sizeof(int) );
                edge2matrix[var] = cnt;
                cnt++;
            }
        }
    }
    if(DEBUG)
    {
        for(auto it:edge2matrix) printf("e%d,%d,%d ", it.first[FIRST]+1, it.first[MIDDLE]+1, it.first[LAST]);
        cout<<endl;
    }

    VerifySolutions(quboSolutions, edge2matrix, setU);
    CalcAccuracy(quboSolutions, edge2matrix, weight);

    return 0;
}

void ProcDWaveOutput(int &quboSize, map<vector<int>, int> &solutions){
    string line;
    vector<int>occurrences;

    while(std::getline(cin, line))
    {
        size_t kpos = line.find("header:");
        if (kpos!=string::npos && line.length()>=kpos+12)
        {
            size_t p1 = line.find('[');
            size_t p2 = line.find(']');
            string ks = line.substr(p1+1,p2-p1-1);
            find_and_replace( ks, "\'", "");
            vector<string>nc = split(ks, ',');

            quboSize = (int)strtol(nc[0].c_str(), nullptr,10);
            depth_constraint = (int)strtol(nc[1].c_str(), nullptr,10);

            if (DEBUG) cout<<"qubo size: "<<quboSize<<endl;
            if (DEBUG) cout<<"degree constraint: "<<depth_constraint<<endl;
            lqubit=quboSize;
        }

        size_t cpos = line.find("embedding=");
        if (cpos!=string::npos){
            line = line.substr(line.find('[')+1);
            while(true){
                size_t pf = line.find('['), pe = line.find(']');
                if (pf!=string::npos && pe!=string::npos)
                {
                    string s = line.substr(pf+1,pe-pf-1);
                    line = line.substr(pe+1);
                    vector<string> chains = split(s,',');
                    if (chainsize < chains.size()) chainsize = (int)chains.size();
                }
                else if(pf==string::npos) break;
            }
        }

        size_t qpos = line.find("Physical qubits used=");
        if (qpos!=string::npos)
        {
            string ks = line.substr(line.find('=')+1,line.length()-line.find('=')-1);
            pqubit = (int)strtol(ks.c_str(), nullptr,10);

        }

        size_t opos = line.find("\'num_occurrences\':");
        if (opos!=string::npos){
            line = line.substr(opos);
            size_t pf = line.find('['), pe = line.find(']');
            if (pf!=string::npos && pe!=string::npos)
            {
                string s = line.substr(pf+1,pe-pf-1);
                vector<string> num_occurrences = split(s,',');
                for(auto const& n : num_occurrences){
                    auto occ = (int)strtol(n.c_str(), nullptr,10);
                    occurrences.push_back(occ);
                    total_run+=occ;
                }
            }
            else if(pf==string::npos) break;
        }

        int idx=0;
        size_t rpos = line.find("newresult:");
        if (rpos!=string::npos){
            line = line.substr(line.find('[')+1);
            find_and_replace( line, "-1", "0");
            while(true){
                size_t pf = line.find('['), pe = line.find(']');
                if (pf!=string::npos && pe!=string::npos)
                {
                    string s = line.substr(pf+1,pe-pf-1);
                    line = line.substr(pe+1);
                    std::vector<int> sol((unsigned long)quboSize,0);
                    for (int i=0;i<quboSize;i++) sol[i]=(s[i*3]-'0');
                    solutions[sol] = occurrences[idx];
                    idx++;
                }
                else if(pf==string::npos) break;
            }
        }
    }
}

void VerifySolutions(map<vector<int> ,int>& quboSolutions, map<vector<int>,int> edge2matrix, vector<int> setU)
{
    int nominated_root=setU[0];

    // set V/U
    vector <int> setV_U;
    for(int i=0;i<order;i++){
        if(std::find(setU.begin(), setU.end(), i) == setU.end()){
            setV_U.push_back(i);
        }
    }

    map<vector<int> ,int> allSols;
    for(auto sol:quboSolutions) allSols[sol.first] = sol.second;

    for(auto sol:allSols)
    {
        auto vars = sol.first;
        /********* F_{I,1} *********/
        int F1=0;
        for(auto v:setU)
        {
            F1=1;
            if (v == nominated_root) continue;
            for(auto iti:edge2matrix)
            {
                if(iti.first[MIDDLE] != v) continue;
                F1 = F1 - vars[iti.second];
            }
            if(F1!=0)
            {
                if(DEBUG)printDebug(1,F1,vars);
                quboSolutions.erase(sol.first);
                break;
            }
        }
        if(F1!=0)continue;

        /********* F_{I,2} *********/
        int F2=0;
        for(auto v:setV_U)
        {
            F2=0;
            for(auto iti:edge2matrix){
                if (iti.first[MIDDLE] != v ) continue;
                for(auto itj:edge2matrix) {
                    if (itj.first[MIDDLE] != v || iti.first[FIRST] >= itj.first[FIRST]) continue;
                    F2 = F2 + vars[iti.second] * vars[itj.second];
                }
            }
        }
        if (F2!=0) {
            if(DEBUG)printDebug(2,F2,vars);
            quboSolutions.erase(sol.first);
            continue;
        }

        /********* F_{I,3} *********/
        int F3=0;
        int sum,u;
        bool value=true;
        for(int v=0; v<order; v++)
        {
            F3=0;
            if (v == nominated_root) continue;
            for(auto iti:edge2matrix)
            {
                if(iti.first[MIDDLE] != v) continue;
                if(iti.first[LAST] < 2) continue;
                u=iti.first[FIRST];
                sum=1;
                for(auto itj:edge2matrix){
                    if(itj.first[MIDDLE] != u) continue;
                    if(itj.first[LAST] != (iti.first[LAST]-1)) continue;
                    sum = sum - vars[itj.second];
                }
                F3 = sum * vars[iti.second];
                if (F3 != 0) {
                    if(DEBUG)printDebug(3,F3,vars);
                    quboSolutions.erase(sol.first);
                    value=false;
                    break;
                }
            }
            if(!value)break;
        }
    }
}

void CalcAccuracy(map<vector<int> ,int> quboSolutions, map<vector<int>,int> edge2matrix, map<pair<int,int>,int> weight)
{
    map<int,int> solOccur;

    for(auto sol:quboSolutions)
    {
        int sum=0;
        /********* O_I *********/
        for(auto it:edge2matrix)
        {
            int u = it.first[FIRST];
            int v = it.first[MIDDLE];
            int x = sol.first[it.second];
            sum = sum + x * weight[make_pair(u,v)];
        }

        if(solOccur.count(sum)==0) solOccur[sum] = sol.second;
        else solOccur[sum] = solOccur[sum] + sol.second;
        if(sum==minCost)
        {
            cout<<"variables:[";
            for (auto i : sol.first) cout<<i<<" ";
            cout<<"]"<<endl;
        }
    }
    int min = solOccur.empty()?0:solOccur.begin()->first;
    if(solOccur.count(minCost)==0)correct_run=0;
    else correct_run = solOccur[minCost];

    if(CSV)printCSV(min);
    else printSol(solOccur);
}

void read_graph(const string& filename, vector <pair<int,int> > &adjacent_list, vector <int> &setU, map<pair<int,int>,int> &weight)
{
    ifstream infile( filename );
    if (infile.is_open()==0)
    {
        cout << "Could not open file" << endl;
        exit(0);
    }
    infile >> order;

    int n = order;
    vector <pair<int,int> > adjacent_tmp;
    map <pair<int,int>,int> adjacent;
    string line;
    int lineCnt=-1;
    for(int i=0;i<n+1;i++)
    {
        std::getline(infile, line);
        istringstream iss(line);
        int a;
        while (iss >> a)
        {
            adjacent[make_pair(lineCnt,a)] = 1;
            adjacent_tmp.emplace_back(make_pair(lineCnt,a));
        }
        lineCnt++;
    }

    lineCnt=0;
    for(int i=0;i<n;i++)
    {
        std::getline(infile, line);
        istringstream iss(line);
        int a;
        while (iss >> a) weight[adjacent_tmp[lineCnt++]] = a;
    }
    for(auto it:adjacent) {
        adjacent_list.push_back(it.first);
    }
    std::getline(infile, line);
    istringstream iss(line);
    int a;
    while (iss >> a) setU.push_back(a);
    size=(int)adjacent_list.size()/2;
    infile.close();
}

// print
void printCSV(int min)
{
    cout<<order<<","<<size<<","<<depth_constraint<<",";
    cout<<lqubit<<","<<pqubit<<",";
    cout<<chainsize<<",";
    //cout<<correct_run<<"/"<<total_run <<",";
    cout<<correct_run<<",";
    cout<<min;
    cout<<",";
    cout<<minCost;
}

void printSol(map<int,int> solOccur)
{
    for(auto it:solOccur)
    {
        cout<<"Solution value  = "<<it.first<<"; occurance  = "<<it.second<<endl;
    }
}

void printDebug(int num, int F, vector<int> vars)
{
    cout<<"F"<<num<< " = "<< F<<endl;
    cout<<"variables:[";
    for (auto i : vars) cout<<i<<" ";
    cout<<"]"<<endl;
}

// utilities
void find_and_replace(string& source, string const& find, string const& replace){
    for(string::size_type i = 0; (i = source.find(find, i)) != string::npos;)
    {
        source.replace(i, find.length(), replace);
        i += replace.length();
    }
}

vector<string> split(const string &text, char sep) {
    vector<string> tokens;
    size_t start = 0, end = 0;
    while ((end = text.find(sep, start)) != string::npos) {
        if (end != start) {
            tokens.push_back(text.substr(start, end - start));
        }
        start = end + 1;
    }
    if (end != start) {
        tokens.push_back(text.substr(start));
    }
    return tokens;
}

