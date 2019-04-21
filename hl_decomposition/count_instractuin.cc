struct Integer{//{{{
    explicit Integer(const int v) : val(v) {}

    static int count(){
        static int cnt = 0;
        return cnt++;
    }

    Integer &operator+=(const Integer &rhs){
        count();
        val += rhs.val;
        return *this;
    }
    Integer &operator-=(const Integer &rhs){
        count();
        val -= rhs.val;
        return *this;
    }
    friend Integer operator+(Integer lhs, const Integer rhs){ return lhs += rhs; }
    friend Integer operator-(Integer lhs, const Integer rhs){ return lhs -= rhs; }

    private:
    int val;
};//}}}

#include <bits/stdc++.h>
#define all(x) begin(x),end(x)
#define rall(x) (x).rbegin(),(x).rend()
#define REP(i,b,n) for(int i=(int)(b);i<(int)(n);++i)
#define rep(i,n) REP(i,0,n)
#define rrep(i,n) for(int i=(int)(n)-1;i>=0;--i)
#define repsz(i,v) rep(i,(v).size())
using namespace std;

bool solve(){
    Integer a(1), b(2);
    rep(_, 10){
        a += b;
        swap(a, b);
    }
    cout << Integer::count() << endl;
    return true;
}
signed main(){
    cin.tie(nullptr);
    ios_base::sync_with_stdio(false);
    cout << std::fixed << std::setprecision(10);
    solve();
    return 0;
}
// vim:set foldmethod=marker commentstring=//%s:
