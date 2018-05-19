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
void read_graph(string filename, vector <pair<int,int> > &adjacent_list, vector <int> &setU, map<pair<int,int>,int> &weight);
void ProcDwaveOutput(int &quboSize, map<vector<int> ,int>&);
void find_and_replace(string& source, string const& find, string const& replace);
vector<string> split(const std::string &text, char sep);
void CalcAccuracy(map<vector<int>, int>, map<pair<int,int>,int>, map<pair<int,int>,int>);
void printCSV(int min);
void printSol(map<int,int> solOccur);

bool DEBUG=false;
bool CSV=false;

int total_run=0,chainsize=0,order=0,size=0,lqubit=0, pqubit=0,minCost=0,correct_run=0;

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
        read_graph(argv[1],adjacent_list,setU, weight);
        minCost = (int)strtol(argv[2], nullptr,10);
    }
    else if (argc==4)
    {
        string arg = string(argv[1]);
        if(arg.find('d')!=string::npos) DEBUG=true;
        if(arg.find('c')!=string::npos) CSV=true;
        read_graph(argv[2],adjacent_list,setU, weight);
        minCost = (int)strtol(argv[3], nullptr,10);
    }

    int quboSize=0;

    map<vector<int> ,int> quboSolutions;
    ProcDwaveOutput(quboSize, quboSolutions);
    if(DEBUG){
        for(map<vector<int> ,int>::iterator it=quboSolutions.begin(); it!=quboSolutions.end(); ++it)
        {
            cout<<"["<<it->second<<"]: ";
            for(int i=0;i<quboSize;i++)cout<<it->first[i]<<" ";
            cout<<endl;
        }
    }

    int nominated_root = setU[0];
    int cnt=0;
    std::map<pair<int,int>,int> edge2matrix;
    vector<pair<int,int> >::const_iterator iterator;
    for (iterator = adjacent_list.begin(); iterator != adjacent_list.end(); ++iterator) {
        if ((*iterator).second == nominated_root) continue;
        edge2matrix[*iterator] = cnt;
        cnt++;
    }

    CalcAccuracy(quboSolutions, edge2matrix, weight);

    return 0;
}

void ProcDwaveOutput(int &quboSize, map<vector<int> ,int>&solutions){
    string line;
    vector<int>occurrences;

    while(std::getline(cin, line))
    {
        size_t kpos = line.find("n=");
        if (kpos!=string::npos && line.length()>=kpos+5)
        {
            string ks = line.substr(kpos,line.length()-kpos);
            ks = ks.substr(ks.find(',')+1, ks.find(')')-ks.find(',')-1);
            quboSize = atoi(ks.c_str());
            if (DEBUG) cout<<"qubo size: "<<quboSize<<endl;
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
                for(int i=0;i<num_occurrences.size();i++){
                    int occ = atoi(num_occurrences[i].c_str());
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
                    std::vector<int> sol;
                    for (int i=0;i<quboSize;i++) sol.push_back(s[i*3]-'0');
                    solutions[sol] = occurrences[idx];
                    idx++;
                }
                else if(pf==string::npos) break;
            }
        }
    }
}


void CalcAccuracy(map<vector<int> ,int> quboSolutions, map<pair<int,int>,int> edge2matrix, map<pair<int,int>,int> weight)
{
    map<int,int> solOccur;

    for(map<vector<int> ,int>::iterator sol=quboSolutions.begin(); sol!=quboSolutions.end(); ++sol)
    {
        int sum=0;
        /********* O_I *********/
        for(std::map<pair<int,int>,int>::iterator it=edge2matrix.begin(); it!=edge2matrix.end(); ++it)
        {
            int u = it->first.first;
            int v = it->first.second;
            int x = sol->first[edge2matrix[make_pair(u,v)]];
            sum = sum + x * weight[make_pair(u,v)];
        }

        if(solOccur.count(sum)==0) solOccur[sum] = sol->second;
        else solOccur[sum] = solOccur[sum] + sol->second;
        if (DEBUG) cout<<"Solution value  = "<<sum<<" occurance  = "<<solOccur[sum]<<endl;
    }

    if(solOccur.count(minCost)==0)correct_run=0;
    else correct_run = solOccur[minCost];

    if(CSV)printCSV(solOccur.begin()->first);
    else printSol(solOccur);

}

// print
void printCSV(int min)
{
    cout<<order<<","<<size<<",";
    cout<<lqubit<<","<<pqubit<<",";
    cout<<chainsize<<",";
    cout<<correct_run<<"/"<<total_run <<",";
    cout<<min;
    cout<<",";
    cout<<minCost;
}

void printSol(map<int,int> solOccur)
{
    for(map<int,int>::iterator it=solOccur.begin(); it!=solOccur.end(); ++it)
    {
        cout<<"Solution value  = "<<it->first<<"; occurance  = "<<it->second<<endl;
    }
}

void read_graph(string filename, vector <pair<int,int> > &adjacent_list, vector <int> &setU, map<pair<int,int>,int> &weight)
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
            adjacent_tmp.push_back(make_pair(lineCnt,a));
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
    for(map <pair<int,int>,int>::iterator it=adjacent.begin(); it!=adjacent.end(); ++it) {
        adjacent_list.push_back(it->first);
    }
    std::getline(infile, line);
    istringstream iss(line);
    int a;
    while (iss >> a) setU.push_back(a);

    infile.close();
#ifdef DEBUG
    for(std::map<pair<int,int>,int>::iterator iti=weight.begin(); iti!=weight.end(); ++iti)
    {
        cout << iti->first.first << " "<< iti->first.second << " -> " << iti->second<<endl;
    }
#endif
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
