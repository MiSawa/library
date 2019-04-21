#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<string.h>
#include<string>
#include<vector>
#include<set>
#include<map>
#include<queue>
#include<algorithm>
#include<utility>
#include<complex>
#include<float.h>
#include<limits.h>
#include<list>
using namespace std;

// (SCALE,WL,SINGLEFLOW) = 0, 0/1, 0 が普通最速, 0, 1, 1 が割と安全
#define SCALE      0
#define WL         1
#define SINGLEFLOW 1

#define OPT        1

/****************************************************************

    short: short版


****************************************************************/

#if OPT == 0

template<class V, class COST>
class edge{
public:
    int i, j;
    COST cost2;
    V flow, cap;
    int dest( int f){ return i == f ?
        j          : i;         }
    V residue(int f){ return i == f ?
#if SCALE
        INT_MAX    : flow;      }
#else
        cap - flow : flow;      }
#endif
    COST cost(int f){ return i == f ?
        cost2      : -cost2;    }
    void flow_add( int f, V r ){ i == f ?
        flow += r  : flow -= r; }
};

template<class V, class COST>
pair<bool, COST>
mincost(
int n,
const V *b0, V *b,
const vector< vector< int > > &adj,
edge<V,COST> *e, int m,
COST inf,

// バッファ領域; 各々 n 要素分確保すること
COST *d, COST *dr, COST *p,
int *prev, int *visit
)
{
    int n1 = n - 1;

// initial equibilium flow and potential
    for( int i = n1; i >= 0; -- i ){
        b[i] = b0[i];
        p[i] = 0;
    }
    for( int i = m - 1; i >= 0; -- i ){
        if( e[i].cost2 < 0 ){
            V r = e[i].cap;
            e[i].flow = r;
            b[e[i].i] -= r;
            b[e[i].j] += r;
        }
        else
            e[i].flow = 0;
    }

    V delta = 1;

#if SCALE
    V bmax = 0;
    for( int i = n1; i >= 0; -- i ){
        if( b[i] < 0 )
            bmax = max( bmax, - b[i] );
        else
            bmax = max( bmax, + b[i] );
    }
    while( delta <= (bmax>>1) ) delta <<= 1;
#endif

    for( ; delta >= 1; delta >>= 1 ){
    while( 1 ){
        priority_queue< pair<COST, int> > wl;

        for( int i = n1; i >= 0; -- i ){
            d[i] = inf;
            if( b[i] >= delta ){
                wl.push( pair<COST, int>( -0, i) );
                d[i] = 0;
            }
        }
        if( wl.empty() )
            break;
        memset( prev,  0xff, sizeof(int)*n );
        memset( visit, 0x00, sizeof(int)*n );

        int t = -1;
        while( !wl.empty() ){
            int i   =   wl.top().second;
            COST di = - wl.top().first;
            wl.pop();
            if( visit[i] )
                continue;
            COST dbase = di + p[i];
            dr[i]      = di;
            d[i]       = inf;
            visit[i] = 1;

            for( vector<int>::const_iterator
            J=adj[i].begin(), JE=adj[i].end();
            J != JE; ++ J ){
                edge<V,COST> &est = e[*J];
                int j = est.dest( i );
                if( visit[j] || est.residue(i)<=0 )
                    continue;
                COST dj = dbase+est.cost(i)-p[j];
                if( d[j] > dj ){
                    d[j] = dj;
                    prev[j] = *J;
                    wl.push(pair<COST,int>(-dj, j));
                }
            }
        } // dij end

        for( int i = n1; i >= 0; -- i )
            if( visit[i] ) p[i] += dr[i];

        for( int t2 = n1; t2 >= 0; -- t2 ){
            if( !visit[t2] || b[t2] > -delta )
                continue;
            t = t2;
            int i, j;

//--------
            V r = delta;
            for( i = t; (j = prev[i]) >= 0; ){
                edge<V,COST> &est = e[j];
                i = est.dest( i );
                r = min( r, est.residue( i ) );
            }
            r = min( r, b[i] );
            if( r < delta )
                continue;

//--------
// r だけ流す
            for( i = t; (j = prev[i]) >= 0; ){
                edge<V,COST> &est = e[j];
                i = est.dest( i );
                est.flow_add( i, r );
            }
            b[t]  += r;
            b[i]  -= r;
        }

        if ( t == -1 ) // no feasible flow
            break;

    }}

// コスト総計を計算
    COST cost_sum = 0;
    bool is_b = true;
    for( int i = m - 1; i >= 0; -- i )
        cost_sum += e[i].cost2 * e[i].flow;
    for( int i = n - 1; i >= 0; -- i ){
        if( b[i] != 0 ){
            is_b = 0;
            break;
        }
    }

    return pair<bool, COST>(is_b, cost_sum);
}
#endif
