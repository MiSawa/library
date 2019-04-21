#include <bits/stdc++.h>
using namespace std;

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

    int value(){ return val; }

    private:
    int val;
};//}}}

struct RightLeaningSegtree{//{{{
    struct Node{
        Integer val;
        Node *par, *l, *r;
        int lidx, ridx;
        Node(Integer val) : val(val), par(nullptr), l(nullptr), r(nullptr){}
    };
    Node *root;
    vector<Node *> nodes;
    RightLeaningSegtree() : root(nullptr) {}
    RightLeaningSegtree(vector<Integer> &data){//{{{
        assert(!data.empty());
        root = new Node(Integer(0));
        if(data.size() == 1){
            nodes.emplace_back(root);
            root->val = data[0];
            root->lidx = 0; root->ridx = 1;
            return;
        }
        Node *u = root;
        int idx = 0;
        for(int k = 0; ; ++k){
            u->l = make_k(k, u, data, idx);
            if(idx == data.size()) break;
            u->r = new Node(Integer(0));
            u->r->par = u;
            u = u->r;
        }
        while(u){
            pull(u);
            u = u->par;
        }
    }//}}}
    Node *make_k(int k, Node *par, vector<Integer> &data, int &idx){//{{{
        if(idx == data.size()) return nullptr;
        Node *u = new Node(Integer(0));
        u->par = par;
        if(k == 0){
            nodes.emplace_back(u);
            u->lidx = idx; u->ridx = idx+1;
            u->val = data[idx++];
        }else{
            u->l = make_k(k-1, u, data, idx);
            u->r = make_k(k-1, u, data, idx);
        }
        return pull(u);
    }//}}}
    Node *pull(Node *u){//{{{
        if(u == nullptr or u->l == nullptr) return u;
        u->lidx = u->l->lidx;
        u->ridx = u->l->ridx;
        if(u->r == nullptr){
            u->val = u->l->val;
        }else{
            u->val = u->l->val + u->r->val;
            u->ridx = u->r->ridx;
        }
        return u;
    }//}}}

    // [l, r)
    Integer sum(int l, int r){//{{{
        if(l == r) return Integer(0);
        return sum(root, l, r);
    }//}}}
    Integer sum(Node *u, int l, int r){//{{{
        assert(u != nullptr);
        if(l <= u->lidx and u->ridx <= r) return u->val;
        if(u->r == nullptr) return sum(u->l, l, r);
        if(r <= u->r->lidx) return sum(u->l, l, r);
        if(u->l->ridx <= l) return sum(u->r, l, r);
        return sum(u->l, l, r) + sum(u->r, l, r);
    }//}}}
    void add(int idx, Integer val){//{{{
        Node *u = nodes[idx];
        u->val += val;
        for(u = u->par; u; u = u->par) pull(u);
    }//}}}

    void print(){
        print(root);
    }
    void print(Node *u, int depth = 0){
        if(u == nullptr) return;
        print(u->l, depth+1);
        cout << string(depth, ' ') << u << ", " << u->lidx << " " << u->ridx << endl;
        print(u->r, depth+1);
    }
};//}}}

struct Tree{//{{{
    using Data = Integer;
    struct E{//{{{
        int s, t;
        E(){}
        E(int s, int t) : s(s), t(t){}
    };//}}}

    vector<vector<E>> g;
    Tree(int n) : g(n){}
    void add_edge(int u, int v){
        g[u].emplace_back(u, v);
        g[v].emplace_back(v, u);
    }
    vector<int> pre_order;
    vector<int> post_order;
    vector<int> par;
    void build_dfs(int root){//{{{
        assert(par.empty());
        par.assign(g.size(), -1);
        vector<int> stack;
        stack.emplace_back(root);
        while(!stack.empty()){
            int u = stack.back(); stack.pop_back();
            pre_order.emplace_back(u);
            for(auto e : g[u]) if(e.t != par[u]){
                par[e.t] = u;
                stack.emplace_back(e.t);
            }
        }
        post_order = pre_order;
        reverse(begin(post_order), end(post_order));
    }//}}}

    vector<int> depth;
    vector<Integer> data;
    vector<int> super_ancestor;
    vector<RightLeaningSegtree> segs;
    void build(int root = 0){//{{{
        build_dfs(root);

        const int n = g.size();
        super_ancestor.assign(n, -1);
        depth.assign(n, 0);
        for(auto u : pre_order) if(par[u] != -1) depth[u] = depth[par[u]]+1;

        vector<int> subtree_size(n, 1);
        vector<pair<int, int>> heavy_edge(n);
        for(auto u : post_order) for(auto &e : g[u]) if(e.t != par[u])
            subtree_size[u] += subtree_size[e.t];
        for(auto u : post_order) if(par[u] != -1)
            heavy_edge[par[u]] = max(heavy_edge[par[u]],
                    make_pair(subtree_size[u], u));

        super_ancestor.assign(n, root);
        for(auto u : pre_order) if(par[u] != -1){
            if(heavy_edge[par[u]].second != u) super_ancestor[u] = u;
            else super_ancestor[u] = super_ancestor[par[u]];
        }
        segs.resize(n);
        for(auto u : post_order) if(subtree_size[u] == 1){
            vector<Integer> seq;
            for(int v = u; ; v = par[v]){
                seq.emplace_back(data[v]);
                if(v == super_ancestor[v]) break;
            }
            reverse(begin(seq), end(seq));
            segs[super_ancestor[u]] = RightLeaningSegtree(seq);
        }
    }//}}}

    void add(int u, Integer val){
        const int p = super_ancestor[u];
        const int idx = depth[u] - depth[p];
        segs[p].add(idx, val);
    }
    Integer sum(int u){
        int p = super_ancestor[u];
        int idx = depth[u] - depth[p];
        Integer res = segs[p].sum(0, idx+1);
        for(int v = par[p]; v != -1; v = par[p]){
            p = super_ancestor[v];
            idx = depth[v] - depth[p];
            res += segs[p].sum(0, idx+1);
        }
        return res;
    }
};//}}}

#include <iostream>
#define REP(i,b,n) for(int i=(b);i<(int)(n);++i)
#define rep(i,n) REP(i,0,n)
int main(){
    int n; cin >> n;
    Tree tree(n);
    for(int i = 1; i < n; ++i){
        int v; cin >> v;
        tree.add_edge(i, v);
    }
    rep(u, n){
        int val; cin >> val;
        tree.data.emplace_back(val);
    }
    tree.build();

    int q; cin >> q;
    rep(_, q){
        int op; cin >> op;
        if(op == 0){
            int u, val; cin >> u >> val;
            tree.add(u, Integer(val));
        }else{
            int u; cin >> u;
            cout << tree.sum(u).value() << endl;
        }
    }
    cerr << Integer::count() << " operations" << endl;
    return 0;
}
// vim:set foldmethod=marker commentstring=//%s:
