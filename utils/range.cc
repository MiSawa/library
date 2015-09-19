#if __INCLUDE_LEVEL__ == 0
#include <vector>
#include <iostream>
#else
#pragma once
#endif

// (0, ..., n-1)    : range(n) = range(0, n, +1)
// (1, ..., n)      : range(1, n+1)
// (n, ..., 1)      : range(n, 0, -1)
// (n-1, ..., 0)    : reversed(range(n)) か range(n-1, -1, -1)
//
// TODO:
//  inline 化, private と public ちゃんとする.
namespace utils{//{{{
    template<class Z> struct _range{//{{{
        // T で,
        // T &inc(), T &dec(), T &inc(const Z &), T &dec(const Z &),
        // Z diff(const T &) const を実装すればよい.
        template<class T>
            struct I : public std::iterator<std::random_access_iterator_tag, const Z, Z>{//{{{
                Z i;
                I(const Z &i) : i(i) {}
                bool inline operator==(const I &r) const { return i == r.i; }
                bool inline operator!=(const I &y) const { return i != y.i; }

                const Z& operator*() const{ return i; }
                const Z *operator->() const { return &i; }

                I &operator++(){ return reinterpret_cast<T*>(this)->inc(); }
                I &operator--(){ return reinterpret_cast<T*>(this)->dec(); }
                I operator++(int){ T t(*reinterpret_cast<T*>(this)); ++*this; return t; }
                I operator--(int){ T t(*reinterpret_cast<T*>(this)); --*this; return t; }

                I &operator+=(const Z &n){
                    return reinterpret_cast<T*>(this)->inc(n); }
                I &operator-=(const Z &n){
                    return reinterpret_cast<T*>(this)->dec(n); }
                I operator+(const Z &n) const {
                    T t(*reinterpret_cast<T*>(this)); t+=n; return t; }
                I operator-(const Z &n) const {
                    T t(*reinterpret_cast<T*>(this)); t-=n; return t; }
                friend I operator+(const Z &n, const I &r){ return r + n; }

                Z operator-(const T &r) const { return reinterpret_cast<const T*>(this)->diff(r); }

                Z operator[](const Z &n) const { return *((*this) + n); }
                bool operator< (const T &r) const {
                    return reinterpret_cast<const T*>(this)->diff(r) > 0; }
                bool operator> (const T &r) const { return r < *this; }
                bool operator<=(const T &r) const { return !(*this > r); }
                bool operator>=(const T &r) const { return !(*this < r); }
            };//}}}
        struct It1 : public I<It1>{//{{{
            using I<It1>::i;
            It1(const Z &i) : I<It1>(i) {}
            It1 &inc(){ ++i; return *this; }
            It1 &dec(){ --i; return *this; }
            It1 &inc(const Z &n){ i += n; return *this; }
            It1 &dec(const Z &n){ i -= n; return *this; }
            Z diff(const It1 &o) const { return i - o.i; }
        };//}}}
        struct It1r : public I<It1r>{//{{{
            using I<It1r>::i;
            It1r(const Z &i) : I<It1r>(i) {}
            It1r &inc(){ --i; return *this; }
            It1r &dec(){ ++i; return *this; }
            It1r &inc(const Z &n){ i -= n; return *this; }
            It1r &dec(const Z &n){ i += n; return *this; }
            Z diff(const It1r &o) const { return o.i - i; }
        };//}}}
        struct Its : public I<Its>{//{{{
            using I<Its>::i;
            Z s;
            Its(const Z &i, const Z &s) : I<Its>(i), s(s) {}
            Its &inc(){ i += s; return *this; }
            Its &dec(){ i -= s; return *this; }
            Its &inc(const Z &n){ i += n * s; return *this; }
            Its &dec(const Z &n){ i -= n * s; return *this; }
            Z diff(const Its &o) const { return (i - o.i) / s; }
        };//}}}
        struct Itsr : public I<Itsr>{//{{{
            using I<Itsr>::i;
            Z s;
            Itsr(const Z &i, const Z &s) : I<Itsr>(i), s(s) {}
            Itsr &inc(){ i -= s; return *this; }
            Itsr &dec(){ i += s; return *this; }
            Itsr &inc(const Z &n){ i -= n * s; return *this; }
            Itsr &dec(const Z &n){ i += n * s; return *this; }
            Z diff(const Itsr &o) const { return (o.i - i) / s; }
        };//}}}
        struct range1{//{{{
            const Z b, e;
            range1(const Z &b, const Z &e) : b(b), e(e) {}
            It1 begin()   const { return It1(b); }
            It1 end()     const { return It1(e); }
            It1r rbegin() const { Z tmp = e; return It1r(--tmp); }
            It1r rend()   const { Z tmp = b; return It1r(--tmp); }
        };//}}}
        struct ranges{//{{{
            const Z b, e, s;
            ranges(const Z &b, const Z &e, const Z &s) : b(b), e(e), s(s) {}
            Its  begin()   const { return Its(b, s); }
            Its  end()     const { return Its(e, s); }
            Itsr rbegin() const { Z tmp = e; return Itsr(tmp - s, s); }
            Itsr rend()   const { Z tmp = b; return Itsr(tmp - s, s); }
        };//}}}
    };//}}}
    template<class Z>
        inline typename _range<Z>::range1 range(const Z &b, const Z &e){
            return typename _range<Z>::range1(b, e); }
    template<class Z>
        inline typename _range<Z>::range1 range(const Z &n){ return range((Z)0, n); }
    template<class Z>
        inline typename _range<Z>::ranges range(const Z &b, Z e, const Z &s){
            int t = (b - e) % s;
            if(t == 0) return typename _range<Z>::ranges(b, e, s);
            if((t > 0) ^ (s > 0)) t += s;
            return typename _range<Z>::ranges(b, e + t, s); }
}//}}}

#if __INCLUDE_LEVEL__ == 0
using namespace utils;
using namespace std;
signed main(void){
    auto rng = range(10);
    auto t = rng.begin();
    vector<int> tmp(rng.rbegin(), rng.rend());
    for(auto i : range(10, 4, -3)) cout << i << endl;
    return 0;
}
#endif
// vim:set foldmethod=marker commentstring=//%s:
