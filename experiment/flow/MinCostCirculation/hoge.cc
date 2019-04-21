#include <bits/stdc++.h>
using namespace std;

template<class It> struct Subseq{//{{{
    It b, e;
    Subseq(){}
    Subseq(const It b, const It e) : b(b), e(e) {}
    It begin() const { return b; }
    It end() const { return e; }
};//}}}

int main(){
    vector<int> v(10);
    iota(begin(v), end(v), 0);
    Subseq<vector<int>::iterator> a;
    a = Subseq<vector<int>::iterator>(begin(v), end(v));

    return 0;
}
// vim:set foldmethod=marker commentstring=//%s:
