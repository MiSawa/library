#include <bits/stdc++.h>
#define all(x) begin(x),end(x)
#define rall(x) (x).rbegin(),(x).rend()
#define REP(i,b,n) for(int i=(b);i<(int)(n);++i)
#define rep(i,n) REP(i,0,n)
#define repsz(i,v) rep(i,(v).size())
#define rrep(i,n) for(int i=(int)(n)-1;i>=0;--i)
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

// verified:
//   AOJ GRL_1_A (Single Source Shortest Path)
//       GRL_1_B (Single Source Shortest Path (Negative Edges))
//       GRL_1_C (All Pairs Shortest Path)
// Graph g(n);
// g.add_edge(u, v, c);
// vector<Cost> g.dijkstra(s)
// vector<Cost> g.spfa(s, inf = numeric_limits<Cost>::max()/4)
// vector<vector<Cost>> g.warshall_floyd(inf = numeric_limits<Cost>::max()/4)
// spfa は, 到達不可能: inf, 到達可能な負閉路: -inf, その他: 有限.
// warshall_floyd は, 到達不可能: inf, 可能で負閉路を通れる: -inf, その他: 有限.

struct Graph{//{{{
    typedef long long Cost;

    struct ShortestPathEdge{//{{{
        int t;
        Cost c;
        ShortestPathEdge(const int s, const int t, const Cost c) : t(t), c(c){}
    };//}}}
    typedef ShortestPathEdge Edge;

    vector<vector<Edge>> g;
    Graph(const int n) : g(n){}

    vector<Edge> &operator[](const size_t u){ return g[u]; }

    template<class ...Args>
    inline void add_edge(const int u, const int v, const Args &...args){
        g[u].emplace_back(u, v, args...);
    }

    int size() const { return g.size(); }

    vector<Cost> dijkstra(const int s){//{{{
        vector<Cost> d(g.size(), numeric_limits<Cost>::max());
        priority_queue<pair<Cost, int>, vector<pair<Cost, int>>, greater<pair<Cost, int>>> pq;
        pq.emplace(d[s] = 0, s);
        while(!pq.empty()){
            Cost c; int u; tie(c, u) = pq.top(); pq.pop();
            if(c > d[u]) continue;
            for(auto &e : g[u]) if(d[e.t] > c + e.c)
                pq.emplace(d[e.t] = c + e.c, e.t);
        }
        return d;
    }//}}}
    vector<Cost> spfa(const int s, const Cost inf = numeric_limits<Cost>::max()/4){//{{{
        vector<Cost> d(g.size(), numeric_limits<Cost>::max());
        vector<int> cnt(g.size(), 0), in_q(g.size(), false);
        queue<int> q;
        auto push = [&](const int u, const Cost c){
            if(d[u] <= c) return;
            d[u] = c;
            if(in_q[u] or cnt[u] == g.size()+2) return;
            ++cnt[u];
            q.emplace(u);
            in_q[u] = true;
        };
        push(s, 0);
        while(!q.empty()){
            int u = q.front(); q.pop(); in_q[u] = false;
            for(auto &e : g[u]) push(e.t, d[u] + e.c);
        }
        for(int u = 0; u < g.size(); ++u) if(cnt[u] == g.size()+2) d[u] = -inf;
        return d;
    }//}}}
    vector<vector<Cost>> warshall_floyd(const Cost inf = numeric_limits<Cost>::max()/4){//{{{
        const int n = g.size();
        vector<vector<Cost>> d(n, vector<Cost>(n, inf));
        for(int u = 0; u < n; ++u) d[u][u] = 0;
        for(int u = 0; u < n; ++u) for(auto &e : g[u]) d[u][e.t] = min(d[u][e.t], e.c);
        for(int w = 0; w < n; ++w) for(int u = 0; u < n; ++u) for(int v = 0; v < n; ++v)
            if(d[u][w] != inf and d[w][v] != inf and d[u][v] > d[u][w] + d[w][v])
                d[u][v] = d[u][w] + d[w][v];
        for(int w = 0; w < n; ++w) for(int u = 0; u < n; ++u) for(int v = 0; v < n; ++v)
            if(d[u][w] != inf and d[w][v] != inf and d[u][v] > d[u][w] + d[w][v])
                d[u][v] = -inf;
        return d;
    }//}}}
};//}}}

bool solve(){
    int n, m, root; cin >> n >> m >> root;
    Graph g(n);
    rep(_, m){
        int s, t, d; cin >> s >> t >> d;
        g.add_edge(s, t, d);
    }
    auto d = g.spfa(root);
    if(*min_element(all(d)) < -(1LL<<40)){
        cout << "NEGATIVE CYCLE" << endl;
        return true;
    }
    rep(u, n){
        if(d[u] > (1LL<<40)) cout << "INF" << endl;
        else cout << d[u] << endl;
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
