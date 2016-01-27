#include <bits/stdc++.h>//{{{
#define all(x) begin(x),end(x)
#define rall(x) (x).rbegin(),(x).rend()
#define REP(i,b,n) for(int i=(int)(b);i<(int)(n);++i)
#define rep(i,n) REP(i,0,n)
#define rrep(i,n) for(int i=(int)(n)-1;i>=0;--i)
#define repsz(i,v) rep(i,(v).size())
#define aur auto&
#define bit(n) (1LL<<(n))
#define eb emplace_back
#define mt make_tuple
#define fst first
#define snd second
using namespace std;
typedef long long ll;
//#define int long long
template<class C>int size(const C &c){ return c.size(); }
template<class T,class U>bool chmin(T&a,const U&b){if(a<=b)return false;a=b;return true;}
template<class T,class U>bool chmax(T&a,const U&b){if(a>=b)return false;a=b;return true;}//}}}

// for local test (tr << hoge, etc)//{{{
// BEGIN CUT HERE
#define LOCAL_TEST_HOGEHOGE
#ifdef LOCAL_TEST_HOGEHOGE
// tr << ( vector, map, pair, tuple, etc... ) << endl//{{{
template <typename T, typename U>
struct sfinae_helper{typedef void type;};
template <typename T, typename U = void>
struct Print{ static ostream &pp(ostream &os, const T &x){ return os << x; } };
struct trace{//{{{
    ostream& os;
    trace(ostream& os): os(os) { }
    template<typename T>
        trace& operator<<(const T& x){ Print<T>::pp(os, x); return *this;}
    trace &operator<<(ostream& f(ostream&)){ f(os); return *this; }
    operator ostream &(){ return os; }
} tr(cout);//}}}
// string//{{{
template <> struct Print<string, void>{ static ostream &pp(ostream &os, const string &x){ return os << x; } };
//}}}
// Container//{{{
template <typename T> struct Print<T, typename sfinae_helper<T, typename T::iterator>::type>{
    static ostream &pp(ostream &os, const T &v){
        trace(os) << '[';
        for(const auto &x : v) trace(os) << x << ", ";
        return trace(os) << ']';
    }
};//}}}
// Pair//{{{
template <typename T> struct Print<T, typename sfinae_helper<T, typename T::first_type>::type>{
    static ostream &pp(ostream &os, const T &x){
        return trace(os) << '(' << x.first << ", " << x.second << ')';
    }
};//}}}
// Tuple//{{{
template<class T, size_t N> struct TuplePrint{
    static ostream &print(ostream &os, const T &x){
        TuplePrint<T, N-1>::print(os, x) << ", ";
        return trace(os) << get<N-1>(x);
        //return Print<decltype(get<N-1>(x))>::pp(os, get<N-1>(x));
    }
};
template<class T> struct TuplePrint<T, 1>{
    static ostream &print(ostream &os, const T &x){
        return trace(os) << get<0>(x);
    }
};
template <typename... Args>//Tuple
struct Print<tuple<Args...>>{
    static ostream &pp(ostream &os, const tuple<Args...> &x){
        os << "(";
        return TuplePrint<decltype(x), sizeof...(Args)>::print(os, x) << ")";
    }
};//}}}
//}}}
#endif
// END CUT HERE
#ifndef LOCAL_TEST_HOGEHOGE//{{{
struct no_trace{//{{{
    template<typename T> no_trace &operator<<(const T &x){(void)x;return *this; }
    no_trace &operator<<(ostream &f(ostream&)){(void)f;return *this; }
} tr;//}}}
#endif//}}}
//}}}
// XorCards class definition {{{
class XorCards{ //{{{
public:
   long long numberOfWays( vector<long long> number, long long limit );
}; //}}}
// BEGIN CUT HERE
// test code {{{
namespace moj_harness {
	int run_test_case(int);
	void run_test(int casenum = -1, bool quiet = false) {
		if (casenum != -1) {
			if (run_test_case(casenum) == -1 && !quiet) {
				cerr << "Illegal input! Test case " << casenum << " does not exist." << endl;
			}
			return;
		}
		
		int correct = 0, total = 0;
		for (int i=0;; ++i) {
			int x = run_test_case(i);
			if (x == -1) {
				if (i >= 100) break;
				continue;
			}
			correct += x;
			++total;
		}
		
		if (total == 0) {
			cerr << "No test cases run." << endl;
		} else if (correct < total) {
			cerr << "Some cases FAILED (passed " << correct << " of " << total << ")." << endl;
		} else {
			cerr << "All " << total << " tests passed!" << endl;
		}
	}
	
	int verify_case(int casenum, const long long &expected, const long long &received, clock_t elapsed) { 
		cerr << "Example " << casenum << "... "; 
		
		string verdict;
		vector<string> info;
		char buf[100];
		
		if (elapsed > CLOCKS_PER_SEC / 200) {
			sprintf(buf, "time %.2fs", elapsed * (1.0/CLOCKS_PER_SEC));
			info.push_back(buf);
		}
		
		if (expected == received) {
			verdict = "PASSED";
		} else {
			verdict = "FAILED";
		}
		
		cerr << verdict;
		if (!info.empty()) {
			cerr << " (";
			for (int i=0; i<(int)info.size(); ++i) {
				if (i > 0) cerr << ", ";
				cerr << info[i];
			}
			cerr << ")";
		}
		cerr << endl;
		
		if (verdict == "FAILED") {
			cerr << "    Expected: " << expected << endl; 
			cerr << "    Received: " << received << endl; 
		}
		
		return verdict == "PASSED";
	}

	int run_test_case(int casenum__) {
		switch (casenum__) {
		case 0: {
			long long number[]        = {1, 2};
			long long limit           = 2;
			long long expected__      = 3;

			clock_t start__           = clock();
			long long received__      = XorCards().numberOfWays(vector<long long>(number, number + (sizeof number / sizeof number[0])), limit);
			return verify_case(casenum__, expected__, received__, clock()-start__);
		}
		case 1: {
			long long number[]        = {5, 5};
			long long limit           = 3;
			long long expected__      = 2;

			clock_t start__           = clock();
			long long received__      = XorCards().numberOfWays(vector<long long>(number, number + (sizeof number / sizeof number[0])), limit);
			return verify_case(casenum__, expected__, received__, clock()-start__);
		}
		case 2: {
			long long number[]        = {1, 2, 3, 4, 5, 6, 7};
			long long limit           = 5;
			long long expected__      = 96;

			clock_t start__           = clock();
			long long received__      = XorCards().numberOfWays(vector<long long>(number, number + (sizeof number / sizeof number[0])), limit);
			return verify_case(casenum__, expected__, received__, clock()-start__);
		}
		case 3: {
			long long number[]        = {123, 456, 789, 147, 258, 369, 159, 357};
			long long limit           = 500;
			long long expected__      = 125;

			clock_t start__           = clock();
			long long received__      = XorCards().numberOfWays(vector<long long>(number, number + (sizeof number / sizeof number[0])), limit);
			return verify_case(casenum__, expected__, received__, clock()-start__);
		}
		case 4: {
			long long number[]        = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
			long long limit           = 1000000000000000LL;
			long long expected__      = 4294967296LL;

			clock_t start__           = clock();
			long long received__      = XorCards().numberOfWays(vector<long long>(number, number + (sizeof number / sizeof number[0])), limit);
			return verify_case(casenum__, expected__, received__, clock()-start__);
		}
		case 5: {
			long long number[]        = {1000000000000000LL, 999999999999999LL};
			long long limit           = 65535;
			long long expected__      = 2;

			clock_t start__           = clock();
			long long received__      = XorCards().numberOfWays(vector<long long>(number, number + (sizeof number / sizeof number[0])), limit);
			return verify_case(casenum__, expected__, received__, clock()-start__);
		}

		// custom cases

/*      case 6: {
			long long number[]        = ;
			long long limit           = ;
			long long expected__      = ;

			clock_t start__           = clock();
			long long received__      = XorCards().numberOfWays(vector<long long>(number, number + (sizeof number / sizeof number[0])), limit);
			return verify_case(casenum__, expected__, received__, clock()-start__);
		}*/
/*      case 7: {
			long long number[]        = ;
			long long limit           = ;
			long long expected__      = ;

			clock_t start__           = clock();
			long long received__      = XorCards().numberOfWays(vector<long long>(number, number + (sizeof number / sizeof number[0])), limit);
			return verify_case(casenum__, expected__, received__, clock()-start__);
		}*/
/*      case 8: {
			long long number[]        = ;
			long long limit           = ;
			long long expected__      = ;

			clock_t start__           = clock();
			long long received__      = XorCards().numberOfWays(vector<long long>(number, number + (sizeof number / sizeof number[0])), limit);
			return verify_case(casenum__, expected__, received__, clock()-start__);
		}*/
		default:
			return -1;
		}
	}
}

// }}}
// main function {{{
int main(int argc, char *argv[]) {
	if (argc == 1) {
		moj_harness::run_test();
	} else {
		for (int i=1; i<argc; ++i)
			moj_harness::run_test(atoi(argv[i]));
	}
}
// }}}
// END CUT HERE
//}}}

struct BinaryVector{//{{{
    using Bits = uint32_t;
    constexpr static size_t log_bits = 5, num_bits = 1<<log_bits;

    vector<Bits> v;
    size_t sz;
    explicit BinaryVector(const int size = 0){ resize(size); }

    void resize(int n){//{{{
        assert(n >= 0);
        sz = n;
        v.resize((n>>log_bits) + 1);
        if(!v.empty()) v.back() &= (Bits(2) << (n&(num_bits-1))) - 1;
    }//}}}

    struct BitRef{//{{{
        Bits &bits;
        const size_t pos;
        BitRef(Bits &bits, const size_t pos) : bits(bits), pos(pos){}
        BitRef &operator=(const BitRef &r){
            *this = (bool)(r);
            return *this;
        }
        BitRef &operator=(const bool &b){
            if(b) bits |= Bits(1)<<pos;
            else  bits &= ~(Bits(1)<<pos);
            return *this;
        }
        BitRef &operator^=(const bool &b){
            if(b) bits ^= Bits(1)<<pos;
            return *this;
        }
        operator bool() const { return bits>>pos&1; }
        void swap(BitRef &o){
            bool a = (bool)(*this), b = (bool)(o);
            o = a; *this = b;
        }
        void swap(BitRef &&o){
            bool a = (bool)(*this), b = (bool)(o);
            o = a; *this = b;
        }
    };//}}}

    const bool operator[](size_t n) const {//{{{
        assert(0 <= n and n < sz);
        const int idx = n>>log_bits;
        return v[idx]>>(n&(num_bits-1))&1;
    }//}}}
    BitRef operator[](size_t n){//{{{
        assert(0 <= n and n < sz);
        const int idx = n>>log_bits;
        return BitRef(v[idx], n&(num_bits-1));
    }//}}}
    BinaryVector &operator&=(const BinaryVector &b){//{{{
        check_dim(b);
        for(int i = 0; i < v.size(); ++i) v[i] &= b.v[i];
        return *this;
    }//}}}
    BinaryVector &operator|=(const BinaryVector &b){//{{{
        check_dim(b);
        for(int i = 0; i < v.size(); ++i) v[i] |= b.v[i];
        return *this;
    }//}}}
    BinaryVector &operator^=(const BinaryVector &b){//{{{
        check_dim(b);
        for(int i = 0; i < v.size(); ++i) v[i] ^= b.v[i];
        return *this;
    }//}}}
    BinaryVector &operator+=(const BinaryVector &b){ return *this ^= b; }
    BinaryVector &operator-=(const BinaryVector &b){ return *this ^= b; }

    int size() const { return sz; }
    private:
    void check_dim(const BinaryVector &b){ assert(size() == b.size()); }
};//}}}

struct BinaryMatrix{//{{{
    vector<BinaryVector> A;
    BinaryMatrix(const int n, const int m) : A(n, BinaryVector(m)){}
    BinaryVector &operator[](const size_t &i){ return A[i]; }
    const BinaryVector &operator[](const size_t &i) const { return A[i]; }

    int size() const { return A.size(); }

    static BinaryMatrix I(const size_t n){//{{{
        BinaryMatrix res(n, n);
        for(int i = 0; i < n; ++i) res[i][i] = true;
        return res;
    }//}}}

    BinaryMatrix &operator+=(const BinaryMatrix &b){//{{{
        check_dim(b);
        for(int i = 0; i < size(); ++i) (*this)[i] += b[i];
        return *this;
    }//}}}
    BinaryMatrix &operator-=(const BinaryMatrix &b){ return *this += b; }
    BinaryMatrix &operator*=(const BinaryMatrix &b){//{{{
        BinaryMatrix a = *this;
        assert(a[0].size() == b.size());
        *this = BinaryMatrix(a.size(), b[0].size());
        for(int i = 0; i < a.size(); ++i) for(int j = 0; j < b[0].size(); ++j)
            for(int k = 0; k < a[0].size(); ++k)
                (*this)[i][j] ^= a[i][k] && b[k][j];
        return *this;
    }//}}}

    BinaryMatrix transpose() const {//{{{
        BinaryMatrix res(A[0].size(), A.size());
        for(int i = 0; i < A.size(); ++i) for(int j = 0; j < A[0].size(); ++j)
            res[j][i] = A[i][j];
        return res;
    }//}}}

    BinaryMatrix pow(long long e) const {//{{{
        BinaryMatrix res = BinaryMatrix::I(size()), a = *this;
        for(; e; e >>= 1){
            if(e&1) res *= *this;
            a *= a;
        }
        return res;
    }//}}}

    int rank() const {//{{{
        BinaryMatrix a = *this;
        const int n = size(), m = a[0].size();
        int r = 0;
        for(int j = 0; r < n and j < m; ++j){
            int piv = r;
            for(int i = r+1; i < n; ++i) if(a[i][j]){ piv = i; break; }
            if(r != piv) std::swap(a[r], a[piv]);
            if(!a[r][j]) continue;
            for(int i = r+1; i < n; ++i) for(int k = m-1; k >= j; --k)
                a[i][k] ^= a[r][k] && a[i][j];
            ++r;
        }
        return r;
    }//}}}

    // dim of solution space of (*this) * x = b
    int dim_of_solution_space(BinaryVector b) const {//{{{
        const int n = size(), m = A[0].size();
        assert(n == b.size());
        BinaryMatrix a(n, m+1);
        for(int i = 0; i < n; ++i) for(int j = 0; j < m; ++j) a[i][j] = (*this)[i][j];
        for(int i = 0; i < n; ++i) a[i][m] = b[i];

        int r = 0;
        for(int j = 0; r < n and j <= m; ++j){
            int piv = r;
            for(int i = r+1; i < n; ++i) if(a[i][j]){ piv = i; break; }
            if(r != piv) std::swap(a[r], a[piv]);
            if(!a[r][j]) continue;
            if(j == m) return -1;
            for(int i = r+1; i < n; ++i) for(int k = m; k >= j; --k)
                a[i][k] ^= a[r][k] && a[i][j];
            ++r;
        }
        return m - r;
    }//}}}
    private:
    void check_dim(const BinaryMatrix &b){//{{{
        assert(size() == b.size());
        if(size() != 0) assert((*this)[0].size() == b[0].size());
    }//}}}
};//}}}

long long XorCards::numberOfWays( vector<long long> number, long long limit ){
    ++limit;
    const int n = size(number);
    long long res = 0;
    rep(i, 60) if(limit>>i&1){
        long long x = limit ^ (1LL<<i);
        BinaryMatrix A(60, n);
        rep(t, n) rep(j, 60) if(i <= j) A[j][t] = number[t]>>j&1;
        BinaryVector v(60);
        rep(j, 60) if(i <= j) v[j] = x>>j&1;
        int tmp = A.dim_of_solution_space(v);
        if(tmp != -1) res += (1LL << tmp);
    }
    return res;
}

// vim:set foldmethod=marker commentstring=//%s:
