#include <iostream>
#include <type_traits>
#include <tuple>

/*
    template<class It> struct it_holder{//{{{
        const It b, e;
        it_holder(const It b, const It e) : b(b), e(e) {}
        It begin() const { return b; }
        It end() const { return e; }
    };//}}}
    template<class V> it_holder<it_in_t::it_t<V>> it(const size_t n){//{{{
        return it_holder<it_in_t::it_t<V>>(it_in_t::it_t<V>(*this, n), it_in_t::it_t<V>(*this, 0));
    }//}}}
*/

// Refwrap は,
// template<clas T> f(T &&x) からの Refwrap<T>(x) で,
// x が lvalue reference -> lvalue reference
// x が rvalue reference -> lvalue を move で作ったの
// を保持する.

class AwesomeIO{//{{{
    std::istream &is;
    std::ostream &os;
    typedef std::ostream& (&OManip)(std::ostream &);

    template<class T> struct Refwrap{//{{{
        T val;
        explicit Refwrap(T &&val) : val(std::forward<T>(val)) {}
        const T &operator*() const { return val; }
        T &operator*() { return val; }
    };//}}}
    template<class T> struct Refwrap<T&>{//{{{
        T &val;
        explicit Refwrap(T &val) : val(val) {}
        const T &operator*() const { return val; }
        T &operator*() { return val; }
    };//}}}

    class NonAssignable{//{{{
        protected:
            NonAssignable(){}
            ~NonAssignable(){}
        private:
            NonAssignable &operator=(const NonAssignable &);
    };//}}}

    template<class TPL, size_t n = std::tuple_size<typename std::remove_reference<TPL>::type>::value, size_t i = 0> struct rec_tuple{//{{{
        template<class Tpl>
        static AwesomeIO &read(AwesomeIO &io, Tpl &&tpl){
            io >> std::get<i>(std::forward<Tpl>(tpl));
            return rec_tuple<TPL, n, i+1>::read(io, tpl);
        }
        template<class T, class ...Args>
        static T construct(AwesomeIO &io, Args &&...args){
            typename std::tuple_element<i, TPL>::type x{};
            io >> x;
            return rec_tuple<TPL, n, i+1>::template construct<T>(
                    io, std::forward<Args>(args)..., std::move(x));
        }
    };//}}}
    template<class TPL, size_t n> struct rec_tuple<TPL, n, n>{//{{{
        template<class Tpl>
        static AwesomeIO &read(AwesomeIO &io, Tpl &&){ return io; }
        template<class T, class ...Args>
        static T construct(AwesomeIO &, Args &&...args){
            return T(std::forward<Args>(args)...);
        }
    };//}}}

    public:
    AwesomeIO(std::istream &is, std::ostream &os) : is(is), os(os) {}
    explicit operator bool() const { return static_cast<bool>(is); }
    explicit operator bool() { return static_cast<bool>(is); }
    const bool operator!() const { return !is; }
    const bool operator!() { return !is; }

    std::string line(){ std::string s; getline(is, s); return s; }
    template<class T>
    operator T(){ T x{}; *this >> x; return std::move(x); }
    template<class T> auto operator>>(T &x) -> decltype(is >> x, *this){ is >> x; return *this; }
    template<class Tpl> auto operator>>(Tpl &&tpl) -> decltype(std::tuple_size<typename std::remove_reference<Tpl>::type>::value, *this){//{{{
        return rec_tuple<Tpl>::read(*this, std::forward<Tpl>(tpl));
    }//}}}

    template<class T = OManip> AwesomeIO &operator<<(const T &x) { os << x; return *this; }

    template<class Int>
    typename std::enable_if<std::is_integral<Int>::value, Int>::type
    operator+(const Int x){ return Int(*this)+x; }
    template<class Int>
    typename std::enable_if<std::is_integral<Int>::value, Int>::type
    operator-(const Int x){ return Int(*this)-x; }

    int operator++(){ return *this + 1; }
    int operator--(){ return *this - 1; }

    template<class IO> struct ItIn {//{{{
        template<class T> struct it_t : public std::iterator<std::input_iterator_tag, T>{//{{{
            Refwrap<IO> &io;
            size_t rest;
            bool has_val;
            T val;
            it_t(Refwrap<IO> &io, size_t rest) : io(io), rest(rest), has_val(false){}
            bool operator!=(const it_t &o) const { return rest != o.rest; }
            const T &operator*(){
                if(!has_val){ has_val = true; (*io) >> val; }
                return val;
            }
            it_t &operator++(){ --rest; has_val = false; return *this; }
        };//}}}
        const size_t n;
        Refwrap<IO> io;
        template<class IOt>
        ItIn(const size_t n, IOt &&io) : n(n), io(std::forward<IOt>(io)){}

        template<class C, class T, class ...>
        C construct(){ return C(it_t<T>(io, n), it_t<T>(io, 0)); }

        template<template<class, class, class...> class C, class T, class U, class...Ts>
        operator C<T, U, Ts...>(){ return construct<C<T, U, Ts...>, T, U, Ts...>(); }
        template<template<class, size_t> class C, class T, size_t N>
        operator C<T, N>(){ return construct<C<T, N>, T>(); }
    };//}}}
    ItIn<AwesomeIO&> operator()(const size_t n){ return ItIn<AwesomeIO&>(n, *this); }

    template<class ...Args>
    struct Builder {//{{{
        AwesomeIO &io;
        Builder(AwesomeIO &io) : io(io){}
        template<class T, typename std::enable_if<std::is_constructible<T, Args...>::value, std::nullptr_t>::type = nullptr>
        Builder &operator>>(T &x){
            x = rec_tuple<std::tuple<Args...>>::template construct<T>(io);
            return *this;
        }

        template<class T, typename std::enable_if<std::is_constructible<T, Args...>::value, std::nullptr_t>::type = nullptr>
        operator T(){
            return rec_tuple<std::tuple<Args...>>::template construct<T>(io);
        }
    };//}}}

    template<class ...Args>
    Builder<Args...> build(){ return Builder<Args...>(*this); }
    template<class ...Args>
    ItIn<Builder<Args...>> build(const size_t n){ return ItIn<Builder<Args...>>(n, Builder<Args...>(*this)); }

    template<class C, class D> class Join : private NonAssignable {//{{{
        const Refwrap<C> c; const Refwrap<D> d;
        Join(C &&c, D &&d) : c(std::forward<C>(c)), d(std::forward<D>(d)) {}
        Join(const Join &) = default;
        friend class AwesomeIO;
    };//}}}
    template<class C = OManip, class D = OManip>
    static Join<C, D> join(C &&c, D &&d){//{{{
        return Join<C, D>(std::forward<C>(c), std::forward<D>(d));
    }//}}}
    template<class C, class D>
    AwesomeIO &operator<<(Join<C, D> &&j){//{{{
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
        friend class AwesomeIO;
    };//}}}
    template<class Bool, class A = OManip, class B = OManip>
    static Cond<A, B> cond(Bool &&c, A &&a, B &&b){//{{{
        return Cond<A, B>(static_cast<bool>(std::forward<Bool>(c)), std::forward<A>(a), std::forward<B>(b));
    }//}}}
    template<class A, class B>
    AwesomeIO &operator<<(const Cond<A, B> &c){//{{{
        if(c.cond) return (*this) << *c.a;
        else       return (*this) << *c.b;
    }//}}}
} io(std::cin, std::cout);//}}}

#if __INCLUDE_LEVEL__ == 0

#include <set>
#include <vector>
using namespace std;
#define all(v) begin(v), end(v)

struct E{
    int s, t, c;
    E(){}
    E(int s, int t, int c = 0) : s(s), t(t), c(c){}
};
signed main(){
    // tuple<int, int, int> tpl = io;
    // io << get<0>(tpl) << get<1>(tpl) << get<2>(tpl) << endl;
    // E e = io.build<int, int, int>();
    int a, b;
    io >> tie(a, b);
    vector<E> es = io.build<int, int>(3);
    for(auto e : es) io << e.s << " " << e.t << " " << e.c << endl;

    // io << e.s << " " << e.t << " " << e.c << endl;
    // e = io.build<int, int>();
    // io << e.s << " " << e.t << " " << e.c << endl;
    // int n = ++io;
    // set<int> s = io(n);
    // s = io(n);
    // io << io.join(s, ", ") << endl;
    // io << io.cond(!s.empty(), s.empty() ? 0 : *s.begin(), "NONE") << endl;
    return 0;
}
#endif
// vim:set foldmethod=marker commentstring=//%s:
