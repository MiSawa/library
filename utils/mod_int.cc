#include <iostream>
#include <type_traits>
#include <tuple>

template<int mod>
struct ModInt{//{{{
    struct Exact {} constexpr static exact{};
    int val;
    ModInt() : val(0) {}
    ModInt(const int v) : val(v % mod) { if(val < 0) val += mod; }
    ModInt(const long long v) : val(v % mod) { if(val < 0) val += mod; }
    ModInt(Exact, const int v) : val(v) {}

    ModInt inv() const {//{{{
        int a = val, b = mod, x = 1, u = 0;
        while(b){
            int t = a / b;
            std::swap(a -= t * b, b);
            std::swap(x -= t * u, u);
        }
        return ModInt(exact, x < 0 ? x+mod : x);
    }//}}}

    ModInt operator+() const { return *this; }
    ModInt operator-() const { return ModInt(exact, val ? mod - val : 0); }

    ModInt operator++(){ if(++val == mod) val = 0; return *this; }
    ModInt operator--(){ if(--val ==  -1) val = mod - 1; return *this; }
    ModInt operator++(int){ ModInt res(*this); ++*this; return res; }
    ModInt operator--(int){ ModInt res(*this); --*this; return res; }

    ModInt &operator+=(const ModInt &rhs){//{{{
        if((val += rhs.val) >= mod) val -= mod;
        return *this;
    }//}}}
    ModInt &operator-=(const ModInt &rhs){//{{{
        if((val -= rhs.val) < 0) val += mod;
        return *this;
    }//}}}
    ModInt &operator*=(const ModInt &rhs){//{{{
        val = (long long)(val) * rhs.val % mod;
        return *this;
    }//}}}
    ModInt &operator/=(const ModInt &rhs){//{{{
        val = (long long)(val) * rhs.inv() % mod;
        return *this;
    }//}}}

    ModInt pow(long long e){//{{{
        if(e < 0) return this->pow(-e).inv();
        long long res = 1, b = val;
        for(; e; e >>= 1, (b *= b) %= mod) if(e&1) (res *= b) %= mod;
        return ModInt(exact, res);
    }//}}}

    friend ModInt operator+(ModInt lhs, const ModInt rhs){ return lhs += rhs; }
    friend ModInt operator-(ModInt lhs, const ModInt rhs){ return lhs -= rhs; }
    friend ModInt operator*(ModInt lhs, const ModInt rhs){ return lhs *= rhs; }
    friend ModInt operator/(ModInt lhs, const ModInt rhs){ return lhs /= rhs; }
};//}}}
using mi = ModInt<1000000007>;

#if __INCLUDE_LEVEL__ == 0

#include <set>
#include <vector>
using namespace std;
#define all(v) begin(v), end(v)

signed main(){
    using mi = ModInt<97>;
    mi a = 0, b = 1;
    mi x = -b;
    a *= b;
    a -= 3;
    mi c = 3 + b;

    cout << (++a).val << endl;
    // for (int i = 1; i < 97; ++i) {
    //     mi a = i;
    //     cout << (a.inv() * a).val << endl;
    // }
    return 0;
}
#endif
// vim:set foldmethod=marker commentstring=//%s:
