#include <bits/stdc++.h>
#define all(x) begin(x),end(x)
#define rall(x) (x).rbegin(),(x).rend()
#define REP(i,b,n) for(int i=(int)(b);i<(int)(n);++i)
#define rep(i,n) REP(i,0,n)
#define rrep(i,n) for(int i=(int)(n)-1;i>=0;--i)
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
template<class T,class ...U>bool chmin(T&a, const U&...bs){const T b(bs...);if(a<=b)return false;a=b;return true;}
template<class T,class ...U>bool chmax(T&a, const U&...bs){const T b(bs...);if(a>=b)return false;a=b;return true;}

struct Dinic{//{{{
    typedef int Cap;
    static const Cap INF = 1<<29;
    struct E{//{{{
        int dst;
        Cap cap;
        int rev;
        E(int dst, Cap cap, int rev) : dst(dst), cap(cap), rev(rev) {}
    };//}}}
    int n;
    vector<vector<E> > g;
    Dinic(int n) : n(n), g(n) {}
    inline void add_edge(int src, int dst, Cap cap){//{{{
        if(src == dst) return;
        g[src].push_back(E(dst,cap,g[dst].size()));
        g[dst].push_back(E(src, 0 ,g[src].size() - 1));
    }//}}}
    inline void add_undirected_edge(int src, int dst, Cap cap){//{{{
        if(src == dst) return;
        g[src].push_back(E(dst,cap,g[dst].size()));
        g[dst].push_back(E(src,cap,g[src].size() - 1));
    }//}}}

    vector<int> level, iter;
    Cap dfs(const int &s, const int &u, Cap crr){//{{{
        if(s == u || crr == 0) return crr;
        Cap sum = 0;
        for(int &i = iter[u]; i < g[u].size(); ++i){
            E &e = g[u][i], &ee = g[e.dst][e.rev];
            const int &v = e.dst; // s -- v - u -- t
            if(level[v] >= level[u] || ee.cap <= 0) continue;
            Cap f = dfs(s, v, min(crr - sum, ee.cap));
            if(f <= 0) continue;
            ee.cap -= f; e.cap += f;
            sum += f;
            if(sum == crr) break;
        }
        return sum;
    }//}}}
    Cap run(int s, int t){//{{{
        vector<int> q(n);
        for(Cap flow = 0; ;){
            level.assign(n, -1);
            int ql = 0, qr = 0;
            level[q[qr++] = s] = 0;
            while(ql != qr && level[t] == -1){
                int u = q[ql++];
                for(auto &e : g[u]) if(e.cap > 0 && level[e.dst] == -1)
                    level[ q[qr++] = e.dst ] = level[u] + 1;
            }
            if(level[t] == -1) return flow;
            iter.assign(n, 0);
            flow += dfs(s, t, INF);
        }
    }//}}}
};//}}}

struct E{ int u, v, cap; };

bool solve(){
    int k, n, m;
    cin >> k >> n >> m;
    const int S = n, T = S+1;
    auto toId = [&](int u){
        if(u == 0) return T;
        if(u <= k) return S;
        return u - k - 1;
    };
    vector<E> es;
    rep(_, m){
        int u, v, cap;
        cin >> u >> v >> cap;
        u = toId(u);
        v = toId(v);
        if(u > v) swap(u, v);
        if(u != v) es.emplace_back(E{u, v, cap});
    }
    for(auto &e : es) if(e.u == S and e.v == T){
        cout << "overfuro" << endl;
        return true;
    }
    int res = 0;
    vector<int> fromS(n+2), toT(n+2);
    fromS[S] = toT[T] = 1<<30;
    rep(u, n){
        Dinic mf(n+2);
        for(auto &e : es) mf.add_undirected_edge(e.u, e.v, e.cap);
        res = mf.run(S, T);
        fromS[u] = mf.run(S, u);
        toT[u] = mf.run(u, T);
    }
    int mx = 0;
    for(auto &e : es) chmax(mx, min(fromS[e.u], toT[e.v]));
    for(auto &e : es) chmax(mx, min(fromS[e.v], toT[e.u]));
    cout << res + mx << endl;
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
