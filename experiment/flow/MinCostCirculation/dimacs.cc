#pragma once

#include <istream>
#include <vector>
#include <cassert>
#include <sstream>
#include <tuple>

#include "utils.cc"

using namespace std;

struct Instance{//{{{
    using Flow = int32_t;
    using Cost = int32_t;

    enum struct ProblemType : char {//{{{
        MaxFlow,
        MinCostFlow,
    };//}}}

    struct Node{//{{{
        size_t id;
        Flow flow;
        Node(const size_t id, const Flow flow) : id(id), flow(flow) {}
    };//}}}
    struct Arc{//{{{
        size_t src, dst;
        Flow low, cap;
        Cost cost;
        Arc(const size_t src, const size_t dst, const Flow low, const Flow cap, const Cost cost) : src(src), dst(dst), low(low), cap(cap), cost(cost) {}
    };//}}}
    ProblemType problem_type;
    size_t n, m;
    vector<Node> nodes;
    vector<Arc> arcs;
    vector<string> comments;

    private:
    void clear(){//{{{
        *this = remove_reference<decltype(*this)>::type();
    }//}}}

    void read(istream &is){//{{{
        clear();
        string line;
        while(getline(is, line) && line[0] == 'c'); // skip comments
        // the line must be the problem line.

        { // read problem line
            assert(line[0] == 'p');
            stringstream ss(line.substr(1));
            string type_s; ss >> type_s;
            if(type_s == "min"){
                this->problem_type = ProblemType::MinCostFlow;
                ss >> this->n >> this->m;
            }else if(type_s == "max"){
                this->problem_type = ProblemType::MaxFlow;
                ss >> this->n >> this->m;
            }else{
                assert(false && "unknown problem type.");
            }
        }
        while(getline(is, line)){
            if(line.empty()) continue;
            if(line[0] == 'c') continue; // slip comments
            if(line[0] == 'a'){
                char c;
                size_t src, dst;
                Flow low = 0, cap = 0;
                Cost cost = 0;
                if(this->problem_type == ProblemType::MaxFlow){
                    tie(c, src, dst, cap) = utils::string_to_tuple<char, size_t, size_t, Flow>(line);
                }else if(this->problem_type == ProblemType::MinCostFlow){
                    tie(c, src, dst, low, cap, cost) = utils::string_to_tuple<char, size_t, size_t, Flow, Flow, Cost>(line);
                }
                arcs.emplace_back(src - 1, dst - 1, low, cap, cost);
            }else if(line[0] == 'n'){
                char c;
                size_t id;
                Flow flow;
                if(this->problem_type == ProblemType::MaxFlow){
                    char st;
                    tie(c, id, st) = utils::string_to_tuple<char, size_t, char>(line);
                    assert(st == 's' or st == 't');
                    flow = (st == 's' ? +1 : -1);
                }else if(this->problem_type == ProblemType::MinCostFlow){
                    tie(c, id, flow) = utils::string_to_tuple<char, size_t, Flow>(line);
                }
                nodes.emplace_back(id - 1, flow);
            }else{
                assert(false && "unknown type line.");
            }
        }
    }//}}}

    public:
    friend istream &operator>>(istream &is, Instance &instance){
        instance.read(is);
    }
};//}}}

#if __INCLUDE_LEVEL__ == 0

#include <bits/stdc++.h>
int main(){
    Instance instance;
    cin >> instance;
    for(auto &v : instance.nodes){
        cout << v.id << " " << v.flow << endl;
    }
    for(auto &e : instance.arcs){
        cout << e.src << " " << e.dst << " " << e.low << " " << e.cap << " " << e.cost << endl;
    }
    return 0;
}
#endif

// vim:set foldmethod=marker commentstring=//%s:
