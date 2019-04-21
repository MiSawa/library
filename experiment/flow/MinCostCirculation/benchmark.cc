#pragma once

#include <vector>
#include <functional>
#include <chrono>
#include <iomanip>

#include "utils.cc"
using namespace std;

template<class ...Args> struct Benchmark{//{{{
    using TaskFunction = function<void(Args...)>;
    struct Task{
        string description;
        TaskFunction task;
        Task(const string &description, TaskFunction task) : description(description), task(task) {}
        void operator()(const Args &...args){
            task(args...);
        }
    };

    vector<Task> tasks;
    size_t size() const { return tasks.size(); }
    void entry(const string &description, TaskFunction task){
        tasks.emplace_back(description, task);
    }
    vector<double> run(const string &description, const Args &...args){
        using Duration = std::chrono::duration<double, std::chrono::seconds::period>;
        vector<double> res;
        for(auto &task : tasks){
            auto start_point = std::chrono::system_clock::now();
            task(args...);
            auto end_point = std::chrono::system_clock::now();
            res.emplace_back(std::chrono::duration_cast<Duration>(end_point - start_point).count());
            cout << setw(10) << task.description << ": " << res.back() << endl;
        }
        return res;
    }
};//}}}


#if __INCLUDE_LEVEL__ == 0

#include <bits/stdc++.h>

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
int main(){
    Benchmark<int, int> bm;
    bm.entry("slow", [](int x, int y){
        int s = 0;
        for(int i = 0; i < 10000000; ++i) s += i;
            });
    bm.entry("fast", [](int x, int y){
        return x * y;
            });
    tr << bm.run(1, 2) << endl;
    tr << bm.run(3, 3) << endl;
}

#endif
// vim:set foldmethod=marker commentstring=//%s:
