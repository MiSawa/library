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

    vector<int> match;
    bool augment(){//{{{
        const int n = g.size();
        vector<int> parity(n, -1);
        vector<int> par(n, -1); // par: 木での親
        UnionFind uf(n);        // 縮約したやつら
        vector<int> even_que;
        // succ[u][0] != match[u],
        // succ[u][1] == match[u] or -1
        // invariants:
        //  - succ != -1 なら, これで交互パスを取って root に行ける.
        //    つまり, succ[u][t] != -1 なら,
        //      succ[u][t], succ[succ[u][t]][!t], ...
        //    で root にたどり着ける.
        //
        //  - parity[u] == 0 ならば, u : root or succ[u][1] != -1
        //
        vector<vector<int>> succ(n, vector<int>(2, -1));
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
                    succ[v][0] = u;
                    succ[w][1] = v;
                    even_que.emplace_back(w);
                }else if(parity[v] == 0){
                    // この if 文の中身は O(n) で動く.
                    const int w = lca(u, v);
                    if(w == -1){//{{{
                        // found!!!
                        assert(par[u] == -1 or succ[u][1] != -1);
                        assert(par[v] == -1 or succ[v][1] != -1);
                        vector<int> aug_path;
                        for(int p = u, t = 1; p != -1; p = succ[p][t], t ^= true)
                            aug_path.emplace_back(p);
                        reverse(begin(aug_path), end(aug_path));
                        for(int p = v, t = 1; p != -1; p = succ[p][t], t ^= true)
                            aug_path.emplace_back(p);
                        assert(aug_path.size() % 2 == 0);
                        for(int i = 0; i < aug_path.size(); i += 2){
                            match[aug_path[i]] = aug_path[i+1];
                            match[aug_path[i+1]] = aug_path[i];
                        }
                        return true;
                    }//}}}
                    // 縮約する
                    assert(uf.unite(u, v)); // ここに来るのは O(n) 回.
                    for(int _ = 0; _ < 2; ++_, swap(u, v)){//{{{
                        if(succ[u][0] == -1) succ[u][0] = v;
                        for(int p = u; p != w; p = par[p]){
                            uf.unite(p, w);
                            if(succ[par[p]][match[p] == par[p]] == -1)
                                succ[par[p]][match[p] == par[p]] = p;
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


bool solve(){
    int n, m; cin >> n >> m;
    Graph g(2 * n);
    rep(_, m){
        int u, v; cin >> u >> v; --u; --v;
        if(u > v) swap(u, v);
        g.add_edge(u, v);
        if(u) g.add_edge(u+n, v+n);
        else  g.add_edge(v, n);
    }
    if(g.maximum_matching() == n) cout << "Yes" << endl;
    else cout << "No" << endl;
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
