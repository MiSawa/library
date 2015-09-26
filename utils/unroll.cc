#if __INCLUDE_LEVEL__ == 0
#include <iostream>
using namespace std;
#else
#pragma once
#endif

template<size_t N> struct unroll_t{
    template<typename F> inline void operator()(const size_t i, const F &f){
        unroll_t<N-1>()(i, f);
        f(i + (N-1));
    }
};
template<> struct unroll_t<0>{
    template<typename F> inline void operator()(const size_t, const F &){}
};
template<size_t B, typename F> inline void unroll(const size_t n, const F &f){
    size_t i = 0;
    for(const size_t u = n / B; i < u; i += B) unroll_t<B>()(i, f);
    while(i < n) f(i++);
}


#if __INCLUDE_LEVEL__ == 0
signed main(void){
    volatile int s = 0;
    for(size_t i = 0; i < 99; ++i) s += i;
    unroll<8>(99, [&](const size_t &i){ s += i; });
    cout << s << endl;
    return 0;
}
#endif
// vim:set foldmethod=marker commentstring=//%s:
