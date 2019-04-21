#include <bits/stdc++.h>
using namespace std;

constexpr int N = 5000;
double C[N+2][N+2];
signed main(){

    C[0][0] = 1;
    for(int i = 1; i <= N; ++i){
        C[i][0] = C[i][i] = C[i-1][0] / 2.;
        for(int j = 1; j < i; ++j)
            C[i][j] = (C[i-1][j] + C[i-1][j-1]) / 2.;
    }
    cout << setprecision(20) << C[N][N/2] << endl;
    return 0;
}
// vim:set foldmethod=marker commentstring=//%s:
