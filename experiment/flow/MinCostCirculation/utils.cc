#pragma once

#include <type_traits>
#include <sstream>
#include <tuple>
#include <iostream>

namespace utils{//{{{
    using namespace std;
    namespace _hidden{//{{{
        template<size_t i> struct str_to_tuple{
            template<class Tpl>
                static void read(Tpl &t, stringstream &ss){
                    str_to_tuple<i-1>::read(t, ss);
                    ss >> get<i-1>(t);
                }
        };
        template<> struct str_to_tuple<0>{
            template<class Tpl>
                static void read(Tpl &t, stringstream &ss){ }
        };
    }//}}}

    template<class ...Ts> tuple<Ts...> string_to_tuple(const string &s){
        using tpl = tuple<Ts...>;
        tpl res;
        stringstream ss(s);
        _hidden::str_to_tuple<tuple_size<tpl>::value>::read(res, ss);
        return res;
    }

    template<class, template<class> class...> struct RecInherit;
    template<class G> struct RecInherit<G>{};
    template<class G, template<class> class A, template<class> class... As> struct RecInherit<G, A, As...> : public virtual A<G>, public virtual RecInherit<A<G>, As...>{};


    struct stringbuilder{//{{{
        std::stringstream ss;
        template<typename T> stringbuilder& operator<<(const T& data){
            ss << data;
            return *this;
        }
        operator std::string(){ return ss.str(); }
    };//}}}

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
    } tr(cout);//}}}
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
}//}}}

// vim:set foldmethod=marker commentstring=//%s:
