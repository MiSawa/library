#include <vector>
#include <type_traits>

// auto v = make_vector<double, 2>([10, [10, [0.0]]]); 省略は 0.
// auto v = make_vector(10, 10, 0.0);
// make_vector //{{{
template<class T, size_t n> struct n_vec{//{{{
    typedef std::vector<typename n_vec<T, n-1>::type> type;
    template<class ...Ts> static type init(const int s, Ts &&...ts){
        return type(s, n_vec<T, n-1>::init(std::forward<Ts>(ts)...));
    }
    template<class ...Ts> static type init(){
        return type(0, n_vec<T, n-1>::init());
    }
};//}}}
template<class T> struct n_vec<T, 0>{//{{{
    typedef typename std::remove_reference<T>::type type;
    static type init(){ return type(); }
    template<class U> static type init(U &&x){ return std::forward<U>(x); }
};//}}}
template<class T, size_t n, class ...Ts>
typename n_vec<T, n>::type make_vector(Ts &&...ts){ return n_vec<T, n>::init(std::forward<Ts>(ts)...); }
template<class ...Ts>
typename n_vec<typename std::tuple_element<std::tuple_size<std::tuple<Ts...>>::value-1, std::tuple<Ts...>>::type, std::tuple_size<std::tuple<Ts...>>::value-1>::type make_vector(Ts &&...ts){
    return make_vector<typename std::tuple_element<std::tuple_size<std::tuple<Ts...>>::value-1, std::tuple<Ts...>>::type,std::tuple_size<std::tuple<Ts...>>::value-1>(std::forward<Ts>(ts)...);
}
//}}}

#include <iostream>
using namespace std;

bool solve(){
    int n = 10;
    auto v = make_vector<double, 3>(3);
    cout << v.size() << endl;
    cout << v[0][0][0] << endl;

    return true;
}
signed main(){
    solve();
    return 0;
}
// vim:set foldmethod=marker commentstring=//%s:
