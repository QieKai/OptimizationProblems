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

using namespace std;
void read_graph(string filename, vector <pair<int,int> > &adjacent_list, map<pair<int,int>,int> &weight);
void ProcDWaveOutput(int &quboSize, map<vector<int>, int> &);
void find_and_replace(string& source, string const& find, string const& replace);
vector<string> split(const std::string &text, char sep);
void VerifySolutions(map<vector<int>, int>&, map<pair<int,int>,int>, map<pair<int,int>,int>, vector <pair<int,int> >);
void CalcAccuracy(map<vector<int>, int>, map<pair<int,int>,int>, map<pair<int,int>,int>);
void printCSV(int min);
void printSol(map<int,int> solOccur);
void printDebug(int, int, vector<int>);

bool DEBUG=false;
bool CSV=false;

int total_run=0,chainsize=0,order=0,size=0,lqubit=0, pqubit=0,minCost=0,correct_run=0,degree_constraint=0;

int main(int argc, char *argv[])
{
    vector <pair<int,int> > adjacent_list;
    map<pair<int,int>,int> weight;
    if (argc < 3)
    {
        cout << "Correct usage: " << argv[0] <<" <filename> <optimalSol>" << endl;
        return 0;
    }
    else if (argc==3)
    {
        read_graph(argv[1],adjacent_list, weight);
        minCost = (int)strtol(argv[2], nullptr,10);
    }
    else if (argc==4)
    {
        string arg = string(argv[1]);
        if(arg.find('d')!=string::npos) DEBUG=true;
        if(arg.find('c')!=string::npos) CSV=true;
        read_graph(argv[2],adjacent_list, weight);
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

    // set nominated root as 0
    int nominated_root = 0;
    int cnt=0;
    map<pair<int,int>,int> edge2matrix;
    for (auto iterator : adjacent_list) {
        if (iterator.second == nominated_root) continue;
        edge2matrix[iterator] = cnt;
        cnt++;
    }
    map<pair<int,int>,int> x2matrix;
    for(int u=1; u<order-1; u++)
    {
        for(int v=u+1; v<order;v++)
        {
            x2matrix[make_pair(u,v)] = cnt;
            cnt++;
        }
    }

    if(DEBUG) for(auto it:edge2matrix) printf("e%d,%d ", it.first.first+1, it.first.second+1);
    cout<<endl;
    //VerifySolutions(quboSolutions, edge2matrix, x2matrix, adjacent_list);
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

            quboSize = atoi(nc[0].c_str());
            degree_constraint = atoi(nc[1].c_str());

            if (DEBUG) cout<<"qubo size: "<<quboSize<<endl;
            if (DEBUG) cout<<"degree constraint: "<<degree_constraint<<endl;
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
            pqubit = atoi(ks.c_str());

        }

        size_t opos = line.find("\'num_occurrences\':");
        if (opos!=string::npos){
            line = line.substr(opos);
            size_t pf = line.find('['), pe = line.find(']');
            if (pf!=string::npos && pe!=string::npos)
            {
                string s = line.substr(pf+1,pe-pf-1);
                vector<string> num_occurrences = split(s,',');
                for(auto const n : num_occurrences){
                    int occ = atoi(n.c_str());
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

void VerifySolutions(map<vector<int> ,int>& quboSolutions, map<pair<int,int>,int> edge2matrix, map<pair<int,int>,int> x2matrix, vector <pair<int,int> > adjacent_list)
{
    int nominated_root=0;
    bool valid=true;
    for(auto sol:quboSolutions)
    {
        auto vars = sol.first;
        int F1=0;
        /********* F_{I,1} *********/
        for(int u=0; u<order-2; u++)
        {
            if (u == nominated_root) continue;
            for(int v=1; v<order-1; v++)
            {
                if (v == nominated_root) continue;
                for(int w=2; w<order; w++)
                {
                    if (w == nominated_root) continue;
                    if (x2matrix.count(make_pair(u,v)) == 0 || x2matrix.count(make_pair(u,w)) == 0
                        || x2matrix.count(make_pair(v,w)) == 0) continue;
                    int x_uv = x2matrix[make_pair(u,v)];
                    int x_uw = x2matrix[make_pair(u,w)];
                    int x_vw = x2matrix[make_pair(v,w)];
                    F1 = F1 + vars[x_uw] + vars[x_uv]*vars[x_vw] - vars[x_uv]*vars[x_uw]- vars[x_uw]*vars[x_vw];
                }
            }
        }
        if (F1 != 0) {
            if(DEBUG)printDebug(1,F1,vars);
            //quboSolutions.erase(sol.first);
            continue;
        }

        /********* F_{I,2} *********/
        int F2=0;
        int u,v;
        for (auto iterator : adjacent_list) {
            u = iterator.first;
            v = iterator.second;
            if(u==nominated_root || v==nominated_root)continue;
            if (u<v)
            {
                // e_{u,v}
                int idx1 = edge2matrix[iterator];
                // x_{u,v}
                int idx2 = x2matrix[iterator];
                // e_{v,u}
                int idx3 = edge2matrix[make_pair(v,u)];
                F2=F2+vars[idx1]*(1-vars[idx2])+vars[idx2]*vars[idx3];
            }
        }
        cout<<"f21"<<endl;
        if (F2 != 0) {
            if(DEBUG)printDebug(2,F2,vars);
            //quboSolutions.erase(sol.first);
            continue;
        }

        /********* F_{I,3} *********/
        int F3=0;
        valid=true;
        for(v=0; v<order; v++)
        {
            F3=0;
            if (v == nominated_root) continue;
            for(auto iti:edge2matrix)
            {
                if(iti.first.second == v){
                    F3=F3+vars[iti.second];
                }
            }
            if (F3 != 1) {
                if(DEBUG)printDebug(3,F3,vars);
                //quboSolutions.erase(sol.first);
                valid=false;
                break;
            }
        }
        if (!valid) {
            cout<<"f31"<<endl;
            continue;
        }
        cout<<"f32"<<endl;

        /********* F_{I,4} *********/
        int F4;
        for(v=0; v<order; v++) {
            F4 = 0;
            if (v == nominated_root) continue;
            for(auto iti:edge2matrix)
            {
                if(iti.first.second == v || iti.first.first == v){
                    F4=F4+vars[iti.second];
                }
            }
            if (F4 > degree_constraint) {
                if(DEBUG)printDebug(4,F4,vars);
                //quboSolutions.erase(sol.first);
                break;
            }
        }
    }
    cout<<"finish verify"<<endl;
}

void CalcAccuracy(map<vector<int> ,int> quboSolutions, map<pair<int,int>,int> edge2matrix, map<pair<int,int>,int> weight)
{
    map<int,int> solOccur;

    for(auto sol:quboSolutions)
    {
        int sum=0;
        /********* O_I *********/
        for(auto it=edge2matrix.begin(); it!=edge2matrix.end(); ++it)
        {
            int u = it->first.first;
            int v = it->first.second;
            int x = sol.first[edge2matrix[make_pair(u,v)]];
            sum = sum + x * weight[make_pair(u,v)];
        }

        if(solOccur.count(sum)==0) solOccur[sum] = sol.second;
        else solOccur[sum] = solOccur[sum] + sol.second;
        if (DEBUG) cout<<"Solution value  = "<<sum<<" occurance  = "<<solOccur[sum]<<endl;
    }

    if(solOccur.count(minCost)==0)correct_run=0;
    else correct_run = solOccur[minCost];

    if(CSV)printCSV(solOccur.begin()->first);
    else printSol(solOccur);
}

void read_graph(string filename, vector <pair<int,int> > &adjacent_list, map<pair<int,int>,int> &weight)
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

    size=(int)adjacent_list.size()/2;
    infile.close();
}

// print
void printCSV(int min)
{
    cout<<order<<","<<size<<","<<degree_constraint<<",";
    cout<<lqubit<<","<<pqubit<<",";
    cout<<chainsize<<",";
    cout<<correct_run<<"/"<<total_run <<",";
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

