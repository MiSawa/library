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


struct NULLSTREAM : ostream{
    template<class T>
    friend NULLSTREAM &operator<<(NULLSTREAM &nulls, const T &){ return nulls; }
} nullstream;
// tr << ( vector, map, pair, tuple, etc... ) << endl//{{{
template <typename T, typename U>
struct sfinae_helper{typedef void type;};
template <typename T, typename U = void>
struct Print{ static ostream &pp(ostream &os, const T &x){ return os << x; } };
struct trace{//{{{
    ostream& os;
    trace(ostream& os): os(os) { }
    template<typename T>
        trace& operator<<(const T& x){ Print<T>::pp(os, x); return *this;}
    trace &operator<<(ostream& f(ostream&)){ f(os); return *this; }
    operator ostream &(){ return os; }
};//}}}
// string//{{{
template <> struct Print<string, void>{ static ostream &pp(ostream &os, const string &x){ return os << x; } };
//}}}
// Container//{{{
template <typename T> struct Print<T, typename sfinae_helper<T, typename T::iterator>::type>{
    static ostream &pp(ostream &os, const T &v){
        trace(os) << '[';
        for(const auto &x : v) trace(os) << x << ", ";
        return trace(os) << ']';
    }
};//}}}
// Pair//{{{
template <typename T> struct Print<T, typename sfinae_helper<T, typename T::first_type>::type>{
    static ostream &pp(ostream &os, const T &x){
        return trace(os) << '(' << x.first << ", " << x.second << ')';
    }
};//}}}
// Tuple//{{{
template<class T, size_t N> struct TuplePrint{
    static ostream &print(ostream &os, const T &x){
        TuplePrint<T, N-1>::print(os, x) << ", ";
        return trace(os) << get<N-1>(x);
        //return Print<decltype(get<N-1>(x))>::pp(os, get<N-1>(x));
    }
};
template<class T> struct TuplePrint<T, 1>{
    static ostream &print(ostream &os, const T &x){
        return trace(os) << get<0>(x);
    }
};
template <typename... Args>//Tuple
struct Print<tuple<Args...>>{
    static ostream &pp(ostream &os, const tuple<Args...> &x){
        os << "(";
        return TuplePrint<decltype(x), sizeof...(Args)>::print(os, x) << ")";
    }
};//}}}
//}}}
trace tr(cerr);

template<class T>
struct UnionFind{//{{{
    mutable vector<int> p;
    vector<T> val;
    UnionFind(const int n) : p(n, -1), val(n){}
    int find(const int u) const {
        return p[u] < 0 ? u : p[u] = find(p[u]);
    }
    inline bool same(const int u, const int v) const {
        return find(u) == find(v);
    }
    inline bool unite(int u, int v){
        if((u = find(u)) == (v = find(v))) return false;
        if(p[u] > p[v]){
            swap(u, v);
            swap(val[u], val[v]);
        }
        p[u] += p[v];
        p[v] = u;
        return true;
    }
    inline T &operator[](const int u){ return val[find(u)]; }
    inline const T &operator[](const int u) const { return val[find(u)]; }
    friend ostream &operator<<(ostream &os, const UnionFind &uf){
        for(int u = 0; u < uf.p.size(); ++u) if(uf.p[u] < 0){
            os << "(" << uf[u] << " : ";
            for(int v = 0; v < uf.p.size(); ++v) if(uf.same(u, v))
                os << v << ", ";
            os << "), ";
        }
        return os;
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
    int phase = 0;
    bool augment(){//{{{
        bool dbg = false;
        if(++phase == 4) dbg = true;
        const int n = g.size();
        vector<int> parity(n, -1);
        // TODO
        // vector<int> par(n, -1); // par: 木での親
        map<int, int> par; rep(i, n) par[i] = -1;

        UnionFind<int> uf(n);   // 縮約したやつら
        for(int u = 0; u < n; ++u) uf[u] = u;
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
        // TODO
        // vector<vector<int>> succ(n, vector<int>(2, -1));
        map<int, vector<int>> succ; rep(i, n) succ[i] = vector<int>(2, -1);

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
            if(dbg){
                tr << "match = " << match << endl;
                tr << "par   = " << par << endl;
                tr << "succ  = " << succ << endl;
                tr << "uf    = " << uf << endl;
            }
            int u = even_que.back(); even_que.pop_back();
            for(int v : g[u]){
                if(match[u] == v or uf.same(u, v)) continue;
                tr << "choise " << u << ", " << v << endl;
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
                        tr << "found path " << aug_path << endl;
                        assert(aug_path.size() % 2 == 0);
                        for(int i = 0; i < aug_path.size(); i += 2){
                            match[aug_path[i]] = aug_path[i+1];
                            match[aug_path[i+1]] = aug_path[i];
                        }
                        return true;
                    }//}}}
                    // 縮約する
                    tr << "contract " << u << " " << v << " ( lca = " << w << " )" << endl;
                    for(int _ = 0; _ < 2; ++_, swap(u, v)){
                        if(par[u] != -1 and succ[u][0] == -1)
                        //if(par[u] != -1)
                            succ[u][0] = v;
                    }
                    for(int _ = 0; _ < 2; ++_, swap(u, v)){//{{{
                        for(int p = uf[u]; p != w; p = uf[par[p]]){
                            cout << p << ", ";
                            //if(par[par[p]] != -1)
                            if(par[par[p]] != -1 and succ[par[p]][match[p] == par[p]] == -1)
                                succ[par[p]][match[p] == par[p]] = p;
                            if(parity[p] == 1){
                                parity[p] = 0;
                                even_que.emplace_back(p);
                            }
                        }
                    }//}}}
                    for(int _ = 0; _ < 2; ++_, swap(u, v))
                        for(int p = uf[u]; !uf.same(p, w); p = uf[par[p]]) uf.unite(p, w);
                    uf.unite(u, w);
                    uf.unite(u, v);
                    uf[w] = w;
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
