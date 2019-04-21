#include <iostream>
#include <cstdio>
#include <vector>
#include <queue>
#include <cassert>
#include <cstdlib>
#include <map>
#include <algorithm>
#include <set>
#include <memory>
#include <cstring>
using namespace std;
#define rep(i, n) for(int i = 0; i < (n); ++i)
#define foreach(i, v) for(__typeof((v).begin()) i = (v).begin(); i != (v).end(); ++i)
#define sz(v) ((int)(v).size())
#define eb emplace_back
#define pb push_back
#define mp make_pair

typedef long long ll;

typedef int Cost;
typedef int Flow;
const Cost INF = 1<<28;

// 流量の復元: cap/flow > 0 か否かで, 元のグラフにcap=0が無ければ,
//  逆辺か元の辺かは大体解る. (未検証)
// src/dst が一つの時は, INF をぶちこんでおけば, 最大流量を計算する.
// 合計流量の復元は INF-b[src] か, sum_i max(0, g[src][i].flow) か何かで.
// 必ず通る辺をつくってそこ見るとかでもいい.
//
// add_edge(src, dst, lower_limit, upper_limit, cost)
// add_undirected_edge(src, dst, upper_limit, cost)
// add_src(i, flow)
// add_dst(i, flow)

struct MinCostFlow{//{{{
    typedef ll Flow;
    typedef ll Cost;
    struct E{
        int src, dst;
        Flow cap, flow;
        Cost cost;
        int rev;
        E(int s, int d, Flow cap, Cost cos, int r)
            : src(s), dst(d), cap(cap), flow(0), cost(cos), rev(r) {}
    };
    typedef vector<E> V;
    typedef vector<V> G;
    int n;
    G g;
    vector<Flow> b;
    MinCostFlow(int _n) : n(_n), g(n), b(n) {}

    void add_edge(int src, int dst, Flow l, Flow u, Cost cos){//{{{
        int i = sz(g[src]), j = sz(g[dst]);
        assert(l <= u);
        g[src].pb(E(src, dst, u, cos, j));
        g[dst].pb(E(dst, src,-l,-cos, i));
    }//}}}
    void add_undirected_edge(int src, int dst, Flow cap, Cost cost){//{{{
        add_edge(src, dst, 0, cap, cost);
        add_edge(dst, src, 0, cap, cost);
    }//}}}
    void add_src(int v, Flow f){ b[v] += f; }
    void add_dst(int v, Flow f){ b[v] -= f; }

    vector<Cost> h, d;
    vector<pair<int, int> > par;

    inline Flow residue(E& e){ return e.cap - e.flow; }
    inline Cost rcost(E& e){ return e.cost + h[e.src] - h[e.dst]; }
    inline Cost add_flow(E& e, Flow f){//{{{
        e.flow += f;
        g[e.dst][e.rev].flow -= f;
        b[e.src] -= f; b[e.dst] += f;
        return e.cost * f;
    }//}}}
    // b は破壊する.
    pair<bool, Flow> run(){//{{{
        pair<bool, Flow> res(true, 0);
        h.assign(n, 0);
        rep(i, n) rep(j, g[i].size()){
            E& e = g[i][j];
            if(e.cost < 0 || residue(e) < 0) res.second += add_flow(e, residue(e));
        }
        for(bool cont = true; cont; ){
            cont = false;
            d.assign(n, INF); par.assign(n, mp(-1, -1));
            priority_queue<pair<Cost, int> > pq;
            rep(i, n) if(b[i] > 0) pq.push(mp(d[i]=0, i));
            while(!pq.empty()){
                int i = pq.top().second;
                Cost c = -pq.top().first;
                pq.pop();
                if(d[i] < c) continue;
                rep(j, g[i].size()) if(residue(g[i][j]) > 0){
                    E &e = g[i][j];
                    Cost nc = c + rcost(e);
                    if(nc >= d[e.dst]) continue;
                    par[e.dst] = mp(e.src, j);
                    pq.push(mp(-(d[e.dst] = nc), e.dst));
                }
            }
            rep(t, n) if(b[t]<0 && d[t]<INF){
                Flow f = -b[t];
                int s;
                for(s = t; par[s].second != -1; s = par[s].first){
                    E &e = g[par[s].first][par[s].second];
                    f = min(f, residue(e));
                }
                f = min(f, b[s]);
                if(f <= 0) continue;
                cont = true;
                for(s = t; par[s].second != -1; s = par[s].first){
                    E &e = g[par[s].first][par[s].second];
                    res.second += add_flow(e, f);
                    if(residue(e) <= 0) par[s].second = -1;
                }
            }
            rep(i, n) if(d[i] < INF) h[i] += d[i];
        }
        rep(i, n) if(b[i] != 0) res.first = false;
        return res;
    }//}}}
};//}}}
void test(){//{{{
    return;
    MinCostFlow mcf(6);
    mcf.add_edge(0, 1, 0, 15, 1);
    mcf.add_edge(0, 2, 0, 15, 4);
    mcf.add_edge(1, 3, 0, 15, 5);
    mcf.add_edge(1, 4, 0, 10, 2);
    mcf.add_edge(2, 1, 0, 20, 7);
    mcf.add_edge(2, 4, 0, 10, 6);
    mcf.add_edge(3, 4, 0, 10, 8);
    mcf.add_edge(3, 5, 0, 10, 3);
    mcf.add_edge(4, 5, 0, 15, 2);
    mcf.add_src(0, 20);
    mcf.add_dst(5, 20);
    pair<bool, ll> res = mcf.run();
    cout << res.first << ", " << res.second << endl;

    exit(0);
}//}}}
namespace MultiPathStory{//{{{
    void solve(){
        int n; cin >> n;
        MinCostFlow mcf(n);
        ll res = 0;
        rep(i, n) if(i) mcf.add_edge(i, 0, 0, INF, 0);
        rep(i, n){
            int k; cin >> k;
            rep(j, k){
                int t, c; cin >> t >> c; --t;
                mcf.add_edge(i, t, 1, INF, c);
            }
        }
        pair<bool, ll> t = mcf.run();
        assert(t.first);
        res += t.second;
        cout << res << endl;
    }
};//}}}
namespace UVa10594{//{{{
    void solve(){
        int n, m, k, d;
        while(cin >> n >> m){
            if(!n)break;
            MinCostFlow mcf(n);
            vector<pair<pair<int, int>, int> > es;
            rep(i, m){
                int a, b, c;
                cin >> a >> b >> c; --a; --b;
                es.pb(mp(mp(a, b), c));
            }
            cin >> d >> k;
            rep(i, m) mcf.add_undirected_edge(es[i].first.first, es[i].first.second, k, es[i].second);
            mcf.add_src(0, d); mcf.add_dst(n-1, d);
            pair<bool, ll> res = mcf.run();
            if(res.first){
                cout << res.second << endl;
            }else{
                cout << "Impossible." << endl;
            }
        }
    }
};//}}}
namespace PKU3422{//{{{
    int n;
    int idx(int i, int j, bool f){ return (i*n+j)*2 + f; }
    void solve(){
        int k; cin >> n >> k;
        const int big = k+10;
        MinCostFlow mcf(n*n*2);

        rep(i, n) rep(j, n){
            int a; cin >> a;
            mcf.add_edge(idx(i, j, false), idx(i, j, true), 0, 1, -a);
            mcf.add_edge(idx(i, j, false), idx(i, j, true), 0, big, 0);
        }
        rep(i, n-1) rep(j, n){
            mcf.add_edge(idx(i, j, true), idx(i+1, j, false), 0, big, 0);
        }
        rep(i, n) rep(j, n-1){
            mcf.add_edge(idx(i, j, true), idx(i, j+1, false), 0, big, 0);
        }
        mcf.add_src(idx(0, 0, false), k);
        mcf.add_dst(idx(n-1, n-1, true), k);
        pair<bool, ll> t = mcf.run();
        assert(t.first);
        cout << -t.second << endl;
    }
};//}}}
namespace AOJ1088{//{{{
    template<typename T> struct ED : map<T, int> {//{{{
        vector<T> r;
        int operator[](T x){
            if(this->count(x)) return this->at(x);
            this->insert(make_pair(x, r.size()));
            r.push_back(x);
            return this->at(x);
        }
        T operator()(int i){ return r[i]; }
    };//}}}
    typedef pair<int, int> pii;
    bool solve2(){
        int n; cin >> n;
        if(!n) return false;
        map<pair<pii, pii>, int> in;
        ED<pii> ed;
        vector<vector<int> > ts(n);
        rep(i, n-1){
            int m; cin >> m;
            rep(j, m){
                int x, y, c; cin >> x >> y >> c;
                ts[i].pb(x); ts[i+1].pb(y);
                pair<pii, pii> key = mp(mp(i, x), mp(i+1, y));
                ed[key.first]; ed[key.second];
                if(in.count(key)) in[key] = min(in[key], c);
                else in[key] = c;
            }
        }
        int cls; cin >> cls;
        int N = ed.size();
        MinCostFlow mcf(N*2);
        rep(i, N) mcf.add_edge(i, i+N, 0, 1, 0);
        foreach(it, in){
            mcf.add_edge(ed[it->first.first]+N, ed[it->first.second],
                    0, 1, it->second);
        }
        rep(i, n){
            sort(ts[i].begin(), ts[i].end());
            ts[i].erase(unique(ts[i].begin(), ts[i].end()), ts[i].end());
            rep(j, sz(ts[i])-1){
                mcf.add_edge(ed[mp(i, ts[i][j])]+N, ed[mp(i, ts[i][j+1])]+N,
                        0, INF, 0);
            }
        }
        int src = ed[mp( 0 , ts[ 0 ].front())] + N;
        int dst = ed[mp(n-1, ts[n-1].back() )] + N;
        mcf.add_src(src, cls);
        mcf.add_dst(dst, cls);
        ll cost = mcf.run().second;
        ll flow = 0;
        rep(i, mcf.g[src].size()) flow += max(0LL, mcf.g[src][i].flow);
        cout << flow << " " << cost << endl;
        return true;
    }
    void solve(){//{{{
        while(solve2());
    }//}}}
};//}}}
namespace PKU2135{//{{{
    void solve(){
        int n, m; cin >> n >> m;
        MinCostFlow mcf(n);
        mcf.add_src( 0 , 2);
        mcf.add_dst(n-1, 2);
        rep(i, m){
            int a, b, c; cin >> a >> b >> c; --a; --b;
            mcf.add_undirected_edge(a, b, 1, c);
        }
        pair<bool, ll> res = mcf.run();
        assert(res.first);
        cout << res.second << endl;
    }
};//}}}
namespace PKU3692{//{{{
    void solve(){
        while(true){
            int g, b, m; cin >> g >> b >> m;
            if(!g) break;
            set<pair<int, int> > es;
            rep(i, g) rep(j, b) es.insert(make_pair(i, j));
            rep(i, m){
                int x, y; cin >> x >> y; --x; --y;
                es.erase(make_pair(x, y));
            }
            int src = g+b, dst = src+1;
            MinCostFlow mcf(g+b+2);
            mcf.add_src(src, INF); mcf.add_dst(dst, INF);
            rep(i, g) mcf.add_edge(src, i, 0, 1, 1);
            rep(j, b) mcf.add_edge(j+g, dst, 0, 1, 0);
            foreach(it, es) mcf.add_edge(it->first, it->second+g, 0, 1, 0);
            ll res = mcf.run().second;
            assert(res == INF - mcf.b[src]);
            cout << g+b-res << endl;
        }
    }
};//}}}
namespace AOJ2266{//{{{
    inline int before(int i){ return i*2; }
    inline int after(int i){ return i*2+1;}
    void solve(){
        int m, n, k; cin >> m >> n >> k;
        vector<int> w(n), a(k);
        rep(i, n) cin >> w[i];
        rep(j, k){
            cin >> a[j]; --a[j];
        }
        MinCostFlow mcf(k*2+2);
        const int src = k*2, dst = k*2+1;
        const int big = m;
        mcf.add_src(src, m);
        mcf.add_dst(dst, m);

        mcf.add_edge(src, before(0), 0, big, w[a[0]]);
        mcf.add_edge(after(k-1), dst, 0, big, 0);

        rep(i, k) mcf.add_edge(before(i), after(i), 1, big, 0);

        rep(i, k-1){
            mcf.add_edge(before(i), before(i+1), 0, big, w[a[i+1]]-w[a[i]]);
            mcf.add_edge(after(i), before(i+1),
                    0, big, w[a[i+1]]);
        }

        vector<vector<int> > works(n);
        rep(i, k) works[a[i]].pb(i);
        rep(i, n) rep(j, sz(works[i])-1)
            mcf.add_edge(after(works[i][j]), before(works[i][j+1]), 0, big, 0);
        mcf.add_edge(before(k-1), dst, 0, big, -w[a[k-1]]);

        pair<bool, ll> res = mcf.run();
        assert(res.first);
        cout << res.second << endl;
    }
};//}}}

int main(){
    AOJ2266::solve();
    return 0;
}
// vim:set foldmethod=marker commentstring=//%s:
