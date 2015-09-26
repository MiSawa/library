#include <array>
#include <cstdlib>
#include <iostream>

// http://www.colun.net/archives/547
// http://d.hatena.ne.jp/wocota/20090628/1246188338

#define BEGIN_STACK_EXTEND(size) void * stack_extend_memory_ = malloc(size);void * stack_extend_origin_memory_;char * stack_extend_dummy_memory_ = (char*)alloca((1+(int)(((long long)stack_extend_memory_)&127))*16);*stack_extend_dummy_memory_ = 0;asm volatile("mov %%rsp, %%rbx\nmov %%rax, %%rsp":"=b"(stack_extend_origin_memory_):"a"((char*)stack_extend_memory_+(size)-1024));

#define END_STACK_EXTEND asm volatile("mov %%rax, %%rsp"::"a"(stack_extend_origin_memory_));free(stack_extend_memory_);

using namespace std;

using big_data = array<char, 1000000>;
int f(int i, big_data d){
    if(i == 0) return 0;
    int res = d[10];
    f(i-1, d);
    res += d[10];
    return res;
}

bool solve(){
    BEGIN_STACK_EXTEND(128 * 1024 * 1024);

    big_data d;
    fill(begin(d), end(d), 1);
    cout << f(10, d) << endl;

    END_STACK_EXTEND;
    return true;
}
signed main(){
    solve();
    return 0;
}
// vim:set foldmethod=marker commentstring=//%s:
