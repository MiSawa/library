#include <iostream>
#include <type_traits>

/*
    template<class It> struct it_holder{//{{{
        const It b, e;
        it_holder(const It b, const It e) : b(b), e(e) {}
        It begin() const { return b; }
        It end() const { return e; }
    };//}}}
    template<class V> inline it_holder<it_in_t::it_t<V>> it(const size_t n){//{{{
        return it_holder<it_in_t::it_t<V>>(it_in_t::it_t<V>(*this, n), it_in_t::it_t<V>(*this, 0));
    }//}}}
*/

// Refwrap は,
// template<clas T> f(T &&x) からの Refwrap<T>(x) で,
// x が lvalue reference -> lvalue reference
// x が rvalue reference -> lvalue を move で作ったの
// を保持する.

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

#if __INCLUDE_LEVEL__ == 0
#include <set>
using namespace std;
#define all(v) begin(v), end(v)
signed main(){
    int n = ++io;
    set<int> s = io(n);
    // s = io(n);
    io << io.join(s, ", ") << endl;
    io << io.cond(!s.empty(), s.empty() ? 0 : *s.begin(), "NONE") << endl;
    return 0;
}
#endif
// vim:set foldmethod=marker commentstring=//%s:
