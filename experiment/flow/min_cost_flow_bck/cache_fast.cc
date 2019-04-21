#include <iostream>
#include <cstdio>
#include <vector>
#include <cassert>
#include <cstdlib>
#include <algorithm>
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

// カリカリにチューンした MinCostFlow で通ってしまった.
// priority_queue の代わりに Radix Heap 使ったりとか.
//
// 流量の復元: cap/flow > 0 か否かで, 元のグラフにcap=0が無ければ,
//  逆辺か元の辺かは大体解る.
//
// add_edge(src, dst, lower_limit, upper_limit, cost)
// add_undirected_edge(src, dst, upper_limit, cost)
// add_src(i, flow)
// add_dst(i, flow)
const int N = 11000 * 2;

namespace DijkHeap{//{{{
    inline int nlz(unsigned int x){//{{{
        x = x | ( x >>  1 );
        x = x | ( x >>  2 );
        x = x | ( x >>  4 );
        x = x | ( x >>  8 );
        x = x | ( x >> 16 );
        x =    (x & 0x55555555) + (x >> 1 & 0x55555555);
        x =    (x & 0x33333333) + (x >> 2 & 0x33333333);
        x =    (x & 0x0f0f0f0f) + (x >> 4 & 0x0f0f0f0f);
        x =    (x & 0x00ff00ff) + (x >> 8 & 0x00ff00ff);
        return (x & 0x0000ffff) + (x >>16 & 0x0000ffff);
    }//}}}
    inline int msd(const unsigned& a, const unsigned& b){ return nlz(a^b); }
    struct Node{
        int v, d;
        Node *prev, *next;
    };
    typedef Node* Ptr;
    const int N = 100000;
    const int K = 32;
    Node nodes[N];
    Node B[K+1];
    int head;
    int mn;
    inline void INIT(int n){//{{{
        rep(i, n){
            nodes[i].v = i;
            nodes[i].d = INF;
        }
    }//}}}
    inline void init(){//{{{
        head = mn = 0;
        rep(i, K) B[i].prev = B[i].next = 0;
    }//}}}
    inline void de_que(const Ptr& p){//{{{
        if(p->next) p->next->prev = p->prev;
        if(p->prev) p->prev->next = p->next;
    }//}}}
    inline void en_que(const int& i, const Ptr& p){//{{{
        if(B[i].next) B[i].next->prev = p;
        p->next = B[i].next;
        p->prev = &B[i];
        B[i].next = p;
    }//}}}
    inline void en_que(const Ptr& p){ en_que(msd(mn, p->d), p); }
    inline void normalize(){//{{{
        if(B[0].next) return;
        rep(i, K) if(B[i].next){
            mn = B[i].next->d;
            for(Ptr p=B[i].next; p; p=p->next) mn = min(mn, p->d);
            Ptr p = B[i].next, q;
            B[i].next = 0;
            for(; p; ){
                q = p->next;
                en_que(msd(mn, p->d), p);
                p = q;
            }
            return;
        }
    }//}}}
    inline void change_dist(const int& v, const int& d){//{{{
        Ptr p = nodes + v;
        if(p->d <= d) return;
        de_que(p); p->d = d; en_que(p);
        if(!B[0].next) normalize();
    }//}}}
    inline void pop(){//{{{
        Ptr p = B[0].next;
        p->d = INF;
        if(p->next) p->next->prev = &B[0];
        B[0].next = p->next;
        p->prev = p->next = 0;
        normalize();
    }//}}}
    inline int top_d(){//{{{
        return mn;
    }//}}}
    inline int top_v(){//{{{
        return B[0].next->v;
    }//}}}
    inline bool empty(){ return !B[0].next; }
};//}}}

namespace MinCostFlow{//{{{
    struct E{//{{{
        int src, dst;
        Flow cap, flow;
        Cost cost;
        int rev;
        E(){}
        E(int s, int d, Flow cap, Cost cos, int r)
            : src(s), dst(d), cap(cap), flow(0), cost(cos), rev(r) {}
    };//}}}
    int n;
    E g[N][8];
    int e_cnt[N];
    Flow b[N];
    void init(int _n){//{{{
        n = _n;
        fill(e_cnt, e_cnt+n, 0);
        fill(b, b+n, 0);
    }//}}}

    inline void add_edge(int src, int dst, Flow l, Flow u, Cost cos){//{{{
        int i = e_cnt[src]++;
        int j = e_cnt[dst]++;
        assert(l <= u);
        g[src][i] = E(src, dst, u, cos, j);
        g[dst][j] = E(dst, src,-l,-cos, i);
    }//}}}
    void add_undirected_edge(int src, int dst, Flow cap, Cost cost){//{{{
        add_edge(src, dst, 0, cap, cost);
        add_edge(dst, src, 0, cap, cost);
    }//}}}
    void add_src(int v, Flow f){ b[v] += f; }
    void add_dst(int v, Flow f){ b[v] -= f; }

    int h[N], d[N];
    E *par[N];

    inline Flow residue(const E& e){ return e.cap - e.flow; }
    inline Cost rcost(const E& e){ return e.cost + h[e.src] - h[e.dst]; }
    inline Cost add_flow(E& e, const Flow f){//{{{
        e.flow += f;
        g[e.dst][e.rev].flow -= f;
        b[e.src] -= f; b[e.dst] += f;
        return e.cost * f;
    }//}}}
    // b は破壊する.
    pair<bool, Flow> run(){//{{{
        pair<bool, Flow> res(true, 0);
        DijkHeap::INIT(n);
        fill(h, h+n, 0);
        rep(i, n) rep(j, e_cnt[i]){
            E& e = g[i][j];
            if(e.cost < 0 || residue(e) < 0)
                res.second += add_flow(e, residue(e));
        }
        for(bool cont = true; cont; ){
            cont = false;

            fill(d, d+n, INF);
            fill(par, par+n, (E*)0);

            DijkHeap::init();
            rep(i, n) if(b[i] > 0) DijkHeap::change_dist(i, d[i]=0);
            while(!DijkHeap::empty()){
                const Cost c = DijkHeap::top_d();
                int i = DijkHeap::top_v();
                DijkHeap::pop();
                if(d[i] < c) continue;
                rep(j, e_cnt[i]) if(residue(g[i][j]) > 0){
                    E &e = g[i][j];
                    const Cost nc = c + rcost(e);
                    if(nc >= d[e.dst]) continue;
                    par[e.dst] = &e;
                    DijkHeap::change_dist(e.dst, d[e.dst] = nc);
                }
            }
            rep(t, n) if(b[t] < 0 && d[t] < INF){
                int s;
                Flow f = -b[t];
                for(E *e = par[t]; e; e = par[s = e->src])
                    f = min(f, residue(*e));
                f = min(f, b[s]);
                if(f <= 0) continue;
                cont = true;
                for(E *e = par[t]; e; e = par[e->src]){
                    res.second += add_flow(*e, f);
                    if(residue(*e) <= 0) par[e->dst] = 0;
                }
            }
            rep(i, n) if(d[i] < INF) h[i] += d[i];
        }
        rep(i, n) if(b[i] != 0) res.first = false;
        return res;
    }//}}}
};//}}}

namespace AOJ2266{//{{{
    inline int before(int i){ return i*2; }
    inline int after(int i){ return i*2+1;}
    const int N = 11000;
    void solve(){
        int m, n, k; scanf("%d%d%d", &m, &n, &k);
        int w[N], a[N];
        rep(i, n) scanf("%d", w+i);
        rep(j, k){
            scanf("%d", a+j); --a[j];
        }
        MinCostFlow::init(k*2+2);
        const int src = k*2, dst = k*2+1;
        const int big = m;
        MinCostFlow::add_src(src, m);
        MinCostFlow::add_dst(dst, m);

        MinCostFlow::add_edge(src, before(0), 0, big, w[a[0]]);
        MinCostFlow::add_edge(after(k-1), dst, 0, big, 0);

        rep(i, k){
            // 少なくとも 1 回はやる.
            MinCostFlow::add_edge(before(i), after(i), 1, big, 0);
        }

        rep(i, k-1){
            MinCostFlow::add_edge(before(i), before(i+1),
                    0, big, w[a[i+1]]-w[a[i]]);
            MinCostFlow::add_edge(after(i), before(i+1),
                    0, big, w[a[i+1]]);
        }

        vector<vector<int> > works(n);
        rep(i, k) works[a[i]].pb(i);
        rep(i, n) rep(j, sz(works[i])-1)
            MinCostFlow::add_edge(after(works[i][j]), before(works[i][j+1]),
                    0, big, 0);
        MinCostFlow::add_edge(before(k-1), dst, 0, big, -w[a[k-1]]);

        pair<bool, int> res = MinCostFlow::run();
        assert(res.first);
        cout << res.second << endl;
    }
};//}}}


int main(){
    //cout<<"end test" << endl;
    //return 0;
    AOJ2266::solve();
    return 0;
}
// vim:set foldmethod=marker commentstring=//%s:
