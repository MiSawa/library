#include <iostream>
#include <cassert>
#include <limits>
#include <sstream>
#include <tuple>
#include <functional>
#include <algorithm>
#include <array>

namespace input_validator{//{{{
    class InputStream{//{{{

        private:

            bool is_valid(){//{{{
                is.peek();
                return (is.rdstate() & (std::istream::failbit | std::istream::badbit)) == 0;
            }//}}}

            bool has_next(){//{{{
                is.peek();
                return is_valid() and !is.eof();
            }//}}}

            char peek(){//{{{
                assert("peek()" && has_next());
                return static_cast<char>(is.peek());
            }//}}}

        public:

            explicit InputStream(std::istream &is) : is(is) {}

            ~InputStream(){//{{{
                is.peek();
                assert("eof" && is.eof());
            }//}}}

            void eat(const char c){//{{{
                assert("eat()" && (next_char() == c));
            }//}}}

            void eat_eol(){//{{{
                eat('\n');
            }//}}}

            void eat_space(){//{{{
                eat(' ');
            }//}}}

            char next_char(){//{{{
                assert("next_char()" && has_next());
                char c; is.get(c);
                return c;
            }//}}}

            std::string next_line(){//{{{
                std::string res;
                while(peek() != '\n') res += next_char();
                return res;
            }//}}}

            std::string next_string(){//{{{
                std::string res;
                while(peek() != ' ' and peek() != '\n') res += next_char();
                return res;
            }//}}}

            template<class Int>
                typename std::enable_if<std::is_integral<Int>::value && !std::is_same<char, Int>::value && !std::is_same<signed char, Int>::value && !std::is_same<unsigned char, Int>::value, Int>::type
                next_int(){//{{{
                    std::string s;
                    if(peek() == '-') s += next_char();
                    while(has_next() and '0' <= peek() and peek() <= '9') s += next_char();
                    // ""
                    assert("read_int" && !s.empty());
                    // "-", "-0", "-01"
                    assert("read_int" && (s[0] != '-' or (s.size() > 1 and s[1] != '0')));
                    if(s[0] == '-') assert("read_int" && std::numeric_limits<Int>::is_signed);
                    // "01"
                    assert("read_int" && (s[0] != '0' or s.size() == 1));

                    if(s[0] == '-'){
                        s = s.substr(1);
                        std::string minimum;
                        {
                            std::stringstream ss; ss << std::numeric_limits<Int>::min();
                            assert("read_int" && ss.str().front() == '-');
                            minimum = ss.str().substr(1);
                        }
                        assert("read_int" && (s.size() <= minimum.size()));
                        s = std::string(minimum.size() - s.size(), '0') + s;
                        assert("read_int" && (s <= minimum));
                        Int res = 0;
                        for(int i = 0; i < s.size(); ++i) res = (res * 10) - (s[i] - '0');
                        return res;
                    }else{
                        std::string maximum;
                        {
                            std::stringstream ss; ss << std::numeric_limits<Int>::max();
                            maximum = ss.str();
                        }
                        assert("read_int" && (s.size() <= maximum.size()));
                        s = std::string(maximum.size() - s.size(), '0') + s;
                        assert("read_int" && (s <= maximum));
                        Int res = 0;
                        for(int i = 0; i < s.size(); ++i) res = (res * 10) + (s[i] - '0');
                        return res;
                    }
                }//}}}

        private:

            std::istream &is;
    };//}}}

    template<class T> class ConstraintsReader{//{{{
        T &val;
        std::function<T(InputStream &)> reader;
        std::function<bool(T)> pred;
        std::string error_msg;

        void read(InputStream &is){//{{{
            val = reader(is);
            if(pred(val)) return;
            if(!error_msg.empty()) std::cerr << error_msg << std::endl;
            assert(false);
        }//}}}

        public:
        ConstraintsReader(T &val, std::function<T(InputStream &)> reader, std::function<bool(T)> pred, std::string error_msg = "") : val(val), reader(reader), pred(pred), error_msg(error_msg) {}

        friend InputStream &operator>>(InputStream &is, ConstraintsReader c){//{{{
            c.read(is);
            return is;
        }//}}}
    };//}}}

    class Eater{//{{{
        char val;
        ConstraintsReader<char> cr;
        public:
        Eater(const char ch, std::string error_msg)
            : val(),
              cr(val,
                [](InputStream &is){ return is.next_char(); },
                [ch](char c){ return c == ch; },
                error_msg) {}
        friend InputStream &operator>>(InputStream &is, Eater &e){//{{{
            is >> e.cr;
            return is;
        }//}}}
    };//}}}

    template<class Int> ConstraintsReader<Int> read(Int &x, Int lower, Int upper, const std::string var_name = ""){//{{{
        std::stringstream ss;
        ss << var_name << " not in [" << lower << ", " << upper << "]";
        return ConstraintsReader<Int>(
                x,
                [](InputStream &is){ return is.next_int<Int>(); },
                [lower, upper](Int in){ return lower <= in and in <= upper; },
                ss.str());
    }//}}}
    const auto pow10i = [](){//{{{
        std::array<int, 10> res;
        res[0] = 1;
        for(size_t i = 1; i < res.size(); ++i) res[i] = res[i-1] * 10;
        return res;
    }();//}}}
    const auto pow10ll = [](){//{{{
        std::array<long long int, 19> res;
        res[0] = 1;
        for(size_t i = 1; i < res.size(); ++i) res[i] = res[i-1] * 10;
        return res;
    }();//}}}

    class Alphabet{//{{{
        const std::string alpha;
        public:

        Alphabet(const std::string alpha) : alpha(alpha) {}
        Alphabet(const char *alpha) : alpha(alpha) {}
        friend Alphabet operator|(const Alphabet a, const Alphabet b){
            return Alphabet(a.alpha + b.alpha);
        }

        bool include(const char c) const {
            return alpha.find(c) != std::string::npos;
        }
        std::string get_alphabet() const { return alpha; }

        static const Alphabet lower, upper, digit;
    };//}}}

    const Alphabet Alphabet::lower("abcdefghijklmnopqrstuvwxyz");
    const Alphabet Alphabet::upper("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    const Alphabet Alphabet::digit("0123456789");

    ConstraintsReader<char> read(char &c, const Alphabet &alphabet, const std::string var_name = ""){//{{{
        std::stringstream ss;
        ss << var_name << " not in \"" << alphabet.get_alphabet() << "\"";
        return ConstraintsReader<char>(
                c,
                [](InputStream &is){ return is.next_char(); },
                [alphabet](char c){ return alphabet.include(c); },
                ss.str());
    }//}}}

    ConstraintsReader<std::string> read(std::string &s, const Alphabet &alphabet, const std::string var_name = ""){//{{{
        std::stringstream ss;
        ss << var_name << " not in \"" << alphabet.get_alphabet() << "\"*";
        return ConstraintsReader<std::string>(
                s,
                [](InputStream &is){ return is.next_string(); },
                [alphabet](std::string s){
                    return std::all_of(s.begin(), s.end(), [alphabet](char c){
                            return alphabet.include(c); });
                    },
                ss.str());
    }//}}}
    ConstraintsReader<std::string> read(std::string &s, size_t len_lower, size_t len_upper, const Alphabet &alphabet, const std::string var_name = ""){//{{{
        std::stringstream ss;
        ss << var_name << " not in \"" << alphabet.get_alphabet() << "\"*";
        return ConstraintsReader<std::string>(
                s,
                [](InputStream &is){ return is.next_string(); },
                [len_lower, len_upper, alphabet](std::string s){
                    return len_lower <= s.size() and s.size() <= len_upper and std::all_of(s.begin(), s.end(), [alphabet](char c){
                            return alphabet.include(c); });
                    },
                ss.str());
    }//}}}

    Eater eat_space(' ', "expect: space");
    Eater eat_eol('\n', "expect: eol");
}//}}}

using namespace std;
using namespace input_validator;

int main(){

    InputStream in(cin);
    string s;
    in >> read(s, Alphabet::lower, "s");
    cout << s << endl;
    in >> eat_eol;
    return 0;
}
// vim:set foldmethod=marker commentstring=//%s:
