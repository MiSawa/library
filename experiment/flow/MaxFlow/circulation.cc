#include <bits/stdc++.h>
using namespace std;

struct Circulation{//{{{
    using Flow = long long;
    struct Edge{//{{{
        int s, t, id;
        Flow l, u;
        Edge(const int s, const int t, const Flow l, const Flow u) : s(s), t(t), id(0), l(l), u(u) {}
    };//}}}
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
    } node;//}}}
    struct {//{{{
        vector<Flow> f, u;
        vector<size_t> s, t, rev;
        void resize(const size_t n){
            f.resize(2 * n);
            u.resize(2 * n);
            s.resize(2 * n);
            t.resize(2 * n);
            rev.resize(2 * n);
        }
        void set(const size_t i, const size_t j, Edge &e){
            f[i] = f[j] = 0;
            u[i] = e.u; u[j] = -e.l;
            s[i] = t[j] = e.s;
            t[i] = s[j] = e.t;
            rev[i] = j; rev[j] = i;
            e.id = i;
        }
        size_t size() const { return f.size(); }
    } edge;//}}}

    vector<Edge> raw_edges;
    Circulation() : n(0){}
    int n;

    size_t add_edge(const int s, const int t, const Flow l, const Flow u){//{{{
        n = max(n, max(s, t) + 1);
        raw_edges.emplace_back(s, t, l, u);
        return raw_edges.size() - 1;
    }//}}}

    int add_v(){ return n++; }
    vector<int> add_vs(const size_t k){//{{{
        vector<int> res(k);
        for(auto &v : res) v = add_v();
        return res;
    }//}}}

    void push(const size_t i, const Flow f){//{{{
        edge.f[edge.rev[i]] -= f;
        edge.f[i] += f;
        node.ex[edge.s[i]] -= f;
        node.ex[edge.t[i]] += f;
    }//}}}

    void build(){//{{{
        node.resize(n);
        edge.resize(raw_edges.size());
        for(const auto &e : raw_edges){
            ++node.start[e.s];
            ++node.start[e.t];
        }
        for(size_t u = 0; u < n; ++u) node.start[u+1] += node.start[u];
        for(auto &e : raw_edges) edge.set(--node.start[e.s], --node.start[e.t], e);
        for(size_t i = 0; i < edge.size(); ++i) if(edge.f[i] > edge.u[i])
            push(i, edge.u[i] - edge.f[i]);
    }//}}}

    struct {
        vector<size_t> vec_node;
    } buf;
    void bfs(){//{{{
        vector<size_t> &que = buf.vec_node;
        node.cur.assign(begin(node.start), begin(node.start) + n);
        que.resize(node.size());
        node.label.assign(n, -1);
        size_t ql = 0, qr = 0;
        for(size_t u = 0; u < node.size(); ++u) if(node.ex[u] > 0)
            node.label[que[qr++] = u] = 0;
        while(ql != qr){
            const size_t u = que[ql++];
            for(size_t i = node.begin(u); i < node.end(u); ++i)
                if(edge.f[i] < edge.u[i] and node.label[edge.t[i]] == -1)
                    node.label[que[qr++] = edge.t[i]] = node.label[u] + 1;
        }
    }//}}}
    Flow dfs(const size_t u, const Flow flow){//{{{
        if(flow == 0) return flow;
        if(node.label[u] == 0){
            Flow f = min(flow, node.ex[u]);
            node.ex[u] -= f;
            return f;
        }
        Flow sum = 0;
        for(size_t &i = node.cur[u]; i < node.end(u); ++i){
            const size_t v = edge.t[i];
            if(node.label[v] != node.label[u] - 1) continue;
            const size_t j = edge.rev[i];
            if(edge.f[j] >= edge.u[j]) continue;
            Flow f = dfs(v, min(edge.u[j] - edge.f[j], flow - sum));
            edge.f[i] -= f;
            edge.f[j] += f;
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
            for(size_t u = 0; u < node.size(); ++u) if(node.ex[u] < 0){
                if(node.label[u] >= 0){
                    Flow f = dfs(u, -node.ex[u]);
                    node.ex[u] += f;
                    update |= f > 0;
                }else{
                    return false;
                }
            }
        }

        for(size_t u = 0; u < node.size(); ++u) if(node.ex[u] != 0) return false;
        return true;
    }//}}}
    pair<bool, Flow> max_flow(const size_t s, const size_t t){//{{{
        constexpr Flow inf = numeric_limits<Flow>::max() / 2;
        add_edge(t, s, 0, inf);
        build();
        raw_edges.pop_back();
        if(!circulation()) return make_pair(false, Flow(0));
        node.ex[s] = inf;
        node.ex[t] = -inf;
        circulation();
        return make_pair(true, inf - node.ex[s]);
    }//}}}
};//}}}

#define REP(i,b,n) for(int i=(int)(b);i<(int)(n);++i)
#define rep(i,n) REP(i,0,n)
typedef long long ll;

namespace CodeFestivalF{//{{{
    bool solve(vector<long long> c){
        const int n = 7;
        rep(i, n) if(c[i] < 0) return false;
        Circulation circulation;
        rep(i, n){
            circulation.add_edge(i, i+n, c[i], c[i]);
            circulation.add_edge(i+n,   (i+1)%n, 0, 1LL<<55);
            circulation.add_edge(i+n, (i+n-1)%n, 0, 1LL<<55);
        }
        return circulation.circulation();
    }

    bool solve(){
        const int n = 7;
        vector<long long> c(n);
        rep(i, n) cin >> c[i];
        if(c[0] == 1 and accumulate(begin(c), end(c), 0LL) == 1) goto ok;
        if(c[0] == 2 and accumulate(begin(c), end(c), 0LL) == 2) goto dame;

        --c[0];
        rep(i, 7) --c[i];
        if(solve(c)) goto ok;
        rep(i, 7) ++c[i];
        for(int left = -6; left <= 0; ++left) for(int right = 0; right <= 6; ++right){
            // 0 -- left -- 0 -- right
            vector<long long> tmp = c;
            vector<int> used(n);
            for(int i = left; i <= right; ++i){
                if(i == left or i == right) tmp[(i+2*n)%n] -= 1;
                else tmp[(i+2*n)%n] -= 2;
                used[(i+2*n)%n] = true;
            }
            bool ng = false;
            rep(i, n) if(!used[i] and c[i] > 0) ng = true;
            if(ng) continue;
            if(solve(tmp)) goto ok;
        }
dame:
        cout << "NO" << endl;
        return true;
ok:
        cout << "YES" << endl;
        return true;
    }
    signed main(){
        cin.tie(nullptr);
        ios_base::sync_with_stdio(false);
        cout << std::fixed << std::setprecision(10);
        solve();
        return 0;
    }
}//}}}

void solve(){
    int n, k; cin >> n >> k;
}

signed main(){
    cin.tie(nullptr);
    ios_base::sync_with_stdio(false);
    cout << std::fixed << std::setprecision(10);
    solve();
    return 0;
}

// vim:set foldmethod=marker commentstring=//%s:
