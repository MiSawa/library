#if __INCLUDE_LEVEL__ == 0
#include <vector>
#include <iostream>
#else
#pragma once
#endif

// TODO:
//  reversed(reversed(v)) 出来るよう,
//  rbegin(), rend() も追加すべき?
namespace utils{//{{{
    template<typename It> struct _iterator_holder{//{{{
    private:
        const It b, e;
    public:
        _iterator_holder(const It &b, const It &e) : b(b), e(e) {}
        inline It begin() const { return b; }
        inline It end() const { return e; }
    };//}}}

    template<typename C>
        inline auto reversed(C &c) -> _iterator_holder<decltype(c.rbegin())>{
            return _iterator_holder<decltype(c.rbegin())>(c.rbegin(), c.rend()); }
    template<typename C>
        inline auto reversed(const C &c) -> _iterator_holder<decltype(c.rbegin())>{
            return _iterator_holder<decltype(c.rbegin())>(c.rbegin(), c.rend()); }
}//}}}

#if __INCLUDE_LEVEL__ == 0
#include "./range.cc"
using namespace utils;
using namespace std;
signed main(void){
    for(auto i : reversed(range(10, 4, -3))) cout << i << endl;
    return 0;
}
#endif
// vim:set foldmethod=marker commentstring=//%s:
