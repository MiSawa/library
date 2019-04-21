#include <bits/stdc++.h>

template<typename ...Args> static inline char *fgets_unlocked(const Args &...args){ return fgets(args...); }
template<typename ...> static inline void putchar_unlocked(const int &c){ putchar(c); }

class {//{{{
    enum { BUF_SIZE = 1<<16 };
    struct input_t {//{{{
        enum { WS = 1<<0, EOL = 1<<1, ALPHA = 1<<2, NUM = 1<<3 };
        char *buf, *table;

        input_t() : buf(get_buf()) { make_table(); }
        inline void make_table(){//{{{
            static char _table[256];
            table = _table;
            table[' '] = table['\t'] = WS;
            table['\r'] = table['\n'] = WS | EOL;
            for(char c = 'a'; c <= 'z'; ++c) table[c] = ALPHA;
            for(char c = 'A'; c <= 'Z'; ++c) table[c] = ALPHA;
            for(char c = '0'; c <= '9'; ++c) table[c] = NUM;
        }//}}}

        static inline char *get_buf(){ static char _buf[BUF_SIZE + 10]; return _buf; }
        inline void fetch(){ buf = fgets_unlocked(get_buf(), BUF_SIZE, stdin); }
        inline bool need_fetch() const { return *buf == '\0'; }
        inline void fetch_if_need(){ if(need_fetch()) fetch(); }
        inline char peek(){ fetch_if_need(); return *buf; }
        inline int peek_type(){ return table[peek()]; }
        inline char getc(){ fetch_if_need(); return *buf++; }
        inline void skip(const int &mask){//{{{
            for(; buf; ++buf){
                fetch_if_need();
                if(buf && !(table[*buf] & mask)) break;
            }
        }//}}}
        inline std::string read_until(const int &mask){//{{{
            std::string res;
            while(buf){
                char *begin = buf;
                while(*buf != '\0' and !(table[*buf] & mask)) ++buf;
                res.append(begin, buf);
                if(*buf != '\0') break;
                fetch();
            }
            return std::move(res);
        }//}}}
        inline std::string read_line(){//{{{
            std::string res = read_until(EOL);
            if(buf) while(*buf) ++buf;
            return std::move(res);
        }//}}}

        template<typename Int> inline void read_int(Int &x){//{{{
            char c = getc(); x = 0;
            while(c != '-' and !(table[c] & NUM)) c = getc();
            if(c == '-') for(; peek_type() & NUM; x = x*10 - (getc() - '0'));
            else for(x = c - '0'; peek_type() & NUM; x = x*10 + (getc() - '0'));
        }//}}}
        template<typename Float> inline void read_float(Float &x){//{{{
            char c = getc(), neg = false; x = 0;
            while(c != '-' and c != '.' and !(table[c] & NUM)) c = getc();
            if(c == '-'){ neg = true; c = getc(); }
            for(; c != '.'; c = getc()) x = x * 10 + (c - '0');
            if(c == '.'){
                Float b = static_cast<Float>(1);
                for(; peek_type() & NUM; x += (b /= static_cast<Float>(10)) * (getc() - '0'));
            }
            if(neg) x = -x;
        }//}}}

        inline void read(char &c){ skip(WS); c = getc(); }
        inline void read(int &x){ read_int<int>(x); }
        inline void read(long long &x){ read_int<long long>(x); }
        inline void read(float &x){ read_float<float>(x); }
        inline void read(double &x){ read_float<double>(x); }
        inline void read(long double &x){ read_float<long double>(x); }
        inline void read(std::string &s){//{{{
            skip(WS);
            s = read_until(WS);
        }//}}}
        inline void readln(std::string &s){//{{{
            s = read_until(EOL);
            skip(EOL);
        }//}}}
    };//}}}

    struct output_t {//{{{
        static inline char *get_buf(){ static char buf[BUF_SIZE]; return buf; }
        inline void pc(const int &c){ putchar_unlocked(c); }
        inline void write(const char &c){ pc(c); }
        inline void write(int x){//{{{
            static char *buf = get_buf();
            int p = 0;
            if(x < 0){
                pc('-');
                for(; x; x /= 10) buf[p++] = -(x%10);
            }else for(; x; x /= 10) buf[p++] = x%10;
            if(!p) buf[p++] = 0;
            while(p--) pc(buf[p] + '0');
        }//}}}
        inline void write(long long x){//{{{
            static char *buf = get_buf();
            int p = 0;
            if(x < 0){
                pc('-');
                for(; x; x /= 10) buf[p++] = -(x%10);
            }else for(; x; x /= 10) buf[p++] = x%10;
            if(!p) buf[p++] = 0;
            while(p--) pc(buf[p] + '0');
        }//}}}
        inline void write(const std::string &s){//{{{
            for(int i = 0; i < s.size(); ++i) pc(s[i]);
        }//}}}
    };//}}}

    static inline input_t &get_input(){ static input_t input; return input; }
    static inline output_t &get_output(){ static output_t output; return output; }

    using manip_t = std::ostream&(std::ostream&);
    public:
    inline std::string line(){ std::string s; get_input().readln(s); return s; }

    template<typename T>
    inline auto operator>>(T &x) -> decltype(get_input().read(x), *this)&{ get_input().read(x); return *this; }

    template<typename T>
    inline operator T(){ T x; *this >> x; return std::move(x); }

    template<typename T>
    inline auto operator<<(const T &x) -> decltype(get_output().write(x), *this)&{//{{{
        get_output().write(x);
        return *this;
    }//}}}
    inline auto operator<<(manip_t &manip) -> decltype(*this)&{//{{{
         if(static_cast<manip_t*>(manip) == static_cast<manip_t*>(std::endl)){
            get_output().write('\n');
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

#define REP(i,b,n) for(int i=(int)(b);i<(int)(n);++i)
#define rep(i,n) REP(i,0,n)
signed main(){
    int n = io, x;
    long long sum = 0;
    rep(_, n) sum += (int)io;
    cout << sum << endl;

    // int x = io, y = io, z = --io;
    // string str = io.line();
    // io >> str;
    // io << x << ", " << y << ", " << z << endl;
    // cout << str << endl;
    // char c = 'x';
    // io << c << endl;
    return 0;
}
// vim:set foldmethod=marker commentstring=//%s:
