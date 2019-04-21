#include <vector>
#include <type_traits>
#include <functional>
#include <tuple>
#include <map>
#include <iostream>


template<class Ret, class ...Args>
class Memoize{
    std::map<std::tuple<Args...>, Ret> memo;
    public:

    using Func = std::function<Ret(Args...)>;
    using FuncRec = std::function<Ret(Func&, Args...)>;

    private:
    FuncRec f;
    public:
    Memoize(FuncRec f) : f(f){}
    Memoize(Ret(*f)(Func&, Args...)) : f(f){}
    Ret operator()(const Args &...args){
        std::tuple<Args...> key(args...);
        if(memo.count(key)) return memo[key];
        return memo[key] = f(*this, args...);
    }
};

template<class Ret, class ...Args, class Rec>
Memoize<Ret, Args...> memoize(Ret(*f)(Rec, Args...)){
    return Memoize<Ret, Args...>(f);
}

#include <iostream>
using namespace std;

template<class Rec>
int f(Rec &rec, int n){
    if(n <= 1) return n;
    return rec(n-1) + rec(n-2);
}

int hoge(int x, int y){
    return 0;
}
bool solve(){
    // cout << memoize(f)(10) << endl;
    auto fib = memoize(f);
    cout << fib(100) << endl;
    return true;
}

template<class> struct memoized;
template<class Ret, class ...Args> struct memoized<Ret(Args...)> : std::function<Ret(Args...)>{//{{{
    using Key = typename std::conditional<sizeof...(Args) == 1, typename std::tuple_element<0, std::tuple<Args...>>::type,
          typename std::conditional<sizeof...(Args) == 2, std::pair<typename std::tuple_element<0, std::tuple<Args...>>::type, typename std::tuple_element<1, std::tuple<Args..., void>>::type>,
          std::tuple<Args...>>::type>::type;
    using super = std::function<Ret(Args...)>;
    std::map<Key, Ret> memo;
    template<class F>
    memoized(F &&f) : super(std::forward<F>(f)){}
    Ret operator()(const Args &...args){
        const Key key(args...);
        auto it = memo.find(key);
        if(it != memo.end()) return it->second;
        return memo.emplace(std::move(key), super::operator()(args...)).first->second;
    }
};//}}}

signed main(){
    memoized<long long(int)> f([&](auto n){
        return n <= 1 ? n : fib(n-1) + fib(n-2);
    });

    solve();
    return 0;
}
// vim:set foldmethod=marker commentstring=//%s:
