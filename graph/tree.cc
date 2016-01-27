#include <bits/stdc++.h>
using namespace std;

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

template<class Edge_> struct Tree : public virtual Edge_::EdgeInfo{//{{{
    typedef Edge_ Edge;
    template<class E> struct has_weight{//{{{
        template<class T, class = typename T::Weight> static true_type check(T);
        static false_type check(...);
        typedef decltype(check(declval<E>())) type;
    };//}}}

    int n;
    vector<Edge> edges_;
    void init(const int n = -1){ this->n = n; }

    template<class ...Args> void add_edge(int s, int t, const Args &...args){//{{{
        edges_.emplace_back(s, t, args...);
        edges_.emplace_back(t, s, args...);
    }//}}}

    void build_n(){
        if(n >= 0) return;
        n = 0;
        for(auto &e : edges_) n = max(n, max(e.s, e.t) + 1);
    }
    int size() { build_n(); return n; }

    vector<vector<Edge>> adj_list_;
    vector<vector<Edge>> &adj_list(){//{{{
        if(!adj_list_.empty()) return adj_list_;
        build_n();
        adj_list_.resize(this->size());
        for(auto &e : edges_) adj_list_[e.s].emplace_back(e);
        return adj_list_;
    }//}}}
    vector<Edge> &operator[](const int u){ return adj_list()[u]; }

    vector<int> parent_, order_;
    void build(int root){//{{{
        if(!parent_.empty() and parent_[root] == -1) return;
        build_n();
        parent_.assign(n, -1);
        order_.clear(); order_.reserve(n);

        stack<int> st;
        st.emplace(root);
        while(!st.empty()){
            int u = st.top(); st.pop();
            order_.emplace_back(u);
            for(auto &e : adj_list()[u]) if(e.t != parent_[u]){
                parent_[e.t] = u;
                st.push(e.t);
            }
        }
    }//}}}

    template<class F> void dfs_previsit(int root, F f){//{{{
        build(root);
        for(int i = 0; i < n; ++i) f(order_[i], parent_[order_[i]]);
    }//}}}
    template<class F> void dfs_postvisit(int root, F f){//{{{
        build(root);
        for(int i = n-1; i >= 0; --i) f(order_[i], parent_[order_[i]]);
    }//}}}
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

bool solve(){
    Tree<WeightedEdge<int>> tree;
    tree.n = -1;
    rrep(_, --io){
        int s = io, t = io, w = io;
        tree.add_edge(s, t, w);
    }
    vector<int> dist(tree.size());
    tree.dfs_previsit(0, [&](int u, int par){
        for(auto &e : tree[u]) if(e.t != par) dist[e.t] = dist[e.s] + e.w;
            });
    int s = std::distance(begin(dist), max_element(begin(dist), end(dist)));
    dist.assign(tree.size(), 0);
    tree.dfs_previsit(s, [&](int u, int par){
        for(auto &e : tree[u]) if(e.t != par) dist[e.t] = dist[e.s] + e.w;
            });
    io << *max_element(begin(dist), end(dist)) << endl;
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
