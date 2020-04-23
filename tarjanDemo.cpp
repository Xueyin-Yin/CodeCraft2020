#include <iostream>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <vector>
#include <stack>
using namespace std;
 
int n , m ;// n 个点，m 条边 
int cnt , K[1005] , num ;// cnt强连通分量 ，K表示每个节点所属的强连通分量  
int DFN[1005] , low[1005] ;
bool inS[1005] ;//vis表示是否访问，inS表示是否在栈中 
vector<int> G[1005] ;
stack<int> S ;
int from[2005] , to[2005] ;//存边 
bool L[1005] ;//是否有入度 
 
void init() {
	memset( inS , 0 , sizeof(inS) );
	memset( K , 0 , sizeof(K) );
	memset( DFN , 0 , sizeof(DFN) );
	memset( low , 0 , sizeof(low) );
	memset( from , 0 , sizeof(from) );
	memset( to , 0 , sizeof(to) );
	memset( L , 0 , sizeof(L) );
	cnt = 0 ;
	num = 0 ;
	while( !S.empty() )
		S.pop() ;
}
 
void Tarjan( int x ) {
	num ++ ;
	DFN[x] = low[x] = num ;
	inS[x] = 1 ;
	S.push( x ) ; 
	for( int i = 0 ; i < G[x].size() ; ++ i ) {
		int s = G[x][i] ;
		if( !DFN[s] ) {
			Tarjan( s );
			low[x] = min( low[x] , low[s] );
		}
		else if( inS[s] ) {
			low[x] = min( low[x] , DFN[s] );
		}
	}
	if( low[x] == DFN[x] ) {
		cnt ++ ;
		int y ;
		do{ 
			y = S.top() ;
			inS[y] = 0 ;
			S.pop() ;
			K[y] = cnt ;
		}while( y != x );
	}
	return ;
}
 
int main() {
	while( scanf("%d%d", &n, &m ) != EOF ) {
		init();
		for(int i = 1 ; i <= m ; ++ i ) {
			int a , b ;
			scanf("%d%d", &a , &b );
			G[a].push_back(b); 
			from[i] = a , to[i] = b ;
		}
		for(int i = 1 ; i <= n ; ++ i ) {
			num = 0 ;
			if( !DFN[i] )
				Tarjan( i );
		}
		printf("%d\n", cnt );
		for(int i = 1 ; i <= n ; ++ i )
			G[i].clear() ;
	}
	return 0;
}
