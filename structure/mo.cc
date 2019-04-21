// verified:
// https://www.hackerrank.com/contests/world-codesprint-april/challenges/little-alexey-and-sum-of-maximums
// RUPC 2016 day 3

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

void reader(int &x){ scanf("%d", &x); }
template<class S, class T, class ...U> void reader(S &x, T &y, U &...u){ reader(x); reader(y, u...); }

template<typename Handler, size_t S = 400> struct Mo{//{{{
    using res_t = typename Handler::res_t;
    Handler h;
    vector<pair<int, int>> queries;
    Mo(Handler h) : h(h) {}
    void add_query(int l, int r){ queries.emplace_back(l, r); }
    vector<res_t> run(){
        const int m = queries.size();
        vector<size_t> order(m);
        {
            vector<long long> packed(m);
            for(int i = 0; i < m; ++i)
                packed[i] = ((long long)(queries[i].first / S) << 50)
                           | ((long long)(queries[i].second)   << 25)
                           | ((long long)(i));
            sort(begin(packed), end(packed));
            for(int i = 0; i < m; ++i) order[i] = packed[i] & ((1<<25)-1);
        }

        vector<res_t> res(m);
        int l = 0, r = 0;
        h.clear(l);
        for(auto i : order){
            int ql = queries[i].first, qr = queries[i].second;
            while(l > ql) h.push_front(--l);
            while(r < qr) h.push_back(r++);
            while(l < ql) h.pop_front(l++);
            // while(r > qr) h.pop_back(--r);
            if(r > qr){
                l = r = ql;
                h.clear(l);
                while(r < qr) h.push_back(r++);
            }
            res[i] = h.get();
        }
        return res;
    }
};//}}}

struct Handler{//{{{
    using res_t = long long;
    const vector<int> a;
    vector<int> par;
    res_t res;
    vector<long long> sum_to_root;
    Handler(const vector<int> &a) : a(a), res(0LL){//{{{
        const int n = size(a);
        vector<int> ord(n); iota(begin(ord), end(ord), 0);
        sort(all(ord), [&](const int &i, const int &j){ return tie(a[i], i) > tie(a[j], j); });
        par.assign(n, -1);
        set<int> S;
        for(auto &x : ord){
            auto it = S.lower_bound(x);
            if(it != S.end()) par[x] = *it;
            S.emplace(x);
        }
        sum_to_root.assign(n, 0);
        for(int u = n-1; u >= 0; --u)
            if(par[u] != -1) sum_to_root[u] = sum_to_root[par[u]] + (long long)(par[u] - u) * a[u];
    }//}}}

    int L, R; // いじらない側しか見ないようにする
    long long indeq;
    deque<int> deq;

    void clear(int idx){
        L = R = idx;
        res = 0;
        deq.clear();
        indeq = 0;
    }
    void push_front(int idx){
        // cout << "push front " << idx << endl;

        if(par[idx] < R){
            res += sum_to_root[idx] - sum_to_root[deq.front()];
            res += (long long)(R - deq.front()) * a[deq.front()];
        }else{
            deq.push_front(idx);
            res += (long long)(R - idx) * a[idx];
        }
        indeq += a[deq.front()];
        // cout << res << endl;

        --L;
    }
    void pop_front(int idx){
        // cout << "pop front " << idx << endl;
        indeq -= a[deq.front()];
        if(par[idx] < R){
            res -= sum_to_root[idx] - sum_to_root[deq.front()];
            res -= (long long)(R - deq.front()) * a[deq.front()];
        }else{
            deq.pop_front();
            res -= (long long)(R - idx) * a[idx];
        }
        // cout << res << endl;

        ++L;
    }
    void push_back(int idx){
        // cout << "push back " << idx << endl;

        while(!deq.empty() and par[deq.back()] == idx){
            if(deq.size() > 1)
                indeq -= (long long)(deq.back() - deq[deq.size()-2]) * a[deq.back()];
            deq.pop_back();
        }
        if(deq.empty()){
            indeq = (long long)(idx - L + 1) * a[idx];
        }else{
            indeq += (long long)(idx - deq.back()) * a[idx];
        }
        res += indeq;
        deq.emplace_back(idx);
        // cout << res << endl;

        ++R;
    }
    void pop_back(int idx){
        assert(false);
    }
    res_t get(){
        return res;
    }
};//}}}

bool solve(){
    int n, Q; reader(n, Q);
    vector<int> a(n);
    rep(i, n) reader(a[i]);
    ++n;
    a.emplace_back(1000000009);

    Handler hdl(a);
    Mo<Handler> mo(hdl);
    rep(_, Q){
        int l, r;
        reader(l, r);
        --l;
        mo.add_query(l, r);
    }
    for(auto &r : mo.run()) cout << r << endl;
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
