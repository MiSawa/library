#include <bits/stdc++.h>

class io_t{//{{{
    std::istream &is;
    std::ostream &os;
    public:
    io_t(std::istream &is, std::ostream &os) : is(is), os(os) {}

    inline std::string line(){ std::string s; getline(is, s); return s; }
    template<class T>
    inline operator T(){ T x; *this >> x; return std::move(x); }
    template<class T> inline io_t &operator>>(T &x){ is >> x; return *this; }
    template<class T> inline io_t &operator<<(const T &x) { os << x; return *this; }
    inline io_t &operator<<(std::ostream& x(std::ostream &)){ os << x; return *this; }

    template<class Int>
    inline typename std::enable_if<std::is_integral<Int>::value, Int>::type
    operator+(const Int x){ return Int(*this)+x; }
    template<class Int>
    inline typename std::enable_if<std::is_integral<Int>::value, Int>::type
    operator-(const Int x){ return Int(*this)-x; }

    inline int operator++(){ return *this + 1; }
    inline int operator--(){ return *this - 1; }

    struct it_in_t {//{{{
        template<class T> struct it_t : public std::iterator<std::input_iterator_tag, T>{//{{{
            io_t &io;
            size_t rest;
            bool has_val;
            T val;
            inline it_t(io_t &io, size_t rest) : io(io), rest(rest), has_val(false){}
            inline bool operator!=(const it_t &o) const { return rest != o.rest; }
            inline const T &operator*(){
                if(!has_val){ has_val = true; io >> val; }
                return val;
            }
            inline it_t &operator++(){ --rest; has_val = false; return *this; }
        };//}}}
        const size_t n;
        io_t &io;
        it_in_t(const size_t n, io_t &io) : n(n), io(io){}

        template<class C, class T, class ...>
        C construct(){ return C(it_t<T>(io, n), it_t<T>(io, 0)); }

        template<template<class, class, class...> class C, class T, class U, class...Ts>
        operator C<T, U, Ts...>(){ return construct<C<T, U, Ts...>, T, U, Ts...>(); }
        template<template<class, size_t> class C, class T, size_t N>
        operator C<T, N>(){ return construct<C<T, N>, T>(); }
    };//}}}
    inline it_in_t operator()(const size_t n){ return it_in_t(n, *this); }

    template<class C, class D> struct join_t {//{{{
        const C &c; const D &d;
        join_t(const C &c, const D &d) : c(c), d(d){}
    };//}}}
    template<class C, class D>
    inline join_t<C, D> join(const C &c, const D &delim){ return join_t<C, D>(c, delim); }
    template<class C, class D>
    inline io_t &operator<<(const join_t<C, D> &j){//{{{
        for(auto it = begin(j.c), e = end(j.c); it != e; ++it){
            if(it != begin(j.c)) (*this) << j.d;
            (*this) << *it;
        }
        return *this;
    }//}}}
} io(std::cin, std::cout);//}}}

using namespace std;

// verified:
//   AOJ GRL_2_A (Minimum Spanning Tree)
//       GRL_2_B (Minimum-Cost Arborescence)
// Graph g(n);
// g.add_edge(u, v, c);
// Cost g.minimum_spanning_tree()
// Cost g.minimum_arborescence(int root)

struct UnionFind{//{{{
    vector<int> p;
    UnionFind(const int n) : p(n, -1) {}
    int find(int u){ return p[u] < 0 ? u : p[u] = find(p[u]); }
    bool unite(int u, int v){
        if((u = find(u)) == (v = find(v))) return false;
        if(p[u] > p[v]) swap(u, v);
        p[u] += p[v];
        p[v] = u;
        return true;
    }
};//}}}

template<typename Key, typename Val>
struct SkewHeap{//{{{
    struct Node{
        Node *ch[2];
        Val val;
        Key key, lazy;
        Node(Key k, Val v) : key(k), val(v), lazy(0) { ch[0] = ch[1] = 0; }
    };
    typedef Node *Ptr;
    Ptr root;
    SkewHeap() : root(0) {}
    Ptr propagate(Ptr a){
        a->key += a->lazy;
        for(auto ch : a->ch) if(ch) ch->lazy += a->lazy;
        a->lazy = 0;
        return a;
    }
    Ptr merge(Ptr a, Ptr b){
        if(!a or !b) return a ? a : b;
        propagate(a); propagate(b);
        if(a->key > b->key) swap(a, b);
        a->ch[1] = merge(b, a->ch[1]);
        swap(a->ch[0], a->ch[1]);
        return a;
    }
    void push(Key k, Val v){
        Ptr x = new Node(k, v);
        root = merge(root, x);
    }
    Val pop(){
        Ptr tmp = propagate(root);
        root = merge(root->ch[0], root->ch[1]);
        Val res = tmp->val;
        delete tmp;
        return res;
    }
    Val top(){ return propagate(root)->val; }
    Key top_key(){ return propagate(root)->key; }
    bool empty(){ return !root; }
    void shift(Key delta){ if(root) root->lazy += delta; }
    void merge(SkewHeap x){ root = merge(root, x.root); }
};//}}}

struct Graph{//{{{
    typedef long long Cost;

    struct SpanningTreeEdge{//{{{
        int s, t;
        Cost c;
        SpanningTreeEdge(const int s, const int t, const Cost c) : s(s), t(t), c(c){
        }
        friend bool operator<(const SpanningTreeEdge &a, const SpanningTreeEdge &b){
            return tie(a.c, a.s, a.t) < tie(b.c, b.s, b.t);
        }
    };//}}}
    typedef SpanningTreeEdge Edge;
    vector<vector<Edge>> g;
    Graph(const int n) : g(n){}

    vector<Edge> &operator[](const size_t u){ return g[u]; }

    template<class ...Args>
    inline void add_edge(const int u, const int v, const Args &...args){
        g[u].emplace_back(u, v, args...);
    }

    int size() const { return g.size(); }

    Cost minimum_spanning_tree(){//{{{
        const int n = g.size();
        vector<Edge> es;
        for(int u = 0; u < n; ++u) for(auto &e : g[u]) es.emplace_back(e);
        sort(begin(es), end(es));
        UnionFind uf(n);
        Cost res = 0;
        for(auto &e : es) if(uf.unite(e.s, e.t)) res += e.c;
        return res;
    }//}}}
    Cost minimum_arborescence(const int r){//{{{
        const int n = g.size();

        UnionFind uf(n);
        vector<SkewHeap<Cost, SpanningTreeEdge>> heap(n);
        for(auto &U : g) for(auto &e : U) heap[e.t].push(e.c, e);

        Cost res = 0;
        vector<int> seen(n, -1);
        seen[r] = r;
        for(int s = 0; s < n; ++s){
            vector<int> path;
            for(int u = s; seen[u] < 0; ){
                path.emplace_back(u);
                seen[u] = s;
                if(heap[u].empty()){
                    assert(false);
                    return numeric_limits<Cost>::max();
                }
                res += heap[u].top_key();
                heap[u].shift(-heap[u].top_key());
                SpanningTreeEdge e = heap[u].pop();

                int v = uf.find(e.s);
                if(seen[v] == s){
                    SkewHeap<Cost, SpanningTreeEdge> new_heap;
                    while(true){
                        int w = path.back(); path.pop_back();
                        new_heap.merge(heap[w]);
                        if(!uf.unite(v, w)) break;
                    }
                    heap[uf.find(v)] = new_heap;
                    seen[uf.find(v)] = -1;
                }
                u = uf.find(v);
            }
        }
        return res;
    }//}}}
};//}}}

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
typedef long long ll;
//#define int long long
template<class C>int size(const C &c){ return c.size(); }
template<class T>bool chmin(T&a,const T&b){if(a<=b)return false;a=b;return true;}
template<class T>bool chmax(T&a,const T&b){if(a>=b)return false;a=b;return true;}

bool solve(){
    int n = io, m = io, root = io;
    Graph g(n);
    rep(_, m){
        int s = io, t = io, c = io;
        g.add_edge(s, t, c);
    }
    io << g.minimum_arborescence(root) << endl;
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
