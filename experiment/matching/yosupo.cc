#include <iostream>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <vector>
#include <valarray>
#include <array>
#include <queue>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <complex>
#include <random>

using namespace std;
using ll = long long;
using ull = unsigned long long;
constexpr int TEN(int n) {return (n==0)?1:10*TEN(n-1);}

const int N = 200;

struct MT {
    bool g[N][N];
    int mt[N];
};


void deb_int(string name, int v[]) {
    printf("db %s: ", name.c_str());
    for (int i = 0; i < N; i++) {
        printf("%d, ", v[i]);
    }
    printf("\n");
}
bool maxmt(MT &mt) {
    int ty[N] = {}; // 頂点のタイプ -1:偶点 0:未接続 1:奇点
    int rt[N]; // 木での根
    int ba[N]; // 偶点での基点
    int nw[N];
    fill_n(nw, N, -1);
    fill_n(rt, N, -1);
    fill_n(ba, N, -1);
    using P = pair<int, int>;

    queue<int> q; // 偶点用のqueue
    for (int i = 0; i < N; i++) {
        if (mt.mt[i] == -1) {
            rt[i] = ba[i] = i; // 木での根, 基点
            ty[i] = -1;
            q.push(i);
        }
    }

    auto get_path = [&](int u) {
        vector<int> v;
        while (rt[u] != u) {
            v.push_back(u);
            v.push_back(mt.mt[u]);
            u = nw[mt.mt[u]];
        }
        v.push_back(u);
        return v;
    };
    while (q.size()) {
        int a = q.front(); q.pop();
        for (int b = 0; b < N; b++) {
            if (!mt.g[a][b] or ty[b] == 1) continue; // 辺が無い or 奇点ならスルー
            if (ty[b] == -1 and ba[a] == ba[b]) continue; // 偶点でもbaseが同じならスルー
            if (ty[b] == 0) {
                // link
                int c = mt.mt[b];
                nw[b] = a; nw[c] = -1;
                rt[b] = rt[c] = rt[a];
                ba[b] = b; ba[c] = c;
                ty[b] = 1; ty[c] = -1;
                q.push(c);
                continue;
            }
            if (rt[a] != rt[b]) {
                // find
                for (int c: {a, b}) {
                    auto v = get_path(c);
                    int m = (int)v.size();
                    assert(m % 2 == 1);
                    for (int i = 1; i < m; i += 2) {
                        mt.mt[v[i]] = v[i+1];
                        mt.mt[v[i+1]] = v[i];
                    }
                }
                mt.mt[a] = b;
                mt.mt[b] = a;
                return true;
            }
            // compress
            auto v1 = get_path(a);
            auto v2 = get_path(b);
            reverse(v1.begin(), v1.end());
            reverse(v2.begin(), v2.end());
            assert(v1[0] == v2[0]);
            int m = min(v1.size(), v2.size()), lca;
            for (int i = 1; i <= m; i++) {
                if (i == m or v1[i] != v2[i]) {
                    lca = v1[i-1];
                    break;
                }
            }
            bool used[N] = {};
            fill_n(used, N, false);
            for (int c: {a, b}) {
                auto v = get_path(c);
                int m = (int)v.size();
                for (int i = 0; ba[v[i]] != ba[lca]; i += 2) {
                    used[ba[v[i]]] = used[ba[v[i+1]]] = true;
                    if (ty[v[i+1]] == 1) {
                        ty[v[i+1]] = -1;
                        q.push(v[i+1]);
                    }
                    if (i) nw[v[i]] = v[i-1];
                }
            }
            if (ba[a] != ba[lca]) nw[a] = b;
            if (ba[b] != ba[lca]) nw[b] = a;
            for (int i = 0; i < N; i++) {
                if (ty[i] and used[ba[i]]) {
                    ba[i] = ba[lca];
                }
            }
        }
    }
    return false;
}

MT first;
int solve() {
    int co = 0;
    while (maxmt(first)) {
        co++;
    }
    return co;
}

int main() {
    memset(first.g, 0, sizeof(first.g));
    memset(first.mt, -1, sizeof(first.mt));
    int n, m;
    cin >> n >> m;
    assert(n <= N);
    for (int i = 0; i < m; i++) {
        int a, b;
        cin >> a >> b;
        assert(0 <= a and a < n);
        assert(0 <= b and b < n);
        assert(a != b);
        assert(!first.g[a][b]);
        first.g[a][b] = true;
        first.g[b][a] = true;
    }
    solve();
    for (int i = 0; i < n; i++) {
        printf("%d\n", first.mt[i]);
    }
    return 0;
}