#include <iostream>

using namespace std;

void prt(int x){ cout << x << endl; }
void prt(long long x){ cout << 0 << endl; }
int main(){
    prt(-2147483648);
    return 0;
}
// vim:set foldmethod=marker commentstring=//%s:
