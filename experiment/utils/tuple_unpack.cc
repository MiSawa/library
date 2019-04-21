#include <tuple>

// http://yak-ex.blogspot.jp/2011/12/forwardastuple.html
template<class ...Args>
class Arguments{//{{{
    template<std::size_t ...args> struct seq{};
    template<std::size_t n, class T> struct make_seq_impl{};
    template<std::size_t val, std::size_t ...args>
        struct make_seq_impl<val, seq<args...>>{
            using type = seq<val, args...>;
        };
    template<std::size_t b, std::size_t e> struct make_seq{
        using type = typename make_seq_impl<b,
              typename make_seq<b+1, e>::type>::type;
    };
    template<std::size_t b> struct make_seq<b, b>{
        using type = seq<>;
    };


    std::tuple<Args...> args;
    template<typename F, std::size_t ...indeces>
        auto call(F f, seq<indeces...>) -> decltype(f(std::get<indeces>(args)...)) {
            return f(std::get<indeces>(args)...);
        }

    public:

    Arguments(Args ...args) : args(std::forward_as_tuple(args...)){ }
    template<typename F>
        auto call(F f) -> decltype(call(f, std::declval<typename make_seq<0, std::tuple_size<std::tuple<Args...>>::value>::type>())) {
            using seq_type = typename make_seq<0, std::tuple_size<std::tuple<Args...>>::value>::type;
            return call(f, seq_type());
        }
};//}}}
