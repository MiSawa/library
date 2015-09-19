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

// verified: SPOJ 6818 CAPCITY, AOJ GRL_3_C (Strongly Connected Components)
// StronglyConnectedComponents scc(n);
// scc.add_edge(u, v);
// scc.run();
// scc.id[u] : id of component (0-origin)      (after run())
// scc.component[id] : vector<int> : component (after run())
// ちゃんと topological 順序になっている.
// scc.build_supar_graph(); returns super graph.
// scc.g[u] : vector<int> : also super graph. (after build_supar_graph())
struct StronglyConnectedComponents{//{{{
    vector<vector<int>> _g;
    StronglyConnectedComponents(const int &n) : _g(n){}
    inline void add_edge(const int &s, const int &t){ _g[s].emplace_back(t); }

    vector<int> id;
    vector<vector<int>> component;

    int dfs(int u, int &time, vector<int> &open){//{{{
        open.emplace_back(u);
        int w = u;
        id[u] = --time;
        for(auto &v : _g[u]) if(id[v] >= 0){          // not closed.
            int x = v;
            if(id[v] == 0)    x = dfs(v, time, open); // not visited yet.
            if(id[w] < id[x]) w = x;
        }
        if(w != u) return w;
        component.emplace_back(0);
        id[u] = -(int)(component.size()); // close.
        do{
            w = open.back(); open.pop_back();
            id[w] = id[u];
            component.back().emplace_back(w);
        }while(w != u);
        return w;
    }//}}}
    void run(){//{{{
        const int &n = _g.size();
        static vector<int> open; open.reserve(n);
        id.assign(n, 0);
        component.clear();
        for(int u = 0, time = n+1; u < n; ++u) if(id[u] == 0) dfs(u, time, open);
        for(int u = 0; u < n; ++u) id[u] += component.size();
        reverse(begin(component), end(component));
    }//}}}

    vector<vector<int>> g;
    vector<vector<int>> build_supar_graph(){//{{{
        if(component.empty()) run();
        g.assign(component.size(), vector<int>());
        vector<int> used(component.size());
        for(int i = 0; i < component.size(); ++i){
            ++used[i];
            for(auto &u : component[i]) for(auto &v : _g[u]) ++used[id[v]];
            for(auto &u : component[i]) for(auto &v : _g[u]) if(!--used[id[v]])
                g[i].emplace_back(id[v]);
            --used[i];
        }
        return g;
    }//}}}
};//}}}

bool solve(){
    int n, m;
    cin >> n >> m;
    StronglyConnectedComponents scc(n);
    rep(_, m){
        int s, t; cin >> s >> t;
        scc.add_edge(s, t);
    }
    scc.run();
    int q; cin >> q;
    rep(_, q){
        int u, v; cin >> u >> v;
        cout << (scc.id[u] == scc.id[v]) << endl;
    }
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
