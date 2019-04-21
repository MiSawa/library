import std.stdio, std.conv;
import std.algorithm, std.range, std.random;
import std.string, std.array, std.container, std.bigint;
import std.exception;
import std.process;

int n;
bool[][] g;

int[] solve(string s) {
    auto pi = pipeProcess(s, Redirect.stdin | Redirect.stdout);
    int m = 0;
    foreach (i; 0..n) {
        foreach (j; i+1..n) {
            if (!g[i][j]) continue;
            m++;
        }
    }
    pi.stdin.writeln(n, " ", m);
    foreach (i; 0..n) {
        foreach (j; i+1..n) {
            if (!g[i][j]) continue;
            pi.stdin.writef("%d %d\n", i, j);
        }
    }
    pi.stdin.flush;
    wait(pi.pid);
    int[] ans = new int[](n);
    foreach (i; 0..n) {
        ans[i] = pi.stdout.readln.strip.to!int;
    }
    return ans;
}

int main() {
    auto gen = Random(unpredictableSeed);
    foreach (t; 0..50000) {
        n = uniform!"[]"(15, 15, gen);
        g = new bool[][](n, n);
        foreach (i; 0..n) {
            foreach (j; i+1..n) {
                if (uniform(0, 4, gen) == 0) {
                    g[i][j] = g[j][i] = true;
                }
            }
        }
//        writeln(n);
/*        int c = 0;
        foreach (i; 0..n) {
            foreach (j; i+1..n) {
                if (g[i][j]) c++;
            }
        }
        writeln(c);
        foreach (i; 0..n) {
            foreach (j; i+1..n) {
                if (g[i][j]) {
                    writeln(i, " ", j);
                }
            }
        }*/
        auto a1 = solve("./yosupo");
        auto b1 = solve("./a.out");
//        writeln(a1);
//        writeln(b1);
        int ac = 0, bc = 0;
        foreach (i; 0..n) {
            if (a1[i] != -1) {
                assert(0 <= a1[i] && a1[i] < n);
                assert(i == a1[a1[i]]);
                assert(g[i][a1[i]]);
                ac++;
            }
            if (b1[i] != -1) {
                assert(0 <= b1[i] && b1[i] < n);
                assert(i == b1[b1[i]]);
                assert(g[i][b1[i]]);
                bc++;
            }
        }
        writeln(n, " ", ac, " ", bc);
        assert(ac == bc);
    }
	return 0;
}
