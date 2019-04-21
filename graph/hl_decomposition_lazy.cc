// verified:
// https://www.hackerrank.com/contests/world-codesprint-april/challenges/fibonacci-numbers-tree

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
template<class T,class U>bool chmin(T&a,const U&b){if(a<=b)return false;a=b;return true;}
template<class T,class U>bool chmax(T&a,const U&b){if(a>=b)return false;a=b;return true;}

void reader(char &c){ scanf(" %c", &c); }
void reader(int &x){ scanf("%d", &x); }
void reader(long long &x){ scanf("%lld", &x); }
template<class S, class T, class ...U> void reader(S &x, T &y, U &...u){ reader(x); reader(y, u...); }

const int MOD = (int)(1E9+7);
struct MOD_INT{//{{{
    int x;
    MOD_INT(const int& x=0): x((x%MOD+MOD)%MOD){}
    MOD_INT(const ll& x): x((x%MOD+MOD)%MOD){}
    MOD_INT(const MOD_INT& o): x(o.x){}
    operator int(){return x;}
    MOD_INT pow(int e){
        MOD_INT res(1), b(*this);
        for(; e; e>>=1, b=b*b) if(e&1) res = res * b;
        return res;
    }
    MOD_INT inv(){ return pow(MOD-2); }
    template<class T> MOD_INT operator+(T o){return MOD_INT(x+MOD_INT(o).x);}
    template<class T> MOD_INT operator-(T o){return MOD_INT(x-MOD_INT(o).x);}
    template<class T> MOD_INT operator*(T o){return MOD_INT(((ll)x*MOD_INT(o).x));}
    template<class T> MOD_INT operator/(T o){return *this*MOD_INT(o).inv();}
    friend ostream &operator<<(ostream &os, const MOD_INT &x){ return os << x.x; }
};
#define DEFOP(OP, OPEQ) \
template<class T> MOD_INT& operator OPEQ(MOD_INT& a, T o){return a=a OP o;}\
template<class T> MOD_INT operator OP(T a, MOD_INT b){return MOD_INT(a).operator OP(b);}
DEFOP(+, +=); DEFOP(-, -=); DEFOP(*, *=); DEFOP(/, /=);
#undef DEFOP
//}}}
typedef MOD_INT mi;

template<class R, size_t n> struct Vec{//{{{
    array<R, n> v = {};

    Vec() : v({}){}
    Vec(const initializer_list<R> v) : v({}) {
        size_t i = 0;
        for(auto it = begin(v); it != end(v) and i < n; ++i, ++it) this->v[i] = *it;
    }
    const R operator[](const size_t &i) const { return v[i]; }
    R &operator[](const size_t &i) { return v[i]; }
    Vec &operator+=(const Vec &other){
        for(size_t i = 0; i < n; ++i) (*this)[i] += other[i];
        return *this;
    }
    Vec &operator-=(const Vec &other){
        for(size_t i = 0; i < n; ++i) (*this)[i] -= other[i];
        return *this;
    }
    friend Vec operator+(Vec a, const Vec &b){ return a += b; }
    friend Vec operator-(Vec a, const Vec &b){ return a -= b; }
    friend R dot(const Vec &a, const Vec &b){
        R res = 0;
        for(size_t i = 0; i < n; ++i) res += a[i] * b[i];
        return res;
    }

    friend ostream &operator<<(ostream &os, const Vec &v){
        for(size_t i = 0; i < n; ++i) os << (i == 0 ? "[" : ", ") << v[i];
        return os << "]";
    }
};//}}}
template<class R, size_t n> struct Mat{//{{{
    array<Vec<R, n>, n> A = {};

    Mat() : A({}){}
    Mat(const initializer_list<initializer_list<R>> A) : A({}){
        size_t i = 0;
        for(auto it = begin(A); it != end(A) and i < n; ++i, ++it) this->A[i] = *it;
    }
    static const Mat zero(){
        return Mat();
    }
    static const Mat identity(){
        Mat res = zero();
        for(size_t i = 0; i < n; ++i) res[i][i] = 1;
        return res;
    }

    const Vec<R, n> operator[](const size_t &i) const { return A[i]; }
    Vec<R, n> &operator[](const size_t &i) { return A[i]; }
    Mat &operator+=(const Mat &other){
        for(size_t i = 0; i < n; ++i) (*this)[i] += other[i];
        return *this;
    }
    Mat &operator-=(const Mat &other){
        for(size_t i = 0; i < n; ++i) (*this)[i] -= other[i];
        return *this;
    }
    friend Mat operator+(Mat a, const Mat &b){ return a += b; }
    friend Mat operator-(Mat a, const Mat &b){ return a -= b; }

    friend Mat operator*(const Mat &a, const Mat &b){
        Mat c = zero();
        for(size_t k = 0; k < n; ++k) for(size_t i = 0; i < n; ++i) for(size_t j = 0; j < n; ++j)
            c[i][j] += a[i][k] * b[k][j];
        return c;
    }
    friend Vec<R, n> operator*(const Mat &a, const Vec<R, n> &b){
        Vec<R, n> c;
        for(size_t k = 0; k < n; ++k) for(size_t i = 0; i < n; ++i) c[i] += a[i][k] * b[k];
        return c;
    }
    friend Vec<R, n> operator*(const Vec<R, n> &a, const Mat &b){
        Vec<R, n> c;
        for(size_t k = 0; k < n; ++k) for(size_t j = 0; j < n; ++j) c[j] += a[k] * b[k][j];
        return c;
    }
    Mat &operator*=(const Mat &other){ return *this = *this * other; }

    Mat pow(const size_t &e) const {
        Mat res = identity(), b = *this;
        for(; e; e >>= 1, b *= b) if(e&1) res *= b;
        return res;
    }

    friend ostream &operator<<(ostream &os, const Mat &m){
        for(size_t i = 0; i < n; ++i) os << (i == 0 ? "[" : ", ") << m[i];
        return os << "]";
    }
};//}}}
template<class Z> struct Fib{//{{{
    constexpr static size_t memo = 64;
    using M = Mat<Z, 2>;
    array<M, memo> powers;
    Fib(){
        powers[0] = {{1, 1}, {1, 0}};
        for(size_t i = 1; i < memo; ++i) powers[i] = powers[i-1] * powers[i-1];
    }
    M get_mat(long long e) const {
        M ret = M::identity();
        for(size_t i = 0; e; ++i, e >>= 1) if(e&1) ret *= powers[i];
        return ret;
    }
    Z operator[](long long i) const {
        return get_mat(i)[1][0];
    }
};//}}}
Fib<mi> fib;

struct HLDecomposition{//{{{
    struct NodeInfo{
        int par, depth, spar, path, pos, heavy;
        NodeInfo() : par(-1), depth(0), spar(-1), path(-1), pos(0), heavy(-1) {}
    };
    vector<NodeInfo> nodes;
    vector<vector<int>> g;
    vector<vector<int>> paths;

    void add_edge(int u, int v){
        if(g.size() <= u) g.resize(u+1);
        if(g.size() <= v) g.resize(v+1);
        g[u].emplace_back(v);
        g[v].emplace_back(u);
    }
    size_t size() const { return g.size(); }
    void build(const int root = 0){//{{{
        const int n = size();
        nodes.assign(n, NodeInfo());
        vector<int> ord(n);
        ord[0] = root;
        for(int i = 0, j = 1; i != j; ++i){
            const int u = ord[i];
            for(auto v : g[u]) if(v != nodes[u].par){
                ord[j++] = v;
                nodes[v].par = u;
                nodes[v].depth = nodes[u].depth + 1;
            }
        }
        const vector<int> rev(ord.rbegin(), ord.rend());
        vector<int> cnt(n, 1);
        for(auto u : rev) if(u != root) cnt[nodes[u].par] += cnt[u];
        for(auto u : ord){
            if(nodes[u].path == -1){
                nodes[u].path = paths.size();
                paths.emplace_back(0);
                nodes[u].spar = nodes[u].par;
            }else{
                nodes[u].spar = nodes[nodes[u].par].spar;
            }
            nodes[u].pos = paths[nodes[u].path].size();
            paths[nodes[u].path].emplace_back(u);

            int c = 0;
            for(auto v : g[u]) if(v != nodes[u].par) c = max(c, cnt[v]);
            for(auto v : g[u]) if(v != nodes[u].par and c == cnt[v]) nodes[u].heavy = v;
            if(nodes[u].heavy != -1) nodes[nodes[u].heavy].path = nodes[u].path;
        }
    }//}}}

    int lca(int u, int v) const {//{{{
        while(nodes[u].path != nodes[v].path){
            int pu = nodes[u].spar, pv = nodes[v].spar;
            if((pu == -1 ? -1 : nodes[pu].depth) > (pv == -1 ? -1 : nodes[pv].depth)){
                u = pu;
            }else{
                v = pv;
            }
        }
        return nodes[u].depth > nodes[v].depth ? v : u;
    }//}}}
    struct Path{//{{{
        int l, r;
        Path(int l, int r) : l(l), r(r) {}
    };//}}}
    int distance(int u, int v) const {//{{{
        return nodes[u].depth + nodes[v].depth - 2 * nodes[lca(u, v)].depth;
    }//}}}

    vector<Path> get_path(int u, int v, char isEdgeQuery){//{{{
        vector<Path> res, rev;
        while(nodes[u].path != nodes[v].path){
            int pu = nodes[u].spar, pv = nodes[v].spar;
            if((pu == -1 ? -1 : nodes[pu].depth) > (pv == -1 ? -1 : nodes[pv].depth)){
                res.emplace_back(u, paths[nodes[u].path].front());
                u = pu;
            }else{
                rev.emplace_back(paths[nodes[v].path].front(), v);
                v = pv;
            }
        }
        if(isEdgeQuery){
            if(nodes[u].depth > nodes[v].depth) res.emplace_back(u, nodes[v].heavy);
            if(nodes[u].depth < nodes[v].depth) res.emplace_back(v, nodes[u].heavy);
        }else{
            res.emplace_back(u, v);
        }
        res.insert(res.end(), rev.rbegin(), rev.rend());
        return res;
    }//}}}
};//}}}

struct HandlerAdd{//{{{
    using val_type = Vec<mi, 2>;
    using opr_type = Vec<mi, 2>;
    using Path = HLDecomposition::Path;
    HLDecomposition &hl;
    HandlerAdd(HLDecomposition &hl) : hl(hl){}

    val_type default_value() const {
        return val_type();
    }
    val_type default_value(const int u) const {
        return val_type();
    }
    opr_type push(const opr_type &old, const opr_type &op, const Path &path){
        return old + op;
    }
    val_type resolve(const val_type &val, const opr_type &op, const Path &path){
        return val + op;
    }
    val_type merge(const val_type &lv, const Path &l, const val_type &rv, const Path &r){
        return lv + rv;
    }
};//}}}
struct HandlerMul{//{{{
    using val_type = Vec<mi, 2>;
    using opr_type = Vec<mi, 2>;
    using Path = HLDecomposition::Path;
    HLDecomposition &hl;
    HandlerMul(HLDecomposition &hl) : hl(hl){}

    val_type default_value() const {
        return val_type();
    }
    val_type default_value(const int u) const {
        return val_type();
    }
    opr_type push(const opr_type &old, const opr_type &op, const Path &path){
        assert(path.l == path.r);
        return old + op;
    }
    val_type resolve(const val_type &val, const opr_type &op, const Path &path){
        assert(path.l == path.r);
        return val + op;
    }
    val_type merge(const val_type &lv, const Path &l, const val_type &rv, const Path &r){
        return fib.get_mat(hl.distance(r.l, r.r)+1) * lv + rv;
    }
};//}}}

template<class Handler>
struct HLLazySegTree{//{{{
    using val_type = typename Handler::val_type;
    using opr_type = typename Handler::opr_type;
    using Path = HLDecomposition::Path;

    struct LazySegtree{//{{{
        static int calc_n(const int size){//{{{
            int n = 1;
            while(n < size) n <<= 1;
            return n;
        }//}}}
        struct LazySegtreeNode{
            val_type val;
            opr_type lazy;
            char has_lazy;
            LazySegtreeNode(Handler &hdl) : val(hdl.default_value()), lazy(), has_lazy(false) {}
        };
        const int offset;
        vector<int> path;
        Handler &hdl;
        vector<LazySegtreeNode> tree;
        LazySegtree(const vector<int> &path, Handler &hdl) : offset(calc_n(path.size())-1), path(path), hdl(hdl){//{{{
            const int n = calc_n(path.size());
            tree.assign(n * 2, hdl);
            for(int i = 0; i < path.size(); ++i) tree[offset + i].val = hdl.default_value(path[i]);
            build(0, path.size(), 0, 0, n);
        }//}}}

        Path get_path(const int ll, const int rr){//{{{
            return Path(path[ll], path[min(rr, (int)path.size())-1]);
        }//}}}
        Path get_path(const int l, const int r, const int ll, const int rr){//{{{
            return Path(path[max(l, ll)], path[min(r, rr) - 1]);
        }//}}}
        val_type build(const int l, const int r, const int k, const int ll, const int rr){//{{{
            if(rr - ll == 1) return tree[k].val;
            const int mm = (ll + rr) >> 1;
            if(r <= mm || mm <= l)
                return tree[k].val = build(l, r, k*2 + (r <= mm ? 1 : 2), ll, mm);
            const auto lv = build(l, r, k*2+1, ll, mm);
            const auto rv = build(l, r, k*2+2, mm, rr);
            return tree[k].val = hdl.merge(
                lv, get_path(l, r, ll, mm),
                rv, get_path(l, r, mm, rr));
        }//}}}

        void push_down(const int k, const int ll, const int rr){//{{{
            if(!tree[k].has_lazy) return;
            tree[k].val = hdl.resolve(tree[k].val, tree[k].lazy, get_path(ll, rr));
            tree[k].has_lazy = false;
            if(k >= offset) return;
            const int mm = (ll + rr) >> 1;
            if(tree[2*k+1].has_lazy) tree[2*k+1].lazy = hdl.push(tree[2*k+1].lazy, tree[k].lazy, get_path(ll, mm));
            else tree[2*k+1].lazy = tree[k].lazy;
            if(tree[2*k+2].has_lazy) tree[2*k+2].lazy = hdl.push(tree[2*k+2].lazy, tree[k].lazy, get_path(mm, rr));
            else tree[2*k+2].lazy = tree[k].lazy;
            tree[2*k+1].has_lazy = tree[2*k+2].has_lazy = true;
        }//}}}
        void update(const int l, const int r, const opr_type &op){//{{{
            update(l, r, op, 0, 0, offset + 1);
        }//}}}
        const val_type &update(const int l, const int r, const opr_type &op, const int k, const int ll, const int rr){//{{{
            push_down(k, ll, rr);
            if(r <= ll || rr <= l) return tree[k].val;
            if(l <= ll && rr <= r){
                tree[k].lazy = op;
                tree[k].has_lazy = true;
                push_down(k, ll, rr);
                return tree[k].val;
            }
            const int mm = (ll + rr) >> 1;
            const auto &lv = update(l, r, op, k*2+1, ll, mm);
            if(path.size() <= mm) return tree[k].val = lv;
            const auto &rv = update(l, r, op, k*2+2, mm, rr);
            return tree[k].val = hdl.merge(
                lv, get_path(ll, mm),
                rv, get_path(mm, rr));
        }//}}}
        val_type query(const int l, const int r){//{{{
            return query(l, r, 0, 0, offset + 1);
        }//}}}
        const val_type query(const int l, const int r, const int k, const int ll, const int rr){//{{{
            if(r <= ll || rr <= l) return hdl.default_value();
            push_down(k, ll, rr);
            if(l <= ll && rr <= r) return tree[k].val;
            const int mm = (ll + rr) >> 1;
            if(r <= mm) return query(l, r, k*2+1, ll, mm);
            if(l >= mm) return query(l, r, k*2+2, mm, rr);
            return hdl.merge(
                query(l, r, k*2+1, ll, mm), get_path(l, r, ll, mm),
                query(l, r, k*2+2, mm, rr), get_path(l, r, mm, rr));
        }//}}}
    };//}}}

    Handler &hdl;
    vector<LazySegtree> segs;
    HLLazySegTree(Handler &hdl) : hdl(hdl){//{{{
        for(auto &path : hdl.hl.paths){
            vector<int> rpath(path.rbegin(), path.rend());
            segs.emplace_back(path, hdl);
            segs.emplace_back(rpath, hdl);
        }
    }//}}}

    void update(int u, int v, const opr_type &op, bool isEdgeQuery){//{{{
        auto ps = hdl.hl.get_path(u, v, isEdgeQuery);
        for(const auto &p : ps){
            const int id = hdl.hl.nodes[p.l].path;
            const int plen = hdl.hl.paths[id].size();
            int l = hdl.hl.nodes[p.l].pos;
            int r = hdl.hl.nodes[p.r].pos;
            if(l > r) swap(l, r);
            segs[2*id].update(l, r+1, op);
            segs[2*id+1].update(plen - r - 1, plen - l, op);
        }
    }//}}}
    val_type query(int u, int v, bool isEdgeQuery){//{{{
        auto ps = hdl.hl.get_path(u, v, isEdgeQuery);
        val_type res;
        Path path(-1, -1);
        for(const auto &p : ps){
            const int id = hdl.hl.nodes[p.l].path;
            const int plen = hdl.hl.paths[id].size();
            int l = hdl.hl.nodes[p.l].pos;
            int r = hdl.hl.nodes[p.r].pos;
            val_type now = l <= r ? segs[2*id].query(l, r+1) : segs[2*id + 1].query(plen - r - 1, plen - l);
            if(path.l == -1){
                res = now;
                path = p;
            }else{
                res = hdl.merge(res, path, now, p);
                path.r = p.r;
            }
        }
        return res;
    }//}}}
};//}}}

bool solve(){
    int n, Q;
    reader(n, Q);
    HLDecomposition tree;
    rep(u, n) if(u){
        int v; reader(v);
        tree.add_edge(u+1, v);
    }
    tree.add_edge(1, 0);
    tree.build(0);
    HandlerAdd hdl_a(tree);
    HandlerMul hdl_m(tree);
    HLLazySegTree<HandlerAdd> hl_a(hdl_a);
    HLLazySegTree<HandlerMul> hl_m(hdl_m);
    const Mat<mi, 2> B = {{1, 1}, {1, 0}};

    auto get_to_root = [&](const int u) -> mi {
        auto res_m = hl_m.query(0, u, false);
        auto res_a = hl_a.query(0, u, false);
        return (B * res_m - res_a)[0];
    };

    rep(_, Q){
        char op; reader(op);
        if(op == 'U'){
            int u; long long k; reader(u, k);
            hl_a.update(u, u, {fib[k+1], fib[k]}, false);
            hl_m.update(u, u, {fib[k+1], fib[k]}, false);
        }else{
            int u, v; reader(u, v);
            int p = tree.lca(u, v);
            int pp = tree.nodes[p].par;
            cout << get_to_root(u) + get_to_root(v) - get_to_root(p) - get_to_root(pp) << endl;
        }
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
