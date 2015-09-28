#include <iostream>
#include <vector>

struct ModComb{//{{{
    const int mod;
    std::vector<int> is, fs, ifs;
    ModComb(const int mod) : mod(mod) {}

    void reserve(int n){//{{{
        if(is.size() > n) return;
        ++n; n += n>>1;
        const size_t k = is.empty() ? 1 : is.size();
        is.resize(n+1); fs.resize(n+1); ifs.resize(n+1);
        fs[0] = ifs[0] = is[1] = 1;
        for(int i = k < 2 ? 2 : k; i <= n; ++i)
            is[i] = is[mod % i] * (long long)(mod - mod / i) % mod;
        for(int i = k; i <= n; ++i)
            fs[i] = fs[i-1] * (long long)(i) % mod;
        for(int i = k; i <= n; ++i)
            ifs[i] = ifs[i-1] * (long long)(is[i]) % mod;
    }//}}}

    int factorial(int n){ reserve(n); return fs[n]; }
    // 異なる n 個から重複を許さず k 個
    int P(int n, int k){//{{{
        if(0 > k and k > n) return 0;
        reserve(n);
        return fs[n] * (long long)(ifs[n-k]) % mod;
    }//}}}
    // n 個から重複を許さず k 個
    int C(int n, int k){//{{{
        if(0 > k or k > n) return 0;
        reserve(n);
        return fs[n] * (long long)(ifs[n-k]) % mod * ifs[k] % mod;
    }//}}}
    // n 個から重複を許して k 個
    int H(int n, int k){ return C(n+k-1, k); }
};//}}}

#if __INCLUDE_LEVEL__ == 0

#include <iostream>
#include <set>
#include <vector>
using namespace std;
#define REP(i,b,n) for(int i=(int)(b);i<(int)(n);++i)
#define rep(i,n) REP(i,0,n)

signed main(){
    ModComb mc(1000000007);
    rep(i, 10){
        rep(j, i+1) cout << mc.C(i, j) << " ";
        cout << endl;
    }
    return 0;
}
#endif
// vim:set foldmethod=marker commentstring=//%s:
