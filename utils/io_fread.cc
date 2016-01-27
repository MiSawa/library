#include <bits/stdc++.h>

template<typename ...Args>
static inline size_t fread_unlocked(const Args ...args){ return fread(args...); }
template<typename ...Args>
static inline char *fgets_unlocked(const Args ...args){ return fgets(args...); }
template<typename ...>
static inline void putchar_unlocked(const int c){ putchar(c); }
class {//{{{
    struct input_t {//{{{
        enum{ use_fgets = 0 }; // fgets or fread
        enum{ buf_size = 1 << 16 };
        enum { WS = 1<<0, EOL = 1<<1, DIGIT = 1<<2, };
        char *buf, *table;
        input_t() : buf(get_buf()), table(make_table()){}

        private:
        static inline char *make_table(){//{{{
            static_assert(std::numeric_limits<char>::max() < std::numeric_limits<int>::max(), "");
            static char table[1 << CHAR_BIT] = {};
            for(int c = 0; c < (1 << CHAR_BIT); ++c){
                if(isspace(c)) table[c] |= WS;
                if(isdigit(c)) table[c] |= DIGIT;
                if(c == '\r' or c == '\n') table[c] |= EOL;
            }
            return table;
        }//}}}
        static inline char *get_buf(){//{{{
            static char _buf[buf_size + 10] = {};
            return _buf;
        }//}}}

        inline void fetch(){//{{{
            if(feof_unlocked(stdin)){ buf = nullptr; return; }
            if(use_fgets){
                buf = fgets_unlocked(get_buf(), buf_size, stdin);
            }else{
                int t = fread_unlocked(static_cast<void *>(buf = get_buf()), sizeof(char), buf_size, stdin);
                buf[t] = '\0';
            }
        }//}}}
        inline void fetch_if_need(){//{{{
            if(*buf == '\0') fetch();
        }//}}}
        inline char peek(){//{{{
            fetch_if_need();
            return *buf;
        }//}}}
        inline int peek_type(){//{{{
            return table[peek()];
        }//}}}
        inline char getc(){//{{{
            fetch_if_need();
            return *buf++;
        }//}}}
        inline void skip_while(const int mask){//{{{
            for(; buf; ++buf){
                fetch_if_need();
                if(buf && !(table[*buf] & mask)) break;
            }
        }//}}}
        inline std::string read_until(const int mask){//{{{
            std::string res;
            for(; buf; fetch()){
                char *begin = buf;
                while(*buf != '\0'  and !(table[*buf] & mask)) ++buf;
                res.append(begin, buf);
                if(*buf != '\0') break;
            }
            return std::move(res);
        }//}}}
        inline std::string read_line(){//{{{
            std::string res = read_until(EOL);
            if(buf == nullptr) return std::move(res);
            char c = getc(); fetch_if_need();
            if(buf == nullptr) return std::move(res);
            if((peek_type() & EOL) && c != peek()) getc();
            return std::move(res);
        }//}}}
        inline void eat_eol(){//{{{
            if(buf == nullptr or !(peek_type() & EOL)) return;
            char c = getc(); fetch_if_need();
            if(buf == nullptr or !(peek_type() & EOL)) return;
            if(c != peek()) getc();
        }//}}}

        template<typename Int> inline void read_int(Int &x){//{{{
            char c; x = 0;
            for(c = getc(); c != '-' and !(table[c] & DIGIT); c = getc());
            if(c == '-') for(; peek_type() & DIGIT; x = x * 10 - (getc() - '0'));
            else for(x = c - '0'; peek_type() & DIGIT; x = x * 10 + (getc() - '0'));
        }//}}}
        template<typename Float> inline void read_float(Float &x){//{{{
            char c, neg = false; x = 0;
            for(c = peek(); c != '-' and !(table[c]&DIGIT) and c != '.'; c = peek())
                getc();
            if(c == '-'){ neg = true; getc(); }
            while(peek_type() & DIGIT) x = x * 10 + (getc() - '0');
            if(peek() == '.'){
                Float f = static_cast<Float>(0), s = static_cast<Float>(1);
                for(getc(); peek_type() & DIGIT; s *= 10) f = f * 10 + (getc() - '0');
                x += f / s;
            }
            if(neg) x = -x;
        }//}}}
        public:
        inline void read(char &c){ skip_while(WS); c = getc(); }
        inline void read(int &x){ read_int<int>(x); }
        inline void read(long long &x){ read_int<long long>(x); }
        inline void read(float &x){ read_float<float>(x); }
        inline void read(double &x){ read_float<double>(x); }
        inline void read(long double &x){ read_float<long double>(x); }
        inline void read(std::string &s){//{{{
            skip_while(WS);
            s = read_until(WS);
        }//}}}
        inline void readln(std::string &s){//{{{
            s = read_line();
        }//}}}
    };//}}}
    struct output_t {//{{{
        constexpr static size_t buf_size = 1 << 10;
        char *buf;
        output_t() : buf(get_buf()){}

        private:
        static inline char *get_buf(){//{{{
            static char _buf[buf_size];
            return _buf;
        }//}}}
        inline void putc(const int c){ putchar_unlocked(c); }
        template<typename Int>
        inline void write_int(Int x){//{{{
            int p = 0;
            if(x == 0){
                putc('0');
                return;
            }else if(x < 0){
                putc('-');
                for(; x; x /= 10) buf[p++] = -(x%10);
            }else for(; x; x /= 10) buf[p++] = x%10;
            while(p--) putc(buf[p] + '0');
        }//}}}
        template<typename Float>
        inline void write_float(Float x, const int prec = 20){//{{{
            if(x == 0){ putc('0'); return; }
            if(std::signbit(x)){ putc('-'); x = -x; }
            Float a, b = std::modf(x, &a);
            int p = 0;
            for(; a >= 1; a = std::trunc(a / 100000000))
                for(int t = std::fmod(a, 100000000), i = 0; i < 8; ++i, t /= 10)
                    buf[p++] = t % 10;
            buf[p++] = 0;
            while(p and !buf[p]) --p;
            while(p >= 0) putc(buf[p--] + '0');
            if(prec <= 0) return;
            putc('.');
            for(int i = 0; i <= prec; i += 8){
                b = std::modf(b * 100000000, &x);
                for(int t = std::trunc(x), j = 7; j >= 0; --j, t /= 10)
                    buf[i+j] = t % 10;
            }
            for(int i = 0; i < prec-1; ++i) putc(buf[i] + '0');
            putc(buf[prec-1] + '0' + (buf[prec] >= 5));
        }//}}}

        public:
        inline void write(const char c){ putc(c); }
        inline void write(const int x){ write_int<int>(x); }
        inline void write(const long long x){ write_int<long long>(x); }
        inline void write(const double x){ write_float<double>(x); }
        inline void write(const long double x){ write_float<long double>(x); }
        inline void write(const std::string &s){//{{{
            for(int i = 0; i < s.size(); ++i) putc(s[i]);
        }//}}}
    };//}}}

    static inline input_t &get_input(){ static input_t input; return input; }
    static inline output_t &get_output(){ static output_t output; return output; }

    using manip_t = std::ostream&(std::ostream&);

    public:
    inline std::string line(){ std::string s; get_input().readln(s); return s; }

    template<typename T>
    inline auto operator>>(T &x) -> decltype(get_input().read(x), *this)&{//{{{
        get_input().read(x);
        return *this;
    }//}}}

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

    inline int operator+(const int x){ return static_cast<int>(*this)+x; }
    inline int operator-(const int x){ return static_cast<int>(*this)-x; }
    inline long long operator+(const long long x){ return static_cast<long long>(*this)+x; }
    inline long long operator-(const long long x){ return static_cast<long long>(*this)-x; }
    inline int operator--(){ return static_cast<int>(*this)-1; }

    template<typename IO> struct ion_t {//{{{
        const int n;
        IO &io;
        ion_t(const int n, IO &io) : n(n), io(io){}
        template<typename T>
        inline auto operator>>(std::vector<T> &v) -> decltype(*this)&{//{{{
            v.resize(n);
            for(int i = 0; i < n; ++i) io >> v[i];
            return *this;
        }//}}}
        template<typename T>
        inline operator std::vector<T>(){//{{{
            std::vector<T> v(n);
            for(int i = 0; i < n; ++i) io >> v[i];
            return std::move(v);
        }//}}}
    };//}}}
    inline auto vec(const int n) -> ion_t<decltype(*this)> {//{{{
        return ion_t<decltype(*this)>(n, *this);
    }//}}}
} io;//}}}

using namespace std;

#define REP(i,b,n) for(int i=(int)(b);i<(int)(n);++i)
#define rep(i,n) REP(i,0,n)
signed main(){
    // int n = io, x;
    // long long sum = 0;
    // rep(_, n) sum += (int)io;
    // cout << sum << endl;

    double x = -M_PI*1E11;
    vector<int> a = io.vec(10), b = io.vec(10);
    io.vec(10) >> a >> b;
    io << x << endl;
    cout << fixed << setprecision(20) << x << endl;
    size_t y = io;
    // io << io.delim(" ") << a << endl;

    // rep(i, 100) io << -i << endl;
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
