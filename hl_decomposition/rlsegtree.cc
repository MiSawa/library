#include <bits/stdc++.h>
#define all(x) begin(x),end(x)
#define rall(x) (x).rbegin(),(x).rend()
#define REP(i,b,n) for(int i=(int)(b);i<(int)(n);++i)
#define rep(i,n) REP(i,0,n)
#define rrep(i,n) for(int i=(int)(n)-1;i>=0;--i)
#define repsz(i,v) rep(i,(v).size())
using namespace std;

vector<int> path_to(int idx){
    vector<int> res;
    ++idx;
    int u = 2;
    for(; u <= idx; u <<= 1) res.emplace_back(u);
    res.emplace_back(u);
    res.emplace_back(++u);
    for(u >>= 2; u; u >>= 1){
        if(idx&u) res.emplace_back(res.back() + u*2);
        else      res.emplace_back(res.back() + 1);
    }
    return res;
}
ostream &operator<<(ostream &os, const vector<int> &v){
    for(auto e : v) os << e << ", ";
    return os;
}
bool solve(){
    rep(i, 10) cout << i << ":  " << path_to(i) << endl;
    return true;
}
signed main(){
    cin.tie(nullptr);
    ios_base::sync_with_stdio(false);
    cout << std::fixed << std::setprecision(10);
    solve();
    return 0;
}
// vim:set foldmethod=marker commentstring=//%s:
