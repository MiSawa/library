#include <iostream>
#include <type_traits>

// param > 0 => use param as the modulo
// param <= 0 => you can set the modulo with ModInt<param>::set_modulo(...) in runtime.
// Of course, those runtime modulo will be distinguished if the param was different.
template<int param>
class ModInt{//{{{
    using Z = int;
    using N = unsigned int;
    using ZZ = long long;
    using NN = unsigned long long;
    struct {} constexpr static exact{};

    // Using Z deliberately to avoid Z -> N promotion.
    template<class Ret = Z&>
    static auto modulo() -> std::enable_if_t<(param<=0), Ret> { static Z mod = 0; return mod; }
    template<class Ret = const Z>
    static auto mod() -> std::enable_if_t<(param<=0), Ret> { return modulo(); }
    template<class Ret = const Z>
    static constexpr auto mod() -> std::enable_if_t<(param>0), Ret> { return param; }
    // std::swap is not a constexpr function in C++14
    static constexpr void swap(Z &a, Z &b){ Z tmp = a; a = b; b = tmp; }

    Z val;
    constexpr ModInt(decltype(exact), const N v) : val(v) {}

public:
    template<class Arg = Z>
    static void set_modulo(const std::enable_if_t<(param<=0), Arg> mod) { modulo() = mod; }

    constexpr ModInt() : val(0) {}
    constexpr ModInt(Z v) : val((v%=mod()) < 0 ? v+mod() : v) {}
    constexpr ModInt(ZZ v) : val((v%=mod()) < 0 ? v+mod() : v) {}

    constexpr int get_val() const { return val; }

    constexpr ModInt inv() const {
        Z a = mod(), b = val;
        Z x = 0, u = 1;
        while (b) {
            const Z t = a / b;
            swap(a -= t * b, b);
            swap(x -= t * u, u);
        }
        return ModInt(exact, x < 0 ? x+mod() : x);
    }

    constexpr explicit operator int() const { return val; }
    constexpr ModInt operator+() const { return *this; }
    constexpr ModInt operator-() const { return ModInt(exact, val ? mod() - val : 0); }

    constexpr ModInt operator++(){ if(++val == mod()) val = 0; return *this; }
    constexpr ModInt operator--(){ val = (val == 0 ? mod()-1 : val-1); return *this; }
    constexpr ModInt operator++(int){ const ModInt res(*this); ++*this; return res; }
    constexpr ModInt operator--(int){ const ModInt res(*this); --*this; return res; }

    constexpr bool operator==(const ModInt &rhs) const { return val == rhs.val; }
    constexpr bool operator!=(const ModInt &rhs) const { return val != rhs.val; }
    constexpr ModInt &operator+=(const ModInt &rhs){
        if((val += rhs.val) >= mod()) val -= mod();
        return *this;
    }
    constexpr ModInt &operator-=(const ModInt &rhs){
        if(rhs.val && (val += mod()-rhs.val) >= mod()) val -= mod();
        return *this;
    }
    constexpr ModInt &operator*=(const ModInt &rhs){ val = (NN)(val) * rhs.val % mod(); return *this; }
    constexpr ModInt &operator/=(const ModInt &rhs){ return *this *= rhs.inv(); }

    constexpr ModInt pow(long long e) const {
        if(e < 0) return pow(-e).inv();
        NN res = 1, b = val;
        for(; e; e >>= 1, (b *= b) %= mod()) if(e&1) (res *= b) %= mod();
        return ModInt(exact, res);
    }
    friend constexpr ModInt operator+(ModInt lhs, const ModInt &rhs){ return lhs += rhs; }
    friend constexpr ModInt operator-(ModInt lhs, const ModInt &rhs){ return lhs -= rhs; }
    friend constexpr ModInt operator*(ModInt lhs, const ModInt &rhs){ return lhs *= rhs; }
    friend constexpr ModInt operator/(ModInt lhs, const ModInt &rhs){ return lhs /= rhs; }

    friend std::ostream &operator<<(std::ostream &os, const ModInt &v) { return os << v.val; }
    friend std::istream &operator>>(std::istream &is, ModInt &v) {
        int val;
        is >> val;
        v = ModInt(val);
        return is;
    }
};//}}}

#if __INCLUDE_LEVEL__ == 0

#include <set>
#include <vector>
#include <cassert>
#include <array>
using namespace std;
#define all(v) begin(v), end(v)

long long gcd(long long a, long long b){
    while(b) swap(a %= b, b);
    return llabs(a);
}

signed main(){
    {
        using mi = ModInt<1000000007>;
        // test constexpr
        constexpr mi c = 8;
        constexpr mi x = c * 3 / 2;
        // This works because x is calculated in compile time.
        array<int, (int)x> arr = {};
        cout << arr.size() << endl;
    }
    {
        using A = ModInt<0>;
        using B = ModInt<-1>;
        A::set_modulo(2);
        B::set_modulo(3);
        A a = 4;
        B b = 4;
        cout << a << " " << b << endl;
    }
    {
        // test inverse
        for (int mod = 1; mod < 1000; ++mod) {
            using mi = ModInt<0>;
            mi::set_modulo(mod);
            for (int x = 1; x < mod; ++x) {
                if (gcd(x, mod) != 1) continue;
                mi a = x;
                mi b = 1 / a;
                if ((a*b) != 1) cout << x << "^-1 mod " << mod << " ins't " << b << endl;
            }
        }
    }
    {
        using mi = ModInt<1000000007>;
        // basic functionality
        mi a = 0, b = 1;
        mi x = -b;
        a *= b;
        a -= 3;
        mi c = 3 + b;

        cout << (++a) << endl;
        cout << a.pow(-15) << endl;
    }
    {
        using mi = ModInt<0>;
        mi::set_modulo(1000000007);
        // basic functionality for non-constexpr version
        mi a = 0, b = 1;
        mi x = -b;
        a *= b;
        a -= 3;
        mi c = 3 + b;

        cout << (++a) << endl;
        cout << a.pow(-15) << endl;
    }
    return 0;
}
#endif
// vim:set foldmethod=marker commentstring=//%s:
