#if __INCLUDE_LEVEL__ == 0
#include <iostream>
using namespace std;
#else
#pragma once
#endif

#define label(blockname) goto blockname; \
                         blockname##_skip: if (0) \
                         blockname:

#define break(blockname) goto blockname##_skip


#if __INCLUDE_LEVEL__ == 0
signed main(void){
    int s = 0;
    const int n = 10;
    label(outer) for(int i = 0; i < n; ++i){
        label(inner) for(int j = 0; j < n; ++j){
            cout << i << " " << j << endl;
            if(j == 5) break(outer);
        }
    }
    cout << s << endl;
    return 0;
}
#endif
// vim:set foldmethod=marker commentstring=//%s:
