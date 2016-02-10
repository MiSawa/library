#if __INCLUDE_LEVEL__ == 0
#include <bits/stdc++.h>
using namespace std;
#else
#pragma once
#endif


void reader(char &x){ scanf("%c", &x); }
void reader(int &x){ scanf("%d", &x); }
void reader(long long &x){ scanf("%" SCNd64, &x); }
void reader(double &x){ scanf("%lf", &x); }
template<class S, class T, class ...U> void reader(S &x, T &y, U &...u){ reader(x); reader(y, u...); }


#if __INCLUDE_LEVEL__ == 0
signed main(void){
    int n, m; reader(n, m);
    cout << n << " " << m << endl;
    return 0;
}
#endif
// vim:set foldmethod=marker commentstring=//%s:
