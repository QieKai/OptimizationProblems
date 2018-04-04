// C++ program to verify solutions from D-Wave
// This is for private use
#include <iostream>
#include <fstream>
#include <list>
#include <sstream>
#include <string>
#include <vector>
using namespace std;
void read_graph(string filename, int &n, list <pair<int,int> > &adjacent_list);
vector<int*> ProcDwaveOutput(const int n, int &k);
void Verifysolution(vector<int*> solutions, int n, int k, list <pair<int,int> > adjacent_list);
void find_and_replace(string& source, string const& find, string const& replace);
vector<string> split(const std::string &text, char sep);

bool DEBUG=false;
bool CSV=false;
bool GRAPHINFO=false;
int main(int argc, char *argv[])
{
    int n=0,k=0;
    list <pair<int,int> > adjacent_list;
    if (argc < 2)
    {
      cout << "Correct usage: " << argv[0] <<" <filename>" << endl;
      return 0;
    }
    else if (argc==2)
      read_graph(argv[1], n,adjacent_list);
    else if (argc==3)
    {
      string arg = string(argv[1]);
      if(arg.find("d")!=string::npos) DEBUG=true;
      if(arg.find("c")!=string::npos) CSV=true;
      if(arg.find("i")!=string::npos) GRAPHINFO=true;
      read_graph(argv[2], n,adjacent_list);
    }

    vector<int*> solutions = ProcDwaveOutput(n, k);
    Verifysolution(solutions,n,k,adjacent_list);
    return 0;
}

void read_graph(string filename, int &n, list <pair<int,int> > &adjacent_list)
{
  ifstream infile( filename );
  if (infile.is_open()==0)
  {
    cout << "Could not open file" << endl;
    exit(0);
  }
  infile >> n;
  string line;
  int lineCnt=-1;
  for(int i=0;i<n+1;i++)
  {
    std::getline(infile, line);
    istringstream iss(line);
    int a;
    while (iss >> a)adjacent_list.push_back(make_pair(lineCnt,a));
    lineCnt++;
  }
  //if (GRAPHINFO) cout<<"Order: "<<n<<" Size: "<<adjacent_list.size()/2<<endl;
  if (GRAPHINFO) cout<<n<<","<<adjacent_list.size()/2<<",";
}

vector<int*> ProcDwaveOutput(const int n, int &k){
  string line;
  vector<int*> solutions;
  while(std::getline(cin, line))
  {
    size_t kpos = line.find("k=");
    if (kpos!=string::npos && line.length()>=kpos+5)
    {
      string ks = line.substr(kpos,line.length()-kpos);
      ks = ks.substr(ks.find(",")+1, ks.find(")")-ks.find(",")-1);
      k = atoi(ks.c_str());
      //if (GRAPHINFO) cout<<k<<",";
    }

    size_t cpos = line.find("embedding=");
    if (cpos!=string::npos){
      line = line.substr(line.find("[")+1);
      int chainsize=0;
      while(1){
        size_t pf = line.find("["), pe = line.find("]");
        if (pf!=string::npos && pe!=string::npos)
        {
          string s = line.substr(pf+1,pe-pf-1);
          line = line.substr(pe+1);
          vector<string> chains = split(s,',');
          //cout << "chain size"<<chains.size();
          if (chainsize < chains.size()) chainsize = chains.size();
        }
        else if(pf==string::npos) break;
      }
      if (CSV) cout<<chainsize<<",";
    }

    size_t qpos = line.find("Physical qubits used=");
    if (qpos!=string::npos)
    {
      string ks = line.substr(line.find("=")+1,line.length()-line.find("=")-1);
      int pqubit = atoi(ks.c_str());
      if (GRAPHINFO) cout<<pqubit<<",";
    }

    size_t opos = line.find("\'num_occurrences\':");
    if (opos!=string::npos){
      line = line.substr(opos);
      int total_run=0;
      size_t pf = line.find("["), pe = line.find("]");
      if (pf!=string::npos && pe!=string::npos)
      {
        string s = line.substr(pf+1,pe-pf-1);
        vector<string> num_occurrences = split(s,',');
        for(int i=0;i<num_occurrences.size();i++){
          total_run+=atoi(num_occurrences[i].c_str());
          //cout <<num_occurrences[i]<<",";
        }
      }
      else if(pf==string::npos) break;

      if (CSV) cout<<total_run <<",";
    }

    size_t rpos = line.find("newresult:");
    if (rpos!=string::npos){
      line = line.substr(line.find("[")+1);
      find_and_replace( line, "-1", "0");
      while(1){
        size_t pf = line.find("["), pe = line.find("]");
        if (pf!=string::npos && pe!=string::npos)
        {
          string s = line.substr(pf+1,pe-pf-1);
          line = line.substr(pe+1);
          solutions.push_back( new int[n*k] );
          // cout << "solution"<<endl
          //cout<<s<<endl;
          for (int i=0;i<n*k;i++)solutions.back()[i] = s[i*3]-'0';
        }
        else if(pf==string::npos) break;
      }
    }
  }
  return solutions;
}

void Verifysolution(vector<int*> solutions, int n, int k, list <pair<int,int> > adjacent_list){
  const int N =n;
  int min=-1, passed=0, failed=0, gColoringNumber=0;
  for(int cnt=0;cnt<solutions.size();cnt++)
  {
    bool legal=true;
    int *x= solutions[cnt];
    int c[N],lColoringNumber=0;

    //check if every vertex is colored by exact one coloring number
    for (int i=0;i<n;i++)
    {
      int sumj=0;
      for(int j=0;j<k;j++)
      {
        sumj+=x[i*k+j];
        if(x[i*k+j] == 1)
        {
          c[i] = j+1;
          if (lColoringNumber < c[i]) lColoringNumber=c[i];
        }
      }
      if (sumj !=1 ) {
        if(DEBUG){
        cout<<"Solution ["<<cnt<<"]:"<<endl;
        for(int i=0;i<n*k;i++) cout << x[i] <<" ";
        cout<<endl;
        cout << "Illegal coloring found: vertex v[" << i <<"]"<<endl;
      }
        failed++;
        legal = false;
        break;
      }
    }
    if(legal==false) continue;

    //check that each pair of ajacent vertices are colored by different number
    list<pair<int,int> >::const_iterator iterator;
    for (iterator = adjacent_list.begin(); iterator != adjacent_list.end(); ++iterator) {
      int u = (*iterator).first, v = (*iterator).second;;
      if (c[u] == c[v])
      {
        if(DEBUG){
          cout<<"Solution ["<<cnt<<"]:"<<endl;
          for(int i=0;i<n*k;i++) cout << x[i] <<" ";
          cout<<endl;
          cout <<"Illegal coloring found: vertex [" << u <<"] and vertex [" <<v <<"] are colored by one number "<<c[u]<<endl;
        }
        failed++;
        legal = false;
        break;
      }
    }

    if(legal==false) continue;
    passed++;
    int optimal=0;
    for (int i = 0; i < n; i++)
      for (int j = 0; j < k; j++) optimal = optimal + x[i*k+j] * (j+1);

    if(DEBUG){ cout<<">>>>>>>>>>>>Solution ["<<cnt<<"] Chromatic Sum is: "<<optimal<<endl; }

    if(min==-1) {min = optimal; gColoringNumber = lColoringNumber;}
    else if(min>optimal) {min = optimal; if(gColoringNumber > lColoringNumber) gColoringNumber = lColoringNumber;}
  }

  if(!CSV) cout<<">>>>>>>>>>>>Verified ["<<solutions.size()<<"] solutions in total. Passed: ["<<passed<<"]; Failed: ["<< failed<<"]."<<endl;

  if ( min!=-1)
  {
    if(CSV) cout<<min<<","<<gColoringNumber;
    else cout<<">>>>>>>>>>>>Minimum optimal solution: "<<min<<". Number userd for coloring: "<<gColoringNumber<<endl;
  }
   else
   {
     if(CSV) cout<<",";
     else cout<<">>>>>>>>>>>>Optimize failed since the coloring is illegal"<<endl;
   }
}

void find_and_replace(string& source, string const& find, string const& replace)
{
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
