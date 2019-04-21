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

struct UnionFind{//{{{
    mutable vector<int> p;
    UnionFind(const int n) : p(n, -1){}
    int find(const int u) const {
        return p[u] < 0 ? u : p[u] = find(p[u]);
    }
    inline bool same(const int u, const int v) const {
        return find(u) == find(v);
    }
    inline bool unite(int u, int v){
        if((u = find(u)) == (v = find(v))) return false;
        if(p[u] > p[v]) swap(u, v);
        p[u] += p[v];
        p[v] = u;
        return true;
    }
};//}}}

struct Graph{//{{{
    vector<vector<int>> g;
    Graph(const int n) : g(n) {}
    void add_edge(const int u, const int v){//{{{
        g[u].emplace_back(v);
        g[v].emplace_back(u);
    }//}}}

    deque<int> rec_path(int u, int v, bool start, const vector<vector<pair<int, int>>> &succ){//{{{
        if(u == v) return deque<int>(1, u);
        if(succ[u][start].first == u){
            auto p = rec_path(succ[u][start].second, v, !start, succ);
            p.emplace_front(u);
            return p;
        }
        int x, y; tie(x, y) = succ[u][start];
        auto p1 = rec_path(x, u, true, succ);
        auto p2 = rec_path(y, v, true, succ);
        reverse(begin(p1), end(p1));
        for(auto &t : p2) p1.emplace_back(t);
        return p1;
    }//}}}

    vector<int> match;
    bool augment(){//{{{
        const int n = g.size();
        vector<int> parity(n, -1);
        vector<int> par(n, -1); // par: 木での親
        UnionFind uf(n);        // 縮約したやつら
        vector<int> even_que;
        // succ[u][flg] : u から flg で始まる交互パスで root に行けるようになったきっかけの辺
        vector<vector<pair<int, int>>> succ(n, vector<pair<int, int>>(2, pair<int, int>(-1, -1)));
        auto lca = [&](int u, int v){//{{{
            assert(u != v);
            vector<int> up, vp;
            for(int p = u; p != -1; p = par[p]) up.emplace_back(p);
            for(int p = v; p != -1; p = par[p]) vp.emplace_back(p);
            if(up.back() != vp.back()) return -1;
            if(up.size() > vp.size()){
                swap(u, v);
                swap(up, vp);
            }
            reverse(begin(up), end(up));
            reverse(begin(vp), end(vp));
            if(vp[up.size()-1] == up.back()) return u;
            for(int i = 1; i < up.size(); ++i) if(up[i] != vp[i])
                return up[i-1];
        };//}}}
        for(int u = 0; u < n; ++u) if(match[u] == -1){//{{{
            parity[u] = 0;
            even_que.emplace_back(u);
        }//}}}
        while(!even_que.empty()){//{{{
            int u = even_que.back(); even_que.pop_back();
            for(int v : g[u]){
                if(match[u] == v or uf.same(u, v)) continue;
                if(parity[v] == -1){
                    // 生やす
                    const int w = match[v];
                    par[v] = u; parity[v] = 1;
                    par[w] = v; parity[w] = 0;
                    succ[v][0] = make_pair(v, u);
                    succ[w][1] = make_pair(w, v);
                    even_que.emplace_back(w);
                }else if(parity[v] == 0){
                    // この if 文の中身は O(n) で動く.
                    const int w = lca(u, v);
                    if(w == -1){//{{{
                        // found!!!

                        int ru = u, rv = v;
                        while(par[ru] != -1) ru = par[ru];
                        while(par[rv] != -1) rv = par[rv];
                        auto pu = rec_path(u, ru, true, succ);
                        auto pv = rec_path(v, rv, true, succ);
                        reverse(begin(pu), end(pu));
                        for(auto t : pv) pu.emplace_back(t);
                        assert(pu.size() % 2 == 0);
                        for(int i = 0; i < pu.size(); i += 2){
                            match[pu[i]] = pu[i+1];
                            match[pu[i+1]] = pu[i];
                        }
                        return true;
                    }//}}}
                    // 縮約する
                    assert(uf.unite(u, v)); // ここに来るのは O(n) 回.
                    for(int _ = 0; _ < 2; ++_, swap(u, v)){//{{{
                        if(succ[u][0].first == -1) succ[u][0] = make_pair(u, v);
                        for(int p = u; p != w; p = par[p]){
                            uf.unite(p, w);
                            if(succ[par[p]][match[p] == par[p]].first == -1)
                                succ[par[p]][match[p] == par[p]] = make_pair(u, v);
                            if(parity[p] == 1){
                                parity[p] = 0;
                                even_que.emplace_back(p);
                            }
                        }
                    }//}}}
                }
            }
        }//}}}
        return false;
    }//}}}
    int maximum_matching(){//{{{
        const int n = g.size();
        match.assign(n, -1);
        int res = 0;
        while(augment()) ++res;
        return res;
    }//}}}
};//}}}


signed main(){
    int n, m; cin >> n >> m;
    Graph g(n);
    rep(_, m){
        int u, v; cin >> u >> v;
        g.add_edge(u, v);
    }
    g.maximum_matching();
    rep(u, n) cout << g.match[u] << endl;
    return 0;
}
// vim:set foldmethod=marker commentstring=//%s:
