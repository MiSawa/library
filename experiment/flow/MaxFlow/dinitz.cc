//#define LOCAL_DEBUG

#ifdef LOCAL_DEBUG
#ifndef _GLIBCXX_DEBUG
#define _GLIBCXX_DEBUG
#endif
#else
#define NDEBUG
#endif

#include <bits/stdc++.h>
using namespace std;

#include "utils.cc"
using namespace utils;

struct Dinitz {//{{{
    using Flow = int64_t;
    struct E{
        size_t t, rev;
        Flow f, u;
        E(const size_t t, const Flow f, const Flow u, const size_t rev)
            : t(t), rev(rev), f(f), u(u) {}
    };
    vector<vector<E>> g;
    Dinitz(const size_t n) : g(n) {}
    void add_edge(const size_t s, const size_t t, const Flow u){//{{{
        if(s == t) return;
        g[s].emplace_back(t, 0, u, g[t].size());
        g[t].emplace_back(s, u, u, g[s].size() - 1);
    }//}}}
    vector<size_t> label, cur;
    Flow dfs(const size_t s, const size_t u, const Flow flow){//{{{
        if(s == u or flow == 0) return flow;
        Flow sum = 0;
        for(size_t &i = cur[u]; i < g[u].size(); ++i){
            E &e = g[u][i];
            const size_t v = e.t;
            if(label[v] >= label[u] or e.f <= 0) continue;
            E &re = g[e.t][e.rev];
            Flow f = dfs(s, v, min(flow - sum, e.f));
            e.f -= f; re.f += f;
            if((sum += f) == flow) break;
        }
        return sum;
    }//}}}
    Flow run(const size_t s, const size_t t){//{{{
        const size_t n = g.size();
        vector<size_t> q(n);
        constexpr size_t undef = ~(size_t(0));
        for(Flow flow = 0; ;){
            label.assign(n, undef);
            size_t ql = 0, qr = 0;
            label[q[qr++] = s] = 0;
            while(ql != qr and label[t] == undef){
                const int u = q[ql++];
                for(auto &e : g[u]) if(e.u - e.f > 0 and label[e.t] == undef)
                    label[ q[qr++] = e.t ] = label[u] + 1;
            }
            if(label[t] == undef) return flow;
            cur.assign(n, 0u);
            flow += dfs(s, t, numeric_limits<Flow>::max());
        }
    }//}}}
};//}}}
struct AugmentRelabel {//{{{
    using Flow = int64_t;
    struct E{
        size_t t, rev;
        Flow f, u;
        E(const size_t t, const Flow f, const Flow u, const size_t rev)
            : t(t), rev(rev), f(f), u(u) {}
    };
    vector<vector<E>> g;
    AugmentRelabel(const size_t n) : g(n) {}
    void add_edge(const size_t s, const size_t t, const Flow u){//{{{
        if(s == t) return;
        g[s].emplace_back(t, 0, u, g[t].size());
        g[t].emplace_back(s, u, u, g[s].size() - 1);
    }//}}}
    constexpr static size_t undef = ~(size_t(0));
    vector<size_t> label, cur;
    Flow dfs(const size_t u, const size_t t, const Flow flow){//{{{
        if(u == t or flow == 0) return flow;
        Flow sum = 0;
        for(size_t &i = cur[u]; i < g[u].size(); ++i){
            E &e = g[u][i], &re = g[e.t][e.rev];
            const int &v = e.t;
            if(label[v]+1 != label[u] or re.f == 0) continue;
            Flow f = dfs(v, t, min(flow - sum, re.f));
            e.f += f; re.f -= f;
            if((sum += f) == flow) return sum;
        }
        // This doesn't work because it doesn't take the parent take into account.
        // We can start from label[u] + 2.
        // label[u] = numeric_limits<size_t>::max() - 1;
        // for(const auto &e : g[u]) if(e.u > e.f) label[u] = std::min(label[u], label[e.t]);
        ++label[u];
        cur[u] = 0;
        return sum;
    }//}}}
    Flow run(const size_t s, const size_t t){//{{{
        const size_t n = g.size();
        cur.assign(n, 0);
        label.assign(n, 0);
        for(Flow flow = 0; ; ){
            if(label[s] > n) return flow;
            flow += dfs(s, t, numeric_limits<Flow>::max());
        }
    }//}}}
};//}}}

struct ICPCAugRelabel {//{{{
    using Flow = int64_t;
    constexpr static Flow INF = numeric_limits<Flow>::max();
    struct E{
        size_t t, rev;
        Flow u;
    };
    vector<vector<E>> g;
    vector<size_t> label, cur;
    ICPCAugRelabel(const size_t n) : g(n), label(n), cur(n) {}
    void add_edge(const size_t s, const size_t t, const Flow u){
        if(s == t) return;
        g[s].emplace_back(E{t, g[t].size(), u});
        g[t].emplace_back(E{s, g[s].size()-1, 0});
    }
    Flow dfs(const size_t u, const size_t t, const Flow flow){
        if(u == t or flow == 0) return flow;
        Flow sum = 0;
        for(size_t &i = cur[u]; i < g[u].size(); ++i){
            E &e = g[u][i], &re = g[e.t][e.rev];
            const int &v = e.t;
            if(label[v]+1 != label[u] or e.u == 0) continue;
            const Flow f = dfs(v, t, min(flow - sum, e.u));
            e.u -= f; re.u += f;
            if((sum += f) == flow) return sum;
        }
        label[u] += 2;
        for(const auto &e : g[u]) if(e.u > 0)
            label[u] = std::min(label[u], label[e.t] + 1);
        cur[u] = 0;
        return sum;
    }
    Flow run(const size_t s, const size_t t){
        for(Flow flow = 0; ; flow += dfs(s, t, INF))
            if(label[s] > g.size()) return flow;
    }
};//}}}
struct ICPCAugRelabel2 {//{{{
    using ll = long long;
    using Flow = ll;
    struct E{
        int t, rev;
        ll u;
    };
    vector<vector<E>> g;
    vector<int> label, cur;
    ICPCAugRelabel2(int n) : g(n), label(n), cur(n) {}
    void add_edge(int s, int t, ll u){
        if(s == t) return;
        g[s].emplace_back(E{t, (int)g[t].size(), u});
        g[t].emplace_back(E{s, (int)g[s].size()-1, 0});
    }
    ll dfs(int u, int t, ll flow){
        if(u == t or flow == 0) return flow;
        ll sum = 0;
        for(int &i = cur[u]; i < g[u].size(); ++i){
            E &e = g[u][i], &re = g[e.t][e.rev];
            int &v = e.t;
            if(label[v]+1 != label[u] or e.u == 0) continue;
            ll f = dfs(v, t, min(flow - sum, e.u));
            e.u -= f; re.u += f;
            if((sum += f) == flow) return sum;
        }
        ++label[u];
        cur[u] = 0;
        return sum;
    }
    ll run(int s, int t){
        for(ll flow = 0; ; flow += dfs(s, t, 1LL<<60))
            if(label[s] > g.size()) return flow;
    }
};//}}}
struct ICPCAugRelabel3 {//{{{
    using ll = long long;
    using Flow = ll;
    struct E{
        int t, rev;
        ll u;
    };
    vector<vector<E>> g;
    vector<int> label, cur;
    ICPCAugRelabel3(int n) : g(n), label(n), cur(n) {}
    void add_edge(int s, int t, ll u){
        if(s == t) return;
        g[s].emplace_back(E{t, (int)g[t].size(), u});
        g[t].emplace_back(E{s, (int)g[s].size()-1, 0});
    }
    ll dfs(int u, int t, ll flow){
        if(u == t or flow == 0) return flow;
        for(int &i = cur[u]; i < g[u].size(); ++i){
            E &e = g[u][i];
            if(label[e.t]+1 != label[u]) continue;
            const ll f = dfs(e.t, t, min(flow, e.u));
            e.u -= f; g[e.t][e.rev].u += f;
            if(f) return f;
        }
        ++label[u];
        cur[u] = 0;
        return 0;
    }
    ll run(int s, int t){
        assert(s != t); // or return infinity
        for(ll flow = 0; ; flow += dfs(s, t, 1LL<<60))
            if(label[s] > g.size()) return flow;
    }
};//}}}

#include "dimacs.cc"
#include "benchmark.cc"

template<class T> struct Checker{//{{{
    bool first;
    T val;
    Checker() : first(true){}
    void reset(){ first = true; }
    void check(const T &x){
        if(first){
            first = false;
            val = x;
        }else{
            if(val == x) return;
            if(val != x) {
                cout << val << " vs " << x << endl;
            }
            assert(val == x);
            abort();
        }
    }
};//}}}
template<class MF> typename MF::Flow run(const Instance &instance){//{{{
    MF mf(instance.n + 2);
    for(auto &a : instance.arcs)
        mf.add_edge(a.src, a.dst, a.cap);
    int s = 0, t = 1;
    if(instance.problem_type == Instance::ProblemType::MaxFlow){
        for(auto &n : instance.nodes) if(n.flow > 0) s = n.id;
        for(auto &n : instance.nodes) if(n.flow < 0) t = n.id;
    }else if(instance.problem_type == Instance::ProblemType::MinCostFlow){
        s = instance.n;
        t = s + 1;
        for(auto &n : instance.nodes) if(n.flow > 0) mf.add_edge(s, n.id, n.flow);
        for(auto &n : instance.nodes) if(n.flow < 0) mf.add_edge(n.id, t,-n.flow);
    }
    auto res = mf.run(s, t);
    return res;
}//}}}

int main(int argc, const char *argv[]){

    Checker<int64_t> checker;
    Benchmark<Instance &> bm;
    bm.entry("Dinitz", [&checker](const Instance &instance){
        auto res = run<Dinitz>(instance);
        checker.check(res);
        cout << res << endl;
    });
    bm.entry("AugRelabel", [&checker](const Instance &instance){
        auto res = run<AugmentRelabel>(instance);
        checker.check(res);
        cout << res << endl;
    });
    bm.entry("ICPCAugRelabel", [&checker](const Instance &instance){
        auto res = run<ICPCAugRelabel>(instance);
        checker.check(res);
        cout << res << endl;
    });
    bm.entry("ICPCAugRelabel2", [&checker](const Instance &instance){
        auto res = run<ICPCAugRelabel2>(instance);
        checker.check(res);
        cout << res << endl;
    });
    bm.entry("ICPCAugRelabel3", [&checker](const Instance &instance){
        auto res = run<ICPCAugRelabel3>(instance);
        checker.check(res);
        cout << res << endl;
    });
    vector<double> sum(bm.size());
    for(int i = 1; i < argc; ++i){
        Instance instance;
        fstream fs(argv[i]);
        fs >> instance;
        string casename = stringbuilder() << argv[i] << "  (" << instance.n << ", " << instance.m << ", " << 1. * instance.m / instance.n << ")";
        checker.reset();
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
