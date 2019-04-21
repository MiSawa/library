// #define LOCAL_DEBUG

#ifdef LOCAL_DEBUG
#ifndef _GLIBCXX_DEBUG
#define _GLIBCXX_DEBUG
#endif
#else
#define NDEBUG
#endif

#include <bits/stdc++.h>
using namespace std;

/*

  基本的に Network Flows を参照した.
 */

#include "utils.cc"
using namespace utils;

struct OldDinic{//{{{
    using Flow = int64_t;
    struct E{
        int t, rev;
        Flow f, u;
        E(int , int t, Flow f, Flow u, int rev) : t(t), rev(rev), f(f), u(u) {}
    };
    vector<vector<E>> g;

    void add_edge(int s, int t, Flow u){
        if(g.size() < max(s, t) + 1) g.resize(max(s, t) + 1);
        if(s == t) return;
        g[s].emplace_back(s, t, 0, u, g[t].size());
        g[t].emplace_back(t, s, u, u, g[s].size() - 1);
    }

    vector<int> level, current_edge;
    Flow dfs(const int u, const int t, const Flow flow){//{{{
        if(u == t or flow == 0) return flow;
        for(int &i = current_edge[u]; i >= 0; --i){
            E &e = g[u][i], &re = g[e.t][e.rev];
            const int &v = e.t;
            if(level[u] >= level[v] or e.u - e.f <= 0) continue;
            Flow f = dfs(v, t, min(flow, e.u - e.f));
            if(f == 0) continue;
            e.f += f; re.f -= f;
            return f;
        }
        return 0;
    }//}}}
    Flow run(const int s, const int t){//{{{
        const int n = std::max({(int)g.size(), s+1, t+1});
        vector<int> q(n);
        for(Flow flow = 0; ;){
            level.assign(n, -1);
            int ql = 0, qr = 0;
            level[q[qr++] = s] = 0;
            while(ql != qr and level[t] == -1){
                const int u = q[ql++];
                for(auto &e : g[u]) if(e.u - e.f > 0 and level[e.t] == -1)
                    level[ q[qr++] = e.t ] = level[u] + 1;
            }
            if(level[t] == -1) return flow;
            current_edge.resize(n);
            for(int u = 0; u < n; ++u) current_edge[u] = (int)(g[u].size()) - 1;
            for(Flow f; (f = dfs(s, t, numeric_limits<Flow>::max())) > 0; flow += f);
        }
    }//}}}
};//}}}
struct Dinic{//{{{
    using Flow = int64_t;
    struct E{
        int t, rev;
        Flow f, u;
        E(int , int t, Flow f, Flow u, int rev) : t(t), rev(rev), f(f), u(u) {}
    };
    vector<vector<E>> g;

    void add_edge(int s, int t, Flow u){
        if(g.size() < max(s, t) + 1) g.resize(max(s, t) + 1);
        if(s == t) return;
        g[s].emplace_back(s, t, 0, u, g[t].size());
        g[t].emplace_back(t, s, u, u, g[s].size() - 1);
    }

    vector<int> level, current_edge;
    Flow dfs(const int s, const int u, const Flow flow){//{{{
        if(s == u or flow == 0) return flow;
        Flow sum = 0;
        for(int &i = current_edge[u]; i >= 0; --i){
            E &e = g[u][i], &re = g[e.t][e.rev];
            const int &v = e.t;
            if(level[v] >= level[u] or e.f <= 0) continue;
            Flow f = dfs(s, v, min(flow - sum, e.f));
            e.f -= f; re.f += f;
            if((sum += f) == flow) break;
        }
        return sum;
    }//}}}
    Flow run(const int s, const int t){//{{{
        const int n = std::max({(int)g.size(), s+1, t+1});
        vector<int> q(n);
        for(Flow flow = 0; ;){
            level.assign(n, -1);
            int ql = 0, qr = 0;
            level[q[qr++] = s] = 0;
            while(ql != qr and level[t] == -1){
                const int u = q[ql++];
                for(auto &e : g[u]) if(e.u - e.f > 0 and level[e.t] == -1)
                    level[ q[qr++] = e.t ] = level[u] + 1;
            }
            if(level[t] == -1) return flow;
            current_edge.resize(n);
            for(int u = 0; u < n; ++u) current_edge[u] = (int)(g[u].size()) - 1;
            flow += dfs(s, t, numeric_limits<Flow>::max());
        }
    }//}}}
};//}}}
struct AugmentRelabel{//{{{
    using Flow = int64_t;
    struct E{
        int t, rev;
        Flow f, u;
        E(int , int t, Flow f, Flow u, int rev) : t(t), rev(rev), f(f), u(u) {}
    };
    vector<vector<E>> g;

    void add_edge(int s, int t, Flow u){
        if(g.size() < max(s, t) + 1) g.resize(max(s, t) + 1);
        if(s == t) return;
        g[s].emplace_back(s, t, 0, u, g[t].size());
        g[t].emplace_back(t, s, u, u, g[s].size() - 1);
    }

    vector<int> label, current_edge;
    vector<int> label_cnt;
    int quit;
    Flow dfs(const int u, const int t, const Flow flow){//{{{
        if(u == t or flow == 0) return flow;
        Flow sum = 0;
        for(int &i = current_edge[u]; i >= 0; --i){
            E &e = g[u][i], &re = g[e.t][e.rev];
            const int &v = e.t;
            if(label[v]+1 != label[u] or re.f == 0) continue;
            Flow f = dfs(v, t, min(flow - sum, re.f));
            e.f += f; re.f -= f;
            if((sum += f) == flow) return sum;
        }
        if(!--label_cnt[label[u]]) quit = true;
        label[u] = g.size() + 2;
        for(auto &e : g[u]) if(e.u > e.f) label[u] = min(label[u], label[e.t] + 1);
        ++label_cnt[label[u]];
        current_edge[u] = (int)(g[u].size()) - 1;
        return sum;
    }//}}}

    vector<int> q;
    void global_relabel(const int t){//{{{
        const int n = g.size();
        label.assign(n, n+1);
        q.resize(n);
        int ql = 0, qr = 0;
        label[ q[qr++] = t ] = 0;
        while(ql != qr){
            const int u = q[ql++];
            for(auto &e : g[u]) if(e.f > 0 and label[e.t] == n+1)
                label[ q[qr++] = e.t ] = label[u] + 1;
        }
        label_cnt.assign(n + 3, 0);
        for(int u = 0; u < n; ++u) ++label_cnt[label[u]];
        for(int u = 0; u < n; ++u) current_edge[u] = (int)(g[u].size()) - 1;
    }//}}}
    Flow run(const int s, const int t){//{{{
        const int n = std::max({(int)g.size(), s+1, t+1});
        label.assign(n, 0);
        label_cnt.resize(n + 3);
        label_cnt[0] = n;
        current_edge.resize(n);
        for(int u = 0; u < n; ++u) current_edge[u] = (int)(g[u].size()) - 1;
        Flow flow = 0;
        for(int cnt = 0; ; ++cnt){
            quit = false;
            if(cnt % 64 == 0) global_relabel(t);
            if(label[s] > n) return flow;
            flow += dfs(s, t, numeric_limits<Flow>::max());
            if(quit) return flow;
        }
    }//}}}
};//}}}

struct BucketList{//{{{
    vector<int> next, prev;  // 各要素の次と前の要素.
    vector<int> bucket;      // 各要素がどのバケットに居るか.
    BucketList(){}
    BucketList(const int n, const int bucket_size) : next(n + bucket_size), prev(n + bucket_size), bucket(n, -1) {
        iota(begin(next), end(next), 0);
        iota(begin(prev), end(prev), 0);
    }
    void init(const int n, const int bucket_size){//{{{
        bucket.assign(n, -1);
        next.resize(n + bucket_size);
        prev.resize(n + bucket_size);
        iota(begin(next), end(next), 0);
        iota(begin(prev), end(prev), 0);
    }//}}}
    int operator[](const int id) const { return bucket[id]; }
    void move_to(const int id, const int b){//{{{
        if(bucket[id] >= 0){
            next[prev[id]] = next[id];
            prev[next[id]] = prev[id];
        }
        if(b >= 0){
            const int key = bucket.size() + b;
            next[id] = next[key];
            prev[id] = key;
            prev[next[key]] = id;
            next[key] = id;
        }else{
            next[id] = prev[id] = id;
        }
        bucket[id] = b;
    }//}}}
    void remove(const int id){//{{{
        move_to(id, -1);
    }//}}}
    void move_all(const int b_s, const int b_t){//{{{
        if(b_s == b_t) return;
        while(!empty(b_s)) move_to(front(b_s), b_t); return;
        if(empty(b_s)) return;
        const int key_s = b_s + bucket.size(), key_t = b_t + bucket.size();
        if(empty(b_t)){
            next[prev[key_s]] = key_t;
            prev[next[key_s]] = key_t;
            next[key_t] = next[key_s];
            prev[key_t] = prev[key_s];
        }else{
            next[prev[key_s]] = next[key_t];
            prev[next[key_s]] = key_t;
            prev[next[key_t]] = prev[key_s];
            next[key_t] = next[key_s];
        }
        next[key_s] = prev[key_s] = key_s;
    }//}}}
    bool empty(const int b){ return next[b + bucket.size()] == b + bucket.size(); }
    int front(const int b){ return next[b + bucket.size()]; }
};//}}}
struct BucketVector{//{{{
    vector<vector<int>> bucket;
    vector<int> current_b, current_i;
    BucketVector(){}
    BucketVector(const int n, const int bucket_size) : bucket(bucket_size), current_b(n, -1), current_i(n){
    }
    void init(const int n, const int bucket_size){//{{{
        bucket.assign(bucket_size, vector<int>());
        current_b.assign(n, -1);
        current_i.resize(n);
    }//}}}
    int operator[](const int id) const { return current_b[id]; }
    void move_to(const int id, const int b){//{{{
        if(b == -1){ remove(id); return;}
        if(current_b[id] != -1){
            current_i[bucket[current_b[id]].back()] = current_i[id];
            bucket[current_b[id]][current_i[id]] = bucket[current_b[id]].back();
            bucket[current_b[id]].pop_back();
        }
        current_i[id] = bucket[current_b[id] = b].size();
        bucket[b].emplace_back(id);
    }//}}}
    void remove(const int id){//{{{
        if(current_b[id] < 0) return;
        current_i[bucket[current_b[id]].back()] = current_i[id];
        bucket[current_b[id]][current_i[id]] = bucket[current_b[id]].back();
        bucket[current_b[id]].pop_back();
        current_b[id] = -1;
    }//}}}
    void move_all(const int b_s, const int b_t){//{{{
        if(b_s == b_t) return;
        for(auto id : bucket[b_s]){
            current_b[id] = b_t;
            current_i[id] += bucket[b_t].size();
        }
        bucket[b_t].insert(end(bucket[b_t]), begin(bucket[b_s]), end(bucket[b_s]));
        bucket[b_s].clear();
    }//}}}
    bool empty(const int b){ return bucket[b].empty(); }
    int front(const int b){ return bucket[b].front(); }
};//}}}

struct Label{//{{{
    vector<size_t> next, prev;
    vector<int> label;
    vector<size_t> active, inactive;
    void init(const int n){//{{{
        const int bucket_size = 2 * n + 10;
        next.resize(n + 2 * bucket_size);
        prev.resize(n + 2 * bucket_size);
        iota(begin(next), begin(next) + n, 0u);
        iota(begin(prev), begin(prev) + n, 0u);
        iota(begin(next) + n, begin(next) + n + bucket_size, n + bucket_size);
        iota(begin(prev) + n, begin(prev) + n + bucket_size, n + bucket_size);
        iota(begin(next) + n + bucket_size, end(next), n);
        iota(begin(prev) + n + bucket_size, end(prev), n);
        active.resize(bucket_size);
        inactive.resize(bucket_size);
        iota(begin(active), end(active), n);
        iota(begin(inactive), end(inactive), n + bucket_size);
        label.assign(n, -1);
    }//}}}
    int operator[](const int u) const { return label[u]; }
    void relabel(const int u, const int l, bool is_active){//{{{
        next[prev[u]] = next[u];
        prev[next[u]] = prev[u];
        const int key = is_active ? active[l] : inactive[l];
        next[u] = next[key];
        prev[u] = key;
        prev[next[key]] = u;
        next[key] = u;
        label[u] = l;
    }//}}}
    void activate(const int u){ relabel(u, label[u], true); }
    void deactivate(const int u){ relabel(u, label[u], false); }
    void splice(const int x, const int b, const int e){//{{{
        if(b == e) return;
        const int bb = prev[b];
        prev[b] = x;
        next[prev[e]] = next[x];
        prev[next[x]] = prev[e];
        next[x] = b;
        next[bb] = e;
        prev[e] = bb;
    }//}}}
    void relabel_all(const int l_src, const int l_dst){//{{{
        assert(l_src != l_dst);
        const int key_as = active[l_src], key_is = inactive[l_src];
        const int key_ad = active[l_dst], key_id = inactive[l_dst];
        splice(key_ad, next[key_as], key_is);
        splice(key_id, next[key_is], key_as);
    }//}}}
    bool has_active(const int l) const { return next[active[l]] != inactive[l]; }
    bool has_inactive(const int l) const { return next[inactive[l]] != active[l]; }
    int first_active(const int l) const {//{{{
        assert(has_active(l));
        return next[active[l]];
    }//}}}
};//}}}

struct PreflowPushRelabel{//{{{
    using Flow = int64_t;
    struct E{
        int t, rev;
        Flow f, u;
        E(int , int t, Flow f, Flow u, int rev) : t(t), rev(rev), f(f), u(u) {}
    };
    vector<vector<E>> g;

    void add_edge(int s, int t, Flow u){
        if(g.size() < max(s, t) + 1) g.resize(max(s, t) + 1);
        if(s == t) return;
        g[s].emplace_back(s, t, 0, u, g[t].size());
        g[t].emplace_back(t, s, u, u, g[s].size() - 1);
    }

    vector<Flow> ex;
    vector<int> label, current_edge;
    vector<int> label_cnt;
    char quit;
    void relabel(const int u){//{{{
        if(!--label_cnt[label[u]]) quit = true;
        label[u] = g.size() + 2;
        for(auto &e : g[u]) if(e.u > e.f) label[u] = min(label[u], label[e.t] + 1);
        current_edge[u] = (int)(g[u].size()) - 1;
        ++label_cnt[label[u]];
    }//}}}
    void push(E &e){//{{{
        E &re = g[e.t][e.rev];
        Flow f = min(re.f, ex[re.t]);
        e.f += f;
        re.f -= f;
        ex[re.t] -= f;
        ex[e.t] += f;
    }//}}}
    bool discharge(const int u){//{{{
        for(int &i = current_edge[u]; i >= 0; --i){
            E &e = g[u][i], &re = g[e.t][e.rev];
            const int v = e.t;
            if(label[v]+1 != label[u] or re.f == 0) continue;
            push(e);
            if(ex[u] == 0) return false;
        }
        relabel(u);
        return true;
    }//}}}

    vector<int> q;
    void global_relabel(const int s, const int t){//{{{
        const int n = g.size();
        label.assign(n, 2 * n + 1);
        q.resize(2 * n);
        int ql = 0, qr = 0;
        label[ q[qr++] = t ] = 0;
        while(ql != qr){
            const int u = q[ql++];
            for(auto &e : g[u]) if(e.f > 0 and label[e.t] == 2 * n + 1)
                label[ q[qr++] = e.t ] = label[u] + 1;
        }
        label[ q[qr++] = s ] = n;
        while(ql != qr){
            const int u = q[ql++];
            for(auto &e : g[u]) if(e.f > 0 and label[e.t] == 2 * n + 1)
                label[ q[qr++] = e.t ] = label[u] + 1;
        }
        label_cnt.assign(2 * n + 3, 0);
        current_edge.resize(n);
        for(int u = 0; u < n; ++u) ++label_cnt[label[u]];
        for(int u = 0; u < n; ++u) current_edge[u] = (int)(g[u].size()) - 1;
    }//}}}
    Flow run(const int s, const int t){//{{{
        const int n = std::max({(int)g.size(), s+1, t+1});
        ex.assign(n, 0);

        for(auto &e : g[s]){
            e.f = e.u;
            g[e.t][e.rev].f = 0;
            ex[s] -= e.u;
            ex[e.t] += e.u;
        }

        deque<int> q;
        while(true){
            global_relabel(s, t);
            for(int u = 0; u < n; ++u) if(ex[u] > 0 and u != t) q.emplace_back(u);
            if(q.empty()) return ex[t];
            while(!q.empty()){
                int u = q.front(); q.pop_front();
                discharge(u);
            }
        }
    }//}}}
};//}}}

template<class Bucket>
struct PPRHighest{//{{{
    using Flow = int64_t;
    struct E{
        int t, rev;
        Flow f, u;
        E(int , int t, Flow f, Flow u, int rev) : t(t), rev(rev), f(f), u(u) {}
    };
    vector<vector<E>> g;

    void add_edge(int s, int t, Flow u){
        if(g.size() < max(s, t) + 1) g.resize(max(s, t) + 1);
        if(s == t) return;
        g[s].emplace_back(s, t, 0, u, g[t].size());
        g[t].emplace_back(t, s, u, u, g[s].size() - 1);
    }

    vector<Flow> ex;
    vector<int> label, current_edge;
    Bucket active;
    int relabel_cnt;
    //BucketVector active;
    //BucketVector active;
    int relabel(const int u){//{{{
        ++relabel_cnt;
        label[u] = 2 * g.size() + 2;
        for(auto &e : g[u]) if(e.u > e.f) label[u] = min(label[u], label[e.t] + 1);
        current_edge[u] = (int)(g[u].size()) - 1;
        active.move_to(u, label[u]);
        return label[u];
    }//}}}
    void push(E &e){//{{{
        E &re = g[e.t][e.rev];
        Flow f = min(re.f, ex[re.t]);
        e.f += f;
        re.f -= f;
        if(f > 0 and ex[e.t] == 0) active.move_to(e.t, label[e.t]);
        ex[re.t] -= f;
        ex[e.t] += f;
    }//}}}
    // return highest value
    int discharge(const int u){//{{{
        int highest = -1;
        for(int &i = current_edge[u]; i >= 0; --i){
            E &e = g[u][i], &re = g[e.t][e.rev];
            const int v = e.t;
            if(label[v]+1 != label[u] or re.f == 0) continue;
            push(e);
            highest = max(highest, label[e.t]);
            if(ex[u] == 0){
                active.remove(u);
                return highest;
            }
        }
        return max(highest, relabel(u));
    }//}}}

    vector<int> q;
    // returns highest label
    int global_relabel(const int s, const int t){//{{{
        const int n = g.size();
        label.assign(n, 2 * n + 1);
        q.resize(2 * n);
        int ql = 0, qr = 0;
        label[ q[qr++] = t ] = 0;
        while(ql != qr){
            const int u = q[ql++];
            for(auto &e : g[u]) if(e.f > 0 and label[e.t] == 2 * n + 1)
                label[ q[qr++] = e.t ] = label[u] + 1;
        }
        label[ q[qr++] = s ] = n;
        while(ql != qr){
            const int u = q[ql++];
            for(auto &e : g[u]) if(e.f > 0 and label[e.t] == 2 * n + 1)
                label[ q[qr++] = e.t ] = label[u] + 1;
        }
        int highest = -1;
        for(int u = 0; u < n; ++u){
            if(ex[u] > 0 and u != t){
                active.move_to(u, label[u]);
                highest = max(highest, label[u]);
            }else{
                active.remove(u);
            }
        }
        current_edge.resize(n);
        for(int u = 0; u < n; ++u) current_edge[u] = (int)(g[u].size()) - 1;
        return highest;
    }//}}}
    Flow run(const int s, const int t){//{{{
        const int n = std::max({(int)g.size(), s+1, t+1});
        active.init(n, 2*n+2);
        ex.assign(n, 0);

        for(auto &e : g[s]){
            e.f = e.u;
            g[e.t][e.rev].f = 0;
            ex[s] -= e.u;
            ex[e.t] += e.u;
        }
        int highest = 0;
        relabel_cnt = 0;
        while(true){
            if(relabel_cnt >= 0){
                highest = global_relabel(s, t);
                relabel_cnt = -n;
            }
            active.remove(s);
            active.remove(t);
            while(highest >= 0 and active.empty(highest)) --highest;
            // for(int i = highest+1; i < 2*n+2; ++i) assert(active.empty(i));
            if(highest < 0) return ex[t];
            const int u = active.front(highest);
            const int next_highest = max(highest, discharge(u));
            // if(active.empty(highest)) for(int i = highest+1; i < n; ++i) active.move_all(i, n);
            highest = next_highest;
        }
    }//}}}
};//}}}

// FIXME: なんか壊れてる.
template<class Bucket>
struct PPRH{//{{{
    using Flow = int64_t;
    struct E{
        int t, rev;
        Flow f, u;
        E(int , int t, Flow f, Flow u, int rev) : t(t), rev(rev), f(f), u(u) {}
    };
    vector<vector<E>> g;

    void add_edge(int s, int t, Flow u){
        if(g.size() < max(s, t) + 1) g.resize(max(s, t) + 1);
        if(s == t) return;
        g[s].emplace_back(s, t, 0, u, g[t].size());
        g[t].emplace_back(t, s, u, u, g[s].size() - 1);
    }

    vector<Flow> ex;
    vector<int> current_edge;
    Bucket label;
    int relabel_cnt;
    int relabel(const int u){//{{{
        ++relabel_cnt;
        int l = 2 * g.size() + 2;
        for(auto &e : g[u]) if(e.u > e.f) l = min(l, label[e.t] + 1);
        current_edge[u] = (int)(g[u].size()) - 1;
        label.relabel(u, l, true);
        return l;
    }//}}}
    void push(E &e){//{{{
        E &re = g[e.t][e.rev];
        Flow f = min(re.f, ex[re.t]);
        e.f += f;
        re.f -= f;
        if(f > 0 and ex[e.t] == 0) label.activate(e.t);
        ex[re.t] -= f;
        ex[e.t] += f;
    }//}}}
    // return highest value
    int discharge(const int u){//{{{
        int highest = 0;
        for(int &i = current_edge[u]; i >= 0; --i){
            E &e = g[u][i], &re = g[e.t][e.rev];
            const int v = e.t;
            if(label[v]+1 != label[u] or re.f == 0) continue;
            push(e);
            highest = max(highest, label[e.t]);
            if(ex[u] == 0){
                label.deactivate(u);
                return highest;
            }
        }
        return max(highest, relabel(u));
    }//}}}

    vector<int> buf1, buf2;
    // returns highest label
    int global_relabel(const int s, const int t){//{{{
        const int n = g.size(), inf = 2 * n + 2;
        auto &d = buf1, &q = buf2;
        d.assign(n, inf);
        q.resize(n);
        int ql = 0, qr = 0;
        for(d[q[qr++] = t] = 0; ql != qr; ++ql){
            const int u = q[ql];
            for(auto &e : g[u]) if(e.f > 0 and d[e.t] == inf) d[q[qr++] = e.t] = d[u] + 1;
        }
        for(d[q[qr++] = s] = n; ql != qr; ++ql){
            const int u = q[ql];
            for(auto &e : g[u]) if(e.f > 0 and d[e.t] == inf) d[q[qr++] = e.t] = d[u] + 1;
        }
        int highest = 0;
        for(int u = 0; u < n; ++u){
            if(ex[u] > 0 and u != t){
                label.relabel(u, d[u], true);
                highest = max(highest, d[u]);
            }else{
                label.relabel(u, d[u], false);
            }
        }
        current_edge.resize(n);
        for(int u = 0; u < n; ++u) current_edge[u] = (int)(g[u].size()) - 1;
        return highest;
    }//}}}
    Flow run(const int s, const int t){//{{{
        const int n = std::max({(int)g.size(), s+1, t+1});
        relabel_cnt = 0;
        label.init(n);
        ex.assign(n, 0);

        for(auto &e : g[s]){
            e.f = e.u;
            g[e.t][e.rev].f = 0;
            ex[s] -= e.u;
            ex[e.t] += e.u;
        }
        int highest = global_relabel(s, t);
        while(true){
            if(relabel_cnt >= 0){
                highest = global_relabel(s, t);
                relabel_cnt = -n;
            }
            while(highest > 0 and !label.has_active(highest)) --highest;
            if(highest == 0) return ex[t];
            const int u = label.first_active(highest);
            int next_highest = max(highest, discharge(u));
            if(!label.has_active(highest) and !label.has_inactive(highest)){
                for(int i = highest+1; i < n; ++i) label.relabel_all(i, n);
                next_highest = highest - 1;
            }
            highest = next_highest;
        }
    }//}}}
};//}}}

struct LabelWithBound{//{{{
    vector<size_t> next, prev;
    vector<int> label;
    vector<size_t> active, inactive;
    int undef;
    int ub, active_ub, active_lb;
    void init(const int n){//{{{
        const int bucket_size = n + 1;
        undef = n + 10;
        next.resize(n + 2 * bucket_size);
        prev.resize(n + 2 * bucket_size);
        iota(begin(next), begin(next) + n, 0u);
        iota(begin(prev), begin(prev) + n, 0u);
        iota(begin(next) + n, begin(next) + n + bucket_size, n + bucket_size);
        iota(begin(prev) + n, begin(prev) + n + bucket_size, n + bucket_size);
        iota(begin(next) + n + bucket_size, end(next), n);
        iota(begin(prev) + n + bucket_size, end(prev), n);
        active.resize(bucket_size);
        inactive.resize(bucket_size);
        iota(begin(active), end(active), n);
        iota(begin(inactive), end(inactive), n + bucket_size);
        label.assign(n, undef);
        ub = active_ub = -1;
        active_lb = n;
    }//}}}
    int operator[](const int u) const { return label[u]; }
    void relabel(const int u, const int l, bool is_active){//{{{
        next[prev[u]] = next[u];
        prev[next[u]] = prev[u];
        const int key = is_active ? active[l] : inactive[l];
        next[u] = next[key];
        prev[u] = key;
        prev[next[key]] = u;
        next[key] = u;
        label[u] = l;
        if(ub < l) ub = u;
        if(is_active and active_ub < l) active_ub = l;
        if(is_active and active_lb > l) active_lb = l;
    }//}}}
    void activate(const int u){ relabel(u, label[u], true); }
    void deactivate(const int u){ relabel(u, label[u], false); }
    void remove(const int u){//{{{
        prev[next[u]] = prev[u];
        next[prev[u]] = next[u];
        prev[u] = next[u] = u;
        label[u] = undef;
    }//}}}
    void splice(const int x, const int b, const int e){//{{{
        if(b == e) return;
        const int bb = prev[b];
        prev[b] = x;
        next[prev[e]] = next[x];
        prev[next[x]] = prev[e];
        next[x] = b;
        next[bb] = e;
        prev[e] = bb;
    }//}}}
    bool has_active(const int l) const { return next[active[l]] != inactive[l]; }
    bool has_inactive(const int l) const { return next[inactive[l]] != active[l]; }
    int highest_active(){//{{{
        while(active_ub >= active_lb and !has_active(active_ub)) --active_ub;
        if(active_ub >= active_lb) return next[active[active_ub]];
        return undef;
    }//}}}
    void remove_upper(const int l){//{{{
        for(int i = ub; i >= l; --i){
            for(int u = next[active[i]]; u != inactive[i]; u = next[u]) label[u] = undef;
            for(int u = next[inactive[i]]; u != active[i]; u = next[u]) label[u] = undef;
            if(has_active(i) and has_inactive(i)){
                prev[next[active[i]]] = prev[active[i]];
                prev[next[inactive[i]]] = prev[inactive[i]];
                next[prev[inactive[i]]] = next[inactive[i]];
                next[prev[active[i]]] = next[active[i]];
            }else if(has_active(i)){
                prev[next[active[i]]] = prev[inactive[i]];
                next[prev[inactive[i]]] = next[active[i]];
            }else if(has_inactive(i)){
                prev[next[inactive[i]]] = prev[active[i]];
                next[prev[active[i]]] = next[inactive[i]];
            }
            next[active[i]] = prev[active[i]] = inactive[i];
            next[inactive[i]] = prev[inactive[i]] = active[i];
        }
        if(ub > l) ub = l;
        if(active_ub > l) active_ub = l;
    }//}}}

    int active_begin(const int l) const { return next[active[l]]; }
    int active_end(const int l) const { return inactive[l]; }
    int inactive_begin(const int l) const { return next[inactive[l]]; }
    int inactive_end(const int l) const { return active[l]; }
    int succ(const int u) const { return next[u]; }
};//}}}
template<class Bucket, bool do_phase_two>
struct TFPPRH{//{{{
    using Flow = int64_t;
    struct E{
        int t, rev;
        Flow f, u;
        E(int , int t, Flow f, Flow u, int rev) : t(t), rev(rev), f(f), u(u) {}
    };
    vector<vector<E>> g;

    void add_edge(int s, int t, Flow u){
        if(g.size() < max(s, t) + 1) g.resize(max(s, t) + 1);
        if(s == t) return;
        g[s].emplace_back(s, t, 0, u, g[t].size());
        g[t].emplace_back(t, s, u, u, g[s].size() - 1);
    }

    vector<Flow> ex;
    vector<int> current_edge;
    Bucket label;
    int relabel_cnt;
    int relabel(const int u){//{{{
        ++relabel_cnt;
        int l = g.size();
        for(auto &e : g[u]) if(e.u > e.f and l > label[e.t]) l = label[e.t];
        current_edge[u] = (int)(g[u].size()) - 1;
        assert(l >= label[u]);
        if(++l < g.size()){
            label.relabel(u, l, true);
            return l;
        }else{
            label.remove(u);
            return 0;
        }
    }//}}}
    void push(E &e){//{{{
        E &re = g[e.t][e.rev];
        Flow f = min(re.f, ex[re.t]);
        e.f += f;
        re.f -= f;
        if(f > 0 and ex[e.t] == 0 and label[e.t] > 0) label.activate(e.t);
        ex[re.t] -= f;
        ex[e.t] += f;
    }//}}}
    void discharge(const int u){//{{{
        for(int &i = current_edge[u]; i >= 0; --i){
            E &e = g[u][i], &re = g[e.t][e.rev];
            const int v = e.t;
            if(label[v]+1 != label[u] or re.f == 0) continue;
            push(e);
            if(ex[u] == 0){
                label.deactivate(u);
                return;
            }
        }
        relabel(u);
    }//}}}

    vector<int> buf1, buf2;
    void global_relabel(const int target){//{{{
        const int n = g.size(), inf = n;
        label.init(n);
        auto &d = buf1, &q = buf2;
        d.assign(n, inf);
        q.resize(n);
        int ql = 0, qr = 0;
        for(d[q[qr++] = target] = 0; ql != qr; ++ql){
            const int u = q[ql];
            for(auto &e : g[u]) if(e.f > 0 and d[e.t] == inf) d[q[qr++] = e.t] = d[u] + 1;
        }
        for(int u = 0; u < n; ++u){
            if(ex[u] > 0 and u != target and d[u] < inf){
                label.relabel(u, d[u], true);
            }else if(d[u] < inf){
                label.relabel(u, d[u], false);
            }
        }
        current_edge.resize(n);
        for(int u = 0; u < n; ++u) current_edge[u] = (int)(g[u].size()) - 1;
    }//}}}
    Flow run(const int s, const int t){//{{{
        const int n = std::max({(int)g.size(), s+1, t+1});
        ex.assign(n, 0);

        for(auto &e : g[s]){
            e.f = e.u;
            g[e.t][e.rev].f = 0;
            ex[s] -= e.u;
            ex[e.t] += e.u;
        }
        for(int phase = 0; phase < 2; ++phase){
            relabel_cnt = 0;
            while(true){
                if(relabel_cnt >= 0){
                    global_relabel(phase ? s : t);
                    relabel_cnt = -n;
                }
                const int u = label.highest_active();
                if(u == label.undef) break;
                const int l = label[u];
                discharge(u);
                if(!label.has_active(l) and !label.has_inactive(l)) label.remove_upper(l);
            }
            if(phase) return -ex[s];
            if(!do_phase_two) return ex[t];
            ex[t] = 0;
        }
        return -1;
    }//}}}
};//}}}
// partial augment relabel
template<class Bucket, bool do_phase_two>
struct TFPPRH_PAR{//{{{
    using Flow = int64_t;
    struct E{
        int t, rev;
        Flow f, u;
        E(int , int t, Flow f, Flow u, int rev) : t(t), rev(rev), f(f), u(u) {}
    };
    vector<vector<E>> g;

    void add_edge(int s, int t, Flow u){
        if(g.size() < max(s, t) + 1) g.resize(max(s, t) + 1);
        if(s == t) return;
        g[s].emplace_back(s, t, 0, u, g[t].size());
        g[t].emplace_back(t, s, u, u, g[s].size() - 1);
    }

    vector<Flow> ex;
    vector<int> current_edge;
    Bucket label;
    int relabel_cnt;
    // return (did gap relabel?)
    bool relabel(const int u){//{{{
        ++relabel_cnt;
        const int prev = label[u];
        int l = g.size();
        for(auto &e : g[u]) if(e.u > e.f and l > label[e.t]) l = label[e.t];
        current_edge[u] = (int)(g[u].size()) - 1;
        if(++l < g.size()) label.relabel(u, l, true);
        else               label.remove(u);
        if(!label.has_active(prev) and !label.has_inactive(prev)){
            label.remove_upper(prev);
            return true;
        }
        return false;
    }//}}}
    void push(E &e){//{{{
        E &re = g[e.t][e.rev];
        Flow f = min(re.f, ex[re.t]);
        e.f += f;
        re.f -= f;
        if(f > 0 and ex[e.t] == 0 and label[e.t] > 0) label.activate(e.t);
        ex[re.t] -= f;
        ex[e.t] += f;
    }//}}}
    // return (did gap relabel?) i.e. force quit?
    bool augment(const int u, const int rest){//{{{
        // if(ex[u] == 0 and label[u] != 0) label.activate(u);
        if(rest == 0 or label[u] == 0) return false;
        for(int &i = current_edge[u]; i >= 0; --i){
            E &e = g[u][i], &re = g[e.t][e.rev];
            const int v = e.t;
            if(label[v]+1 != label[u] or re.f == 0) continue;
            push(e);
            if(augment(v, rest-1)) return true;
            if(label[v] == label[u]+1){
                push(re);
                if(ex[v] == 0) label.deactivate(v);
            }
            if(ex[u] == 0){
                label.deactivate(u);
                return false;
            }
        }
        return relabel(u);
    }//}}}

    vector<int> buf1, buf2;
    void global_relabel(const int target){//{{{
        const int n = g.size(), inf = n;
        label.init(n);
        auto &d = buf1, &q = buf2;
        d.assign(n, inf);
        q.resize(n);
        int ql = 0, qr = 0;
        for(d[q[qr++] = target] = 0; ql != qr; ++ql){
            const int u = q[ql];
            for(auto &e : g[u]) if(e.f > 0 and d[e.t] == inf) d[q[qr++] = e.t] = d[u] + 1;
        }
        for(int u = 0; u < n; ++u){
            if(ex[u] > 0 and u != target and d[u] < inf){
                label.relabel(u, d[u], true);
            }else if(d[u] < inf){
                label.relabel(u, d[u], false);
            }
        }
        current_edge.resize(n);
        for(int u = 0; u < n; ++u) current_edge[u] = (int)(g[u].size()) - 1;
    }//}}}
    Flow run(const int s, const int t){//{{{
        const int n = std::max({(int)g.size(), s+1, t+1});
        ex.assign(n, 0);

        for(auto &e : g[s]){
            e.f = e.u;
            g[e.t][e.rev].f = 0;
            ex[s] -= e.u;
            ex[e.t] += e.u;
        }
        for(int phase = 0; phase < 2; ++phase){
            relabel_cnt = 0;
            while(true){
                if(relabel_cnt >= 0){
                    global_relabel(phase ? s : t);
                    relabel_cnt = - 2 * n;
                }
                const int u = label.highest_active();
                if(u == label.undef) break;
                augment(u, 4);
            }
            if(phase) return -ex[s];
            if(!do_phase_two) return ex[t];
            ex[t] = 0;
        }
        return -1;
    }//}}}
};//}}}
/*
template<class Bucket, bool do_phase_two>
struct TFPPRH_PAR_H{//{{{
    using Flow = int64_t;
    struct E{
        int t, rev;
        Flow f, u;
        E(int , int t, Flow f, Flow u, int rev) : t(t), rev(rev), f(f), u(u) {}
    };
    vector<vector<E>> g;

    void add_edge(int s, int t, Flow u){
        if(g.size() < max(s, t) + 1) g.resize(max(s, t) + 1);
        if(s == t) return;
        g[s].emplace_back(s, t, 0, u, g[t].size());
        g[t].emplace_back(t, s, u, u, g[s].size() - 1);
    }

    vector<Flow> ex;
    vector<int> current_edge;
    Bucket label;
    int relabel_cnt;
    // return (did gap relabel?)
    bool relabel(const int u){//{{{
        ++relabel_cnt;
        const int prev = label[u];
        int l = g.size();
        for(auto &e : g[u]) if(e.u > e.f and l > label[e.t]) l = label[e.t];
        current_edge[u] = (int)(g[u].size()) - 1;
        if(++l < g.size()) label.relabel(u, l, true);
        else               label.remove(u);
        if(!label.has_active(prev) and !label.has_inactive(prev)){
            label.remove_upper(prev);
            return true;
        }
        return false;
    }//}}}
    void push(E &e){//{{{
        E &re = g[e.t][e.rev];
        Flow f = min(re.f, ex[re.t]);
        e.f += f;
        re.f -= f;
        if(f > 0 and ex[e.t] == 0 and label[e.t] > 0) label.activate(e.t);
        ex[re.t] -= f;
        ex[e.t] += f;
    }//}}}
    // return (did gap relabel?) i.e. force quit?
    bool augment(const int u, const int rest){//{{{
        // if(ex[u] == 0 and label[u] != 0) label.activate(u);
        if(rest == 0 or label[u] == 0) return false;
        for(int &i = current_edge[u]; i >= 0; --i){
            E &e = g[u][i], &re = g[e.t][e.rev];
            const int v = e.t;
            if(label[v]+1 != label[u] or re.f == 0) continue;
            push(e);
            if(augment(v, rest-1)) return true;
            if(label[v] == label[u]+1){
                push(re);
                if(ex[v] == 0) label.deactivate(v);
            }
            if(ex[u] == 0){
                label.deactivate(u);
                return false;
            }
        }
        return relabel(u);
    }//}}}

    vector<int> buf1, buf2;
    int incremental_restart_th;
    void global_relabel(const int target){//{{{
        const int n = g.size(), inf = n;
        label.init(n);
        auto &d = buf1, &q = buf2;
        d.assign(n, inf);
        q.resize(n);
        int ql = 0, qr = 0;
        for(d[q[qr++] = target] = 0; ql != qr; ++ql){
            const int u = q[ql];
            for(auto &e : g[u]) if(e.f > 0 and d[e.t] == inf) d[q[qr++] = e.t] = d[u] + 1;
        }
        for(int u = 0; u < n; ++u){
            if(ex[u] > 0 and u != target and d[u] < inf){
                label.relabel(u, d[u], true);
            }else if(d[u] < inf){
                label.relabel(u, d[u], false);
            }
        }
        current_edge.resize(n);
        for(int u = 0; u < n; ++u) current_edge[u] = (int)(g[u].size()) - 1;
    }//}}}
    void incremental_restart(){
        // もっとうまく書けないかなぁと思ったけれど,
        // queue に入ってるの見れば前の bfs の状態完全にわかるじゃん.
        // これ管理すれば速くない?
        const int n = g.size(), inf = n;
        auto &d = buf1, &q = buf2;
        d.assign(n, inf);
        q.resize(n);
        int ql = 0, qr = 0;
        for(int u = 0; u < n; ++u) if(label[u] <= incremental_restart_th){
            d[u] = label[u];
            if(label[u] == incremental_restart_th) q[qr++] = u;
        }
        for(; ql != qr; ++ql){
            const int u = q[ql];
            for(auto &e : g[u]) if(e.f > 0 and d[e.t] == inf) d[ q[qr++] = e.t ] = d[u] + 1;
        }
        for(int u = 0; u < n; ++u) if(d[u] != label[u] and d[u] < inf)
            label.relabel(u, d[u], ex[u] > 0);
        incremental_restart_th = n;
    }

    Flow run(const int s, const int t){//{{{
        const int n = std::max({(int)g.size(), s+1, t+1});
        ex.assign(n, 0);

        for(auto &e : g[s]){
            e.f = e.u;
            g[e.t][e.rev].f = 0;
            ex[s] -= e.u;
            ex[e.t] += e.u;
        }
        for(int phase = 0; phase < 2; ++phase){
            relabel_cnt = 0;
            while(true){
                if(relabel_cnt >= 0){
                    global_relabel(phase ? s : t);
                    relabel_cnt = - 2 * n;
                }
                const int u = label.highest_active();
                if(u == label.undef) break;
                augment(u, 4);
            }
            if(phase) return -ex[s];
            if(!do_phase_two) return ex[t];
            ex[t] = 0;
        }
    }//}}}
};//}}}
*/

struct DinitzCirculation{//{{{
    using Flow = int64_t;
    struct Edge{//{{{
        int s, t, id;
        Flow l, u;
        Edge(const int s, const int t, const Flow l, const Flow u) : s(s), t(t), id(0), l(l), u(u) {}
    };//}}}
    struct {//{{{
        struct {//{{{
            vector<size_t> start;
            vector<size_t> cur;
            vector<int> label;
            vector<Flow> ex;
            void resize(const size_t n){
                start.assign(n + 1, 0);
                cur.resize(n);
                label.resize(n);
                ex.assign(n, Flow(0));
            }
            size_t begin(const size_t u) const { return start[u]; }
            size_t end(const size_t u) const { return start[u+1]; }
            size_t size() const { return ex.size(); }
        } v;//}}}
        struct {//{{{
            vector<Flow> f, u;
            vector<size_t> s, t, rev;
            void resize(const size_t n){
                f.assign(2 * n, Flow(0));
                u.resize(2 * n);
                s.resize(2 * n);
                t.resize(2 * n);
                rev.resize(2 * n);
            }
            void set(const size_t i, const size_t j, Edge &e){
                // f[i] = f[j] = 0;
                u[i] = e.u; u[j] = -e.l;
                s[i] = t[j] = e.s;
                t[i] = s[j] = e.t;
                rev[i] = j; rev[j] = i;
                e.id = i;
            }
            size_t size() const { return f.size(); }
        } e;//}}}
        void construct(const int n, vector<Edge> &raw_edges){//{{{
            v.resize(n);
            for(const auto &edge : raw_edges){
                ++v.start[edge.s];
                ++v.start[edge.t];
            }
            for(size_t u = 0; u < n; ++u) v.start[u+1] += v.start[u];
            e.resize(raw_edges.size());
            for(auto &edge : raw_edges) e.set(--v.start[edge.s], --v.start[edge.t], edge);
        }//}}}
    } g;//}}}

    int n = 0;
    vector<Edge> raw_edges;

    size_t add_edge(const int s, const int t, const Flow l, const Flow u){//{{{
        n = max(n, max(s, t) + 1);
        raw_edges.emplace_back(s, t, l, u);
        return raw_edges.size() - 1;
    }//}}}
    size_t add_edge(const int s, const int t, const Flow u){//{{{
        return add_edge(s, t, Flow(0), u);
    }//}}}

    int add_v(){ return n++; }
    vector<int> add_vs(const size_t k){//{{{
        vector<int> res(k);
        for(auto &v : res) v = add_v();
        return res;
    }//}}}

    void push(const size_t i, const Flow f){//{{{
        g.e.f[g.e.rev[i]] -= f;
        g.e.f[i] += f;
        g.v.ex[g.e.s[i]] -= f;
        g.v.ex[g.e.t[i]] += f;
    }//}}}

    void build(){//{{{
        g.construct(n, raw_edges);
        for(size_t i = 0; i < g.e.size(); ++i) if(g.e.f[i] > g.e.u[i])
            push(i, g.e.u[i] - g.e.f[i]);
    }//}}}

    struct {
        vector<size_t> vec_node;
    } buf;
    void bfs(size_t s, size_t t){//{{{
        vector<size_t> &que = buf.vec_node;
        g.v.cur.assign(begin(g.v.start), begin(g.v.start) + n);
        que.resize(g.v.size());
        g.v.label.assign(n, -1);
        size_t ql = 0, qr = 0;
        g.v.label[que[qr++] = s] = 0;
        while(ql != qr and g.v.label[t] == -1){
            const size_t u = que[ql++];
            for(size_t i = g.v.begin(u); i < g.v.end(u); ++i)
                if(g.e.f[i] < g.e.u[i] and g.v.label[g.e.t[i]] == -1)
                    g.v.label[que[qr++] = g.e.t[i]] = g.v.label[u] + 1;
        }
    }//}}}
    void bfs(){//{{{
        vector<size_t> &que = buf.vec_node;
        g.v.cur.assign(begin(g.v.start), begin(g.v.start) + n);
        que.resize(g.v.size());
        g.v.label.assign(n, -1);
        size_t ql = 0, qr = 0;
        for(size_t u = 0; u < g.v.size(); ++u) if(g.v.ex[u] > 0)
            g.v.label[que[qr++] = u] = 0;
        while(ql != qr){
            const size_t u = que[ql++];
            for(size_t i = g.v.begin(u); i < g.v.end(u); ++i)
                if(g.e.f[i] < g.e.u[i] and g.v.label[g.e.t[i]] == -1)
                    g.v.label[que[qr++] = g.e.t[i]] = g.v.label[u] + 1;
        }
    }//}}}
    Flow dfs(const size_t u, const Flow flow){//{{{
        if(flow == 0) return flow;
        if(g.v.label[u] == 0){
            Flow f = min(flow, g.v.ex[u]);
            g.v.ex[u] -= f;
            return f;
        }
        Flow sum = 0;
        for(size_t &i = g.v.cur[u]; i < g.v.end(u); ++i){
            const size_t v = g.e.t[i];
            if(g.v.label[v] != g.v.label[u] - 1) continue;
            const size_t j = g.e.rev[i];
            if(g.e.f[j] >= g.e.u[j]) continue;
            Flow f = dfs(v, min(g.e.u[j] - g.e.f[j], flow - sum));
            g.e.f[i] -= f;
            g.e.f[j] += f;
            sum += f;
            if(sum == flow) break;
        }
        return sum;
    }//}}}
    bool circulation(){//{{{
        build();
        for(bool update = true; update;){
            update = false;
            bfs();
            for(size_t u = 0; u < g.v.size(); ++u) if(g.v.ex[u] < 0){
                if(g.v.label[u] >= 0){
                    Flow f = dfs(u, -g.v.ex[u]);
                    g.v.ex[u] += f;
                    update |= f > 0;
                }else{
                    return false;
                }
            }
        }

        for(size_t u = 0; u < g.v.size(); ++u) if(g.v.ex[u] != 0) return false;
        return true;
    }//}}}
    Flow run(const size_t s, const size_t t){//{{{
        constexpr Flow inf = numeric_limits<Flow>::max() / 4;

        auto id = add_edge(t, s, 0, inf);
        if(!circulation()) return 0; // make_pair(false, Flow(0));

        Flow res = -g.e.f[raw_edges[id].id];
        raw_edges.pop_back();

        g.v.ex[s] += inf;
        g.v.ex[t] -= inf;
        while(true){
            bfs(s, t);
            if(g.v.label[t] == -1) break;
            res += dfs(t, inf);
        }
        return res;
    }//}}}
};//}}}

struct PseudoFlow{//{{{
    using Flow = int64_t;
    struct Edge{//{{{
        int s, t, id;
        Flow l, u;
        Edge(const int s, const int t, const Flow l, const Flow u) : s(s), t(t), id(0), l(l), u(u) {}
    };//}}}
    struct {//{{{
        struct {//{{{
            vector<size_t> start;
            vector<size_t> cur;
            vector<Flow> ex;
            void resize(const size_t n){
                start.assign(n + 1, 0);
                cur.resize(n);
                ex.assign(n, Flow(0));
            }
            size_t begin(const size_t u) const { return start[u]; }
            size_t end(const size_t u) const { return start[u+1]; }
            size_t size() const { return ex.size(); }
        } v;//}}}
        struct {//{{{
            vector<Flow> f, u;
            vector<size_t> s, t, rev;
            void resize(const size_t n){
                f.assign(2 * n, Flow(0));
                u.resize(2 * n);
                s.resize(2 * n);
                t.resize(2 * n);
                rev.resize(2 * n);
            }
            void set(const size_t i, const size_t j, Edge &e){
                // f[i] = f[j] = 0;
                u[i] = e.u; u[j] = -e.l;
                s[i] = t[j] = e.s;
                t[i] = s[j] = e.t;
                rev[i] = j; rev[j] = i;
                e.id = i;
            }
            size_t size() const { return f.size(); }
        } e;//}}}
        void construct(const int n, vector<Edge> &raw_edges){//{{{
            v.resize(n);
            for(const auto &edge : raw_edges){
                ++v.start[edge.s];
                ++v.start[edge.t];
            }
            for(size_t u = 0; u < n; ++u) v.start[u+1] += v.start[u];
            e.resize(raw_edges.size());
            for(auto &edge : raw_edges) e.set(--v.start[edge.s], --v.start[edge.t], edge);
            v.cur.assign(begin(v.start), begin(v.start) + n);
        }//}}}
    } g;//}}}

    int n = 0;
    vector<Edge> raw_edges;

    size_t add_edge(const int s, const int t, const Flow l, const Flow u){//{{{
        n = max(n, max(s, t) + 1);
        raw_edges.emplace_back(s, t, l, u);
        return raw_edges.size() - 1;
    }//}}}
    size_t add_edge(const int s, const int t, const Flow u){//{{{
        return add_edge(s, t, Flow(0), u);
    }//}}}

    int add_v(){ return n++; }
    vector<int> add_vs(const size_t k){//{{{
        vector<int> res(k);
        for(auto &v : res) v = add_v();
        return res;
    }//}}}

    void build(){//{{{
        g.construct(n, raw_edges);
        for(size_t i = 0; i < g.e.size(); ++i) if(g.e.f[i] > g.e.u[i])
            push(i, g.e.u[i] - g.e.f[i]);
    }//}}}

    void push(const size_t i, const Flow f){//{{{
        g.e.f[i] += f;
        g.e.f[g.e.rev[i]] -= f;
    }//}}}
    // tree は下には容量が正でなければならない
    struct {//{{{
        vector<vector<size_t>> roots;
        vector<size_t> label;
        vector<int> par; // par は par への辺番号
        vector<size_t> label_cnt;
        void build(const decltype(PseudoFlow::g) &g){
            const size_t n = g.v.size();
            roots.assign(n + 1, vector<size_t>());
            label.assign(n, 0);
            par.assign(n, 0);
            roots[0].resize(n);
            iota(begin(roots[0]), end(roots[0]), 0);
            label_cnt.assign(n + 1, 0);
            label_cnt[0] = n;
        }
        void make_root(const size_t u){
            //if(par[u] == -1) return;
            par[u] = -1;
            roots[label[u]].emplace_back(u);
        }
        void gap_relabel(const size_t l){
            const size_t n = label.size();
            for(size_t u = 0; u < n; ++u) if(label[u] > l){
                --label_cnt[label[u]];
                ++label_cnt[label[u] = n];
            }
        }
        void relabel(const size_t u){
            --label_cnt[label[u]];
            ++label_cnt[++label[u]];
        }
    } tree;//}}}
    void check(int line){//{{{
        return;
        for(size_t u = 0; u < g.v.size(); ++u){
            // for(size_t i = g.v.begin(u); i < g.v.end(u); ++i){
            // }
            if(tree.par[u] != -1){
                const size_t v = g.e.t[tree.par[u]];
                if(tree.label[v] > tree.label[u]) cerr << "LINE " << line << endl;
                assert(tree.label[v] <= tree.label[u]);
            }
        }
    }//}}}
    int relabel_count;
    void relabel(const size_t u){//{{{
        for(size_t i = g.v.begin(u); i < g.v.end(u); ++i){
            const size_t v = g.e.t[i];
            const size_t j = g.e.rev[i];
            if(tree.par[v] == j){
                assert(g.e.u[i] > g.e.f[i]);
                assert(tree.label[v] > tree.label[u]);
            }
        }

        ++relabel_count;
        tree.relabel(u);
        g.v.cur[u] = g.v.begin(u);
    }//}}}

    void partial_aug(const size_t u, const size_t length, const Flow flow){//{{{
        if(length == 0 || tree.par[u] == -1){
            g.v.ex[u] += flow;
            if(g.v.ex[u] > 0) tree.make_root(u);
        }else{
            const size_t i = tree.par[u];
            const size_t v = g.e.t[i];
            Flow f = std::min(flow, g.e.u[i] - g.e.f[i]);
            push(i, f);
            partial_aug(v, length-1, f);
            g.v.ex[u] += flow - f;
            if(g.v.ex[u] > 0){
                tree.make_root(u);
            }
        }
    }//}}}

    Flow dfs(const size_t u, const Flow flow){//{{{
        assert(flow > 0);
        Flow sum = 0;
        for(size_t i = g.v.begin(u); i < g.v.end(u); ++i){
            const size_t v = g.e.t[i];
            if(tree.label[v] > tree.label[u]) continue;
            if(g.e.u[i] == g.e.f[i]) continue;
            if(tree.label[v] + 1 == tree.label[u]){
                assert(tree.par[v] != g.e.rev[i]);
                // (u, v) is merger arc
                const Flow f = std::min(flow - sum, g.e.u[i] - g.e.f[i]);
                push(i, f);
                sum += f;
                partial_aug(v, 10, f);
                if(flow == sum){
                    tree.par[u] = i;
                    return flow;
                }
            }else if(tree.par[v] == g.e.rev[i]){
                const Flow residual = g.e.u[i] - g.e.f[i];
                assert(residual > 0);
                const Flow f = dfs(v, std::min(flow - sum, residual));
                push(i, f);
                sum += f;
                if(tree.par[v] != g.e.rev[i]){
                    assert(f > 0);
                    if(tree.label[v] == tree.label[u]) tree.par[u] = i;
                }else{
                    assert(tree.label[v] > tree.label[u]);
                }
            }
            if(flow == sum) return flow;
        }
        relabel(u);
        return sum;
    }//}}}

    struct {
        vector<size_t> vec_node;
    } buf;
    size_t global_relabel(){//{{{
        vector<size_t> &que = buf.vec_node;
        g.v.cur.assign(begin(g.v.start), begin(g.v.start) + n);
        que.resize(g.v.size());
        tree.label.assign(n, n);
        tree.par.assign(n, -1);
        size_t ql = 0, qr = 0;
        for(size_t u = 0; u < g.v.size(); ++u) if(g.v.ex[u] < 0)
            tree.label[que[qr++] = u] = 0;
        while(ql != qr){
            const size_t u = que[ql++];
            for(size_t i = g.v.begin(u); i < g.v.end(u); ++i){
                const size_t v = g.e.t[i], j = g.e.rev[i];
                if(g.e.f[j] < g.e.u[j] and tree.label[v] == n){
                    tree.label[que[qr++] = v] = tree.label[u] + 1;
                    if(g.e.f[i] < g.e.u[i]) tree.par[v] = j;
                }
            }
        }
        tree.roots.assign(n + 1, vector<size_t>());
        size_t res = 0;
        for(size_t u = 0; u < g.v.size(); ++u) if(g.v.ex[u] > 0){
            tree.par[u] = -1;
            tree.make_root(u);
            const size_t l = tree.label[u];
            if(l != n) res = max(res, l);
        }
        tree.label_cnt.assign(n + 1, 0);
        for(auto l : tree.label) ++tree.label_cnt[l];
        return res;
    }//}}}

    Flow run(const size_t s, const size_t t){//{{{
        build();
        const Flow inf = [&]{
            Flow f = 0;
            for(size_t i = g.v.begin(s); i < g.v.end(s); ++i) f += g.e.u[i] - g.e.f[i];
            return f;
        }();
        g.v.ex[s] =  inf;
        g.v.ex[t] = -inf;

        relabel_count = 0;
        tree.build(g);
        for(size_t l = 0; ; ++l){
            if(relabel_count >= 0){
                l = global_relabel();
                relabel_count = -2 * g.v.size();
            }
            while(l > 0 and (l == n or tree.roots[l].empty())) --l;
            if(tree.roots[l].empty()) break;
            // cout << "phase " << l << endl;
            for(const auto u : tree.roots[l]){
                if(g.v.ex[u] <= 0) continue;
                if(tree.label[u] != l) continue;
                g.v.ex[u] -= dfs(u, g.v.ex[u]);
                if(tree.label[u] < n and g.v.ex[u] > 0){
                    assert(tree.label[u] == l+1);
                    tree.make_root(u);
                }
            }
            tree.roots[l].clear();
            if(tree.label_cnt[l] == 0) tree.gap_relabel(l);
        }
        return g.v.ex[t] + inf;
    }//}}}
};//}}}



struct UnionFind{ //{{{
    vector<int> par;
    int n, cnt;
    UnionFind(const int &x=0){init(x);}
    void init(const int &x){par.assign(cnt = n = x, -1);}
    inline int find(const int &x){ return par[x]<0 ? x : par[x] = find(par[x]); }
    inline bool same(const int &x, const int &y){ return find(x) == find(y); }
    inline bool unite(int x, int y){
        if((x = find(x)) == (y = find(y))) return false;
        --cnt;
        if(par[x] > par[y]) swap(x, y);
        par[x] += par[y];
        par[y] = x;
        return true;
    }
    inline int count() const { return cnt; }
    inline int count(int x){ return -par[find(x)]; }
};
//}}}

struct PseudoFlow2{//{{{
    using Flow = int64_t;
    struct RawEdge{//{{{
        int s, t, id;
        Flow l, u;
        RawEdge(const int s, const int t, const Flow l, const Flow u) : s(s), t(t), id(0), l(l), u(u) {}
    };//}}}

    // STATS
    uint64_t numPushes = 0;
    uint64_t numMergers = 0;
    uint64_t numRelabels = 0;
    uint64_t numArcScans = 0;

    size_t n = 0;
    vector<RawEdge> raw_edges;

    size_t add_edge(const int s, const int t, const Flow l, const Flow u){//{{{
        n = max<size_t>(n, max(s, t) + 1);
        raw_edges.emplace_back(s, t, l, u);
        return raw_edges.size() - 1;
    }//}}}
    size_t add_edge(const int s, const int t, const Flow u){//{{{
        return add_edge(s, t, Flow(0), u);
    }//}}}

    int add_v(){ return n++; }
    vector<int> add_vs(const size_t k){//{{{
        vector<int> res(k);
        for(auto &v : res) v = add_v();
        return res;
    }//}}}

    struct {//{{{
        struct Arc{//{{{
            Flow f, u;
            size_t s, t, rev;
        };//}}}
        struct Node{//{{{
            const size_t edge_begin, edge_end;
            Node(const size_t edge_begin, const size_t edge_end) :
                edge_begin(edge_begin),
                edge_end(edge_end) {}
            size_t begin() const { return edge_begin; }
            size_t end() const { return edge_end; }
        };//}}}
        vector<Node> nodes;
        vector<Arc> arcs;
        void set_edge(const size_t i, const size_t j, RawEdge &e){//{{{
            e.id = i;
            arcs[i].u = e.u; arcs[j].u = -e.l;
            arcs[i].f = arcs[j].f = 0;
            arcs[i].rev = j; arcs[j].rev = i;
            arcs[i].s = arcs[j].t = e.s;
            arcs[i].t = arcs[j].s = e.t;
        }//}}}
        void init(const size_t n, vector<RawEdge> &raw_edges){//{{{
            vector<size_t> acc(n + 1);
            for(const auto &e : raw_edges){
                ++acc[e.s];
                ++acc[e.t];
            }
            for(size_t u = 0; u < n; ++u) acc[u+1] += acc[u];
            arcs.resize(acc.back());
            for(auto &e : raw_edges) set_edge(--acc[e.s], --acc[e.t], e);
            nodes.reserve(n);
            for(size_t u = 0; u < n; ++u) nodes.emplace_back(acc[u], acc[u+1]);
        }//}}}
    } g;//}}}
    struct {//{{{
        const size_t undef = ~(size_t(0));
        struct BucketList {//{{{
            const size_t undef = ~(size_t(0));
            vector<size_t> label;
            vector<size_t> next, prev;
            vector<size_t> bucket;
            void init(const size_t n, const size_t b){//{{{
                next.resize(n + b);
                prev.resize(n + b);
                iota(std::begin(next), std::end(next), 0u);
                iota(std::begin(prev), std::end(prev), 0u);
                label.assign(n, undef);
            }//}}}
            void init_zero(const size_t n, const size_t b){//{{{
                next.resize(n + b);
                prev.resize(n + b);
                iota(std::begin(next), std::begin(next) + n, 1u);
                iota(std::begin(prev) + 1, std::begin(prev) + n + 1, 0u);
                next[n] = 0; prev[0] = n;
                iota(std::begin(next) + n + 1, std::end(next), n + 1);
                iota(std::begin(prev) + n + 1, std::end(prev), n + 1);
                label.assign(n, 0);
            }//}}}
            size_t operator[](const size_t u) const { return label[u]; }
            void set(const size_t u, size_t l){//{{{
                next[prev[u]] = next[u];
                prev[next[u]] = prev[u];
                label[u] = l;
                l += label.size();
                next[u] = next[l];
                prev[u] = l;
                prev[next[l]] = u;
                next[l] = u;
            }//}}}
            void set_fifo(const size_t u, size_t l){//{{{
                next[prev[u]] = next[u];
                prev[next[u]] = prev[u];
                label[u] = l;
                l += label.size();
                next[u] = l;
                prev[u] = prev[l];
                next[prev[l]] = u;
                prev[l] = u;
            }//}}}
            void remove(const size_t u){//{{{
                prev[next[u]] = prev[u];
                next[prev[u]] = next[u];
                prev[u] = next[u] = u;
                label[u] = undef;
            }//}}}
            size_t begin(const size_t l) const { return next[l + label.size()]; }
            size_t end(const size_t l) const { return l + label.size(); }
            size_t succ(const size_t i) const { return next[i]; }
            bool empty(const size_t l) const { return begin(l) == end(l); }
        };//}}}

        size_t _highest_root;
        vector<size_t> par;
        BucketList arc;
        BucketList label;
        BucketList strong_roots;
        void init(const decltype(PseudoFlow2::g) &g){//{{{
            const size_t n = g.nodes.size(), m = g.arcs.size();
            label.init_zero(n, n+1);
            _highest_root = 0;
            strong_roots.init(n, n+1);
            arc.init(m, n);
            par.assign(n, undef);
        }//}}}
        void add_edge(const size_t u, const size_t v, const size_t i, const size_t j){//{{{
            arc.set(i, u);
            arc.set(j, v);
        }//}}}
        void remove_edge(const size_t i, const size_t j){//{{{
            arc.remove(i);
            arc.remove(j);
        }//}}}
        size_t begin(const size_t u) const { return arc.begin(u); }
        size_t end(const size_t u) const { return arc.end(u); }
        size_t succ(const size_t i) const { return arc.succ(i); }

