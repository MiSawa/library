#define LOCAL_DEBUG

#ifdef LOCAL_DEBUG
#ifndef _GLIBCXX_DEBUG
#define _GLIBCXX_DEBUG
#endif
#else
#define NDEBUG
#endif

#include<bits/stdc++.h>
#include <sys/time.h>
#include <sys/resource.h>
using namespace std;

double timer(){
    struct rusage r;
    getrusage(0, &r);
    return (double) (r.ru_utime.tv_sec + r.ru_utime.tv_usec / (double) 1000000);
}


struct PseudoFlow3{//{{{
    using Flow = int64_t;

    struct RawEdge{//{{{
        int s, t, id;
        Flow l, u;
        RawEdge(const int s, const int t, const Flow l, const Flow u) : s(s), t(t), id(0), l(l), u(u) {}
    };//}}}
    vector<RawEdge> raw_edges;
    size_t add_edge(const int s, const int t, const Flow l, const Flow u){//{{{
        n = max<size_t>(n, max(s, t) + 1);
        raw_edges.emplace_back(s, t, l, u);
        return raw_edges.size() - 1;
    }//}}}
    size_t add_edge(const int s, const int t, const Flow u){//{{{
        return add_edge(s, t, Flow(0), u);
    }//}}}

    size_t n = 0;
    int add_v(){ return n++; }
    vector<int> add_vs(const size_t k){//{{{
        vector<int> res(k);
        for(auto &v : res) v = add_v();
        return res;
    }//}}}

    constexpr static size_t undef = ~(size_t(0));

    struct Node{//{{{
        size_t label;
        Flow excess;

        size_t par_arc;          // parent arc or undef
        vector<size_t> tree_arc; // children (lazy deletion)
        vector<size_t> free_arc; // outgoing residual arc (lazy deletion)
        size_t current;          // scanning position of free arc
        Node() : label(0), excess(0), par_arc(undef), current(0) {}
    };//}}}
    constexpr static char IN_TREE = 1, IN_FREE = 2;
    struct Arc{//{{{
        size_t s, t, rev;
        Flow flow, cap;
        char in_flg; // in_tree
        Arc(const size_t s, const size_t t, const size_t rev, const Flow flow, const Flow cap)
            : s(s), t(t), rev(rev), flow(flow), cap(cap), in_flg(0) {}
    };//}}}
    vector<Node> nodes;
    vector<Arc> arcs;
    vector<queue<size_t>> roots;
    vector<size_t> label_cnt;
    size_t highest_strong;

    // STATS
    uint64_t numPushes = 0;
    uint64_t numMergers = 0;
    uint64_t numRelabels = 0;
    uint64_t numArcScans = 0;
    uint64_t numGaps = 0;

    void lift_all(const size_t u){//{{{
        for(size_t p = 0; p < nodes[u].tree_arc.size(); ++p){
            const size_t i = nodes[u].tree_arc[p];
            const size_t v = arcs[i].t;
            if(nodes[v].par_arc != arcs[i].rev){
                arcs[i].in_flg ^= IN_TREE;
                swap(nodes[u].tree_arc[p], nodes[u].tree_arc.back());
                nodes[u].tree_arc.pop_back();
                --p;
                continue;
            }
            lift_all(v);
        }
        --label_cnt[nodes[u].label];
        nodes[u].label = n;
    }//}}}
    void add_strong_bucket(const size_t u){//{{{
        roots[nodes[u].label].push(u);
    }//}}}

    size_t pop_highest_strong(){//{{{
        if(highest_strong == n) --highest_strong;
        while(true){//{{{
            while(highest_strong > 0 and roots[highest_strong].empty()) --highest_strong;
            auto &que = roots[highest_strong];
            if(que.empty()) return undef;
            if(highest_strong > 0 and label_cnt[highest_strong - 1] == 0){
                ++numGaps;
                while(!que.empty()){
                    const size_t u = que.front();
                    que.pop();
                    lift_all(u);
                }
                continue;
            }
            const size_t res = que.front();
            que.pop();
            return res;
        }//}}}
    }//}}}

    void build(const size_t s = -1, const size_t t = -1){//{{{
        const size_t m = raw_edges.size();
        nodes.resize(n);
        {
            vector<size_t> cnt(n);
            for(auto &e : raw_edges){
                ++cnt[e.s];
                ++cnt[e.t];
            }
            for(size_t u = 0; u < n; ++u){
                nodes[u].tree_arc.reserve(cnt[u]);
                nodes[u].free_arc.reserve(cnt[u]);
            }
        }
        arcs.reserve(2 * m);
        for(auto &e : raw_edges){
            size_t i = arcs.size(), j = i + 1;
            Flow f = e.l;
            if(e.s == s or e.t == t) f = e.u;
            arcs.emplace_back(e.s, e.t, j,  f,  e.u);
            arcs.emplace_back(e.t, e.s, i, -f, -e.l);
            if(f == e.l){
                nodes[e.s].free_arc.emplace_back(i);
                arcs[i].in_flg |= IN_FREE;
            }else{
                nodes[e.t].free_arc.emplace_back(j);
                arcs[j].in_flg |= IN_FREE;
            }
            nodes[e.s].excess -= f;
            nodes[e.t].excess += f;
        }
        roots.resize(n + 1);
        label_cnt.resize(n+1);
        for(size_t u = 0; u < n; ++u) if(nodes[u].excess > 0){
            nodes[u].label = 1;
            ++label_cnt[1];
            add_strong_bucket(u);
        }
        label_cnt[0] = n - label_cnt[1] - (s >= 0);
        if(s >= 0) nodes[s].excess = 0;
        if(t >= 0) nodes[t].excess = 0;
        if(s >= 0) nodes[s].label = n;
        highest_strong = 1;
    }//}}}

    // tree は内向きの arc で与える
    void add_tree(const size_t i){//{{{
        const size_t j = arcs[i].rev;
        const size_t par = arcs[i].t;
        const size_t ch = arcs[j].t;
        nodes[ch].par_arc = i;
        if(!(arcs[j].in_flg & IN_TREE)){
            arcs[j].in_flg |= IN_TREE;
            nodes[par].tree_arc.emplace_back(j);
        }
    }//}}}
    void remove_tree(const size_t i){//{{{
        const size_t j = arcs[i].rev;
        const size_t ch = arcs[j].t;
        nodes[ch].par_arc = undef;
    }//}}}

    // strong -> weak
    void merge(const size_t merger){//{{{
        ++numMergers;

        size_t new_arc = merger;
        size_t u = arcs[arcs[merger].rev].t;
        // cerr << "merge " << u << ' ' << arcs[merger].t << endl;
        while(nodes[u].par_arc != undef){
            size_t i = nodes[u].par_arc;
            size_t v = arcs[i].t;
            add_tree(new_arc);
            u = v;
            new_arc = arcs[i].rev;
        }
        add_tree(new_arc);
    }//}}}

    void push(const size_t u, const size_t v, const size_t i){//{{{
        ++numPushes;
        const size_t j = arcs[i].rev;
        const Flow residual = arcs[i].cap - arcs[i].flow;
        if(residual >= nodes[u].excess){
            nodes[v].excess += nodes[u].excess;
            arcs[i].flow += nodes[u].excess;
            arcs[j].flow -= nodes[u].excess;
            nodes[u].excess = 0;
        }else{
            nodes[v].excess += residual;
            arcs[i].flow = arcs[i].cap;
            arcs[j].flow -= residual;
            nodes[u].excess -= residual;

            remove_tree(i);
            if(!(arcs[j].in_flg & IN_FREE)){
                arcs[j].in_flg |= IN_FREE;
                nodes[v].free_arc.emplace_back(j);
            }
            add_strong_bucket(u);
        }
    }//}}}
    void push_excess(const size_t strong){//{{{
        Flow prev_excess = 1;
        size_t u, v;
        for(u = strong; nodes[u].excess > 0 and nodes[u].par_arc != undef; u = v){
            const size_t i = nodes[u].par_arc;
            v = arcs[i].t;
            prev_excess = nodes[v].excess;
            push(u, v, i);
        }
        if(nodes[u].excess > 0 and prev_excess <= 0){
            if(nodes[u].label == 0){
                --label_cnt[0];
                nodes[u].label = 1;
                ++label_cnt[1];
            }
            add_strong_bucket(u);
        }
    }//}}}

    size_t find_weak(const size_t strong){//{{{
        Node &node = nodes[strong];
        for(size_t &p = node.current; p < node.free_arc.size(); ++p){
            ++numArcScans;
            const size_t i = node.free_arc[p], j = arcs[i].rev;
            if(arcs[j].cap > arcs[j].flow){
                // cerr << "lazy deletion" << endl;
                // this is tree arc or reverse arc of free arc
                arcs[i].in_flg ^= IN_FREE;
                swap(node.free_arc[p], node.free_arc.back());
                node.free_arc.pop_back();
                --p;
                continue;
            }
            const size_t v = arcs[i].t;
            if(nodes[v].label == node.label - 1) return i;
        }
        return undef;
    }//}}}

    bool process_root(const size_t strong, const size_t strong_root){//{{{
        // cerr << "process " << strong << " (root = " << strong_root << ")" << endl;
        const size_t i = find_weak(strong);
        if(i != undef){
            merge(i);
            push_excess(strong_root);
            return true;
        }
        Node &node = nodes[strong];
        for(size_t p = 0; p < node.tree_arc.size(); ++p){
            const size_t i = node.tree_arc[p];
            const size_t v = arcs[i].t;
            if(nodes[v].par_arc != arcs[i].rev){
                arcs[i].in_flg ^= IN_TREE;
                swap(node.tree_arc[p], node.tree_arc.back());
                node.tree_arc.pop_back();
                --p;
                continue;
            }
            if(nodes[v].label == node.label) if(process_root(v, strong_root)) return true;
        }
        node.current = 0;
        --label_cnt[node.label];
        ++node.label;
        ++label_cnt[node.label];
        ++numRelabels;
        if(strong == strong_root){
            add_strong_bucket(strong);
            ++highest_strong;
        }
        return false;
    }//}}}

    Flow run(const size_t s, const size_t t){//{{{
        double start;
        start = timer();
        build(s, t);
        cout << "build: " << timer() - start << endl; start = timer();
        while(true){
            const size_t strong = pop_highest_strong();
            if(strong == undef) break;
            process_root(strong, strong);
        }
        cout << "cut: " << timer() - start << endl; start = timer();
        Flow res = 0;
        for(const auto &arc : arcs){
            const size_t u = arc.s, v = arc.t;
            if(nodes[u].label >= n and nodes[v].label < n) res += arc.cap;
        }
        cout << "restore: " << timer() - start << endl; start = timer();
        cerr << " Number of arc scans :" << numArcScans << endl;
        cerr << " Number of mergers   :" << numMergers << endl;
        cerr << " Number of pushes    :" << numPushes << endl;
        cerr << " Number of relabels  :" << numRelabels << endl;
        cerr << " Number of gaps      :" << numGaps << endl;
        return res;
    }//}}}
};//}}}

struct PseudoFlow4{//{{{
    using Flow = int64_t;

    struct RawEdge{//{{{
        int s, t, id;
        Flow l, u;
        RawEdge(const int s, const int t, const Flow l, const Flow u) : s(s), t(t), id(0), l(l), u(u) {}
    };//}}}
    vector<RawEdge> raw_edges;
    size_t add_edge(const int s, const int t, const Flow l, const Flow u){//{{{
        n = max<size_t>(n, max(s, t) + 1);
        raw_edges.emplace_back(s, t, l, u);
        return raw_edges.size() - 1;
    }//}}}
    size_t add_edge(const int s, const int t, const Flow u){//{{{
        return add_edge(s, t, Flow(0), u);
    }//}}}

    size_t n = 0;
    int add_v(){ return n++; }
    vector<int> add_vs(const size_t k){//{{{
        vector<int> res(k);
        for(auto &v : res) v = add_v();
        return res;
    }//}}}

    constexpr static size_t undef = ~(size_t(0));

    struct Arc{//{{{
        size_t s, t, rev, pos;
        Flow flow, cap;
        Arc(){}
    };//}}}
    struct Node{//{{{
        size_t label, current;
        Flow excess;
        size_t par_arc;
        // forward, tree, backward
        vector<size_t> arcs;
        size_t tree_b, tree_e;
        size_t forward_begin() const { return 0u; }
        size_t forward_end() const { return tree_b; }
        size_t tree_begin() const { return tree_b; }
        size_t tree_end() const { return tree_e; }
        size_t backward_begin() const { return tree_e; }
        size_t backward_end() const { return arcs.size(); }

        Node() : label(0), current(0), excess(0), par_arc(undef) {}
        template<class It> void build(size_t id, const It b, const It e){//{{{
            const size_t m = std::distance(b, e);
            arcs.resize(m);
            tree_b = 0, tree_e = m;
            for(It it = b; it != e; ++it){
                excess -= it->flow;
                if(it->flow == it->cap){
                    arcs[it->pos = --tree_e] = id++;
                }else{
                    arcs[it->pos = tree_b++] = id++;
                }
            }
            assert(tree_b == tree_e);
        }//}}}
    };//}}}

    vector<Node> nodes;
    vector<Arc> arcs;
    vector<queue<size_t>> roots;
    vector<size_t> label_cnt;
    size_t highest_strong;

    // STATS
    uint64_t numPushes = 0;
    uint64_t numMergers = 0;
    uint64_t numRelabels = 0;
    uint64_t numArcScans = 0;
    uint64_t numGaps = 0;

    void lift_all(const size_t u){//{{{
        Node &node = nodes[u];
        for(size_t p = node.tree_begin(); p < node.tree_end(); ++p)
            if(node.par_arc != node.arcs[p]) lift_all(arcs[node.arcs[p]].t);
        --label_cnt[node.label];
        node.label = n;
    }//}}}

    void add_strong_bucket(const size_t u){//{{{
        roots[nodes[u].label].push(u);
    }//}}}

    size_t pop_highest_strong(){//{{{
        if(highest_strong == n) --highest_strong;
        while(true){//{{{
            while(highest_strong > 0 and roots[highest_strong].empty()) --highest_strong;
            auto &que = roots[highest_strong];
            if(que.empty()) return undef;
            if(highest_strong > 0 and label_cnt[highest_strong - 1] == 0){
                ++numGaps;
                while(!que.empty()){
                    const size_t u = que.front();
                    que.pop();
                    lift_all(u);
                }
                continue;
            }
            const size_t res = que.front();
            que.pop();
            return res;
        }//}}}
    }//}}}

    void build(const size_t s = undef, const size_t t = undef){//{{{
        const size_t m = raw_edges.size();
        vector<size_t> cnt(n + 1);
        for(const auto &e : raw_edges){
            ++cnt[e.s];
            ++cnt[e.t];
        }
        for(size_t u = 0; u < n; ++u) cnt[u+1] += cnt[u];
        arcs.resize(2 * m);
        for(auto &e : raw_edges){
            size_t i = --cnt[e.s], j = --cnt[e.t];
            Flow f = e.l;
            if(e.s == s or e.t == t) f = e.u;
            arcs[i].s = e.s; arcs[i].t = e.t; arcs[i].rev = j;
            arcs[i].cap = e.u; arcs[i].flow = f;
            arcs[j].s = e.t; arcs[j].t = e.s; arcs[j].rev = i;
            arcs[j].cap = -e.l; arcs[j].flow = -f;
        }
        nodes.resize(n);
        roots.resize(n + 1);
        label_cnt.resize(n+1);
        for(size_t u = 0; u < n; ++u){
            nodes[u].build(
                cnt[u],
                std::next(std::begin(arcs), cnt[u]),
                std::next(std::begin(arcs), cnt[u+1]));
            if(nodes[u].excess > 0){
                nodes[u].label = 1;
                ++label_cnt[1];
                add_strong_bucket(u);
            }
        }
        label_cnt[0] = n - label_cnt[1] - (s >= 0);
        if(s != undef) nodes[s].excess = 0;
        if(t != undef) nodes[t].excess = 0;
        if(s != undef) nodes[s].label = n;
        highest_strong = 1;
        for(size_t u = 0; u < n; ++u) for(size_t p = 0; p < nodes[u].arcs.size(); ++p)
            assert(arcs[nodes[u].arcs[p]].pos == p);
    }//}}}

    // tree は内向きの arc で与える
    void add_tree(const size_t i){//{{{
        Arc &arc = arcs[i], &rev = arcs[arc.rev];
        Node &u = nodes[rev.t], &v = nodes[arc.t];
        // u.par_arc = i;
        if(arc.pos < u.tree_begin()){
            --u.tree_b;
            swap(u.arcs[arc.pos], u.arcs[u.tree_b]);
            swap(arc.pos, arcs[u.arcs[arc.pos]].pos);
        }else{
            assert(arc.pos >= u.tree_end());
            swap(u.arcs[arc.pos], u.arcs[u.tree_e]);
            swap(arc.pos, arcs[u.arcs[arc.pos]].pos);
            ++u.tree_e;
        }
        if(rev.pos < v.tree_begin()){
            --v.tree_b;
            swap(v.arcs[rev.pos], v.arcs[v.tree_b]);
            swap(rev.pos, arcs[v.arcs[rev.pos]].pos);
        }else{
            assert(rev.pos >= v.tree_end());
            swap(v.arcs[rev.pos], v.arcs[v.tree_e]);
            swap(rev.pos, arcs[v.arcs[rev.pos]].pos);
            ++v.tree_e;
        }
    }//}}}
    // i が saturate される
    void remove_tree(const size_t i){//{{{
        Arc &arc = arcs[i], &rev = arcs[arc.rev];
        Node &u = nodes[rev.t], &v = nodes[arc.t];
        // u.par_arc = undef;
        --u.tree_e;
        swap(u.arcs[arc.pos], u.arcs[u.tree_e]);
        swap(arc.pos, arcs[u.arcs[arc.pos]].pos);
        swap(v.arcs[rev.pos], v.arcs[v.tree_b]);
        swap(rev.pos, arcs[v.arcs[rev.pos]].pos);
        ++v.tree_b;
        assert(arc.pos >= u.tree_e);
        assert(rev.pos < v.tree_b);
    }//}}}

    // for(size_t u = 0; u < n; ++u) cerr << "p[" << u << "] = " << (nodes[u].par_arc == undef ? -1 : arcs[nodes[u].par_arc].t) << endl;

    // strong -> weak
    void merge(const size_t merger){//{{{
        ++numMergers;

        add_tree(merger);
        size_t new_arc = merger;
        size_t u = arcs[arcs[merger].rev].t;
        // cerr << "merge " << u << ' ' << arcs[merger].t << endl;
        while(nodes[u].par_arc != undef){
            size_t i = nodes[u].par_arc;
            size_t v = arcs[i].t;
            nodes[u].par_arc = new_arc;
            u = v;
            new_arc = arcs[i].rev;
        }
        nodes[u].par_arc = new_arc;
    }//}}}

    void push(const size_t u, const size_t v, const size_t i){//{{{
        ++numPushes;
        const size_t j = arcs[i].rev;
        const Flow residual = arcs[i].cap - arcs[i].flow;
        if(residual >= nodes[u].excess){
            nodes[v].excess += nodes[u].excess;
            arcs[i].flow += nodes[u].excess;
            arcs[j].flow -= nodes[u].excess;
            nodes[u].excess = 0;
        }else{
            nodes[v].excess += residual;
            arcs[i].flow = arcs[i].cap;
            arcs[j].flow -= residual;
            nodes[u].excess -= residual;

            remove_tree(i);
            nodes[u].par_arc = undef;
            if(nodes[u].label == 0) ++nodes[u].label;
            add_strong_bucket(u);
        }
    }//}}}
    void push_excess(const size_t strong){//{{{
        Flow prev_excess = 1;
        size_t u, v;
        for(u = strong; nodes[u].excess > 0 and nodes[u].par_arc != undef; u = v){
            const size_t i = nodes[u].par_arc;
            v = arcs[i].t;
            prev_excess = nodes[v].excess;
            push(u, v, i);
        }
        if(nodes[u].excess > 0 and prev_excess <= 0){
            if(nodes[u].label == 0){
                --label_cnt[0];
                nodes[u].label = 1;
                ++label_cnt[1];
            }
            add_strong_bucket(u);
        }
    }//}}}

    size_t find_weak(const size_t strong){//{{{
        Node &node = nodes[strong];
        for(size_t &p = node.current; p < node.forward_end(); ++p){
            ++numArcScans;
            const size_t i = node.arcs[p];
            const size_t v = arcs[i].t;
            if(nodes[v].label == node.label - 1) return i;
        }
        return undef;
    }//}}}

    bool process(const size_t strong, const size_t strong_root){//{{{
        // cerr << "process " << strong << " (root = " << strong_root << ")" << endl;
        const size_t i = find_weak(strong);
        if(i != undef){
            merge(i);
            push_excess(strong_root);
            return true;
        }
        Node &node = nodes[strong];
        for(size_t p = node.tree_begin(); p < node.tree_end(); ++p){
            const size_t i = node.arcs[p];
            if(i == node.par_arc) continue;
            const size_t v = arcs[i].t;
            if(nodes[v].label == node.label) if(process(v, strong_root)) return true;
        }
        node.current = node.forward_begin();
        --label_cnt[node.label];
        ++node.label;
        ++label_cnt[node.label];
        ++numRelabels;
        if(strong == strong_root){
            add_strong_bucket(strong);
            ++highest_strong;
        }
        return false;
    }//}}}

    Flow run(const size_t s, const size_t t){//{{{
        double start;
        start = timer();
        build(s, t);
        cout << "build: " << timer() - start << endl; start = timer();
        while(true){
            const size_t strong = pop_highest_strong();
            if(strong == undef) break;
            process(strong, strong);
        }
        cout << "cut: " << timer() - start << endl; start = timer();
        Flow res = 0;
        for(const auto &arc : arcs){
            const size_t u = arc.s, v = arc.t;
            if(nodes[u].label >= n and nodes[v].label < n) res += arc.cap;
        }
        cout << "restore: " << timer() - start << endl; start = timer();
        cerr << " Number of arc scans :" << numArcScans << endl;
        cerr << " Number of mergers   :" << numMergers << endl;
        cerr << " Number of pushes    :" << numPushes << endl;
        cerr << " Number of relabels  :" << numRelabels << endl;
        cerr << " Number of gaps      :" << numGaps << endl;
        return res;
    }//}}}
};//}}}




#if __INCLUDE_LEVEL__ == 0//{{{
#include "../dimacs.cc"
#include "../benchmark.cc"
#include "../utils.cc"
using namespace utils;
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
            assert(val == x);
            abort();
        }
    }
};//}}}
template<class MF> typename MF::Flow run(const Instance &instance){//{{{
    MF mf;
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
    bm.entry("PseudoFlow3", [&checker](const Instance &instance){
        auto res = run<PseudoFlow3>(instance);
        checker.check(res);
        cout << res << endl;
    });
    bm.entry("PseudoFlow4", [&checker](const Instance &instance){
        auto res = run<PseudoFlow4>(instance);
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
#endif//}}}
// vim:set foldmethod=marker commentstring=//%s:
