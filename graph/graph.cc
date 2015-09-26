#include <bits/stdc++.h>
using namespace std;

// verified:
//   AOJ GRL_1_A (Single Source Shortest Path)
//       GRL_1_B (Single Source Shortest Path (Negative Edges))
//       GRL_1_C (All Pairs Shortest Path)
//       GRL_2_A (Minimum Spanning Tree)
//       GRL_2_B (Minimum-Cost Arborescence)
// using G = GraphAlgorithms<Graph<WeightedEdge<int>>,
//           Dijkstra, MinimumSpanningTree, e.t.c.>;
//
// G g(n); g.add_edge(s, t, weight);
// n を自動で決定したければ, g(-1); とかする. (但し, dijkstra(s) の s とかがはみ出すと普通に死ぬからやめたほうがよい)
// inf は numeric_limits<Weight>::max() / 4 くらい.
// g.dijkstra(s, inf = default)
// g.spfa(s, inf = default)
// g.warshall_floyd(s, inf = default)
// dijkstra は到達不可能: inf.
// spfa は, 到達不可能: inf, 到達可能な負閉路: -inf, その他: 有限.
// warshall_floyd は, 到達不可能: inf, 可能で負閉路を通れる: -inf, その他: 有限.

struct UnweightedEdge{//{{{
    struct EdgeInfo{
        typedef UnweightedEdge Edge;
    };
    int s, t;
    UnweightedEdge() {}
    UnweightedEdge(const int s, const int t) : s(s), t(t) {}
};//}}}
template<class Weight_> struct WeightedEdge : public UnweightedEdge{//{{{
    struct EdgeInfo{
        typedef WeightedEdge Edge;
        typedef Weight_ Weight;
        static constexpr Weight INF = numeric_limits<Weight>::max()/4;
    };
    typedef typename EdgeInfo::Weight Weight;
    static constexpr Weight INF = EdgeInfo::INF;

    Weight w;
    WeightedEdge(){}
    WeightedEdge(const int s, const int t, const Weight w) : UnweightedEdge(s, t), w(w) {}
    friend bool operator<(const WeightedEdge &a, const WeightedEdge &b){
        return tie(a.w, a.s, a.t) < tie(b.w, b.s, b.t);
    }
};//}}}
template<class Edge_> struct Graph : public virtual Edge_::EdgeInfo{//{{{
    typedef Edge_ Edge;
    template<class E> struct has_weight{//{{{
        template<class T, class = typename T::Weight> static true_type check(T);
        static false_type check(...);
        typedef decltype(check(declval<E>())) type;
    };//}}}

    int n;
    vector<Edge> edges;
    inline void init(const int n){ this->n = n; }

    template<class ...Args> inline void add_edge(Args &&...args){//{{{
        edges.emplace_back(forward<Args>(args)...);
    }//}}}

    inline int size() const { return n; }

    inline vector<Edge> &build_edges(){//{{{
        if(n < 0){
            n = 0;
            for(auto &e : edges) n = max(n, max(e.s, e.t) + 1);
        }
        return edges;
    }//}}}

    vector<vector<Edge>> adj_list;
    inline vector<vector<Edge>> &build_adj_list(){//{{{
        if(!adj_list.empty()) return adj_list;
        const auto &es = build_edges();
        adj_list.resize(this->size());
        for(auto &e : es) adj_list[e.s].emplace_back(e);
        return adj_list;
    }//}}}
    inline vector<Edge> &operator[](const int u){//{{{
        build_adj_list();
        return adj_list[u];
    }//}}}

    template<class E = Edge, class Weight = typename E::Weight>
    inline vector<vector<Weight>> build_adj_matrix(const Weight inf = Edge::INF){//{{{
        const auto &es = build_edges();
        const int n = this->size();
        vector<vector<Weight>> g(n, vector<Weight>(n, inf));
        for(int u = 0; u < n; ++u) g[u][u] = 0;
        for(auto &e : es) g[e.s][e.t] = min(g[e.s][e.t], e.w);
        return g;
    }//}}}
    template<class E = Edge, class Weight = int, typename enable_if<!has_weight<E>::value, int>::type = 0>
    inline vector<vector<Weight>> build_adj_matrix(const Weight inf = numeric_limits<Weight>::max()/4){//{{{
        const auto &es = build_edges();
        const int n = this->size();
        vector<vector<Weight>> g(n, vector<Weight>(n, inf));
        for(int u = 0; u < n; ++u) g[u][u] = 0;
        for(auto &e : es) g[e.s][e.t] = min<Weight>(g[e.s][e.t], 1);
        return g;
    }//}}}
};//}}}

template<class Graph> struct Dijkstra : public virtual Graph{//{{{
    typedef typename Graph::Edge Edge;
    typedef typename Edge::Weight Weight;

    inline vector<Weight> dijkstra(const int s, const Weight inf = Edge::INF){//{{{
        const auto &g = this->build_adj_list();
        const int n = g.size();
        vector<Weight> d(n, inf);
        priority_queue<pair<Weight, int>, vector<pair<Weight, int>>, greater<pair<Weight, int>>> pq;
        pq.emplace(d[s] = 0, s);
        while(!pq.empty()){
            Weight c; int u; tie(c, u) = pq.top(); pq.pop();
            if(c > d[u]) continue;
            for(auto &e : g[u]) if(d[e.t] > c + e.w)
                pq.emplace(d[e.t] = c + e.w, e.t);
        }
        return d;
    }//}}}
};//}}}
template<class Graph> struct SPFA : public virtual Graph{//{{{
    typedef typename Graph::Edge Edge;
    typedef typename Edge::Weight Weight;

    vector<Weight> spfa(const int s, const Weight inf = Edge::INF){//{{{
        const auto &g = this->build_adj_list();
        const int n = g.size();
        vector<Weight> d(n, inf);
        vector<int> cnt(n, 0), in_q(n, false);
        queue<int> q;
        auto push = [&, n](const int u, const Weight c){
            if(d[u] <= c) return;
            d[u] = c;
            if(in_q[u] or cnt[u] == n+2) return;
            ++cnt[u];
            q.emplace(u);
            in_q[u] = true;
        };
        push(s, 0);
        while(!q.empty()){
            int u = q.front(); q.pop(); in_q[u] = false;
            for(auto &e : g[u]) push(e.t, d[u] + e.w);
        }
        for(int u = 0; u < n; ++u) if(cnt[u] == n+2) d[u] = -inf;
        return d;
    }//}}}
};//}}}
template<class Graph> struct WarsallFloyd : public virtual Graph{//{{{
    typedef typename Graph::Edge Edge;
    typedef typename Edge::Weight Weight;

    vector<vector<Weight>> warshall_floyd(const Weight inf = Edge::INF){//{{{
        auto d = this->build_adj_matrix();
        const int n = d.size();
        for(int w = 0; w < n; ++w) for(int u = 0; u < n; ++u) for(int v = 0; v < n; ++v)
            if(d[u][w] != inf and d[w][v] != inf and d[u][v] > d[u][w] + d[w][v])
                d[u][v] = d[u][w] + d[w][v];
        for(int w = 0; w < n; ++w) for(int u = 0; u < n; ++u) for(int v = 0; v < n; ++v)
            if(d[u][w] != inf and d[w][v] != inf and d[u][v] > d[u][w] + d[w][v])
                d[u][v] = -inf;
        return d;
    }//}}}
};//}}}
template<class Graph> struct MinimumSpanningTree : public virtual Graph{//{{{
    typedef typename Graph::Edge Edge;
    typedef typename Edge::Weight Weight;

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

    Weight minimum_spanning_tree(){//{{{
        auto &edges = this->build_edges();
        const int n = this->size();
        sort(begin(edges), end(edges));
        UnionFind uf(n);
        Weight res = 0;
        for(auto &e : edges) if(uf.unite(e.s, e.t)) res += e.w;
        return res;
    }//}}}
};//}}}
template<class Graph> struct MinimumArborescence : public virtual Graph{//{{{
    typedef typename Graph::Edge Edge;
    typedef typename Edge::Weight Weight;

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
    template<typename Key, typename Val> struct SkewHeap{//{{{
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
    Weight minimum_arborescence(const int root){//{{{
        const auto &g = this->build_adj_list();
        const int n = g.size();
        typedef SkewHeap<Weight, Edge> Heap;

        UnionFind uf(n);
        vector<Heap> heap(n);
        for(auto &U : g) for(auto &e : U) heap[e.t].push(e.w, e);

        Weight res = 0;
        vector<int> seen(n, -1);
        seen[root] = root;
        for(int s = 0; s < n; ++s){
            vector<int> path;
            for(int u = s; seen[u] < 0; ){
                path.emplace_back(u);
                seen[u] = s;
                if(heap[u].empty()){
                    assert(false);
                    return Graph::INF;
                }
                res += heap[u].top_key();
                heap[u].shift(-heap[u].top_key());
                auto e = heap[u].pop();

                int v = uf.find(e.s);
                if(seen[v] == s){
                    Heap new_heap;
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

template<class, template<class> class...> struct RecInherit;//{{{
template<class G> struct RecInherit<G>{};
template<class G, template<class> class A, template<class> class... As> struct RecInherit<G, A, As...> : public virtual A<G>, public virtual RecInherit<A<G>, As...>{};
template<class G, template<class> class... As> struct GraphAlgorithms : public virtual RecInherit<G, As...>{
    template<class ...Ts>GraphAlgorithms(Ts &&...t){ this->init(forward<Ts>(t)...); }
};//}}}

class IO{//{{{
    std::istream &is;
    std::ostream &os;
    typedef std::ostream& (&OManip)(std::ostream &);

    template<class T> struct Refwrap{//{{{
        T val;
        explicit Refwrap(T &&val) : val(std::forward<T>(val)) {}
        const T &operator*() const { return val; }
    };//}}}
    template<class T> struct Refwrap<T&>{//{{{
        T &val;
        explicit Refwrap(T &val) : val(val) {}
        const T &operator*() const { return val; }
    };//}}}

    class NonAssignable{//{{{
        protected:
            NonAssignable(){}
            ~NonAssignable(){}
        private:
            NonAssignable &operator=(const NonAssignable &);
    };//}}}

    public:
    IO(std::istream &is, std::ostream &os) : is(is), os(os) {}
    explicit operator bool() const { return static_cast<bool>(is); }
    explicit operator bool() { return static_cast<bool>(is); }
    const bool operator!() const { return !is; }
    const bool operator!() { return !is; }

    inline std::string line(){ std::string s; getline(is, s); return s; }
    template<class T>
    inline operator T(){ T x; *this >> x; return std::move(x); }
    template<class T> inline IO &operator>>(T &x){ is >> x; return *this; }
    template<class T = OManip> inline IO &operator<<(const T &x) { os << x; return *this; }

    template<class Int>
    inline typename std::enable_if<std::is_integral<Int>::value, Int>::type
    operator+(const Int x){ return Int(*this)+x; }
    template<class Int>
    inline typename std::enable_if<std::is_integral<Int>::value, Int>::type
    operator-(const Int x){ return Int(*this)-x; }

    inline int operator++(){ return *this + 1; }
    inline int operator--(){ return *this - 1; }

    struct ItIn {//{{{
        template<class T> struct it_t : public std::iterator<std::input_iterator_tag, T>{//{{{
            IO &io;
            size_t rest;
            bool has_val;
            T val;
            inline it_t(IO &io, size_t rest) : io(io), rest(rest), has_val(false){}
            inline bool operator!=(const it_t &o) const { return rest != o.rest; }
            inline const T &operator*(){
                if(!has_val){ has_val = true; io >> val; }
                return val;
            }
            inline it_t &operator++(){ --rest; has_val = false; return *this; }
        };//}}}
        const size_t n;
        IO &io;
        ItIn(const size_t n, IO &io) : n(n), io(io){}

        template<class C, class T, class ...>
        C construct(){ return C(it_t<T>(io, n), it_t<T>(io, 0)); }

        template<template<class, class, class...> class C, class T, class U, class...Ts>
        operator C<T, U, Ts...>(){ return construct<C<T, U, Ts...>, T, U, Ts...>(); }
        template<template<class, size_t> class C, class T, size_t N>
        operator C<T, N>(){ return construct<C<T, N>, T>(); }
    };//}}}
    inline ItIn operator()(const size_t n){ return ItIn(n, *this); }

    template<class C, class D> class Join : private NonAssignable {//{{{
        const Refwrap<C> c; const Refwrap<D> d;
        Join(C &&c, D &&d) : c(std::forward<C>(c)), d(std::forward<D>(d)) {}
        Join(const Join &) = default;
        friend class IO;
    };//}}}
    template<class C = OManip, class D = OManip>
    static inline Join<C, D> join(C &&c, D &&d){//{{{
        return Join<C, D>(std::forward<C>(c), std::forward<D>(d));
    }//}}}
    template<class C, class D>
    inline IO &operator<<(Join<C, D> &&j){//{{{
        bool first = true;
        for(auto it = std::begin(*j.c), e = std::end(*j.c); it != e; ++it){
            if(first) first = false;
            else      (*this) << *j.d;
            (*this) << *it;
        }
        return *this;
    }//}}}

    template<class A, class B> class Cond : private NonAssignable {//{{{
        const bool cond;
        const Refwrap<A> a; const Refwrap<B> b;
        Cond(const bool cond, A &&a, B &&b) : cond(cond), a(std::forward<A>(a)), b(std::forward<B>(b)){}
        Cond(const Cond &) = default;
        friend class IO;
    };//}}}
    template<class Bool, class A = OManip, class B = OManip>
    static inline Cond<A, B> cond(Bool &&c, A &&a, B &&b){//{{{
        return Cond<A, B>(static_cast<bool>(std::forward<Bool>(c)), std::forward<A>(a), std::forward<B>(b));
    }//}}}
    template<class A, class B>
    inline IO &operator<<(const Cond<A, B> &c){//{{{
        if(c.cond) return (*this) << *c.a;
        else       return (*this) << *c.b;
    }//}}}
} io(std::cin, std::cout);//}}}

#define all(x) begin(x),end(x)
#define rall(x) (x).rbegin(),(x).rend()
#define REP(i,b,n) for(int i=(b);i<(int)(n);++i)
#define rep(i,n) REP(i,0,n)
#define repsz(i,v) rep(i,(v).size())
#define rrep(i,n) for(int i=(int)(n)-1;i>=0;--i)
typedef long long ll;

typedef GraphAlgorithms<Graph<WeightedEdge<long long>>,
      Dijkstra, SPFA, WarsallFloyd, MinimumSpanningTree, MinimumArborescence>
      G;

bool solve(){
    int n = io, m = io, root = io;
    G g(n);
    rep(_, m){
        int s = io, t = io, w = io;
        g.add_edge(s, t, w);
    }
    io << io.join(g.dijkstra(root), " ") << endl;
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
