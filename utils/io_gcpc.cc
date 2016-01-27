#include <bits/stdc++.h>

template<typename ...> static inline int getchar_unlocked(void){ return getchar(); }
template<typename ...> static inline void putchar_unlocked(const int &c){ putchar(c); }
class {//{{{
    static inline int gc(){ return getchar_unlocked(); }
    static inline void pc(const int &c){ putchar_unlocked(c); }
    static inline void ugc(const int &c){ ungetc(c, stdin); }

    enum{ BUF_SIZE = 1<<17 };
    static inline char *get_buf(){ static char buf[BUF_SIZE]; return buf; }
    /*
    static inline void skip_whitespace(){//{{{
        int c;
        for(c = gc(); c == ' ' or c == '\r' or c == '\n' or c == '\t'; c = gc());
        ugc(c);
    }//}}}
    static inline void skip_endline(){//{{{
    }//}}}
    */
    static inline void read(char &c){ c = gc(); }
    static inline void read(int &x){//{{{
        int c; x = 0;
        for(c = gc(); c != '-' and (c < '0' or c > '9'); c = gc());
        if(c == '-') for(c = gc(); '0' <= c and c <= '9'; c = gc()) x = x * 10 - (c - '0');
        else for(; '0' <= c and c <= '9'; c = gc()) x = x * 10 + (c - '0');
        ugc(c);
    }//}}}
    static inline void read(long long &x){//{{{
        int c; x = 0;
        for(c = gc(); c != '-' and (c < '0' or c > '9'); c = gc());
        if(c == '-') for(c = gc(); '0' <= c and c <= '9'; c = gc()) x = x * 10 - (c - '0');
        else for(; '0' <= c and c <= '9'; c = gc()) x = x * 10 + (c - '0');
        ugc(c);
    }//}}}
    static inline void read(std::string &s){//{{{
        static char *buf = get_buf(); s = "";
        int c;
        for(c = gc(); c == ' ' or c == '\r' or c == '\n' or c == '\t' or c == EOF; c = gc());
        int p = 0;
        for(; !(c == ' ' or c == '\r' or c == '\n' or c == '\t' or c == EOF); c = gc()){
            buf[p++] = c;
            if(p == BUF_SIZE){ s.append(buf, p); p = 0; }
        }
        s.append(buf, p);
        ugc(c);
    }//}}}
    static inline void readln(std::string &s){//{{{
        static char *buf = get_buf(); s = "";
        int c, p = 0;
        for(c = gc(); !(c == '\r' or c == '\n' or c == EOF); c = gc()){
            buf[p++] = c;
            if(p == BUF_SIZE){ s.append(buf, p); p = 0; }
        }
        if(c == '\r'){ c = gc(); if(c != '\n') ungetc(c, stdin); }
        s.append(buf, p);
    }//}}}

    static inline void write(const char &c){ pc(c); }
    static inline void write(int x){//{{{
        static char *buf = get_buf();
        int p = 0;
        if(x < 0){
            pc('-');
            for(; x; x /= 10) buf[p++] = -(x%10);
        }else for(; x; x /= 10) buf[p++] = x%10;
        if(!p) buf[p++] = 0;
        while(p--) pc(buf[p] + '0');
    }//}}}
    static inline void write(long long x){//{{{
        static char *buf = get_buf();
        int p = 0;
        if(x < 0){
            pc('-');
            for(; x; x /= 10) buf[p++] = -(x%10);
        }else for(; x; x /= 10) buf[p++] = x%10;
        if(!p) buf[p++] = 0;
        while(p--) pc(buf[p] + '0');
    }//}}}
    static inline void write(const std::string &s){//{{{
        for(int i = 0; i < s.size(); ++i) pc(s[i]);
    }//}}}

    using manip_t = std::ostream&(std::ostream&);
    public:
    inline std::string line(){ std::string s; readln(s); return s; }
    template<typename T>
    inline operator T(){ T x; read(x); return std::move(x); }

    template<typename T>
    inline auto operator>>(T &x) -> decltype(read(x), *this)&{ read(x); return *this; }

    template<typename T>
    inline auto operator<<(const T &x) -> decltype(write(x), *this)&{//{{{
        write(x);
        return *this;
    }//}}}
    inline auto operator<<(manip_t &manip) -> decltype(*this)&{//{{{
         if(static_cast<manip_t*>(manip) == static_cast<manip_t*>(std::endl)){
            write('\n');
        }else assert(false && "only std::endl supported");
        return *this;
    }//}}}

    inline int operator+(const int &x){ return static_cast<int>(*this)+x; }
    inline int operator-(const int &x){ return static_cast<int>(*this)-x; }
    inline long long operator+(const long long &x){ return static_cast<long long>(*this)+x; }
    inline long long operator-(const long long &x){ return static_cast<long long>(*this)-x; }
    inline int operator--(){ return static_cast<int>(*this)-1; }
} io;//}}}

using namespace std;

void solve(){
    int x = io, y = io, z = --io;
    string str = io.line();
    io >> str;
    io << x << ", " << y << ", " << z << endl;
    cout << str << endl;
    char c = 'x';
    io << c << endl;
}

#define REP(i,b,n) for(int i=(int)(b);i<(int)(n);++i)
#define rep(i,n) REP(i,0,n)
signed main(){
    int n = io, x;
    rep(_, n) x = io;
    // cin.tie(nullptr);
    // ios_base::sync_with_stdio(false);
    // cout << std::fixed << std::setprecision(10);
    // solve();
    return 0;
}
// vim:set foldmethod=marker commentstring=//%s:
