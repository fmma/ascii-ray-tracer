#include <math.h>
#include <algorithm>
#include <string>
#include <immintrin.h>
using namespace std;typedef float R;
#define _W 79
#define _H 39
#define EP 0.01f
#define OP operator
#define C const
#define E return
#define PQ M*(3.1415f/180)
union J{__m128 V;J(__m128 V):V(V){}J(R X,R Y,R Z):X(X),Y(Y),Z(Z){}struct{R X,Y,Z;};R L()C{E _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(V,V,0x71)));}J N()C{E _mm_mul_ps(V,_mm_rsqrt_ps(_mm_dp_ps(V,V,0x7f)));}R D(C J& O)C{E _mm_cvtss_f32(_mm_dp_ps(V,O.V,0x71));}J A()C{E J{abs(X),abs(Y),abs(Z)};}J OP+(C J& O)C{E _mm_add_ps(V,O.V);}J OP-(C J& O)C{E _mm_sub_ps(V,O.V);}J OP*(C J& O)C{E _mm_mul_ps(V,O.V);}J OP/(C J& O)C{E _mm_div_ps(V,O.V);}J OP+(C R& O)C{E _mm_add_ps(V,_mm_set1_ps(O));}J OP-(C R& O)C{E _mm_sub_ps(V,_mm_set1_ps(O));}J OP*(C R& O)C{E J{_mm_mul_ps(V,_mm_set1_ps(O))};}};R BX(C J& P,J B){J D=P.A()-B;E min(max(D.X,max(D.Y, D.Z)),.0f)+J{max(D.X,.0f),max(D.Y,.0f),max(D.Z,0.0f)}.L();}R RB(C J& P,J B,R R){E BX(P,B)-R;}J RY(C J& P,R A){E J{P.Z*sin(A)+P.X*cos(A),P.Y,P.Z*cos(A)-P.X*sin(A),};}J RZ(C J& P,R A){E J{P.X*cos(A)-P.Y*sin(A),P.X*sin(A)+P.Y*cos(A),P.Z,};}static size_t M=0;R Q(C J& P){R D=P.Y+2;D=min(D,RB(RY(RZ(P,PQ),PQ),J{2,2,2},0.5f));E D;}J Nor(C J& P){E J{Q(P+J{EP,0,0})-Q(P-J{EP,0,0}),Q(P+J{0,EP,0})-Q(P-J{0,EP,0}),Q(P+J{0,0,EP})-Q(P-J{0,0,EP}),}.N();}R Y(C J& O,C J& D,bool* H){R T=0;for(size_t i=0;i<128;i++){R Y=Q(O+(D*T));if(Y<EP){*H=1;break;}T+=Y;}E T;}R B(C J& O,C J& D,R U,R I,R K){R S=1;for(R t=U;t<I;){R di=Q(O+D*t);if(di<EP){E 0;}S=min(S,K*di/t);t+=di;}E S;}int main(){J LD=J{0.5,1,0.25}.N();J G=J{0,1,-8};string S;S.reserve(_W*_H);do{M++;for(size_t y=0;y<_H;y++){for(size_t x=0;x<_W;x++){J K(x,y,1);K=K/J(_W,_H,1);K=(K*2.0)-1;K.Y=-K.Y;K.X*=_W/_H;K.Z=R(1/0.726);K=K.N();bool H=0;R Z=Y(G,K,&H);J P=G+(K*Z);if(H){J N=Nor(P);R D=N.D(LD);D*=0.5;D+=0.5;D*=D;D*=B(P,LD,R(0.5),10,10);S+=".:*oe$&#%@"[static_cast<size_t>(D*10)];}else{S+=' ';}}S+="\n";}printf("%s",S.c_str());S.clear();}while(M<360);E 0;}
