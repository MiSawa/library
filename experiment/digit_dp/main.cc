#include <bits/stdc++.h>
#define all(x) begin(x),end(x)
#define rall(x) (x).rbegin(),(x).rend()
#define REP(i,b,n) for(int i=(int)(b);i<(int)(n);++i)
#define rep(i,n) REP(i,0,n)
#define rrep(i,n) for(int i=(int)(n)-1;i>=0;--i)
#define repsz(i,v) rep(i,(v).size())
#define aur auto&
#define bit(n) (1LL<<(n))
#define eb emplace_back
#define mt make_tuple
#define fst first
#define snd second
using namespace std;
typedef long long ll;
//#define int long long
template<class C>int size(const C &c){ return c.size(); }
template<class T,class U>bool chmin(T&a,const U&b){if(a<=b)return false;a=b;return true;}
template<class T,class U>bool chmax(T&a,const U&b){if(a>=b)return false;a=b;return true;}


/*
 * propagate を実装する.
 * 今は範囲外見ないようにしてる.(単位元が無い時用).
 * 範囲外に zero をちゃんと使いたいなら末尾コメントの行を削除.
 *  SegTree(int n, T zero = T())
 *  SegTree(vector<T> xs, T zero = T())
 *  at(int idx), set(int idx, T x), add(int i, T x), sum(int l, int r)
 */
template<typename T>
struct SegTree{//{{{
    const T zero;
    vector<T> tree;
    int offset, N;

    T propagate(const T &l, const T &r){
        return l + r;
        return max(l, r);
        // gcd と, その gcd そのものを値に持つ子孫の数.
        // vec{ tuple(x_i, 1) } で初期化.
        T res(__gcd(get<0>(l), get<0>(r)), 0);
        if(get<0>(res) == get<0>(l)) get<1>(res) += get<1>(l);
        if(get<0>(res) == get<0>(r)) get<1>(res) += get<1>(r);
        return res;
    }

    SegTree(int n, const T zero = T()) : zero(zero){
        N = 1;
        while(N < n) N <<= 1;
        tree.assign(N*2, zero);
        offset = N-1;
    }
    SegTree(const vector<T> &xs, const T zero = T()) : zero(zero){
        int n = xs.size();
        N = 1;
        while(N < n) N <<= 1;
        tree.assign(N*2, zero);
        offset = N-1;
        rep(i, n) tree[i + offset] = xs[i];
        build(0, n, 0, 0, N);
    }
    const T &build(const int &l, const int &r, const int &k, const int &ll, const int &rr){
        if(r <= ll || rr <= l) return zero;
        if(rr - ll == 1) return tree[k];
        const int mm = (ll + rr) >> 1;
        if(r <= mm) return tree[k] = build(l, r, k*2+1, ll, mm); ////
        if(l >= mm) return tree[k] = build(l, r, k*2+2, mm, rr); ////
        return tree[k] = propagate(build(l, r, k*2+1, ll, mm), build(l, r, k*2+2, mm, rr));
    }
    T at(int i){ return tree[i + offset]; }
    void set(int i, const T &x){
        i += offset;
        tree[i] = x;
        while(i){
            i = (i-1) >> 1;
            tree[i] = propagate(tree[i*2+1], tree[i*2+2]);
        }
    }
    void add(int i, const T &x){
        i += offset;
        tree[i] = propagate(x, tree[i]);
        while(i){
            i = (i-1) >> 1;
            tree[i] = propagate(tree[i*2+1], tree[i*2+2]);
        }
    }
    T sum(const int &l, const int &r){ return sum(l, r, 0, 0, N); }
    T sum(const int &l, const int &r, const int &k, const int &ll, const int &rr){
        if(r <= ll || rr <= l) return zero;
        if(l <= ll && rr <= r) return tree[k];
        const int mm = (ll + rr) >> 1;
        if(r <= mm) return sum(l, r, k*2+1, ll, mm); ////
        if(l >= mm) return sum(l, r, k*2+2, mm, rr); ////
        return propagate(sum(l, r, k*2+1, ll, mm), sum(l, r, k*2+2, mm, rr));
    }
};//}}}

struct Hundler{
    using Int    = long long;
    using String = vector<int>;
    using Result = long long;

    struct Node {
        vector<Node *> ch;
        int depth;
        Int l, r;
        String ls, rs;
        Result result;
    };
    Result calc_leaf(const Node node){
    }
    // 左から i 番目 (右から ls.size() - i - 1 桁目)
    Value calc_inner(int i, Int left, Int right, const String &ls, const String &rs){
    }
};

template<typename Handler, const Handler::Int B = 10>
struct DigitTree{
    Handler hdl;
    using Int = Handler::Int;
    using String = Handler::String;

    Handler::Result query(int i, String left, String right, bool lower, bool upper){
        vector<Node> nodes;
        if(!lower && !upper){
            for(int c = 0; c < B; ++c){
                left[i] = c;
            }
            return handler.calc_leaf();
        }
        string now = left;
    }

    Handler::Result query(String left, String right){
        left = String(right.size() - left.size(), 0);
        
    }
};

bool solve(){
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