        void make_strong_root(const size_t u){//{{{
            par[u] = undef;
            strong_roots.set_fifo(u, label[u]);
        }//}}}
        void remove_strong_root(const size_t u){//{{{
            strong_roots.remove(u);
        }//}}}
        void add_par(const size_t u, const size_t i){//{{{
            par[u] = i;
        }//}}}
        void relabel(const size_t u){//{{{
            label.set(u, label[u] + 1);
            if(strong_roots[u] != undef){
                strong_roots.set_fifo(u, label[u]);
                _highest_root = max(_highest_root, label[u]);
            }
        }//}}}
        size_t highest_strong_root_label(){//{{{
            const size_t n = par.size();
            if(_highest_root == n) --_highest_root;
            while(true){
                while(_highest_root > 0 and strong_roots.empty(_highest_root)) --_highest_root;
                if(strong_roots.empty(_highest_root)) return undef;
                if(_highest_root > 0 and label.empty(_highest_root - 1)){
                    // gap relabel
                    // cerr << "gap relabel" << endl;
                    for(size_t l = _highest_root; l < n; ++l){
                        for(size_t u = label.begin(l); u != label.end(l); u = label.begin(l)){
                            label.set(u, n);
                            if(strong_roots[u] != undef) strong_roots.set_fifo(u, n);
                        }
                    }
                    --_highest_root;
                }else{
                    return _highest_root;
                }
            }
        }//}}}
        size_t highest_strong_root(){//{{{
            const size_t l = highest_strong_root_label();
            if(l == undef) return undef;
            return strong_roots.begin(l);
        }//}}}
    } tree;//}}}

    void check(const size_t line){//{{{
        return;
        // cerr << "check " << line << endl;
        UnionFind uf(n);
        for(size_t u = 0; u < n; ++u){
            if(tree.par[u] == tree.undef){
                if(excess[u] > 0) assert(tree.strong_roots[u] == tree.label[u]);
                else assert(tree.label[u] == 0 or tree.label[u] == n);
            }else{
                size_t i = tree.par[u], j = g.arcs[i].rev, p = g.arcs[i].t;
                assert(uf.unite(u, p));
                assert(tree.label[p] <= tree.label[u]);
            }
        }
    }//}}}

    vector<Flow> excess;
    vector<size_t> cur;
    void build(){//{{{
        g.init(n, raw_edges);
        tree.init(g);
        excess.resize(n);
        cur.resize(n);
        for(size_t u = 0; u < n; ++u) cur[u] = g.nodes[u].begin();
    }//}}}

    void merge(const size_t s, const size_t w, const size_t i){//{{{
        ++numMergers;
        //cerr << "merge " << s << " " << w << endl;
        check(__LINE__);
        const size_t j = g.arcs[i].rev;
        tree.add_edge(s, w, i, j);
        size_t leaf = s;
        for(size_t prev = i; ; ){
            const size_t i = tree.par[leaf];
            tree.add_par(leaf, prev);
            if(i == tree.undef) break;
            prev = g.arcs[i].rev;
            leaf = g.arcs[i].t;
        }
        tree.remove_strong_root(leaf);

        Flow flow = excess[leaf];
        excess[leaf] = 0;
        size_t u = leaf;
        while(tree.par[u] != tree.undef){
            const size_t i = tree.par[u];
            const Flow residual = g.arcs[i].u - g.arcs[i].f;
            const Flow f = std::min(flow, residual);
            excess[u] += flow - f;
            if(excess[u] > 0){
                tree.remove_edge(i, g.arcs[i].rev);
                tree.make_strong_root(u);
            }
            if(f == 0) return;
            g.arcs[i].f += f;
            g.arcs[g.arcs[i].rev].f -= f;
            u = g.arcs[i].t;
            flow = f;
            ++numPushes;
        }
        excess[u] += flow;
        if(excess[u] > 0) tree.make_strong_root(u);
        check(__LINE__);
    }//}}}

    void relabel(const size_t u){
        ++numRelabels;
        tree.relabel(u);
        cur[u] = g.nodes[u].begin();
    }
    // merged or not
    bool process(size_t u){//{{{
        //cerr << "process " << u << endl;
        const size_t l = tree.label[u];
        for(size_t &i = cur[u]; i < g.nodes[u].end(); ++i){
            ++numArcScans;
            const size_t v = g.arcs[i].t;
            if(tree.label[v] != l-1) continue;
            if(g.arcs[i].u == g.arcs[i].f) continue;
            merge(u, v, i);
            return true;
        }
        for(size_t i = tree.begin(u); i < tree.end(u); i = tree.succ(i)){
            if(tree.par[u] == i) continue;
            const size_t v = g.arcs[i].t;
            if(tree.label[v] != l) continue;
            if(process(v)) return true;
        }
        relabel(u);
        return false;
    }//}}}

    void run(){//{{{
        while(true){
            check(__LINE__);
            const size_t u = tree.highest_strong_root();
            check(__LINE__);
            if(u == tree.undef) break;
            //cerr << "root = " << u << " (" << tree.label[u] << ")" << endl;
            process(u);
            check(__LINE__);
        }
    }//}}}
    Flow run(const size_t s, const size_t t){//{{{
        build();
        for(size_t i = g.nodes[s].begin(); i < g.nodes[s].end(); ++i){
            const size_t j = g.arcs[i].rev;
            const Flow f = g.arcs[i].u - g.arcs[i].f;
            g.arcs[i].f += f;
            g.arcs[j].f -= f;
            excess[g.arcs[i].t] += f;
            excess[g.arcs[j].t] -= f;
        }
        for(size_t j = g.nodes[t].begin(); j < g.nodes[t].end(); ++j){
            const size_t i = g.arcs[j].rev;
            const Flow f = g.arcs[i].u - g.arcs[i].f;
            g.arcs[i].f += f;
            g.arcs[j].f -= f;
            excess[g.arcs[i].t] += f;
            excess[g.arcs[j].t] -= f;
        }
        excess[s] = 0;
        excess[t] = 0;
        tree.label.set(s, n);
        for(size_t u = 0; u < n; ++u) if(excess[u] > 0){
            tree.make_strong_root(u);
            relabel(u);
        }
        run();
        cerr << " Number of arc scans :" << numArcScans << endl;
        cerr << " Number of mergers   :" << numMergers << endl;
        cerr << " Number of pushes    :" << numPushes << endl;
        cerr << " Number of relabels  :" << numRelabels << endl;
        Flow res = 0;

        for(const auto &arc : g.arcs){
            const auto rev = g.arcs[arc.rev];
            const size_t u = rev.t, v = arc.t;
            if(tree.label[u] == n and tree.label[v] < n) res += arc.f;
        }
        return res;
    }//}}}
};//}}}

#include "./src/pseudo2.cc"

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
    bm.entry("PseudoFlow2", [&checker](const Instance &instance){
        auto res = run<PseudoFlow2>(instance);
        checker.check(res);
        cout << res << endl;
    });
    bm.entry("PseudoFlow", [&checker](const Instance &instance){
        auto res = run<PseudoFlow>(instance);
        checker.check(res);
        cout << res << endl;
    });
//  bm.entry("new Dinic", [&checker](const Instance &instance){
//      auto res = run<DinitzCirculation>(instance);
//      checker.check(res);
//      cout << res << endl;
//  });
//  //bm.entry("OldDinic", [](const Instance &instance){
//  //        cout << run<OldDinic>(instance) << endl;
//  //        });
//  bm.entry("Dinic", [&checker](const Instance &instance){
//      auto res = run<Dinic>(instance);
//      checker.check(res);
//      cout << res << endl;
//  });
//  bm.entry("AugmentRelabel", [&checker](const Instance &instance){
//      auto res = run<AugmentRelabel>(instance);
//      checker.check(res);
//      cout << res << endl;
//  });
    // bm.entry("PushRelabel", [&checker](const Instance &instance){
    //     auto res = run<PreflowPushRelabel>(instance);
    //     checker.check(res);
    //     cout << res << endl;
    // });
    bm.entry("PPRHighestL", [&checker](const Instance &instance){
        auto res = run<PPRHighest<BucketList>>(instance);
        checker.check(res);
        cout << res << endl;
    });
    bm.entry("PPRHighestV", [&checker](const Instance &instance){
        auto res = run<PPRHighest<BucketVector>>(instance);
        checker.check(res);
        cout << res << endl;
    });
    /*
    bm.entry("PPRH", [&checker](const Instance &instance){
        auto res = run<PPRH<Label>>(instance);
        checker.check(res);
        cout << res << endl;
    });
    */
    bm.entry("TFPPRH", [&checker](const Instance &instance){
        auto res = run<TFPPRH<LabelWithBound, false>>(instance);
        checker.check(res);
        cout << res << endl;
    });
    bm.entry("TFPPRH2", [&checker](const Instance &instance){
        auto res = run<TFPPRH<LabelWithBound, true>>(instance);
        checker.check(res);
        cout << res << endl;
    });
    bm.entry("TFPPRH_PAR", [&checker](const Instance &instance){
        auto res = run<TFPPRH_PAR<LabelWithBound, true>>(instance);
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
