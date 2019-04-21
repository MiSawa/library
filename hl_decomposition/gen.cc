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
typedef long long ll;

int genInt(int r) {
	return rand() % r;
}

const int B = 16;
int trsm[1000];
int pe[100100];

int main() {
	srand(time(NULL));

	//446 99681
	trsm[1] = 1;
	int trs = 1;
	for (int i = 2; i < 1000; i++) {
		trsm[i] = trs + 2;
		trs += trsm[i];
	}

	int sm = 0;
	for (int i = B; i >= 1; i--) {
		if (i != B) {
			pe[sm] = sm - trsm[(i+1)];
		}
		for (int j = 1; j < trsm[i]; j++) {
			pe[sm+j] = sm+j-1;
		}
		sm += trsm[i];
	}

	int n = sm;
	printf("%d\n", n);
	for (int i = 1; i < n; i++) {
		printf("%d\n", pe[i]);
	}

	for (int i = 0; i < n; i++) {
		printf("%d\n", genInt(100));
	}

	int q = 100000;
	printf("%d\n", q);
	for (int i = 0; i < q; i++) {
		int ty = genInt(2);
		ty = 1; // get query only
		if (ty == 0) {
			int k = genInt(n);
			int x = genInt(100);
			k = n-1;
			printf("%d %d %d\n", ty, k, x);
		} else {
			int k = genInt(n);
			k = n-1;
			printf("%d %d\n", ty, k);
		}
	}
    return 0;
}
