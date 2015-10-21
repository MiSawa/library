#include <bits/stdc++.h>
using namespace std;

template<class Cost>
struct CowGame{//{{{
    CowGame(const int n, const Cost inf = numeric_limits<Cost>::max()/4)
        : n(n), inf(inf), infeasible(-inf), unbound(inf) {}
    const char LE[3] = "<=", GT[3] = ">=", EQ[3] = "==";
    void add_constraints(const int lhs, const Cost lc, const string &op, const int rhs, const Cost rc = 0){//{{{
        add_constraints(lhs, op, rhs, rc - lc);
    }//}}}
    void add_constraints(const int lhs, const string &op, const int rhs, const Cost rc = 0){//{{{
        if(op == LE){
            add_edge(rhs, lhs, rc);
        }else if(op == GT){
            add_edge(lhs, rhs,-rc);
        }else if(op == EQ){
            add_edge(rhs, lhs, rc);
            add_edge(lhs, rhs,-rc);
        }else assert(false);
    }//}}}

    private:
    struct E{
        int s, t; Cost c;
        E(const int s, const int t, const Cost c) : s(s), t(t), c(c) {}
    };
    int n;
    const Cost inf;
    vector<E> edges;
    void add_edge(const int s, const int t, const Cost c){
        edges.emplace_back(s, t, c);
    }
    public:
    const Cost infeasible, unbound;
    vector<Cost> v;
    Cost solve(){//{{{
        v.assign(n, 0);
        for(int i = n+2, cont = true; cont and i >= 0; --i){
            cont = false;
            for(auto &e : edges){
                if(v[e.t] > v[e.s] + e.c){
                    v[e.t] = v[e.s] + e.c;
                    cont = true;
                }
            }
            if(cont and !i) return infeasible;
        }
        return 0;
    }//}}}
    // maximize v[t] - v[s]
    Cost solve(const int s, const int t){//{{{
        v.assign(n, inf);
        v[s] = 0;
        for(int i = n, cont = true; cont and i >= 0; --i){
            cont = false;
            for(auto &e : edges){
                if(v[e.t] > v[e.s] + e.c){
                    v[e.t] = v[e.s] + e.c;
                    cont = true;
                }
            }
            if(cont and !i) return -inf;
        }
        if(v[t] > inf/2) return +inf;
        return v[t];
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
    template<class T> inline auto operator>>(T &x) -> decltype(is >> x, *this){ is >> x; return *this; }
    template<size_t n, size_t i = 0> struct rec_tuple{//{{{
        template<class Tpl>
            static IO &read(IO &io, Tpl &&tpl){
                io >> std::get<i>(std::forward<Tpl>(tpl));
                return rec_tuple<n, i+1>::read(io, tpl);
            }
    };//}}}
    template<size_t n> struct rec_tuple<n, n>{//{{{
        template<class TPL>
            static IO &read(IO &io, TPL &&){ return io; }
    };//}}}
    template<class Tpl> IO &operator>>(Tpl &&tpl){//{{{
        return rec_tuple<std::tuple_size<typename std::remove_reference<Tpl>::type>::value>::read(*this, std::forward<Tpl>(tpl));
    }//}}}

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
// http://ttpc2015.contest.atcoder.jp/tasks/ttpc2015_n
// SRM 586 MED
bool solve(){
    int n = io, m = io, k = io;
    map<int, int> fixed;
    rep(_, k){
        int u = io, val = io;
        fixed[u] = val;
    }
    vector<tuple<int, int, int>> edges = io(m);
    long long inf = 1LL<<40;
    long long res = inf;
    for(long long d = inf*2; d; d >>= 1){
        long long t = res - d;
        CowGame<long long> cow(n+1);
        const int zero = 0;
        for(auto &v : fixed) cow.add_constraints(v.first, "==", zero, v.second);
        for(auto &e : edges) cow.add_constraints(
                get<0>(e), get<2>(e), "<=", get<1>(e), t);
        if(cow.solve() != cow.infeasible) res = t;
    }
    io << io.cond(res < -inf/2, "#", res) << endl;
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
