#include <bits/stdc++.h>

/*
 * 二部グラフ化しなくてもいい.
 * BipartiteMatching g;
 * g.add_edge(u, v); // A 側と B 側を左右どっちに置いてもいい. 番号も混ざっていてよい
 * g.run();
 * マッチング自体は match に入る.
 *
 * |最大マッチング| + |  最小辺被覆(辺で被覆)  | = |V| - (孤立点)
 * | 最大独立集合 | + |最小頂点被覆(頂点で被覆)| = |V|
 * |最大マッチング| = |最小頂点被覆(頂点で被覆)|
 *
 * 二部グラフを (A cup B), 最小カットを (S cup T) とする.
 *
 * | 最小辺被覆 | = |V| - |最大マッチング| - |孤立点|
 *      - 最大マッチング + マッチしていない頂点に接続する辺から一本ずつ.
 * |最小頂点被覆| = |最大マッチング|
 *      - (A cap T) cup (B cap S)
 *      - used を一回初期化し, 全 A 側の未マッチ頂点から dfs を呼び,
 *        used でない A 側と used な B 側が最小頂点被覆
 * |最大独立集合| = |V| - |最大マッチング|
 *      - 頂点被覆の補集合
 */
struct BipartiteMatching{//{{{
    vector<vector<int>> g;
    void add_edge(int u, int v){//{{{
        if(g.size() <= u) g.resize(u+1);
        if(g.size() <= v) g.resize(v+1);
        g[u].emplace_back(v);
        g[v].emplace_back(u);
    }//}}}
    vector<char> used;
    vector<int> match;
    bool dfs(int u, bool dbg = false){//{{{
        used[u] = true;
        for(const auto v : g[u]){
            used[v] = true;
            const int w = match[v];
            if(w < 0 or (!used[w] and dfs(w, dbg))){
                match[match[u] = v] = u;
                return true;
            }
        }
        return false;
    }//}}}
    int run(){//{{{
        const int n = g.size();
        match.assign(n, -1);
        int res = 0;
        for(int u = 0; u < n; ++u) if(match[u] < 0){
            used.assign(n, false);
            if(dfs(u)) ++res;
        }
        return res;
    }//}}}
    vector<char> bipartize(){//{{{
        const int n = g.size();
        vector<char> d(n, 2);
        for(int u = 0; u < n; ++u) if(d[u] > 1){
            vector<int> s = {u}; d[u] = 0;
            while(!s.empty()){
                const int u = s.back(); s.pop_back();
                for(const auto v : g[u]) if(d[v] > 1){
                    s.emplace_back(v);
                    d[v] = d[u] ^ 1;
                }
            }
        }
        return d;
    }//}}}

    vector<int> min_vertex_cover(){//{{{
        // return run()
        run();
        auto b = bipartize();
        vector<int> res;
        used.assign(g.size(), false);
        for(int u = 0; u < g.size(); ++u) if(b[u] and match[u] < 0 and !used[u]) dfs(u);
        for(int u = 0; u < g.size(); ++u) if(b[u] != used[u]) res.emplace_back(u);
        return res;
    }//}}}
    vector<int> max_independent_set(){//{{{
        // return g.size() - run()
        run();
        auto b = bipartize();
        vector<int> res;
        used.assign(g.size(), false);
        for(int u = 0; u < g.size(); ++u) if(b[u] and match[u] < 0 and !used[u]) dfs(u);
        for(int u = 0; u < g.size(); ++u) if(b[u] == used[u]) res.emplace_back(u);
        return res;
    }//}}}
};//}}}

signed main(){//{{{
    BipartiteMatching g;
    g.add_edge(0, 2);
    g.add_edge(2, 1);
    g.add_edge(1, 3);
    // g.add_edge(0, 5);
    // g.add_edge(0, 6);
    // g.add_edge(1, 7);
    // g.add_edge(2, 4);
    // g.add_edge(2, 6);
    // g.add_edge(2, 8);
    // g.add_edge(3, 7);
    // tr << g.min_vertex_cover() << endl;
    // tr << g.max_independent_set() << endl;
    return 0;
}//}}}
// vim:set foldmethod=marker commentstring=//%s:
