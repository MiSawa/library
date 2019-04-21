#include <iostream>
#include <type_traits>
#include <tuple>

template<int mod>
class ModInt{//{{{
    using Z = int;
    using N = unsigned int;
    using ZZ = long long;
    struct {} constexpr static exact{};

public:
    Z val;
    constexpr ModInt() : val(0) {}
    constexpr ModInt(Z v) { v %= mod; val = v < 0 ? v+mod : v; }
    constexpr ModInt(ZZ v) { v %= mod; val = v < 0 ? v+mod : v; }
    constexpr ModInt(decltype(exact), const N v) : val(v) {}

    constexpr ModInt inv() const {
        Z a = mod, b = val;
        Z x = 0, u = 1;
        while (b) {
            const Z t = a / b;
            std::swap(a -= t * b, b);
            std::swap(x -= t * u, u);
        }
        return ModInt(exact, x < 0 ? x+mod : x);
    }

    constexpr ModInt operator+() const { return *this; }
    constexpr ModInt operator-() const { return ModInt(exact, val ? mod - val : 0); }

    ModInt operator++(){ if(++val == mod) val = 0; return *this; }
    ModInt operator--(){ val = (val == 0 ? mod-1 : val-1); return *this; }
    ModInt operator++(int){ const ModInt res(*this); ++*this; return res; }
    ModInt operator--(int){ const ModInt res(*this); --*this; return res; }

    ModInt &operator+=(const ModInt &rhs){
        if((val += rhs.val) >= mod) val -= mod;
        return *this;
    }
    ModInt &operator-=(const ModInt &rhs){
        if(rhs.val && (val += mod-rhs.val) >= mod) val -= mod;
        return *this;
    }
    ModInt &operator*=(const ModInt &rhs){ val = (ZZ)(val) * rhs.val % mod; return *this; }
    ModInt &operator/=(const ModInt &rhs){ return *this *= rhs.inv(); }

    ModInt pow(long long e){
        if(e < 0) return pow(-e).inv();
        ZZ res = 1, b = val;
        for(; e; e >>= 1, (b *= b) %= mod) if(e&1) (res *= b) %= mod;
        return ModInt(exact, res);
    }

    friend ModInt operator+(ModInt lhs, const ModInt &rhs){ return lhs += rhs; }
    friend ModInt operator-(ModInt lhs, const ModInt &rhs){ return lhs -= rhs; }
    friend ModInt operator*(ModInt lhs, const ModInt &rhs){ return lhs *= rhs; }
    friend ModInt operator/(ModInt lhs, const ModInt &rhs){ return lhs /= rhs; }
};//}}}
using mi = ModInt<1000000007>;

#if __INCLUDE_LEVEL__ == 0

#include <set>
#include <vector>
using namespace std;
#define all(v) begin(v), end(v)

signed main(){
    using mi = ModInt<91>;
    for (int i = 1; i < 91; ++i) {
        mi a = i;
        mi b = a.inv();
        cout << "inv " << i << "  =  " << b.val << ",  mul = " << (a*b).val << endl;
    }
    mi a = 0, b = 1LL;
    mi x = -b;
    a *= b;
    a -= 3;
    mi c = 3 + b;

    cout << (++a).val << endl;
    cout << a.pow(-15).val << endl;
    return 0;
}
#endif
// vim:set foldmethod=marker commentstring=//%s:
