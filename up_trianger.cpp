#include <iostream>
#include <fstream>
using namespace std;

//Comments: prints a n-by-n matrix
void printMatrix(double **Q, int n)
{
    for (int i=0; i<n; i++)
    {
        for (int j=0; j<n; j++)
            cout << Q[i][j] << " ";
        cout << endl;
    }
}
//Comments: prints a n-by-n matrix
void printMatrixUpTriangular(double **Q, int n)
{
    for (int i=0; i<n; i++)
    {
        for (int j=0; j<n; j++)
        {
            if(j<i)cout<<"& ";
            else if(j==i)cout << Q[i][j]<<" & ";
            else cout << Q[i][j]+Q[j][i] << " & ";
        }
        cout << endl;
    }
}

void read_qubo(int &n, double **&Q) {
    // Start of: initializing Q
    string line;
    getline(cin,line);
    std::size_t pos = line.find(" ");
    if(pos!=std::string::npos)
    {
        line = line.substr(0,pos);
    }
    n = atoi(line.c_str());
    Q = new double*[n];
    for (int i=0; i<n; i++)
    {
        Q[i] = new double[n];
        for (int j=0; j<n; j++)
            Q[i][j] = 0;
    }
    for (int i=0; i<n; i++)
    {
        for(int j=0;j<n;j++)
            cin >> Q[i][j];
    }
}
int main(int argc, char* argv[])
{
    int n,k;
    double **Q;

    read_qubo(n,Q);
    
    for (int i=0; i<n; i++)
    {
        for(int j=i+1;j<n;j++)
        {
            //Q[i][j]=Q[i][j]+Q[j][i];
            Q[j][i]=Q[i][j];
            
        }
    }
    cout<<n<<endl;
    //printMatrix(Q,n);
    printMatrixUpTriangular(n,Q);
    
    for (int i=0; i<n; i++)
        delete [] Q[i];
    delete [] Q;
    
    return 0;
}
