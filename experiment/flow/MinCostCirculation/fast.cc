
#define LOCAL_DEBUG

#ifndef LOCAL_DEBUG
#define NDEBUG
#endif

#include <bits/stdc++.h>
using namespace std;

/*

  メインは
  Andrew V. Goldberg and Robert E. Tarjan
  Finding Minimum-Cost Circulations by Successive Approximation
  Mathematics of Operations Research
  Vol. 15, No. 3 (Aug., 1990), pp. 430-466

  Andrew V. Goldberg
  An efficient implementation of a scaling minimum-cost flow algorithm
  Journal of algorithms
  Vol. 22, No. 1 (Jan.1997), pp. 1-29
  DOI: 10.1006/jagm.1995.0805
  も詳しい. 1992 バージョンと 1997 バージョンがある?
  1997 バージョンが, いくつかのヒューリスティックが詳しく載っていてよい.
  price refine が詳しめに載っているが, [19] を見ないとたぶんわからん.
  多分 p += d のところ, p += d * eps だと思う(?)
  あと, ceil(hoge/eps) は多分大体 floor((hoge+eps)/eps) のはず.
  [19] と eps-optimality とかの定義が微妙に違うせい (0 を含むか否か).
  [19] でそうしてるのは, log(n C) じゃなくて log(C) にするためだけれど,
  MinCostFlow の context だとこれは関係ないのでどうでもいい.

  Andrew V. Goldberg and M. Kharitonov
  On Implementing Scaling Push-Relabel Algorithms for Minimum-Cost Flow Problem
  In D. S. Johnson and C. C. McGeoch, editors
  Dimacs Implementation Challenge Workshop : Algorithms for Network Flows and Matching
  DIMACS Series in Discrete Mathematics and THeoretical Computer Science
  Vol. 12, pages 157-198, Providence, RI, 1993. American Mathematical Society.


  - Active Node の選び方の実装方法として, PFA implementation が載っている.
    topological order を番号で管理しつつ, priority queue に active なのだけ突っ込む感じ.

  - Price Updates のところに set-relabel が載っている.
    1997 の An efficient implementation of a scaling minimum-cost flow algorithm. の方が読みやすいかも.
    これは \Omega(n) 回の relabel に一回行ってよい.
    1. S <- negative excess なの全部
    2. S <- admissible network で S に到達可能なのの閉包
       すると, S に入る admissible arc が無いから, S に入る枝が全部 eps 安くなっても良い.
    3. もし positive excess なのが全部 S に入っているなら終了.
    4. S 以外のやつらの price を eps 下げる. したがって, S に入る枝が全部 eps 安くなる.
    5. 2. へ.
    実際は, "何回目の -= eps で amissible になるか" でバケットに入れながら DP する感じ. (1997 のやつ読むと書いてある)


  LEMON のやつも結構よい.

  Ursula Bunnagel, Bernhard Korte and Jens Vygen
  Efficient Implementation of the Goldberg-Tarjan Minimum-Cost Flow Algorithm
  Optimization Method and Software
  Vol. 10, Issue 2, 1998, pp. 157-174
  DOI: 10.1080/10556789808805709
  にもヒューリスティックが詳しくのっている.


  Potential refinment は, 結局
  Andrew
  Andrew V. Goldberg and M. Kharitonov
  Scaling algorithms for the shortest path problem
  SODA '93 Proceedings in the fourth annual ACM-SIAM Symposium on Discrete algorithms
  ISBN: 0-89871-313-7
 */

#include "utils.cc"

using namespace utils;

template<class Flow, class Cost>
struct FlowNetwork{//{{{
    struct Edge{//{{{
        size_t s, t, rev;
        Flow u;
        Cost c;
        Edge(){}
        Edge(const size_t s, const size_t t, const Flow u, const Cost c) : s(s), t(t), rev(), u(u), c(c) {}
    };//}}}

    template<class It> struct Subseq{//{{{
        It b, e;
        Subseq(){}
        Subseq(const It b, const It e) : b(b), e(e) {}
        It begin() const { return b; }
        It end() const { return e; }
    };//}}}

    size_t n;
    vector<Edge> edges;
    vector<Subseq<typename vector<Edge>::iterator>> g;
    vector<Flow> ex;
    vector<Cost> pot;
    private:
    vector<Flow> original_cap;
    public:

    void add_edge(const size_t s, const size_t t, const Flow l, const Flow u, const Cost c){//{{{
        edges.emplace_back(s, t, u, c);
        edges.emplace_back(t, s,-l,-c);
    }//}}}
    void add_src(const size_t v, const Flow f){//{{{
        if(ex.size() <= v) ex.resize(v + 1);
        ex[v] += f;
    }//}}}
    void add_dst(const size_t v, const Flow f){//{{{
        add_src(v, -f);
    }//}}}

    vector<size_t> edge_pos;
    void build(){//{{{
        n = ex.empty() ? 0 : ex.size()-1;
        for(auto &e : edges) n = std::max(n, e.s);
        ++n;
        ex.resize(n);
        pot.assign(n, 0);
        vector<size_t> pos(n+1, 0u);
        for(auto &e : edges) ++pos[e.s];
        for(size_t i = 0; i < n; ++i) pos[i+1] += pos[i];
        vector<Edge> sorted(edges.size());
        for(size_t i = 0; i < edges.size(); i += 2){
            Edge &e = edges[i], &re = edges[i+1];
            re.rev = --pos[e.s];
            e.rev = --pos[re.s];
            sorted[re.rev] = e;
            sorted[e.rev] = re;
        }
        sorted.swap(edges);
        g.resize(n);
        for(size_t v = 0; v < n; ++v)
            g[v] = Subseq<typename vector<Edge>::iterator>(
                    begin(edges) + pos[v], begin(edges) + pos[v+1]);
        edge_pos = pos;
        original_cap.resize(edges.size());
        for(size_t i = 0; i < edges.size(); ++i) original_cap[i] = edges[i].u;
    }//}}}
    bool is_active(const size_t u) const {//{{{
        return ex[u] > 0;
    }//}}}
    const Edge &reversed(const Edge &e) const {//{{{
        return edges[e.rev];
    }//}}}
    Edge &reversed(const Edge &e) {//{{{
        return edges[e.rev];
    }//}}}
    Flow residual_cap(const Edge &e) const {//{{{
        return e.u;
    }//}}}
    Cost reduced_cost(const Edge &e) const {//{{{
        return e.c + pot[e.s] - pot[e.t];
    }//}}}
    bool is_admissible(const Edge &e) const {//{{{
        return residual_cap(e) > 0 and reduced_cost(e) < 0;
    }//}}}
    void add_flow(Edge &e, const Flow f){//{{{
        e.u -= f;
        reversed(e).u += f;
        ex[e.s] -= f;
        ex[e.t] += f;
    }//}}}
    Cost potential(const size_t u){//{{{
        return pot[u];
    }//}}}
    Flow get_flow(const Edge &e) const {//{{{
        return original_cap[reversed(e).rev] - e.u;
    }//}}}
};//}}}
template<class Flow, class Cost>
struct FlowNetworkUseFlow{//{{{
    struct Edge{//{{{
        size_t s, t, rev;
        Flow f, u;
        Cost c;
        Edge(){}
        Edge(const size_t s, const size_t t, const Flow u, const Cost c) : s(s), t(t), rev(), f(0), u(u), c(c) {}
    };//}}}

    template<class It> struct Subseq{//{{{
        It b, e;
        Subseq(){}
        Subseq(const It b, const It e) : b(b), e(e) {}
        It begin() const { return b; }
        It end() const { return e; }
    };//}}}

    size_t n;
    vector<Edge> edges;
    vector<Subseq<typename vector<Edge>::iterator>> g;
    vector<Flow> ex;
    vector<Cost> pot;
    private:
    vector<Flow> original_cap;
    public:

    void add_edge(const size_t s, const size_t t, const Flow l, const Flow u, const Cost c){//{{{
        edges.emplace_back(s, t, u, c);
        edges.emplace_back(t, s,-l,-c);
    }//}}}
    void add_src(const size_t v, const Flow f){//{{{
        if(ex.size() <= v) ex.resize(v + 1);
        ex[v] += f;
    }//}}}
    void add_dst(const size_t v, const Flow f){//{{{
        add_src(v, -f);
    }//}}}

    vector<size_t> edge_pos;
    void build(){//{{{
        n = ex.empty() ? 0 : ex.size()-1;
        for(auto &e : edges) n = std::max(n, e.s);
        ++n;
        ex.resize(n);
        pot.assign(n, 0);
        vector<size_t> pos(n+1, 0u);
        for(auto &e : edges) ++pos[e.s];
        for(size_t i = 0; i < n; ++i) pos[i+1] += pos[i];
        vector<Edge> sorted(edges.size());
        for(size_t i = 0; i < edges.size(); i += 2){
            Edge &e = edges[i], &re = edges[i+1];
            re.rev = --pos[e.s];
            e.rev = --pos[re.s];
            sorted[re.rev] = e;
            sorted[e.rev] = re;
        }
        sorted.swap(edges);
        g.resize(n);
        for(size_t v = 0; v < n; ++v)
            g[v] = Subseq<typename vector<Edge>::iterator>(
                    begin(edges) + pos[v], begin(edges) + pos[v+1]);
        edge_pos = pos;
        original_cap.resize(edges.size());
        for(size_t i = 0; i < edges.size(); ++i) original_cap[i] = edges[i].u;
    }//}}}
    bool is_active(const size_t u) const {//{{{
        return ex[u] > 0;
    }//}}}
    const Edge &reversed(const Edge &e) const {//{{{
        return edges[e.rev];
    }//}}}
    Edge &reversed(const Edge &e) {//{{{
        return edges[e.rev];
    }//}}}
    Flow residual_cap(const Edge &e) const {//{{{
        return e.u - e.f;
    }//}}}
    Cost reduced_cost(const Edge &e) const {//{{{
        return e.c + pot[e.s] - pot[e.t];
    }//}}}
    bool is_admissible(const Edge &e) const {//{{{
        return residual_cap(e) > 0 and reduced_cost(e) < 0;
    }//}}}
    void add_flow(Edge &e, const Flow f){//{{{
        e.f += f;
        reversed(e).f -= f;
        ex[e.s] -= f;
        ex[e.t] += f;
    }//}}}
    Cost potential(const size_t u){//{{{
        return pot[u];
    }//}}}
    Flow get_flow(const Edge &e) const {//{{{
        return e.f;
        // return original_cap[reversed(e).rev] - e.u;
    }//}}}
};//}}}

struct SSP : public FlowNetwork<int64_t, int64_t>{//{{{
    using Flow = int64_t;
    using Cost = int64_t;
    tuple<bool, Cost> run(){//{{{
        build();
        constexpr Cost inf = numeric_limits<Cost>::max();

        for(auto &e : edges) if((reduced_cost(e) < 0 and residual_cap(e) > 0) or residual_cap(e) < 0)
            add_flow(e, residual_cap(e));

        for(bool cont = true; cont; ){//{{{
            cont = false;
            priority_queue<pair<Cost, size_t>, vector<pair<Cost, size_t>>, greater<pair<Cost, size_t>>> pq;
            vector<size_t> prev(n, -1);
            vector<Cost> d(n, inf);
            for(size_t s = 0; s < n; ++s) if(ex[s] > 0) pq.emplace(d[s] = 0, s);
            while(!pq.empty()){//{{{
                Cost c; size_t v;
                tie(c, v) = pq.top(); pq.pop();
                if(d[v] < c) continue;
                for(auto &e : g[v]) if(residual_cap(e) > 0){
                    const Cost nc = c + reduced_cost(e);
                    if(d[e.t] <= nc) continue;
                    pq.emplace(d[e.t] = nc, e.t);
                    prev[e.t] = e.rev;
                }
            }//}}}
            for(size_t t = 0; t < n; ++t) if(ex[t] < 0 and d[t] < inf){
                Flow f = -ex[t];
                size_t s;
                for(s = t; prev[s] != -1; ){
                    auto &re = edges[prev[s]];
                    f = min(f, residual_cap(reversed(re)));
                    s = re.t;
                }
                f = min(f, ex[s]);
                if(f <= 0) continue;
                cont = true;
                for(s = t; prev[s] != -1; ){
                    auto &re = edges[prev[s]];
                    add_flow(re, -f);
                    if(residual_cap(re) <= 0) prev[s] = -1;
                    s = re.t;
                }
            }
            for(size_t v = 0; v < n; ++v) if(d[v] < inf)
                pot[v] += d[v];
        }//}}}

        for(size_t v = 0; v < n; ++v) if(ex[v] != 0)
            return make_tuple(false, 0);
        Cost res = 0;
        for(auto &v : g) for(auto &e : v) res += get_flow(e) * e.c;
        return make_tuple(true, res / 2);
    }//}}}
};//}}}

struct SuccessiveShortestPath{//{{{
    using Cost = int64_t;
    using Flow = int64_t;
    static const Cost minus_infinity = - numeric_limits<Cost>::max();
    struct E{//{{{
        int s, t, rev;
        Flow f, u;
        Cost c;
        E(int s, int t, Flow u, Cost c, int rev) : s(s), t(t), rev(rev), f(0), u(u), c(c) {}
    };//}}}
    vector<vector<E>> g;

    vector<Flow> ex;           // excess
    vector<Cost> p;            // potential (price, dual variables)

    void add_edge(int s, int t, Flow l, Flow u, Cost c){//{{{
        if(g.size() < max(s, t) + 1) g.resize(max(s, t) + 1);
        int i = g[s].size(), j = s == t ? i+1 : g[t].size();
        assert(l <= u);
        g[s].emplace_back(s, t, u, c, j);
        g[t].emplace_back(t, s,-l,-c, i);
    }//}}}
    void add_src(int v, Flow f){//{{{
        if(ex.size() < v + 1) ex.resize(v + 1);
        ex[v] += f;
    }//}}}
    void add_dst(int v, Flow f){//{{{
        if(ex.size() < v + 1) ex.resize(v + 1);
        ex[v] -= f;
    }//}}}

    // residueal capacity
    Flow residue(const E& e) const { return e.u - e.f; }
    // reduced cost
    Cost rcost(const E& e) const { return e.c + p[e.s] - p[e.t]; }
    bool is_active(const int v) const { return ex[v] > 0; }
    bool is_admissible(const E &e) const { return residue(e) > 0 and rcost(e) < 0; }

    void add_flow(E& e, const Flow f){//{{{
        e.f += f;
        g[e.t][e.rev].f -= f;
        ex[e.s] -= f;
        ex[e.t] += f;
    }//}}}

    tuple<bool, Cost> run(){//{{{
        constexpr Cost inf = numeric_limits<Cost>::max();
        const int n = max(ex.size(), g.size());
        ex.resize(n);
        g.resize(n);
        p.assign(n, 0);

        for(auto &v : g) for(auto &e : v) if(rcost(e) < 0 or residue(e) < 0)
            add_flow(e, residue(e));

        for(bool cont = true; cont; ){//{{{
            cont = false;
            priority_queue<pair<Cost, int>, vector<pair<Cost, int>>, greater<pair<Cost, int>>> pq;
            vector<int> prev(n, -1);
            vector<Cost> d(n, inf);
            for(int s = 0; s < n; ++s) if(ex[s] > 0) pq.emplace(d[s] = 0, s);
            while(!pq.empty()){//{{{
                Cost c; int v;
                tie(c, v) = pq.top(); pq.pop();
                if(d[v] < c) continue;
                for(auto &e : g[v]) if(residue(e) > 0){
                    const Cost nc = c + rcost(e);
                    if(d[e.t] <= nc) continue;
                    pq.emplace(d[e.t] = nc, e.t);
                    prev[e.t] = e.rev;
                }
            }//}}}
            for(int t = 0; t < n; ++t) if(ex[t] < 0 and d[t] < inf){
                Flow f = -ex[t];
                int s;
                for(s = t; prev[s] != -1; ){
                    auto &re = g[s][prev[s]];
                    f = min(f, residue(g[re.t][re.rev]));
                    s = re.t;
                }
                f = min(f, ex[s]);
                if(f <= 0) continue;
                cont = true;
                for(s = t; prev[s] != -1; ){
                    auto &re = g[s][prev[s]];
                    add_flow(re, -f);
                    if(residue(re) <= 0) prev[s] = -1;
                    s = re.t;
                }
            }
            for(int v = 0; v < n; ++v) if(d[v] < inf)
                p[v] += d[v];
        }//}}}

        for(int v = 0; v < n; ++v) if(ex[v] != 0)
            return make_tuple(false, 0);
        Cost res = 0;
        for(auto &v : g) for(auto &e : v) res += e.f * e.c;
        return make_tuple(true, res / 2);
    }//}}}
};//}}}


template<class Detail, const int _alpha = 2>
struct GoldbergTarjan1990{//{{{
    using Cost = int64_t;
    using Flow = int64_t;
    static const Cost minus_infinity = - numeric_limits<Cost>::max();
    Detail *detail = static_cast<Detail*>(this);

    struct E{//{{{
        int s, t, rev;
        Flow f, u;
        Cost c;
        E(int s, int t, Flow u, Cost c, int rev) : s(s), t(t), rev(rev), f(0), u(u), c(c) {}
    };//}}}
    vector<vector<E>> g;

    const Cost alpha = _alpha; // scaleing factor
    Cost eps;                  // current eps
    bool feasible;             // feasibility
    vector<Flow> ex;           // excess
    vector<Cost> p;            // potential (price, dual variables)
    vector<int> cur;           // current edge


    void add_edge(int s, int t, Flow l, Flow u, Cost c){//{{{
        if(g.size() < max(s, t) + 1) g.resize(max(s, t) + 1);
        int i = g[s].size(), j = s == t ? i+1 : g[t].size();
        assert(l <= u);
        g[s].emplace_back(s, t, u, c, j);
        g[t].emplace_back(t, s,-l,-c, i);
    }//}}}
    void add_src(int v, Flow f){//{{{
        if(ex.size() < v + 1) ex.resize(v + 1);
        ex[v] += f;
    }//}}}
    void add_dst(int v, Flow f){//{{{
        if(ex.size() < v + 1) ex.resize(v + 1);
        ex[v] -= f;
    }//}}}

    // residueal capacity
    Flow residue(const E& e) const { return e.u - e.f; }
    // reduced cost
    Cost rcost(const E& e) const { return e.c + p[e.s] - p[e.t]; }
    bool is_active(const int v) const { return ex[v] > 0; }
    bool is_admissible(const E &e) const { return residue(e) > 0 and rcost(e) < 0; }

    void add_flow(E& e, const Flow f){//{{{
        e.f += f;
        g[e.t][e.rev].f -= f;
        ex[e.s] -= f;
        ex[e.t] += f;
    }//}}}

    void check_invariants() const {//{{{
#ifndef LOCAL_DEBUG
        return;
#endif
        const int n = g.size();
        // capacity constraints
        for(auto &v : g) for(auto &e : v) assert(e.f <= e.u);

        // consistency of excess
        assert(accumulate(begin(ex), end(ex), Flow(0)) == 0);

        // eps optimality
        for(int v = 0; v < n; ++v) for(auto &e : g[v])
            if(residue(e) > 0) assert(rcost(e) >= -eps);

        // admissible graph is acyclic
        vector<int> deg(n);
        for(int v = 0; v < n; ++v) for(auto &e : g[v])
            if(is_admissible(e)) ++deg[e.t];
        queue<int> q;
        for(int v = 0; v < n; ++v) if(deg[v] == 0) q.emplace(v);
        while(!q.empty()){
            int v = q.front(); q.pop();
            for(auto &e : g[v]) if(is_admissible(e))
                if(!--deg[e.t]) q.emplace(e.t);
        }
        assert(*max_element(begin(deg), end(deg)) == 0);
    }//}}}

    tuple<bool, Cost> run(){//{{{
        feasible = true;
        const int n = max(ex.size(), g.size());
        ex.resize(n);
        g.resize(n);

        eps = 1;
        for(auto &v : g) for(auto &e : v) e.c *= alpha * (n + 1);
        for(auto &v : g) for(auto &e : v) while(eps < e.c) eps *= alpha;
        p.assign(n, 0);
        for(; eps >= 1; eps /= alpha) if(!detail->refine())
            return make_tuple(false, 0);
        for(auto &v : g) for(auto &e : v) e.c /= alpha * (n + 1);
        Cost res = 0;
        for(auto &v : g) for(auto &e : v) res += e.f * e.c;
        return make_tuple(true, res / 2);
    }//}}}
};//}}}

template<class MCF>
class WaveRule{//{{{
    // typename add_pointer<typename add_const<MCF>::type>::type mcf = nullptr;
    // Give me C++14 (alias template)!!!!
    const MCF *mcf = nullptr;
    size_t cur;
    deque<size_t> q;
    bool is_active(const size_t v){
        return this->mcf->ex[v] > 0;
    }
    void next_scan(){
        cur = 0;
        deque<size_t> nq;
        for(auto v : q){
            if(is_active(v)) nq.emplace_front(v);
            else             nq.emplace_back(v);
        }
        swap(q, nq);
    }
    public:
    void init(const MCF &mcf){
        this->mcf = &mcf;
        const size_t n = this->mcf->g.size();
        q.resize(n);
        iota(begin(q), end(q), 0u);
        next_scan();
    }
    void check_update(const size_t v){
        return;
    }
    bool has_active(){
        for(; cur < this->mcf->g.size(); ++cur)
            if(is_active(cur)) return true;
        next_scan();
        return is_active(q.front());
    }
    size_t extract_active(){
        assert(is_active(q[cur]));
        return q[cur++];
    }
};//}}}

template<class MCF>
class FIFORule{//{{{
    // typename add_pointer<typename add_const<MCF>::type>::type mcf = nullptr;
    // Give me C++14 (alias template)!!!!
    const MCF *mcf = nullptr;
    size_t cur;
    deque<size_t> q;
    vector<char> in_q;
    bool is_active(const size_t v){
        return this->mcf->ex[v] > 0;
    }
    void next_scan(){
        cur = 0;
        deque<size_t> nq;
        for(auto v : q){
            if(is_active(v)) nq.emplace_front(v);
            else             nq.emplace_back(v);
        }
        swap(q, nq);
    }
    void push(const size_t v){
        in_q[v] = true;
        q.emplace_back(v);
    }
    size_t pop(){
        const size_t v = q.front();
        in_q[v] = false;
        q.pop_front();
        return v;
    }
    public:
    void init(const MCF &mcf){
        this->mcf = &mcf;
        const size_t n = this->mcf->g.size();
        in_q.resize(n);
        for(size_t v = 0; v < n; ++v) if(is_active(v))
            push(v);
    }
    void check_update(const size_t v){
        if(is_active(v) and !in_q[v])
            push(v);
    }
    bool has_active(){
        while(!q.empty() and !is_active(q.front())) pop();
        return !q.empty();
    }
    size_t extract_active(){
        assert(is_active(q.front()));
        return pop();
    }
};//}}}

template<class MCF>
class LIFORule{//{{{
    // typename add_pointer<typename add_const<MCF>::type>::type mcf = nullptr;
    // Give me C++14 (alias template)!!!!
    const MCF *mcf = nullptr;
    size_t cur;
    deque<size_t> q;
    vector<char> in_q;
    bool is_active(const size_t v){
        return this->mcf->ex[v] > 0;
    }
    void next_scan(){
        cur = 0;
        deque<size_t> nq;
        for(auto v : q){
            if(is_active(v)) nq.emplace_front(v);
            else             nq.emplace_back(v);
        }
        swap(q, nq);
    }
    void push(const size_t v){
        in_q[v] = true;
        q.emplace_front(v);
    }
    size_t pop(){
        const size_t v = q.front();
        in_q[v] = false;
        q.pop_front();
        return v;
    }
    public:
    void init(const MCF &mcf){
        this->mcf = &mcf;
        const size_t n = this->mcf->g.size();
        in_q.resize(n);
        for(size_t v = 0; v < n; ++v) if(is_active(v))
            push(v);
    }
    void check_update(const size_t v){
        if(is_active(v) and !in_q[v])
            push(v);
    }
    bool has_active(){
        while(!q.empty() and !is_active(q.front())) pop();
        return !q.empty();
    }
    size_t extract_active(){
        assert(is_active(q.front()));
        return pop();
    }
};//}}}

// push/relabel variant. (wave ordering)
// O((n^3 + nm) log(nC))
struct PushRelabelVariant : GoldbergTarjan1990<PushRelabelVariant> {//{{{

    // return is_surturate_push
    bool push(E &e){//{{{
        assert(is_active(e.s) and is_admissible(e));
        if(ex[e.s] < residue(e)){
            add_flow(e, ex[e.s]);
            return false;
        }else{
            add_flow(e, residue(e));
            return true;
        }
    }//}}}
    long long relabel_count;
    void relabel(const int v){//{{{
        assert(is_active(v));
        feasible &= --relabel_count > 0;
        for(auto &e : g[v]) assert(!is_admissible(e));
        p[v] = minus_infinity;
        for(auto &e : g[v]) if(residue(e) > 0) p[v] = max(p[v], p[e.t] - e.c);
        feasible &= p[v] != minus_infinity;
        p[v] -= eps;
        cur[v] = 0;
    }//}}}

    // return relabeled
    bool discharge(const int v){//{{{
        assert(is_active(v));
        for(int &i = cur[v]; i < g[v].size(); ++i){
            if(!is_admissible(g[v][i])) continue;
            if(!push(g[v][i])) return false;
            if(!is_active(v)){ ++i; return false; }
        }
        relabel(v);
        return true;
    }//}}}

    // return feasibility
    bool refine(){//{{{
        // TODO: check eps-optimality.
        for(auto &v : g) for(auto &e : v) if(residue(e) < 0 or rcost(e) < 0)
            add_flow(e, residue(e));
        relabel_count = (long long)(alpha + 1) * g.size() * g.size();
        cur.assign(g.size(), 0);

        deque<int> vs(g.size()); iota(begin(vs), end(vs), 0);
        for(bool cont = true; cont; ){
            cont = false;
            deque<int> nvs;
            for(auto v : vs){
                cont |= is_active(v);
                if(is_active(v) and discharge(v)){
                    nvs.emplace_front(v);
                    if(!feasible) return false;
                }else{
                    nvs.emplace_back(v);
                }
            }
            swap(vs, nvs);
        }
        return true;
    }//}}}
};//}}}

// blocking flow variant.
// O(n B(n,m) log(nC)) where B(n, m) = nm
struct BlockingFlowVariant : GoldbergTarjan1990<BlockingFlowVariant>{//{{{

    Flow blocking_flow(const int v, Flow flow){//{{{
        if(ex[v] < 0 or flow == 0) return min(-ex[v], flow);
        Flow sum = 0;
        for(int &i = cur[v]; i < g[v].size(); ++i){
            E &e = g[v][i];
            if(!is_admissible(e)) continue;
            Flow f = blocking_flow(e.t, min(flow - sum, residue(e)));
            if(f > 0) add_flow(e, f);
            if(flow == (sum += f)) break;
        }
        return sum;
    }//}}}

    // return feasibility
    bool refine(){//{{{
        // TODO: check eps-optimality.
        for(auto &v : g) for(auto &e : v) if(residue(e) < 0 or rcost(e) < 0)
            add_flow(e, residue(e));

        const int n = g.size();
        for(int i = (alpha + 1) * n + 10; i >= 0; --i){
            vector<int> used(n, 0), q;
            for(int v = 0; v < n; ++v) if(ex[v] > 0){
                used[v] = true;
                q.emplace_back(v);
            }
            if(q.empty()) return true;
            while(!q.empty()){
                int v = q.back(); q.pop_back();
                for(auto &e : g[v]) if(!used[e.t] and is_admissible(e)){
                    used[e.t] = true;
                    q.emplace_back(e.t);
                }
            }
            for(int v = 0; v < n; ++v) if(used[v]) p[v] -= eps;
            cur.assign(g.size(), 0);
            for(int v = 0; v < n; ++v) if(ex[v] > 0) blocking_flow(v, ex[v]);
        }
        return false;
    }//}}}
};//}}}

/*
//{{{
たまに price update (a.k.a. global update, set relabel) をする.
price update は, 上に書いた通り.
v が i 回目の -= eps で S に入るとき,
w : (v, w) は
    i + (rcost(v, w) + eps) / eps  ( >= i )
回目の -= eps で S に入れる.
各ノードで, refine 間に -= eps は高々 (alpha + 1) n 回しか行われないから,
(alpha + 1) n バケットあれば足りる.

O(n + m) で動く.
relabel は O(n^2) 回/refine だから, Omega(n) 回の refine に一回やると,
合計で O(n(n+m)) のコスト.


q[v] = p[v] - d[v] * eps
とすると, eps-optimality から,
for all residueal edge (u, v):
    cost_q(u, v) >= -eps
だから,
for all residual edge (u, v):
    cost(u, v) + q[u] - q[v] >= -eps
    <=>
    cost(u, v) + p[u] - p[v] - (d[u] - d[v]) * eps >= -eps
    <=>
    cost_p(u, v) - (d[u] - d[v]) * eps >= -eps
    <=>
    (d[u] - d[v]) * eps <= eps + cost_p(u, v)
    <=>
    (d[u] - d[v]) <= floor((eps + cost_p(u, v)) / eps)
で, 右辺は非負だから, residua graph の逆グラフで
コストを floor((eps + cost_p(u, v)) / eps) にしたやつの牛ゲー.

for all residual edge (u, v):
    d[u] <= floor((eps + cost_p(u,v))/eps) + d[v]
//}}}
 */
struct GlobalUpdate : GoldbergTarjan1990<GlobalUpdate, 4> {//{{{

    // return is_surturate_push
    bool push(E &e){//{{{
        assert(is_active(e.s) and is_admissible(e));
        if(ex[e.s] < residue(e)){
            add_flow(e, ex[e.s]);
            return false;
        }else{
            add_flow(e, residue(e));
            return true;
        }
    }//}}}
    long long relabel_count;
    void relabel(const int v){//{{{
        assert(is_active(v));
        feasible &= --relabel_count > 0;
        for(auto &e : g[v]) assert(!is_admissible(e));
        p[v] = minus_infinity;
        for(auto &e : g[v]) if(residue(e) > 0) p[v] = max(p[v], p[e.t] - e.c);
        feasible &= p[v] != minus_infinity;
        p[v] -= eps;
        cur[v] = 0;
    }//}}}

    // return relabeled
    bool discharge(const int v){//{{{
        assert(is_active(v));
        for(int &i = cur[v]; i < g[v].size(); ++i){
            if(!is_admissible(g[v][i])) continue;
            if(!push(g[v][i])) return false;
            if(!is_active(v)){ ++i; return false; }
        }
        relabel(v);
        return true;
    }//}}}

    vector<vector<int>> bucket;

    // return true iff it updates potential
    bool global_update(){//{{{
        check_invariants();
        const int n = g.size();
        const int L = (alpha + 1) * n; // + 1;
        bucket.assign(L + 1, vector<int>());
        vector<int> d(n, L + 1);
        for(int v = 0; v < n; ++v) if(ex[v] < 0)
            bucket[d[v] = 0].emplace_back(v);
        Flow sum = 0;
        for(int l = 0; true; ++l){
            while(!bucket[l].empty()){
                int v = bucket[l].back(); bucket[l].pop_back();
                if(d[v] < l) continue;
                sum += ex[v];
                for(auto &re : g[v]){
                    const int u = re.t;
                    auto &e = g[u][re.rev];
                    if(residue(e) <= 0) continue;
                    assert(rcost(e) >= -eps);
                    Cost len = (rcost(e)+eps)/eps;
                    assert(len >= 0);
                    if(len > L) continue;
                    int nd = d[v] + static_cast<int>(len);
                    if(d[u] <= nd) continue;
                    bucket[d[u] = nd].emplace_back(u);
                }
            }
            if(sum < 0) continue;
            if(l == 0) return false;
            cur.assign(n, 0);
            for(int v = 0; v < n; ++v) p[v] -= min(d[v], l) * eps;
            check_invariants();
            return true;
        }
        assert(false);
        return true;
    }//}}}

    // return feasibility
    bool refine(){//{{{
        const int set_relabel_period = g.size();

        for(auto &v : g) for(auto &e : v) if(residue(e) < 0 or rcost(e) < 0)
            add_flow(e, residue(e));
        relabel_count = (long long)(alpha + 1) * g.size() * g.size();
        long long next_set_relabel = relabel_count - set_relabel_period;
        cur.assign(g.size(), 0);

        deque<int> vs(g.size()); iota(begin(vs), end(vs), 0);
        for(bool cont = true; cont; ){
            cont = false;
            deque<int> nvs;
            for(auto v : vs){
                cont |= is_active(v);
                if(is_active(v) and discharge(v)){
                    nvs.emplace_front(v);
                    if(!feasible) return false;
                }else{
                    nvs.emplace_back(v);
                }
                if(relabel_count < next_set_relabel){
                    check_invariants();
                    cont |= global_update();
                    next_set_relabel -= set_relabel_period;
                }
            }
            swap(vs, nvs);
        }
        Cost base = *min_element(begin(p), end(p));
        //for(int v = 0; v < g.size(); ++v) p[v] -= base;
        check_invariants();
        for(int v = 0; v < g.size(); ++v) assert(ex[v] == 0);
        return true;
    }//}}}
};//}}}

/*
//{{{
potential refinement をする.

potential refinement は,
alpha eps optimal な flow f w.r.t. pot. p が与えられたとき,
f が alpha optimal w.r.t. pot q = p + d * eps となるような d を見つけるか,
f が alpha optimal で無いことを示すか, 失敗する.

失敗するのは, admissible graph が cycle を含むようになったとき.
これはもちろん負サイクルだから, こいつをサチュってごにょりたいけれど,
それは potential の update に繋げないと(どうせすぐ後で変わるので)意味がない.
そうするのは面倒なので, 放置して失敗したことにする.

f が alpha optimal w.r.t. p + d * eps
<=>
residual graph G_f 上の各辺 e = (u, v) が,
    c^{p + d * eps}_e >= - eps
    <=>
    c^p_e + (d_u - d_v) * eps >= -eps
    <=>
    (d_v - d_u) * eps <= c^p_e + eps.
    <=>
    (d_v - d_u) <= floor((c^p_e + eps) / eps)
なので, これは residual graph 上コスト floor((c^p_e+eps)/eps) の牛.
ところで, f は alpha eps optimal w.r.t. p だから,
    c^p_e >= - alpha eps
なので,
    c^p_e + eps >= (1 - alpha) eps
    (c^p_e + eps)/eps >= (1 - alpha)
    floor((c^p_e+eps)/eps) >= floor(1 - alpha) = 1 - alpha (if alpha : integer)
で 負コストはあっても (1 - alpha) 程度.
これに負サイクルがあってはいけない.

e = (u, v) が admissible arc である条件は,
    e が residual graph の辺かつ (-alpha eps <=) c^p_e < 0
admissible graph が acyclic だから,
この上での(indeg = 0 な点全体からの)最短路の計算は O(n+m) でできる.

admissible arc e = (u, v) による制約は
    d_v - d_u <= floor((c^p_e + eps) / eps)
    で,
    floor(1 - alpha) <= floor((c^p_e + eps)/eps) < floor(eps/eps) = 1
だから, コストが非正な辺のみ.
コストが負な辺は,
    floor((c^p_e + eps)/eps) < 0,
    i.e. c^p_e < -eps
だから, eps-optimality を violate している辺.

最短路長が最も短い(i.e. 最も indeg = 0 から遠い)頂点集合 S を取ってくる.
これが全頂点ならば, すなわち負コストの辺が無いから, eps-optimal.
そうでない場合を考える.
S のポテンシャルを -= eps すると,
  ポテンシャルが下がる辺は, e = (u, v) で u \in S, v \not \in S.
  v \not \in S だから, v は u よりも indeg=0 から近い(距離の意味で遠い).
  admissible arc はコストが非正になるはずだったから, e は admissible でない.
  すなわち, c^p_e >= 0 だから, -= eps しても eps-optimality は変わらない.

Global Update と同様に, 後何回 set を -= eps したらこいつに入る枝が
admissible になって最短路長が最も短いところになるかみたいなのを,
バケットで管理する.

//}}}
 */
struct PotentialRefine : GoldbergTarjan1990<PotentialRefine, 4> {//{{{

    // return is_surturate_push
    bool push(E &e){//{{{
        assert(is_active(e.s) and is_admissible(e));
        if(ex[e.s] < residue(e)){
            add_flow(e, ex[e.s]);
            return false;
        }else{
            add_flow(e, residue(e));
            return true;
        }
    }//}}}
    long long relabel_count;
    void relabel(const int v){//{{{
        assert(is_active(v));
        feasible &= --relabel_count > 0;
        for(auto &e : g[v]) assert(!is_admissible(e));
        p[v] = minus_infinity;
        for(auto &e : g[v]) if(residue(e) > 0) p[v] = max(p[v], p[e.t] - e.c);
        feasible &= p[v] != minus_infinity;
        p[v] -= eps;
        cur[v] = 0;
    }//}}}

    // return relabeled
    bool discharge(const int v){//{{{
        assert(is_active(v));
        for(int &i = cur[v]; i < g[v].size(); ++i){
            if(!is_admissible(g[v][i])) continue;
            if(!push(g[v][i])) return false;
            if(!is_active(v)){ ++i; return false; }
        }
        relabel(v);
        return true;
    }//}}}

    vector<vector<int>> bucket;

    // return true iff success to refine to eps-feasible potential
    bool potential_refine(){//{{{
        const int n = g.size();
        const int L = (alpha - 1) * n + 1;
        bucket.assign(L+1, vector<int>());
        queue<int> q;
        vector<int> deg(n), d(n);
        while(true){
            for(int v = 0; v < n; ++v) for(auto &e : g[v])
                if(is_admissible(e)) ++deg[e.t];
            for(int v = 0; v < n; ++v) if(deg[v] == 0) q.emplace(v);
            d.assign(n, 0);
            for(; !q.empty(); q.pop()){
                const int v = q.front();
                for(auto &e : g[v]) if(is_admissible(e)){
                    if(!--deg[e.t]) q.emplace(e.t);
                    const int c = (-rcost(e)-1) / eps;
                    assert(c >= 0);
                    d[e.t] = max(d[e.t], d[v] + c);
                }
            }
            if(*max_element(begin(deg), end(deg)) != 0) return false;
            if(*max_element(begin(d), end(d)) == 0) return true;
            for(int v = 0; v < n; ++v) bucket[d[v]].emplace_back(v);
            for(int l = L; l >= 0; --l){
                for(; !bucket[l].empty(); bucket[l].pop_back()){
                    const int v = bucket[l].back();
                    if(d[v] > l) continue;
                    for(auto &e : g[v]) if(d[e.t] < l){
                        if(residue(e) <= 0) continue;
                        const Cost c = rcost(e) < 0 ? 0 : (rcost(e)+eps)/eps;
                        if(c >= d[v]) continue;
                        if(d[e.t] >= d[v] - c) continue;
                        bucket[d[e.t] = d[v] - c].emplace_back(e.t);
                    }
                }
            }
            for(int v = 0; v < n; ++v) p[v] -= eps * d[v];
        }
        assert(false);
    }//}}}

    const int start_potential_refine = 2;
    int refine_count = 0;
    // return feasibility
    bool refine(){//{{{
        if(++refine_count >= start_potential_refine && potential_refine()) return true;

        for(auto &v : g) for(auto &e : v) if(residue(e) < 0 or rcost(e) < 0)
            add_flow(e, residue(e));
        relabel_count = (long long)(alpha + 1) * g.size() * g.size();
        cur.assign(g.size(), 0);

        deque<int> vs(g.size()); iota(begin(vs), end(vs), 0);
        for(bool cont = true; cont; ){
            cont = false;
            deque<int> nvs;
            for(auto v : vs){
                cont |= is_active(v);
                if(is_active(v) and discharge(v)){
                    nvs.emplace_front(v);
                    if(!feasible) return false;
                }else{
                    nvs.emplace_back(v);
                }
            }
            swap(vs, nvs);
        }
        //Cost base = *min_element(begin(p), end(p));
        //for(int v = 0; v < g.size(); ++v) p[v] -= base;
        check_invariants();
        for(int v = 0; v < g.size(); ++v) assert(ex[v] == 0);
        return true;
    }//}}}
};//}}}

struct GlobalUpdatePotentialRefine : GoldbergTarjan1990<GlobalUpdatePotentialRefine, 4> {//{{{

    // return is_surturate_push
    bool push(E &e){//{{{
        assert(is_active(e.s) and is_admissible(e));
        if(ex[e.s] < residue(e)){
            add_flow(e, ex[e.s]);
            return false;
        }else{
            add_flow(e, residue(e));
            return true;
        }
    }//}}}
    long long relabel_count;
    void relabel(const int v){//{{{
        assert(is_active(v));
        feasible &= --relabel_count > 0;
        for(auto &e : g[v]) assert(!is_admissible(e));
        p[v] = minus_infinity;
        for(auto &e : g[v]) if(residue(e) > 0) p[v] = max(p[v], p[e.t] - e.c);
        feasible &= p[v] != minus_infinity;
        p[v] -= eps;
        cur[v] = 0;
    }//}}}

    // return relabeled
    bool discharge(const int v){//{{{
        assert(is_active(v));
        for(int &i = cur[v]; i < g[v].size(); ++i){
            if(!is_admissible(g[v][i])) continue;
            if(!push(g[v][i])) return false;
            if(!is_active(v)){ ++i; return false; }
        }
        relabel(v);
        return true;
    }//}}}

    vector<vector<int>> bucket;

    // return true iff it updates potential
    bool global_update(){//{{{
        check_invariants();
        const int n = g.size();
        const int L = (alpha + 1) * n;
        bucket.assign(L + 1, vector<int>());
        vector<int> d(n, L + 1);
        for(int v = 0; v < n; ++v) if(ex[v] < 0)
            bucket[d[v] = 0].emplace_back(v);
        Flow sum = 0;
        for(int l = 0; true; ++l){
            while(!bucket[l].empty()){
                int v = bucket[l].back(); bucket[l].pop_back();
                if(d[v] < l) continue;
                sum += ex[v];
                for(auto &re : g[v]){
                    const int u = re.t;
                    auto &e = g[u][re.rev];
                    if(residue(e) <= 0) continue;
                    assert(rcost(e) >= -eps);
                    Cost len = (rcost(e)+eps)/eps;
                    assert(len >= 0);
                    if(len > L) continue;
                    int nd = d[v] + static_cast<int>(len);
                    if(d[u] <= nd) continue;
                    bucket[d[u] = nd].emplace_back(u);
                }
            }
            if(sum < 0) continue;
            if(l == 0) return false;
            cur.assign(n, 0);
            for(int v = 0; v < n; ++v) p[v] -= min(d[v], l) * eps;
            check_invariants();
            return true;
        }
        assert(false);
        return true;
    }//}}}
    // return true iff success to refine to eps-feasible potential
    bool potential_refine(){//{{{
        const int n = g.size();
        const int L = (alpha - 1) * n + 1;
        bucket.assign(L+1, vector<int>());
        queue<int> q;
        vector<int> deg(n), d(n);
        while(true){
            for(int v = 0; v < n; ++v) for(auto &e : g[v])
                if(is_admissible(e)) ++deg[e.t];
            for(int v = 0; v < n; ++v) if(deg[v] == 0) q.emplace(v);
            d.assign(n, 0);
            for(; !q.empty(); q.pop()){
                const int v = q.front();
                for(auto &e : g[v]) if(is_admissible(e)){
                    if(!--deg[e.t]) q.emplace(e.t);
                    const int c = (-rcost(e)-1) / eps;
                    assert(c >= 0);
                    d[e.t] = max(d[e.t], d[v] + c);
                }
            }
            if(*max_element(begin(deg), end(deg)) != 0) return false;
            if(*max_element(begin(d), end(d)) == 0) return true;
            for(int v = 0; v < n; ++v) bucket[d[v]].emplace_back(v);
            for(int l = L; l >= 0; --l){
                for(; !bucket[l].empty(); bucket[l].pop_back()){
                    const int v = bucket[l].back();
                    if(d[v] > l) continue;
                    for(auto &e : g[v]) if(d[e.t] < l){
                        if(residue(e) <= 0) continue;
                        const Cost c = rcost(e) < 0 ? 0 : (rcost(e)+eps)/eps;
                        if(c >= d[v]) continue;
                        if(d[e.t] >= d[v] - c) continue;
                        bucket[d[e.t] = d[v] - c].emplace_back(e.t);
                    }
                }
            }
            for(int v = 0; v < n; ++v) p[v] -= eps * d[v];
        }
        assert(false);
    }//}}}

    const int start_potential_refine = 2;
    int refine_count = 0;
    // return feasibility
    bool refine(){//{{{
        if(++refine_count >= start_potential_refine && potential_refine()) return true;
        const int set_relabel_period = g.size();

        for(auto &v : g) for(auto &e : v) if(residue(e) < 0 or rcost(e) < 0)
            add_flow(e, residue(e));
        relabel_count = (long long)(alpha + 1) * g.size() * g.size();
        long long next_set_relabel = relabel_count - set_relabel_period;
        cur.assign(g.size(), 0);

        deque<int> vs(g.size()); iota(begin(vs), end(vs), 0);
        for(bool cont = true; cont; ){
            cont = false;
            deque<int> nvs;
            for(auto v : vs){
                cont |= is_active(v);
                if(is_active(v) and discharge(v)){
                    nvs.emplace_front(v);
                    if(!feasible) return false;
                }else{
                    nvs.emplace_back(v);
                }
                if(relabel_count < next_set_relabel){
                    check_invariants();
                    cont |= global_update();
                    next_set_relabel -= set_relabel_period;
                }
            }
            swap(vs, nvs);
        }
        Cost base = *min_element(begin(p), end(p));
        //for(int v = 0; v < g.size(); ++v) p[v] -= base;
        check_invariants();
        for(int v = 0; v < g.size(); ++v) assert(ex[v] == 0);
        return true;
    }//}}}
};//}}}

/*
単純に push するんじゃなくて, 微妙に path を探してそこに push する.
したがって, 連続的に push しつつ, relabel する.
discharge を augment path 上に push しながら行う感じ.

もうちょっと relabel chance 的なものをうまく活かす.
 */
struct PartialAugmentRelabel : GoldbergTarjan1990<PartialAugmentRelabel, 4> {//{{{

    // return is_surturate_push
    bool push(E &e){//{{{
        assert(is_active(e.s) and is_admissible(e));
        if(ex[e.s] < residue(e)){
            add_flow(e, ex[e.s]);
            return false;
        }else{
            add_flow(e, residue(e));
            return true;
        }
    }//}}}
    long long relabel_count;
    void relabel(const int v){//{{{
        assert(is_active(v));
        feasible &= --relabel_count > 0;
        for(auto &e : g[v]) assert(!is_admissible(e));
        p[v] = minus_infinity;
        for(auto &e : g[v]) if(residue(e) > 0) p[v] = max(p[v], p[e.t] - e.c);
        feasible &= p[v] != minus_infinity;
        p[v] -= eps;
        cur[v] = 0;
    }//}}}

    // return relabeled
    bool discharge(const int v){//{{{
        assert(is_active(v));
        for(int &i = cur[v]; i < g[v].size(); ++i){
            if(!is_admissible(g[v][i])) continue;
            if(!push(g[v][i])) return false;
            if(!is_active(v)){ ++i; return false; }
        }
        relabel(v);
        return true;
    }//}}}

    bool augment(int v, int rest){//{{{
        if(rest == 0 or !is_active(v)) return false;
        bool res = false;
        for(int &i = cur[v]; i < g[v].size(); ++i){
            E &e = g[v][i];
            if(!is_admissible(e)) continue;
            push(e);
            res |= augment(e.t, rest - 1);
            if(!is_active(v)) return res;
        }
        relabel(v);
        return true;
    }//}}}
    int refine_count = 0;
    // return feasibility
    bool refine(){//{{{
        // if(refine_count++ && potential_refine()) return true;

        for(auto &v : g) for(auto &e : v) if(residue(e) < 0 or rcost(e) < 0)
            add_flow(e, residue(e));
        relabel_count = (long long)(alpha + 1) * g.size() * g.size();
        cur.assign(g.size(), 0);

        deque<int> vs(g.size()); iota(begin(vs), end(vs), 0);
        for(bool cont = true; cont; ){
            cont = false;
            deque<int> nvs;
            for(auto v : vs){
                cont |= is_active(v);
                if(is_active(v) and augment(v, g.size()+1)){
                    nvs.emplace_front(v);
                    if(!feasible) return false;
                }else{
                    nvs.emplace_back(v);
                }
            }
            swap(vs, nvs);
        }
        //Cost base = *min_element(begin(p), end(p));
        //for(int v = 0; v < g.size(); ++v) p[v] -= base;
        check_invariants();
        for(int v = 0; v < g.size(); ++v) assert(ex[v] == 0);
        return true;
    }//}}}
};//}}}

constexpr size_t SCALING_FACTOR = 16;
constexpr size_t AUG_LENGTH = 3;
struct PartialAugmentRelabelWithHeuristics : GoldbergTarjan1990<PartialAugmentRelabelWithHeuristics, SCALING_FACTOR> {//{{{

    // return is_surturate_push
    bool push(E &e){//{{{
        assert(is_active(e.s) and is_admissible(e));
        if(ex[e.s] < residue(e)){
            add_flow(e, ex[e.s]);
            return false;
        }else{
            add_flow(e, residue(e));
            return true;
        }
    }//}}}
    long long relabel_count;
    void relabel(const int v){//{{{
        assert(is_active(v));
        feasible &= --relabel_count > 0;
        for(auto &e : g[v]) assert(!is_admissible(e));
        p[v] = minus_infinity;
        for(auto &e : g[v]) if(residue(e) > 0) p[v] = max(p[v], p[e.t] - e.c);
        feasible &= p[v] != minus_infinity;
        p[v] -= eps;
        cur[v] = 0;
    }//}}}

    // return relabeled
    bool discharge(const int v){//{{{
        assert(is_active(v));
        for(int &i = cur[v]; i < g[v].size(); ++i){
            if(!is_admissible(g[v][i])) continue;
            if(!push(g[v][i])) return false;
            if(!is_active(v)){ ++i; return false; }
        }
        relabel(v);
        return true;
    }//}}}

    vector<vector<int>> bucket;

    // return true iff it updates potential
    bool global_update(){//{{{
        check_invariants();
        const int n = g.size();
        const int L = (alpha + 1) * n;
        bucket.assign(L + 1, vector<int>());
        vector<int> d(n, L + 1);
        for(int v = 0; v < n; ++v) if(ex[v] < 0)
            bucket[d[v] = 0].emplace_back(v);
        Flow sum = 0;
        for(int l = 0; true; ++l){
            while(!bucket[l].empty()){
                int v = bucket[l].back(); bucket[l].pop_back();
                if(d[v] < l) continue;
                sum += ex[v];
                for(auto &re : g[v]){
                    const int u = re.t;
                    auto &e = g[u][re.rev];
                    if(residue(e) <= 0) continue;
                    assert(rcost(e) >= -eps);
                    Cost len = (rcost(e)+eps)/eps;
                    assert(len >= 0);
                    if(len > L) continue;
                    int nd = d[v] + static_cast<int>(len);
                    if(d[u] <= nd) continue;
                    bucket[d[u] = nd].emplace_back(u);
                }
            }
            if(sum < 0) continue;
            if(l == 0) return false;
            cur.assign(n, 0);
            for(int v = 0; v < n; ++v) p[v] -= min(d[v], l) * eps;
            check_invariants();
            return true;
        }
        assert(false);
        return true;
    }//}}}
    // return true iff success to refine to eps-feasible potential
    bool potential_refine(){//{{{
        const int n = g.size();
        const int L = (alpha - 1) * n + 1;
        bucket.assign(L+1, vector<int>());
        queue<int> q;
        vector<int> deg(n), d(n);
        while(true){
            for(int v = 0; v < n; ++v) for(auto &e : g[v])
                if(is_admissible(e)) ++deg[e.t];
            for(int v = 0; v < n; ++v) if(deg[v] == 0) q.emplace(v);
            d.assign(n, 0);
            for(; !q.empty(); q.pop()){
                const int v = q.front();
                for(auto &e : g[v]) if(is_admissible(e)){
                    if(!--deg[e.t]) q.emplace(e.t);
                    const int c = (-rcost(e)-1) / eps;
                    assert(c >= 0);
                    d[e.t] = max(d[e.t], d[v] + c);
                }
            }
            if(*max_element(begin(deg), end(deg)) != 0) return false;
            if(*max_element(begin(d), end(d)) == 0) return true;
            for(int v = 0; v < n; ++v) bucket[d[v]].emplace_back(v);
            for(int l = L; l >= 0; --l){
                for(; !bucket[l].empty(); bucket[l].pop_back()){
                    const int v = bucket[l].back();
                    if(d[v] > l) continue;
                    for(auto &e : g[v]) if(d[e.t] < l){
                        if(residue(e) <= 0) continue;
                        const Cost c = rcost(e) < 0 ? 0 : (rcost(e)+eps)/eps;
                        if(c >= d[v]) continue;
                        if(d[e.t] >= d[v] - c) continue;
                        bucket[d[e.t] = d[v] - c].emplace_back(e.t);
                    }
                }
            }
            for(int v = 0; v < n; ++v) p[v] -= eps * d[v];
        }
        assert(false);
    }//}}}

    bool augment(int v, int rest){//{{{
        if(rest == 0 or !is_active(v)) return false;
        bool res = false;
        for(int &i = cur[v]; i < g[v].size(); ++i){
            E &e = g[v][i];
            if(!is_admissible(e)) continue;
            push(e);
            res |= augment(e.t, rest - 1);
            if(!is_active(v)) return res;
        }
        relabel(v);
        return true;
    }//}}}

    const int start_potential_refine = 2;
    int refine_count = 0;
    // return feasibility
    bool refine(){//{{{
        if(++refine_count >= start_potential_refine && potential_refine()) return true;
        const int set_relabel_period = g.size();

        for(auto &v : g) for(auto &e : v) if(residue(e) < 0 or rcost(e) < 0)
            add_flow(e, residue(e));
        relabel_count = (long long)(alpha + 1) * g.size() * g.size();
        long long next_set_relabel = relabel_count - set_relabel_period;
        cur.assign(g.size(), 0);

        deque<int> vs(g.size()); iota(begin(vs), end(vs), 0);
        for(bool cont = true; cont; ){
            cont = false;
            deque<int> nvs;
            for(auto v : vs){
                cont |= is_active(v);
                if(is_active(v) and augment(v, AUG_LENGTH)){
                    nvs.emplace_front(v);
                    if(!feasible) return false;
                }else{
                    nvs.emplace_back(v);
                }
                if(relabel_count < next_set_relabel){
                    check_invariants();
                    cont |= global_update();
                    next_set_relabel -= set_relabel_period;
                }
            }
            swap(vs, nvs);
        }
        Cost base = *min_element(begin(p), end(p));
        //for(int v = 0; v < g.size(); ++v) p[v] -= base;
        check_invariants();
        for(int v = 0; v < g.size(); ++v) assert(ex[v] == 0);
        return true;
    }//}}}
};//}}}

struct PartialAugmentRelabelWithHeuristicsFIFO : GoldbergTarjan1990<PartialAugmentRelabelWithHeuristicsFIFO, SCALING_FACTOR> {//{{{
    FIFORule<PartialAugmentRelabelWithHeuristicsFIFO> rule;

    // return is_surturate_push
    bool push(E &e){//{{{
        assert(is_active(e.s) and is_admissible(e));
        if(ex[e.s] < residue(e)){
            add_flow(e, ex[e.s]);
            return false;
        }else{
            add_flow(e, residue(e));
            return true;
        }
    }//}}}
    long long relabel_count;
    void relabel(const int v){//{{{
        assert(is_active(v));
        feasible &= --relabel_count > 0;
        for(auto &e : g[v]) assert(!is_admissible(e));
        p[v] = minus_infinity;
        for(auto &e : g[v]) if(residue(e) > 0) p[v] = max(p[v], p[e.t] - e.c);
        feasible &= p[v] != minus_infinity;
        p[v] -= eps;
        cur[v] = 0;
    }//}}}

    // return relabeled
    bool discharge(const int v){//{{{
        assert(is_active(v));
        for(int &i = cur[v]; i < g[v].size(); ++i){
            if(!is_admissible(g[v][i])) continue;
            if(!push(g[v][i])) return false;
            if(!is_active(v)){ ++i; return false; }
        }
        relabel(v);
        return true;
    }//}}}

    vector<vector<int>> bucket;

    // return true iff it updates potential
    bool global_update(){//{{{
        check_invariants();
        const int n = g.size();
        const int L = (alpha + 1) * n;
        bucket.assign(L + 1, vector<int>());
        vector<int> d(n, L + 1);
        for(int v = 0; v < n; ++v) if(ex[v] < 0)
            bucket[d[v] = 0].emplace_back(v);
        Flow sum = 0;
        for(int l = 0; true; ++l){
            while(!bucket[l].empty()){
                int v = bucket[l].back(); bucket[l].pop_back();
                if(d[v] < l) continue;
                sum += ex[v];
                for(auto &re : g[v]){
                    const int u = re.t;
                    auto &e = g[u][re.rev];
                    if(residue(e) <= 0) continue;
                    assert(rcost(e) >= -eps);
                    Cost len = (rcost(e)+eps)/eps;
                    assert(len >= 0);
                    if(len > L) continue;
                    int nd = d[v] + static_cast<int>(len);
                    if(d[u] <= nd) continue;
                    bucket[d[u] = nd].emplace_back(u);
                }
            }
            if(sum < 0) continue;
            if(l == 0) return false;
            cur.assign(n, 0);
            for(int v = 0; v < n; ++v) p[v] -= min(d[v], l) * eps;
            check_invariants();
            return true;
        }
        assert(false);
        return true;
    }//}}}
    // return true iff success to refine to eps-feasible potential
    bool potential_refine(){//{{{
        const int n = g.size();
        const int L = (alpha - 1) * n + 1;
        bucket.assign(L+1, vector<int>());
        queue<int> q;
        vector<int> deg(n), d(n);
        while(true){
            for(int v = 0; v < n; ++v) for(auto &e : g[v])
                if(is_admissible(e)) ++deg[e.t];
            for(int v = 0; v < n; ++v) if(deg[v] == 0) q.emplace(v);
            d.assign(n, 0);
            for(; !q.empty(); q.pop()){
                const int v = q.front();
                for(auto &e : g[v]) if(is_admissible(e)){
                    if(!--deg[e.t]) q.emplace(e.t);
                    const int c = (-rcost(e)-1) / eps;
                    assert(c >= 0);
                    d[e.t] = max(d[e.t], d[v] + c);
                }
            }
            if(*max_element(begin(deg), end(deg)) != 0) return false;
            if(*max_element(begin(d), end(d)) == 0) return true;
            for(int v = 0; v < n; ++v) bucket[d[v]].emplace_back(v);
            for(int l = L; l >= 0; --l){
                for(; !bucket[l].empty(); bucket[l].pop_back()){
                    const int v = bucket[l].back();
                    if(d[v] > l) continue;
                    for(auto &e : g[v]) if(d[e.t] < l){
                        if(residue(e) <= 0) continue;
                        const Cost c = rcost(e) < 0 ? 0 : (rcost(e)+eps)/eps;
                        if(c >= d[v]) continue;
                        if(d[e.t] >= d[v] - c) continue;
                        bucket[d[e.t] = d[v] - c].emplace_back(e.t);
                    }
                }
            }
            for(int v = 0; v < n; ++v) p[v] -= eps * d[v];
        }
        assert(false);
    }//}}}

    bool augment(int v, int rest){//{{{
        if(rest == 0 or !is_active(v)) return false;
        bool res = false;
        for(int &i = cur[v]; i < g[v].size(); ++i){
            E &e = g[v][i];
            if(!is_admissible(e)) continue;
            push(e);
            res |= augment(e.t, rest - 1);
            rule.check_update(e.t);
            if(!is_active(v)) return res;
        }
        relabel(v);
        return true;
    }//}}}

    const int start_potential_refine = 2;
    int refine_count = 0;
    // return feasibility
    bool refine(){//{{{
        if(++refine_count >= start_potential_refine && potential_refine()) return true;
        const int set_relabel_period = g.size();

        for(auto &v : g) for(auto &e : v) if(residue(e) < 0 or rcost(e) < 0)
            add_flow(e, residue(e));
        relabel_count = (long long)(alpha + 1) * g.size() * g.size();
        long long next_set_relabel = relabel_count - set_relabel_period;
        cur.assign(g.size(), 0);

        rule.init(*this);
        while(rule.has_active()){
            const int v = rule.extract_active();
            augment(v, AUG_LENGTH);
            rule.check_update(v);
            if(!feasible) return false;
            if(relabel_count < next_set_relabel){
                check_invariants();
                global_update();
                next_set_relabel -= set_relabel_period;
            }
        }
        Cost base = *min_element(begin(p), end(p));
        //for(int v = 0; v < g.size(); ++v) p[v] -= base;
        check_invariants();
        for(int v = 0; v < g.size(); ++v) assert(ex[v] == 0);
        return true;
    }//}}}
};//}}}

/*
partial aug relabel で e に push した後, その先が relabel したせいで
rev e が admissible になった場合, pull してくる.
 */

struct PartialDinicRelabelWithHeuristics : GoldbergTarjan1990<PartialDinicRelabelWithHeuristics, SCALING_FACTOR> {//{{{

    // return is_surturate_push
    bool push(E &e){//{{{
        assert(is_active(e.s) and is_admissible(e));
        if(ex[e.s] < residue(e)){
            add_flow(e, ex[e.s]);
            return false;
        }else{
            add_flow(e, residue(e));
            return true;
        }
    }//}}}
    long long relabel_count;
    void relabel(const int v, Cost init = minus_infinity){//{{{
        assert(is_active(v));
        feasible &= --relabel_count > 0;
        for(auto &e : g[v]) assert(!is_admissible(e));
        p[v] = init;
        for(auto &e : g[v]) if(residue(e) > 0) p[v] = max(p[v], p[e.t] - e.c);
        feasible &= p[v] != minus_infinity;
        p[v] -= eps;
        cur[v] = 0;
    }//}}}

    // return relabeled
    bool discharge(const int v){//{{{
        assert(is_active(v));
        for(int &i = cur[v]; i < g[v].size(); ++i){
            if(!is_admissible(g[v][i])) continue;
            if(!push(g[v][i])) return false;
            if(!is_active(v)){ ++i; return false; }
        }
        relabel(v);
        return true;
    }//}}}

    vector<vector<int>> bucket;

    /*
    bool global_update_sf(){//{{{
        const int n = g.size();
        cur.assign(n, 0);
        vector<char> inS(n, false);
        vector<int> S;
        for(int v = 0; v < n; ++v) if(ex[v] < 0) S.emplace_back(v);
        for(int v = 0; v < n; ++v) if(ex[v] < 0) inS[v] = true;
        for(int turn = 0; true; ++turn){
            assert(turn <= (alpha + 1) * n);
            Flow sum = 0;
            for(int i = 0; i < S.size(); ++i){
                const int v = S[i];
                sum += ex[v];
                for(const auto &re : g[v]){
                    const auto &e = g[re.t][re.rev];
                    if(inS[e.s] or !is_admissible(e)) continue;
                    inS[e.s] = true;
                    S.emplace_back(e.s);
                }
            }
            if(sum == 0){
                cout << turn << endl;
                return true;
            }
            for(int v = 0; v < n; ++v) if(!inS[v]) p[v] -= eps;
            cout << "turn " << turn << endl;
        cout << *min_element(begin(p), end(p)) << endl << *max_element(begin(p), end(p)) << endl;
        }
    }//}}}
    */

    // return true iff it updates potential
    bool global_update(){//{{{
        check_invariants();
        const int n = g.size();
        const int L = (alpha + 1) * n;
        bucket.assign(L + 1, vector<int>());
        vector<int> d(n, L + 1);
        for(int v = 0; v < n; ++v) if(ex[v] < 0)
            bucket[d[v] = 0].emplace_back(v);
        Flow sum = 0;
        for(int l = 0; l <= L; ++l){
            while(!bucket[l].empty()){
                int v = bucket[l].back(); bucket[l].pop_back();
                if(d[v] < l) continue;
                sum += ex[v];
                for(auto &re : g[v]){
                    const int u = re.t;
                    auto &e = g[u][re.rev];
                    if(residue(e) <= 0) continue;
                    assert(rcost(e) >= -eps);
                    Cost len = (rcost(e)+eps)/eps;
                    assert(len >= 0);
                    if(len > L) continue;
                    int nd = d[v] + static_cast<int>(len);
                    if(d[u] <= nd) continue;
                    bucket[d[u] = nd].emplace_back(u);
                }
            }
            if(sum < 0) continue;
            if(l == 0) return false;
            cur.assign(n, 0);
            for(int v = 0; v < n; ++v) p[v] -= min(d[v], l) * eps;
            check_invariants();
            return true;
        }
        // for(int v = 0; v < n; ++v) if(ex[v] > 0) assert(d[v] <= L);
        assert(false);
        return true;
    }//}}}
    // return true iff success to refine to eps-feasible potential
    bool potential_refine(){//{{{
        const int n = g.size();
        const int L = (alpha - 1) * n + 1;
        bucket.assign(L+1, vector<int>());
        queue<int> q;
        vector<int> deg(n), d(n);
        while(true){
            for(int v = 0; v < n; ++v) for(auto &e : g[v])
                if(is_admissible(e)) ++deg[e.t];
            for(int v = 0; v < n; ++v) if(deg[v] == 0) q.emplace(v);
            d.assign(n, 0);
            for(; !q.empty(); q.pop()){
                const int v = q.front();
                for(auto &e : g[v]) if(is_admissible(e)){
                    if(!--deg[e.t]) q.emplace(e.t);
                    const int c = (-rcost(e)-1) / eps;
                    assert(c >= 0);
                    d[e.t] = max(d[e.t], d[v] + c);
                }
            }
            if(*max_element(begin(deg), end(deg)) != 0) return false;
            if(*max_element(begin(d), end(d)) == 0) return true;
            for(int v = 0; v < n; ++v) bucket[d[v]].emplace_back(v);
            for(int l = L; l >= 0; --l){
                for(; !bucket[l].empty(); bucket[l].pop_back()){
                    const int v = bucket[l].back();
                    if(d[v] > l) continue;
                    for(auto &e : g[v]) if(d[e.t] < l){
                        if(residue(e) <= 0) continue;
                        const Cost c = rcost(e) < 0 ? 0 : (rcost(e)+eps)/eps;
                        if(c >= d[v]) continue;
                        if(d[e.t] >= d[v] - c) continue;
                        bucket[d[e.t] = d[v] - c].emplace_back(e.t);
                    }
                }
            }
            for(int v = 0; v < n; ++v) p[v] -= eps * d[v];
        }
        assert(false);
    }//}}}

    void augment(int v, int rest_len){//{{{
        if(rest_len == 0 or ex[v] <= 0) return;
        for(int &i = cur[v]; i < g[v].size(); ++i){
            E &e = g[v][i];
            if(!is_admissible(e)) continue;
            push(e);
            augment(e.t, rest_len-1);
            if(is_active(e.t) and rcost(e) > 0) push(g[e.t][e.rev]);
            if(!is_active(v)) return;
        }
        relabel(v);
    }//}}}

    const int start_potential_refine = 2;
    int refine_count = 0;
    // return feasibility
    bool refine(){//{{{
        if(++refine_count >= start_potential_refine && potential_refine()) return true;
        const int set_relabel_period = g.size();

        for(auto &v : g) for(auto &e : v) if(residue(e) < 0 or rcost(e) < 0)
            add_flow(e, residue(e));
        relabel_count = (long long)(alpha + 1) * g.size() * g.size();
        long long next_set_relabel = relabel_count - set_relabel_period;
        cur.assign(g.size(), 0);

        deque<int> vs(g.size()); iota(begin(vs), end(vs), 0);
        for(bool cont = true; cont; ){
            cont = false;
            deque<int> nvs;
            for(auto v : vs){
                cont |= is_active(v);
                const auto relabel_count_bck = relabel_count;
                if(is_active(v)) augment(v, AUG_LENGTH);
                if(relabel_count != relabel_count_bck){
                    nvs.emplace_front(v);
                    if(!feasible) return false;
                }else{
                    nvs.emplace_back(v);
                }
                if(relabel_count < next_set_relabel){
                    check_invariants();
                    cont |= global_update();
                    next_set_relabel -= set_relabel_period;
                }
            }
            swap(vs, nvs);
        }
        Cost base = *min_element(begin(p), end(p));
        for(int v = 0; v < g.size(); ++v) p[v] -= base;
        check_invariants();
        for(int v = 0; v < g.size(); ++v) assert(ex[v] == 0);
        return true;
    }//}}}
};//}}}

struct PartialDinicRelabelWithHeuristicsFIFO : GoldbergTarjan1990<PartialDinicRelabelWithHeuristicsFIFO, SCALING_FACTOR> {//{{{
    FIFORule<PartialDinicRelabelWithHeuristicsFIFO> rule;

    // return is_surturate_push
    bool push(E &e){//{{{
        assert(is_active(e.s) and is_admissible(e));
        if(ex[e.s] < residue(e)){
            add_flow(e, ex[e.s]);
            return false;
        }else{
            add_flow(e, residue(e));
            return true;
        }
    }//}}}
    long long relabel_count;
    void relabel(const int v, Cost init = minus_infinity){//{{{
        assert(is_active(v));
        feasible &= --relabel_count > 0;
        for(auto &e : g[v]) assert(!is_admissible(e));
        p[v] = init;
        for(auto &e : g[v]) if(residue(e) > 0) p[v] = max(p[v], p[e.t] - e.c);
        feasible &= p[v] != minus_infinity;
        p[v] -= eps;
        cur[v] = 0;
    }//}}}

    // return relabeled
    bool discharge(const int v){//{{{
        assert(is_active(v));
        for(int &i = cur[v]; i < g[v].size(); ++i){
            if(!is_admissible(g[v][i])) continue;
            if(!push(g[v][i])) return false;
            if(!is_active(v)){ ++i; return false; }
        }
        relabel(v);
        return true;
    }//}}}

    vector<vector<int>> bucket;

    /*
    bool global_update_sf(){//{{{
        const int n = g.size();
        cur.assign(n, 0);
        vector<char> inS(n, false);
        vector<int> S;
        for(int v = 0; v < n; ++v) if(ex[v] < 0) S.emplace_back(v);
        for(int v = 0; v < n; ++v) if(ex[v] < 0) inS[v] = true;
        for(int turn = 0; true; ++turn){
            assert(turn <= (alpha + 1) * n);
            Flow sum = 0;
            for(int i = 0; i < S.size(); ++i){
                const int v = S[i];
                sum += ex[v];
                for(const auto &re : g[v]){
                    const auto &e = g[re.t][re.rev];
                    if(inS[e.s] or !is_admissible(e)) continue;
                    inS[e.s] = true;
                    S.emplace_back(e.s);
                }
            }
            if(sum == 0){
                cout << turn << endl;
                return true;
            }
            for(int v = 0; v < n; ++v) if(!inS[v]) p[v] -= eps;
            cout << "turn " << turn << endl;
        cout << *min_element(begin(p), end(p)) << endl << *max_element(begin(p), end(p)) << endl;
        }
    }//}}}
    */

    // return true iff it updates potential
    bool global_update(){//{{{
        check_invariants();
        const int n = g.size();
        const int L = (alpha + 1) * n;
        bucket.assign(L + 1, vector<int>());
        vector<int> d(n, L + 1);
        for(int v = 0; v < n; ++v) if(ex[v] < 0)
            bucket[d[v] = 0].emplace_back(v);
        Flow sum = 0;
        for(int l = 0; l <= L; ++l){
            while(!bucket[l].empty()){
                int v = bucket[l].back(); bucket[l].pop_back();
                if(d[v] < l) continue;
                sum += ex[v];
                for(auto &re : g[v]){
                    const int u = re.t;
                    auto &e = g[u][re.rev];
                    if(residue(e) <= 0) continue;
                    assert(rcost(e) >= -eps);
                    Cost len = (rcost(e)+eps)/eps;
                    assert(len >= 0);
                    if(len > L) continue;
                    int nd = d[v] + static_cast<int>(len);
                    if(d[u] <= nd) continue;
                    bucket[d[u] = nd].emplace_back(u);
                }
            }
            if(sum < 0) continue;
            if(l == 0) return false;
            cur.assign(n, 0);
            for(int v = 0; v < n; ++v) p[v] -= min(d[v], l) * eps;
            check_invariants();
            return true;
        }
        // for(int v = 0; v < n; ++v) if(ex[v] > 0) assert(d[v] <= L);
        assert(false);
        return true;
    }//}}}
    // return true iff success to refine to eps-feasible potential
    bool potential_refine(){//{{{
        const int n = g.size();
        const int L = (alpha - 1) * n + 1;
        bucket.assign(L+1, vector<int>());
        queue<int> q;
        vector<int> deg(n), d(n);
        while(true){
            for(int v = 0; v < n; ++v) for(auto &e : g[v])
                if(is_admissible(e)) ++deg[e.t];
            for(int v = 0; v < n; ++v) if(deg[v] == 0) q.emplace(v);
            d.assign(n, 0);
            for(; !q.empty(); q.pop()){
                const int v = q.front();
                for(auto &e : g[v]) if(is_admissible(e)){
                    if(!--deg[e.t]) q.emplace(e.t);
                    const int c = (-rcost(e)-1) / eps;
                    assert(c >= 0);
                    d[e.t] = max(d[e.t], d[v] + c);
                }
            }
            if(*max_element(begin(deg), end(deg)) != 0) return false;
            if(*max_element(begin(d), end(d)) == 0) return true;
            for(int v = 0; v < n; ++v) bucket[d[v]].emplace_back(v);
            for(int l = L; l >= 0; --l){
                for(; !bucket[l].empty(); bucket[l].pop_back()){
                    const int v = bucket[l].back();
                    if(d[v] > l) continue;
                    for(auto &e : g[v]) if(d[e.t] < l){
                        if(residue(e) <= 0) continue;
                        const Cost c = rcost(e) < 0 ? 0 : (rcost(e)+eps)/eps;
                        if(c >= d[v]) continue;
                        if(d[e.t] >= d[v] - c) continue;
                        bucket[d[e.t] = d[v] - c].emplace_back(e.t);
                    }
                }
            }
            for(int v = 0; v < n; ++v) p[v] -= eps * d[v];
        }
        assert(false);
    }//}}}

    void augment(int v, int rest_len){//{{{
        if(rest_len == 0 or ex[v] <= 0) return;
        for(int &i = cur[v]; i < g[v].size(); ++i){
            E &e = g[v][i];
            if(!is_admissible(e)) continue;
            push(e);
            augment(e.t, rest_len-1);
            if(is_active(e.t) and rcost(e) > 0) push(g[e.t][e.rev]);
            rule.check_update(e.t);
            if(!is_active(v)) return;
        }
        relabel(v);
    }//}}}

    const int start_potential_refine = 2;
    int refine_count = 0;
    // return feasibility
    bool refine(){//{{{
        if(++refine_count >= start_potential_refine && potential_refine()) return true;
        const int set_relabel_period = g.size();

        for(auto &v : g) for(auto &e : v) if(residue(e) < 0 or rcost(e) < 0)
            add_flow(e, residue(e));
        relabel_count = (long long)(alpha + 1) * g.size() * g.size();
        long long next_set_relabel = relabel_count - set_relabel_period;
        cur.assign(g.size(), 0);
        rule.init(*this);

        while(rule.has_active()){
            const int v = rule.extract_active();
            augment(v, AUG_LENGTH);
            rule.check_update(v);
            if(!feasible) return false;
            if(relabel_count < next_set_relabel){
                check_invariants();
                global_update();
                next_set_relabel -= set_relabel_period;
            }
        }
        Cost base = *min_element(begin(p), end(p));
        for(int v = 0; v < g.size(); ++v) p[v] -= base;
        check_invariants();
        for(int v = 0; v < g.size(); ++v) assert(ex[v] == 0);
        return true;
    }//}}}
};//}}}

struct PartialDinicRelabelWithHeuristicsLIFO : GoldbergTarjan1990<PartialDinicRelabelWithHeuristicsLIFO, SCALING_FACTOR> {//{{{
    LIFORule<PartialDinicRelabelWithHeuristicsLIFO> rule;

    // return is_surturate_push
    bool push(E &e){//{{{
        assert(is_active(e.s) and is_admissible(e));
        if(ex[e.s] < residue(e)){
            add_flow(e, ex[e.s]);
            return false;
        }else{
            add_flow(e, residue(e));
            return true;
        }
    }//}}}
    long long relabel_count;
    void relabel(const int v, Cost init = minus_infinity){//{{{
        assert(is_active(v));
        feasible &= --relabel_count > 0;
        for(auto &e : g[v]) assert(!is_admissible(e));
        p[v] = init;
        for(auto &e : g[v]) if(residue(e) > 0) p[v] = max(p[v], p[e.t] - e.c);
        feasible &= p[v] != minus_infinity;
        p[v] -= eps;
        cur[v] = 0;
    }//}}}

    // return relabeled
    bool discharge(const int v){//{{{
        assert(is_active(v));
        for(int &i = cur[v]; i < g[v].size(); ++i){
            if(!is_admissible(g[v][i])) continue;
            if(!push(g[v][i])) return false;
            if(!is_active(v)){ ++i; return false; }
        }
        relabel(v);
        return true;
    }//}}}

    vector<vector<int>> bucket;

    /*
    bool global_update_sf(){//{{{
        const int n = g.size();
        cur.assign(n, 0);
        vector<char> inS(n, false);
        vector<int> S;
        for(int v = 0; v < n; ++v) if(ex[v] < 0) S.emplace_back(v);
        for(int v = 0; v < n; ++v) if(ex[v] < 0) inS[v] = true;
        for(int turn = 0; true; ++turn){
            assert(turn <= (alpha + 1) * n);
            Flow sum = 0;
            for(int i = 0; i < S.size(); ++i){
                const int v = S[i];
                sum += ex[v];
                for(const auto &re : g[v]){
                    const auto &e = g[re.t][re.rev];
                    if(inS[e.s] or !is_admissible(e)) continue;
                    inS[e.s] = true;
                    S.emplace_back(e.s);
                }
            }
            if(sum == 0){
                cout << turn << endl;
                return true;
            }
            for(int v = 0; v < n; ++v) if(!inS[v]) p[v] -= eps;
            cout << "turn " << turn << endl;
        cout << *min_element(begin(p), end(p)) << endl << *max_element(begin(p), end(p)) << endl;
        }
    }//}}}
    */

    // return true iff it updates potential
    bool global_update(){//{{{
        check_invariants();
        const int n = g.size();
        const int L = (alpha + 1) * n;
        bucket.assign(L + 1, vector<int>());
        vector<int> d(n, L + 1);
        for(int v = 0; v < n; ++v) if(ex[v] < 0)
            bucket[d[v] = 0].emplace_back(v);
        Flow sum = 0;
        for(int l = 0; l <= L; ++l){
            while(!bucket[l].empty()){
                int v = bucket[l].back(); bucket[l].pop_back();
                if(d[v] < l) continue;
                sum += ex[v];
                for(auto &re : g[v]){
                    const int u = re.t;
                    auto &e = g[u][re.rev];
                    if(residue(e) <= 0) continue;
                    assert(rcost(e) >= -eps);
                    Cost len = (rcost(e)+eps)/eps;
                    assert(len >= 0);
                    if(len > L) continue;
                    int nd = d[v] + static_cast<int>(len);
                    if(d[u] <= nd) continue;
                    bucket[d[u] = nd].emplace_back(u);
                }
            }
            if(sum < 0) continue;
            if(l == 0) return false;
            cur.assign(n, 0);
            for(int v = 0; v < n; ++v) p[v] -= min(d[v], l) * eps;
            check_invariants();
            return true;
        }
        // for(int v = 0; v < n; ++v) if(ex[v] > 0) assert(d[v] <= L);
        assert(false);
        return true;
    }//}}}
    // return true iff success to refine to eps-feasible potential
    bool potential_refine(){//{{{
        const int n = g.size();
        const int L = (alpha - 1) * n + 1;
        bucket.assign(L+1, vector<int>());
        queue<int> q;
        vector<int> deg(n), d(n);
        while(true){
            for(int v = 0; v < n; ++v) for(auto &e : g[v])
                if(is_admissible(e)) ++deg[e.t];
            for(int v = 0; v < n; ++v) if(deg[v] == 0) q.emplace(v);
            d.assign(n, 0);
            for(; !q.empty(); q.pop()){
                const int v = q.front();
                for(auto &e : g[v]) if(is_admissible(e)){
                    if(!--deg[e.t]) q.emplace(e.t);
                    const int c = (-rcost(e)-1) / eps;
                    assert(c >= 0);
                    d[e.t] = max(d[e.t], d[v] + c);
                }
            }
            if(*max_element(begin(deg), end(deg)) != 0) return false;
            if(*max_element(begin(d), end(d)) == 0) return true;
            for(int v = 0; v < n; ++v) bucket[d[v]].emplace_back(v);
            for(int l = L; l >= 0; --l){
                for(; !bucket[l].empty(); bucket[l].pop_back()){
                    const int v = bucket[l].back();
                    if(d[v] > l) continue;
                    for(auto &e : g[v]) if(d[e.t] < l){
                        if(residue(e) <= 0) continue;
                        const Cost c = rcost(e) < 0 ? 0 : (rcost(e)+eps)/eps;
                        if(c >= d[v]) continue;
                        if(d[e.t] >= d[v] - c) continue;
                        bucket[d[e.t] = d[v] - c].emplace_back(e.t);
                    }
                }
            }
            for(int v = 0; v < n; ++v) p[v] -= eps * d[v];
        }
        assert(false);
    }//}}}

    void augment(int v, int rest_len){//{{{
        if(rest_len == 0 or ex[v] <= 0) return;
        for(int &i = cur[v]; i < g[v].size(); ++i){
            E &e = g[v][i];
            if(!is_admissible(e)) continue;
            push(e);
            augment(e.t, rest_len-1);
            if(is_active(e.t) and rcost(e) > 0) push(g[e.t][e.rev]);
            rule.check_update(e.t);
            if(!is_active(v)) return;
        }
        relabel(v);
    }//}}}

    const int start_potential_refine = 2;
    int refine_count = 0;
    // return feasibility
    bool refine(){//{{{
        if(++refine_count >= start_potential_refine && potential_refine()) return true;
        const int set_relabel_period = g.size();

        for(auto &v : g) for(auto &e : v) if(residue(e) < 0 or rcost(e) < 0)
            add_flow(e, residue(e));
        relabel_count = (long long)(alpha + 1) * g.size() * g.size();
        long long next_set_relabel = relabel_count - set_relabel_period;
        cur.assign(g.size(), 0);
        rule.init(*this);

        while(rule.has_active()){
            const int v = rule.extract_active();
            augment(v, AUG_LENGTH);
            rule.check_update(v);
            if(!feasible) return false;
            if(relabel_count < next_set_relabel){
                check_invariants();
                global_update();
                next_set_relabel -= set_relabel_period;
            }
        }
        Cost base = *min_element(begin(p), end(p));
        for(int v = 0; v < g.size(); ++v) p[v] -= base;
        check_invariants();
        for(int v = 0; v < g.size(); ++v) assert(ex[v] == 0);
        return true;
    }//}}}
};//}}}


#include "dimacs.cc"
#include "benchmark.cc"

template<class MCF> typename MCF::Cost run(const Instance &instance){//{{{
    MCF mcf;
    for(auto &a : instance.arcs)
        mcf.add_edge(a.src, a.dst, a.low, a.cap, a.cost);
    for(auto &n : instance.nodes)
        mcf.add_src(n.id, n.flow);
    auto res = mcf.run();
    assert(get<0>(res));
    return get<1>(res);
}//}}}

int main(int argc, const char *argv[]){

    Benchmark<Instance &> bm;
    // bm.entry("PR", [](const Instance &instance){
    //         cout << run<PushRelabelVariant>(instance) << endl;
    //         });
    //bm.entry("GU", [](const Instance &instance){
    //        cout << run<GlobalUpdate>(instance) << endl;
    //        });
    //bm.entry("PR", [](const Instance &instance){
    //        cout << run<PotentialRefine>(instance) << endl;
    //        });
    // bm.entry("BF", [](const Instance &instance){
    //         cout << run<BlockingFlowVariant>(instance) << endl;
    //         });
    // bm.entry("Both", [](const Instance &instance){
    //         cout << run<GlobalUpdatePotentialRefine>(instance) << endl;
    //         });
    // bm.entry("SSP", [](const Instance &instance){
    //         cout << run<SuccessiveShortestPath>(instance) << endl;
    //         });
    bm.entry("SSP", [](const Instance &instance){
            cout << run<SuccessiveShortestPath>(instance) << endl;
            });
    bm.entry("SSP2", [](const Instance &instance){
            cout << run<SSP>(instance) << endl;
            });
    // bm.entry("AugFIFO", [](const Instance &instance){
    //         cout << run<PartialAugmentRelabelWithHeuristicsFIFO>(instance) << endl;
    //         });
    // bm.entry("DN", [](const Instance &instance){
    //         cout << run<PartialDinicRelabelWithHeuristics>(instance) << endl;
    //         });
    // bm.entry("DN_FIFO", [](const Instance &instance){
    //         cout << run<PartialDinicRelabelWithHeuristicsFIFO>(instance) << endl;
    //         });
    // bm.entry("DN_LIFO", [](const Instance &instance){
    //         cout << run<PartialDinicRelabelWithHeuristicsLIFO>(instance) << endl;
    //         });
    //bm.entry("BF", [](const Instance &instance){
    //        cout << run<BlockingFlowVariant>(instance) << endl;
    //        });
    vector<double> sum(bm.size());
    for(int i = 1; i < argc; ++i){
        Instance instance;
        fstream fs(argv[i]);
        fs >> instance;
        string casename = stringbuilder() << argv[i] << "  (" << instance.n << ", " << instance.m << ", " << 1. * instance.m / instance.n << ")";
        cout << casename << endl;
        auto res = bm.run(casename, instance);
        for(int i = 0; i < bm.size(); ++i) sum[i] += res[i];
        auto fastest = *min_element(begin(res), end(res));
        for(int i = 0; i < bm.size(); ++i) res[i] /= fastest;
        tr << res << endl;
    }
    tr << "sum:" << endl << sum << endl;
    return 0;
}
// vim:set foldmethod=marker commentstring=//%s:
