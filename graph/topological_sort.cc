#include <bits/stdc++.h>
#define all(x) begin(x),end(x)
#define rall(x) (x).rbegin(),(x).rend()
#define REP(i,b,n) for(int i=(int)(b);i<(int)(n);++i)
#define rep(i,n) REP(i,0,n)
#define repsz(i,v) rep(i,(v).size())
#define aur auto&
#define bit(n) (1LL<<(n))
#define eb emplace_back
#define mt make_tuple
#define fst first
#define snd second
using namespace std;
typedef long long ll;
//#define int long long
template<class C>int size(const C &c){ return c.size(); }
template<class T>bool chmin(T&a,const T&b){if(a<=b)return false;a=b;return true;}
template<class T>bool chmax(T&a,const T&b){if(a>=b)return false;a=b;return true;}

// verified: AOJ GRL_4_A(Cycle Detection), GRL_4_B (Topological Sort)
// TopologicalSort ts(n);
// ts.add_edge(u, v);
// bool ts.run(); true if this graph is DAG.
// ts.ord[idx] = v
// ts.idx[v] = idx
struct TopologicalSort{//{{{
    vector<vector<int>> g;
    TopologicalSort(const int &n) : g(n){}
    inline void add_edge(const int &s, const int &t){ g[s].emplace_back(t); }
    vector<int> ord, idx;
    bool run(){//{{{
        const int n = g.size();
        ord.resize(n); idx.resize(n);
        vector<int> d(n);
        for(int u = 0; u < n; ++u) for(auto &v : g[u]) ++d[v];
        int ql = -1, qr = -1;
        for(int u = 0; u < n; ++u) if(d[u] == 0) ord[++qr] = u;
        while(ql != qr){
            int u = ord[++ql]; idx[u] = ql;
            for(auto &v : g[u]) if(!--d[v]) ord[++qr] = v;
        }
        return qr + 1 == g.size();
    }//}}}
};//}}}

bool solve(){
    int n, m; cin >> n >> m;
    TopologicalSort g(n);
    rep(_, m){
        int u, v; cin >> u >> v;
        g.add_edge(u, v);
    }
    g.run();
    rep(u, n) cout << g.ord[u] << endl;
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
