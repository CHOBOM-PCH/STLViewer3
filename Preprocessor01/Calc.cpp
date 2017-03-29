#include "StdAfx.h"
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <conio.h>
#include <math.h>
#include "stdafx.h"

#include "Calc.h"
#include "Mainfrm.h"

extern int add_count, del_count;


#define M_MAX(a,b) ((a>=b) ? a:b);
#define M_MIN(a,b) ((a<=b) ? a:b);

//double VSCALE = 10.0;
//double line_width = 0.0015;       // 0.001 = 1u -> 0.0015      0.0001 = 100n  0.00001 = 10n
extern double VSCALE;
extern double line_width;
extern double line_gap;

//extern double mindist;

double mindist2 = 0.5;

CCalc::CCalc(void)
{
	Solid = NULL;
	Slice = NULL;      //
	DepartH = NULL;
	ScanH = NULL;
	
	Solid =	new CSolid;     // 처음
	
	ES = 0.00000000001;

	max_size=1;				// 오토 스케일링을 위한 변수
	hole_count = 0, normal_count = 0, nonmanifold_count = 0;
	pParent=NULL;			//부모 윈도우의 주소
	
	stl_number=0;			//stl 패치의 갯수
	face_number=0;			//face의 갯수

	
	pParent=NULL;			//부모 윈도우의 주소
	position = 1;

	SetDefaultColor();

	IntersecSave[0].x = Intersec[0].x = -250;
	IntersecSave[0].y = Intersec[0].y = -250;
	IntersecSave[0].z = Intersec[0].z = 0.0;
	IntersecSave[1].x = Intersec[1].x = 250;
	IntersecSave[1].y = Intersec[1].y = -250;
	IntersecSave[1].z = Intersec[1].z = 0.0;
	IntersecSave[2].x = Intersec[2].x = 250;
	IntersecSave[2].y = Intersec[2].y = 250;
	IntersecSave[2].z = Intersec[2].z = 0.0;
	IntersecSave[3].x = Intersec[3].x = -250;
	IntersecSave[3].y = Intersec[3].y = 250;
	IntersecSave[3].z = Intersec[3].z = 0.0;

	seg_counter = 1;
	gsel = 0;
}


CCalc::~CCalc(void)
{
	if (Solid) ClearNode((CNode*&)Solid, 1);
	if (Slice) ClearNode((CNode*&)Slice, 2);
	if (ScanH) ClearNode((CNode*&)ScanH, 3);        // CHalfedge = 3
	if (DepartH) ClearNode((CNode*&)DepartH, 3);    // CHalfedge = 3
}


void CCalc::SetDefaultColor(void)
{
	color_back[0]=0.92f, color_back[1]=0.9f, color_back[2]=0.95f, color_back[3]=1.0f;	// mod_Antique white
	color_solid[0]=0.3f,color_solid[1]=0.5f,color_solid[2]=0.7f,color_solid[3]=0.5f;
	color_select[0]=0.5f, color_select[1]=0.7f, color_select[2]=0.7f, color_select[3]=1.0f;
	color_select3[0]=0.3f, color_select3[1]=0.5f, color_select3[2]=0.7f, color_select3[3]=0.5f;
	color_select1[0]=0.0f, color_select1[1]=1.0f, color_select1[2]=0.0f, color_select1[3]=0.5f;
	color_select2[0]=0.8f, color_select2[1]=0.8f, color_select2[2]=0.0f, color_select2[3]=0.5f;
	color_net[0]=0.3f,color_net[1]=0.3f,color_net[2]=0.3f,color_net[3]=0.4f;
	color_line[0]=0.8f, color_line[1]=0.0f, color_line[2]=0.0f, color_line[3]=0.5f;
	
	//color_x[0]=1.0f,color_x[1]=0.0f,color_x[2]=0.0f,color_x[3]=1.0f;
	//color_y[0]=0.0f,color_y[1]=1.0f,color_y[2]=0.0f,color_y[3]=1.0f;
	//color_z[0]=0.6f,color_z[1]=0.7f,color_z[2]=1.0f,color_z[3]=1.0f;
	
	//color_back[0]=0.0, color_back[1]=0.3, color_back[2]=0.5, color_back[3]=1.0;
	
	color_layout[0]=0.6f,color_layout[1]=0.4f,color_layout[2]=0.8f,color_layout[3]=0.1f;
	color_cli[0]=1.0f,color_cli[1]=0.2f,color_cli[2]=0.2f,color_cli[3]=1.0f;
	color_int[0]=0.3f,color_int[1]=0.06f,color_int[2]=0.05f,color_int[3]=1.0f;

	ambient[0]=0.4f,ambient[1]=0.4f,ambient[2]=0.4f,ambient[3]=1.0;
	diffuse[0]=0.1f,diffuse[1]=0.1f,diffuse[2]=0.1f,diffuse[3]=1.0;
	specular[0]=0.2f,specular[1]=0.2f,specular[2]=0.2f,specular[3]=0.0 ;

	position0[0]=100.0f,position0[1]=100.0f,position0[2]=100.0f,position0[3]=0.0 ;
	position1[0]=100.0f,position1[1]=0.0f,  position1[2]=50.0f,  position1[3]=0.0 ;
	position2[0]=0.0f,  position2[1]=0.0f,  position2[2]=100.0f,position2[3]=0.0 ;

	emission[0]=0.0f,emission[1]=0.0f,emission[2]=0.0,emission[3]=1.0 ;
	shininess[0] = 120.0f;
}

/*******************************************************
	두개의 실수가 같은값인지 판별한다
	오차 범위 이내의 값은 같은 것으로 판단한다
*******************************************************/
bool CCalc::IsEqual(double a, double b)
{
	double c = a-b;
	if (c<ES && (c>(-ES))) return true;
	else return false;
}

double CCalc::RoundOff(double v)
{
	double k = fmod(v,ES);
	return (k > ES/2) ? v+ES-k : v-k;
}

/*******************************************************
	모델의 최대 최소 좌표값을 구하고 저장한다
*******************************************************/
void CCalc::SetMinMax()
{
	if (Solid == NULL) return;
	GoFirst((CNode*&)Solid);
	if (Solid) {
	while (1) {
		Solid->Min[0] = 999999;
		Solid->Min[1] = 999999;
		Solid->Min[2] = 999999;
		Solid->Max[0] = -999999;
		Solid->Max[1] = -999999;
		Solid->Max[2] = -999999;
		GoFirst((CNode*&)Solid->Vertex);
		if (Solid->Vertex) {
			while (1) {
				if (Solid->Vertex->x < Solid->Min[0])  Solid->Min[0] = Solid->Vertex->x;
				if (Solid->Vertex->x > Solid->Max[0])  Solid->Max[0] = Solid->Vertex->x;
				if (Solid->Vertex->y < Solid->Min[1])  Solid->Min[1] = Solid->Vertex->y;
				if (Solid->Vertex->y > Solid->Max[1])  Solid->Max[1] = Solid->Vertex->y;
 				if (Solid->Vertex->z < Solid->Min[2])  Solid->Min[2] = Solid->Vertex->z;
				if (Solid->Vertex->z > Solid->Max[2])  Solid->Max[2] = Solid->Vertex->z;
				if (Solid->Vertex->nxt == NULL) break;
				else Solid->Vertex = (CVertex*)Solid->Vertex->nxt;
			}
		} else {
			GoFirst((CNode*&)Solid->Face);
			if (Solid->Face) while (1) {
				if (Solid->Face->v0->x < Solid->Min[0])  Solid->Min[0] = Solid->Face->v0->x;
				if (Solid->Face->v0->x > Solid->Max[0])  Solid->Max[0] = Solid->Face->v0->x;
				if (Solid->Face->v0->y < Solid->Min[1])  Solid->Min[1] = Solid->Face->v0->y;
				if (Solid->Face->v0->y > Solid->Max[1])  Solid->Max[1] = Solid->Face->v0->y;
 				if (Solid->Face->v0->z < Solid->Min[2])  Solid->Min[2] = Solid->Face->v0->z;
				if (Solid->Face->v0->z > Solid->Max[2])  Solid->Max[2] = Solid->Face->v0->z;

				if (Solid->Face->v1->x < Solid->Min[0])  Solid->Min[0] = Solid->Face->v1->x;
				if (Solid->Face->v1->x > Solid->Max[0])  Solid->Max[0] = Solid->Face->v1->x;
				if (Solid->Face->v1->y < Solid->Min[1])  Solid->Min[1] = Solid->Face->v1->y;
				if (Solid->Face->v1->y > Solid->Max[1])  Solid->Max[1] = Solid->Face->v1->y;
 				if (Solid->Face->v1->z < Solid->Min[2])  Solid->Min[2] = Solid->Face->v1->z;
				if (Solid->Face->v1->z > Solid->Max[2])  Solid->Max[2] = Solid->Face->v1->z;

				if (Solid->Face->v2->x < Solid->Min[0])  Solid->Min[0] = Solid->Face->v2->x;
				if (Solid->Face->v2->x > Solid->Max[0])  Solid->Max[0] = Solid->Face->v2->x;
				if (Solid->Face->v2->y < Solid->Min[1])  Solid->Min[1] = Solid->Face->v2->y;
				if (Solid->Face->v2->y > Solid->Max[1])  Solid->Max[1] = Solid->Face->v2->y;
 				if (Solid->Face->v2->z < Solid->Min[2])  Solid->Min[2] = Solid->Face->v2->z;
				if (Solid->Face->v2->z > Solid->Max[2])  Solid->Max[2] = Solid->Face->v2->z;

				if (Solid->Face->nxt == NULL) break;
				else Solid->Face = (CFace*)Solid->Face->nxt;
			}
		}
		if (Solid->nxt == NULL) break;
		else Solid = (CSolid*)Solid->nxt;
	}
	}

	Min[0] =  999999;
	Min[1] =  999999;
	Min[2] =  999999;
	Max[0] = -999999;
	Max[1] = -999999;
	Max[2] = -999999;
	GoFirst((CNode*&)Solid);
	if (Solid) while (1) {
		/* if (Solid->IS_SELECTED) */ {
			if (Solid->Min[0] < Min[0]) Min[0] = Solid->Min[0];
			if (Solid->Min[1] < Min[1]) Min[1] = Solid->Min[1];
			if (Solid->Min[2] < Min[2]) Min[2] = Solid->Min[2];
			if (Solid->Max[0] > Max[0]) Max[0] = Solid->Max[0];
			if (Solid->Max[1] > Max[1]) Max[1] = Solid->Max[1];
			if (Solid->Max[2] > Max[2]) Max[2] = Solid->Max[2];
		}
		if (Solid->nxt == NULL) break;
		else Solid = (CSolid*)Solid->nxt;
	}
	max_size = Max[0]-Min[0];
	if (max_size < Max[1]-Min[1]) max_size = Max[1]-Min[1];
	if (max_size < Max[2]-Min[2]) max_size = Max[2]-Min[2];

	//ES = max_size/1000000;
	if (ES<0.00001)  ES = 0.00001;
//	if (ES>0.00001)  ES = 0.00001;
	IntersecSave[0].x = Intersec[0].x = Min[0]-5;
	IntersecSave[0].y = Intersec[0].y = Min[1]-5;
	IntersecSave[0].z = Intersec[0].z = 0.0;
	IntersecSave[1].x = Intersec[1].x = Max[0]+5;
	IntersecSave[1].y = Intersec[1].y = Min[1]-5;
	IntersecSave[1].z = Intersec[1].z = 0.0;
	IntersecSave[2].x = Intersec[2].x = Max[0]+5;
	IntersecSave[2].y = Intersec[2].y = Max[1]+5;
	IntersecSave[2].z = Intersec[2].z = 0.0;
	IntersecSave[3].x = Intersec[3].x = Min[0]-5;
	IntersecSave[3].y = Intersec[3].y = Max[1]+5;
	IntersecSave[3].z = Intersec[3].z = 0.0;
	
	//printf("%f %f %f %f %f %f\n", Solid->Min[0], Solid->Min[1], Solid->Min[2], Solid->Max[0], Solid->Max[1], Solid->Max[2]);
}

/*********************************************************
	모델의 가장 긴 길이를 구한다.
*********************************************************/
void CCalc::AutoScale()
{
	double lx,ly,lz;
	double size=-1000;

	lx=Max[0]-Min[0], ly=Max[1]-Min[1], lz=Max[2]-Min[2];
	if (lx>size) size=lx;
	if (ly>size) size=ly;
	if (lz>size) size=lz;
	
	max_size=size; //max_size는 CCalc의 멤버 변수입니다.
}

/*************************************************************************
	3차원 Vertex Table 에서 해당점(x,y,z)를 찾아 리턴한다
	없을 경우 NULL을 리턴
**************************************************************************/
CVertex* CCalc::FindVertex(CVertexIndex *vTable[VTABLE_SIZE][VTABLE_SIZE][VTABLE_SIZE], CVertex * Vertex, double x, double y, double z,double min[3],double max[3])
{
	if (Vertex==NULL) return NULL;

	int	p=int(((x-min[0])*VTABLE_SIZE)/(max[0]-min[0]));
	int	q=int(((y-min[1])*VTABLE_SIZE)/(max[1]-min[1]));
	int	r=int(((z-min[2])*VTABLE_SIZE)/(max[2]-min[2]));
	
	int i,j,k,i0,i1,j0,j1,k0,k1;

	if (p<0) p = 0;
	else if (p>(VTABLE_SIZE-1)) p = (VTABLE_SIZE-1);
	if (q<0) q = 0;
	else if (q>(VTABLE_SIZE-1)) q = (VTABLE_SIZE-1);
	if (r<0) r = 0;
	else if (r>(VTABLE_SIZE-1)) r = (VTABLE_SIZE-1);

	if (p==(VTABLE_SIZE-1)) {// 둘러싼 곳 모두 체크
		i0=j0=k0=-1;
		i1=j1=k1=0;
	} else if (p==0) {
		i0=j0=k0=0;
		i1=j1=k1=1;
	} else {
		i0=j0=k0=-1;
		i1=j1=k1=1;
	}


	for (i=i0;i<=i1;i++) {
	for (j=j0;j<=j1;j++) {
	for (k=k0;k<=k1;k++) {

		if (p+i>-1 && q+j>-1 && r+k>-1 && 
			p+i<VTABLE_SIZE && q+j<VTABLE_SIZE && r+k<VTABLE_SIZE && 
			vTable[p+i][q+j][r+k] != NULL) {

			CVertexIndex *tmp = vTable[p+i][q+j][r+k]; //<== ERROR!!!!!!

			GoFirst((CNode*&)tmp);
			
			while (1) {
				if ((IsEqual(tmp->v->x,x)) &&
					(IsEqual(tmp->v->y,y)) &&
					(IsEqual(tmp->v->z,z))) {
					
					return tmp->v;
				} //find the vertex in list
				if (tmp->nxt == NULL) break;
				else tmp = (CVertexIndex*)tmp->nxt;
			}
		}
	}
	}
	}

	return NULL; // cannot find
}
int CCalc::Near2(CVector * s0, CVector * s1, CVector * s2, CVector * t0, CVector * t1, CVector * t2, int first)
{
	double std = 0.0001, std2 = 0.0001;
	int count = 0;



	if (first) goto LOOSE;

	RE:
	if (fabs(s0->X()-t0->X()) < std && fabs(s0->Y()-t0->Y()) < std && fabs(s0->Z()-t0->Z()) < std) {
			count++;
	} else if (fabs(s0->X()-t1->X()) < std && fabs(s0->Y()-t1->Y()) < std && fabs(s0->Z()-t1->Z()) < std) {
			count++;
	} else if (fabs(s0->X()-t2->X()) < std && fabs(s0->Y()-t2->Y()) < std && fabs(s0->Z()-t2->Z()) < std) {
			count++;
	}
	if (fabs(s1->X()-t0->X()) < std && fabs(s1->Y()-t0->Y()) < std && fabs(s1->Z()-t0->Z()) < std) {
			count++; //if (count==2) return 1;
	} else if (fabs(s1->X()-t1->X()) < std && fabs(s1->Y()-t1->Y()) < std && fabs(s1->Z()-t1->Z()) < std) {
			count++; //if (count==2) return 1;
	} else if (fabs(s1->X()-t2->X()) < std && fabs(s1->Y()-t2->Y()) < std && fabs(s1->Z()-t2->Z()) < std) {
			count++; //if (count==2) return 1;
	}
	if (fabs(s2->X()-t0->X()) < std && fabs(s2->Y()-t0->Y()) < std && fabs(s2->Z()-t0->Z()) < std) {
			count++; //if (count==2) return 1;
	} else if (fabs(s2->X()-t1->X()) < std && fabs(s2->Y()-t1->Y()) < std && fabs(s2->Z()-t1->Z()) < std) {
			count++; //if (count==2) return 1;
	} else if (fabs(s2->X()-t2->X()) < std && fabs(s2->Y()-t2->Y()) < std && fabs(s2->Z()-t2->Z()) < std) {
			count++; //if (count==2) return 1;
	}
	LOOSE:
	if (first) {
		first--;
		std = line_gap;
		goto RE;
	}
	return count;
}
int CCalc::SumLines(int line1, int line2, int final)        // line2 = line1 + 1
{
	int k, ss;
	bool ret;
	GoFirst((CNode*&)Solid);  // added
	GoFirst((CNode*&)Solid->Face);
	CFace * Now = Solid->Face;
	CFace * Idx = Solid->Face;

	GoFirst((CNode*&)Now);
	if (Now) {
		L1:
		if (Now->line == line1 && Now->linenext == -100) {
			//
			ss = Now->subseq;
		} else {
			if (Now->nxt == NULL) {
				return 0;
			} else {
				Now = (CFace*) Now->nxt;
				goto L1;
			}
		}
	} else {
		return 0;
	}
	GoFirst((CNode*&)Idx);
	if (Idx) {
		L2:
		if (Idx->line == line2 && Idx->linebefore == -100) {
			Idx->linebefore = Now->id; Now->linenext = Idx->id;  // 연결
			while (1) {
				k = Idx->linenext;
				Idx->subseq += ss;
				Idx->line = line1;           // line2 -> line1
				if (k == -100) {
					//
					goto L3;
				}
				ret = Go((CNode*&) Idx, k); // pkc
				if (ret) {
					// sucess
				} else {
					return 0;
				}
			}
		} else {
			if (Idx->nxt == NULL) {
				return 0;
			} else {
				Idx = (CFace*) Idx->nxt;
				goto L2;
			}
		}
	} else {
		return 0;
	}
	L3:
	GoFirst((CNode*&)Now);
	if (Now) {
		L5:
		if (Now->line > line2) {
			Now->line -= 1;         // 감소
			goto L6;
		} else {
			L6:
			if (Now->nxt == NULL) {
				return 1;
			} else {
				Now = (CFace*) Now->nxt;
				goto L5;
			}
		}
	}
	return 0;
}
int CCalc::ChangeLines(int line1, int line2)  // swap
{
	int sequ = 0, line;

	GoFirst((CNode*&)Solid);  // added
	GoFirst((CNode*&)Solid->Face);
	CFace * Now = Solid->Face;
	CFace * Idx = Solid->Face;

	line = line1;  // 0
	RE:
	GoFirst((CNode*&)Now);
	if (Now) {
		L1:
		if (Now->line == line) {
			if (sequ == 0) {
				Now->line = -77;       // line1-> -77
			} else if (sequ == 1) {
				Now->line = line1;     // line2->line1
			} else {
				Now->line = line2;     // line1->line2
			}
			goto L3;
		} else {
			L3:
			if (Now->nxt == NULL) {
				if (sequ == 0) {
					sequ++;
					line = line2;  // 1
					goto RE;
				} else if (sequ == 1) {
					sequ++;
					line = -77;  // 2
					goto RE;
				} else {
					return 1;
				}
			} else {
				Now = (CFace*) Now->nxt;
				goto L1;
			}
		}
	}
	return 0;
}
int  CCalc::FindEndPoint(int line, CVector* start, CVector* end)
{
	int k;
	bool ret;
	GoFirst((CNode*&)Solid);  // added
	GoFirst((CNode*&)Solid->Face);
	CFace * Now = Solid->Face;

	//RE:
	GoFirst((CNode*&)Now);
	if (Now) {
		L1:
		if (Now->line == line) {
			if (Now->linebefore == -100) {
				//start->Set(Now->mm[0], Now->mm[1], Now->mm[2]);  // 무게중심
				start->Set(Now->m[0], Now->m[1], Now->m[2]);
				while (1) {
					if (Now->linenext == -100) {
						//end->Set(Now->mm[0], Now->mm[1], Now->mm[2]);
						end->Set(Now->m[0], Now->m[1], Now->m[2]);
						return 1;
					}
					k = Now->linenext;
					ret = Go((CNode*&) Now, k); // pkc
					if (ret) {
						// sucess
					} else {
						return 0;
					}
				}
			} else {
				goto L3;
			}
		} else {
			L3:
			if (Now->nxt == NULL) {
				return 0;
			} else {
				Now = (CFace*) Now->nxt;
				goto L1;
			}
		}
	}
	return 0;
}
int CCalc::ChangeLine(int line)  // order
{
	int k, count = 0, init = 1;
	bool ret;
	GoFirst((CNode*&)Solid);  // added
	GoFirst((CNode*&)Solid->Face);
	CFace * Now = Solid->Face;
	CFace * Idx = Solid->Face;

	RE:
	GoFirst((CNode*&)Now);
	if (Now) {
		L1:
		if (Now->line == line) {
			if (Now->linebefore == -100) {  // 처음
				while (1) {
					if (init) {
						k = Now->linenext;
						Now->linenext = Now->linebefore;
						Now->linebefore = k;
						if (k == -100) {
							//return 1;
							init = 0;
							goto RE;
						}
						ret = Go((CNode*&) Now, k); // pkc
						if (ret) {
							// sucess
						} else {
							return 0;
						}
					} else {         // 2단계에서 subseq 를 바꾸어 준다
						count++;
						Now->subseq = count;  // subseq
						if (Now->linenext == -100) {
							return 1;
						}
						k = Now->linenext;
						ret = Go((CNode*&) Now, k); // pkc
						if (ret) {
							// sucess
						} else {
							return 0;
						}
					}
				}
			} else {
				goto L3;
			}
		} else {
			L3:
			if (Now->nxt == NULL) {
				return 0;
			} else {
				Now = (CFace*) Now->nxt;
				goto L1;
			}
		}
	}
	return 0;
}
int CCalc::FindLine(int line, int * sharp, int remove)     // mn[] 을 이용
{
	int k, count=0, run = 1;
	bool ret;
	GoFirst((CNode*&)Solid);  // added
	GoFirst((CNode*&)Solid->Face);
	CFace * Now = Solid->Face;
	CFace * Cur = Solid->Face;
	CFace * Idx = Solid->Face;
	CFace * Idx2 = Solid->Face;
	CVector pre2, pre, cur, v2, v1;
	double inner, x, y, z;

	GoFirst((CNode*&)Now); GoFirst((CNode*&)Idx);
	if (Now) {
		L1:
		if (Now->line == line) {
			if (Now->linebefore == -100) {
				while (1) {
					count++;
					Now->subseq = count;  // subseq 넘버 부여

					pre2.Set(pre.X(), pre.Y(), pre.Z());  // pre.a[0]
					pre.Set(cur.X(), cur.Y(), cur.Z());
					//cur.Set(Now->mm[0], Now->mm[1], Now->mm[2]);  // 무게중심
					//cur.Set(Now->mn[0], Now->mn[1], Now->mn[2]);  // normal
					cur.Set(Now->n[0], Now->n[1], Now->n[2]);  // normal
					Idx2 = Idx;
					Idx = Cur;
					Cur = Now;
					if (count >= 3 && run) {
						/*
						v2.Set(pre.a[0]-pre2.a[0], pre.a[1]-pre2.a[1], pre.a[2]-pre2.a[2]);
						v1.Set(cur.a[0]-pre.a[0], cur.a[1]-pre.a[1], cur.a[2]-pre.a[2]);
						inner = v2.a[0]*v1.a[0] + v2.a[1]*v1.a[1] + v2.a[2]*v1.a[2];
						if (inner < 0) {
							//Idx->linenext = -100; // 제거하지 않음
							//run = 0;
							*sharp += 1;
						}
						*/
						x = pre2.a[0] + cur.a[0];
						y = pre2.a[1] + cur.a[1];
						z = pre2.a[2] + cur.a[2];
						inner = sqrt(x*x + y*y + z*z);
						//Idx->mn[0] = pre.a[0] = x/inner;      // 여기서
						//Idx->mn[1] = pre.a[1] = y/inner;
						//Idx->mn[2] = pre.a[2] = z/inner;
						Idx->mn[0] = x/inner;      // 여기서       순전히 n[] 로부터 mn[] 를 만듬
						Idx->mn[1] = y/inner;
						Idx->mn[2] = z/inner;
						//test
						//pre.a[0] = x/inner; pre.a[1] = y/inner; pre.a[2] = z/inner;  // Not bad
						//
					}
					//////////////////////
					if (run == 0) {
						Now->line = Now->seq = -1;
					}
					//////////////////////
#ifdef _DEBUG
					if (Now->linenext == -100) {
						TRACE("Line %d : Index %6d -> %6d,     %6d    (%d)  -----------  %d\n", line, Now->id, Now->linenext, count, remove, line);
					} else {
						TRACE("Line %d : Index %6d -> %6d,     (%d)\n", line, Now->id, Now->linenext, remove);
					}
#endif
					if (remove) {
						Now->line = -1;
						Now->seq = -1;
					}
					//count++;
					if (Now->linenext == -100) {
						//////////////////////
						if (run == 0) {
							Now->linenext = -1;
						}
						//////////////////////
						return count;
					}

					k = Now->linenext;
					ret = Go((CNode*&) Now, k); // pkc
					if (ret) {
					} else {
						return count;
					}
				} // while
			} else {
				goto L3;
			} // if (Now->linebefore == -100)
		} else {
			L3:
			if (Now->nxt == NULL) {
				return 0;
			} else {
				Now = (CFace*) Now->nxt;
				goto L1;
			}
		} // if (Now->line == line)
	}
	return 0;  // 해당 라인이 없음
}

int CCalc::ReadStl(char* name, int mode, double offset)
{
	bool IsBin = false;
	char p[200],c[96];
	char title[100];
	double v[3][3];    // f->d
	double n[3];       // f->d
	float vv[3][3];    // f
	float nn[3];       // f
	int  ii = 0, jj = 0, count = 0, k, index = 0, rev, err = 0;
	int  facet_num = 0;

	
	CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
	frame->setProgressCtrl(1,100,"Loading......");    // 제거대상
	
	FILE *a;
	a = fopen(name, "rb");
	if (a == NULL) {
		frame->removeProgressCtrl();
		MessageBox(pParent->m_hWnd,"Loading STL failure -> Directory not found","ERROR", MB_OK | MB_ICONEXCLAMATION);
		return 1;
	}
	hole_count = 0;
	
	if (mode == 0) {        //기존의 솔리드 모델을 메모리에서 지웁니다
		if (Solid) {
			ClearNode((CNode*&)Solid, 1);              // 이곳에 구현
			Solid = new CSolid;
		}
		if (Slice) ClearNode((CNode*&)Slice, 2);
	}
	else Add((CNode*&)Solid, new CSolid);
	//printf("solid : %d\n", CountNode((CNode*)Solid));


	k = add_count+del_count;
	// Initialize Min,Max Value
	vertex_number = 0;
	Solid->Min[0]=99999999.9f;	Solid->Min[1]=99999999.9f;	Solid->Min[2]=99999999.9f;
	Solid->Max[0]=-99999999.9f;	Solid->Max[1]=-99999999.9f;	Solid->Max[2]=-99999999.9f;
	
	stl_vertex_number = 0;
	
	fseek(a,80,SEEK_SET);
	fread(c,1,95,a);		// char
	for (ii=0; ii<95; ii++) {		
		if (((c[ii]<' ')||(c[ii]>'~'))&&(c[ii]!=10)&&(c[ii]!=13) &&(c[ii]!='\t')) {
			IsBin = true;
			break;
		}
	}
	
	fclose(a);
	
	if (!IsBin) {                           // if ascii
	
		//a = fopen(name, "rt");   //-> %f
		a = fopen(name, "r");      //-> %lf
		fscanf(a,"%s %s",p,title);
		while (1) {
			fscanf(a,"%s",p);
			
			if (!stricmp(p,"LOOP")) { //read vertex
				for (ii=0;ii<=2;ii++){
					fscanf(a,"%s",p);  //VERTEX
					fscanf(a,"%lf",&v[ii][0]); //X
					fscanf(a,"%lf",&v[ii][1]); //Y
					fscanf(a,"%lf",&v[ii][2]); //Z
				}
			} else if (!stricmp(p,"NORMAL")){ //read normal vector
				fscanf(a,"%lf",&n[0]);  //normal vector
				fscanf(a,"%lf",&n[1]);
				fscanf(a,"%lf",&n[2]);
			} else if (!stricmp(p,"ENDFACET")) { //make a triangle patch
				
				//ID번호를 챙기면서 삼각형 패치 데이타를 만듭니다
				//if (IsEqual(v[0][0],v[1][0]) && IsEqual(v[0][1],v[1][1]) && IsEqual(v[0][2],v[1][2]))
				//	{}
				//else if (IsEqual(v[0][0],v[2][0]) && IsEqual(v[0][1],v[2][1]) && IsEqual(v[0][2],v[2][2]))
				//	{}
				//else if (IsEqual(v[1][0],v[2][0]) && IsEqual(v[1][1],v[2][1]) && IsEqual(v[1][2],v[2][2]))
				//	{}
				//else{
				CVertex *v0 = new CVertex(v[0][0],v[0][1],v[0][2]);
				CVertex *v1 = new CVertex(v[1][0],v[1][1],v[1][2]);
				CVertex *v2 = new CVertex(v[2][0],v[2][1],v[2][2]);

				CFace *f = new CFace(v0,v1,v2,facet_num++);
				f->Set();
				if ((n[0] * f->n[0] + n[1] * f->n[1] + n[2] * f->n[2]) < 0) {
					fclose(a);
					frame->removeProgressCtrl();
					MessageBox(pParent->m_hWnd,"Loading STL failure -> Bad STL","ERROR", MB_OK | MB_ICONEXCLAMATION);
					return 2;
				}
				//Add((CNode*&)Solid->Face, new CFace(v0,v1,v2,facet_num++));  // 이곳
				Add((CNode*&)Solid->Face, f);  // 이곳
				Solid->Face->Set();      // n벡터 만듬
				//Solid->Face->n[0] = n[0];
				//Solid->Face->n[1] = n[1];
				//Solid->Face->n[2] = n[2];


				if (fnew == 0) {
				//	Solid->Face->mn[0] = n[0];
				//	Solid->Face->mn[1] = n[1];
				//	Solid->Face->mn[2] = n[2];
					Solid->Face->Set2();
				}
				stl_vertex_number +=3;
				//}
			} else if (!stricmp(p,"END") || !stricmp(p,"ENDSOLID")) {
				break;
			}
		}
		fclose(a);
	} else {	                             //if binary
		
		short Attr;
		long Number=0;

		a = fopen(name, "rb");
		fseek(a, 80, SEEK_SET);
		fread(&Number, 4, 1, a);  // long

		for (ii=0, jj=0; ii < Number; ii++, jj++) {
			fread(nn, 4, 3, a);  //normal vector    data 크기, 반복횟수
			fread(vv, 4, 9, a);  //VERTEX
			fread(&Attr, 2, 1, a);        // short
			//if (IsEqual(v[0][0],v[1][0]) && IsEqual(v[0][1],v[1][1]) && IsEqual(v[0][2],v[1][2]))
			//	{}
			//else if (IsEqual(v[0][0],v[2][0]) && IsEqual(v[0][1],v[2][1]) && IsEqual(v[0][2],v[2][2]))
			//	{}
			//else if (IsEqual(v[1][0],v[2][0]) && IsEqual(v[1][1],v[2][1]) && IsEqual(v[1][2],v[2][2]))
			//	{}
			//else{
				/*
				for (int xx=0;xx<3;xx++) {
					for (int yy=0;yy<3;yy++) {
						v[xx][yy] = float (RoundOff(v[xx][yy]));
					}
				}
				*/
			CVertex *v0 = new CVertex(vv[0][0],vv[0][1],vv[0][2]);
			CVertex *v1 = new CVertex(vv[1][0],vv[1][1],vv[1][2]);
			CVertex *v2 = new CVertex(vv[2][0],vv[2][1],vv[2][2]);

			CFace *f = new CFace(v0,v1,v2,facet_num++);
			f->Set();
			if ((nn[0] * f->n[0] + nn[1] * f->n[1] + nn[2] * f->n[2]) < 0) {
				fclose(a);
				frame->removeProgressCtrl();
				MessageBox(pParent->m_hWnd,"Loading STL failure -> Bad STL","ERROR", MB_OK | MB_ICONEXCLAMATION);
				return 2;
				/*
				v0->x = vv[2][0];
				v0->y = vv[2][1];
				v0->z = vv[2][2];
				v2->x = vv[0][0];
				v2->y = vv[0][1];
				v2->z = vv[0][2];
				*/
			}
			Add((CNode*&)Solid->Face, f);
			Solid->Face->Set();      // n벡터 만듬
			//Solid->Face->n[0] = nn[0];
			//Solid->Face->n[1] = nn[1];
			//Solid->Face->n[2] = nn[2];

			if (fnew == 0) {
			//	Solid->Face->mn[0] = nn[0];
			//	Solid->Face->mn[1] = nn[1];
			//	Solid->Face->mn[2] = nn[2];
				Solid->Face->Set2();
			}
			stl_vertex_number +=3;
			//}
		}
		fclose(a);
	}

	//printf("IsBin : %d\n", IsBin);
	stl_number=facet_num; //stl_number는 CStl의 멤버 변수이다
	k = add_count+del_count;

	//delete v0; delete v1; delete v2;
	
	MakeId((CNode*)Solid);

	//Solid->IS_SELECTED =true;
	Solid->IS_SELECTED = false;
	SetMinMax();

	GoFirst((CNode*&)Solid->Face);
	CFace * Now = Solid->Face;
	CFace * Idx = Solid->Face;
	CFace * Fdx = Solid->Face;
	CFace * Bdx = Solid->Face;
	CVector s0, s1, s2, t0, t1, t2, t3;

	ii = jj = k = 1; count = 0;
	int fdir, bdir, ret;
	//double Vx, Vy, Vz, Vd;
	if (fnew == 0) {    //----------------------------------------------------------------------------------------
		L0:
		GoFirst((CNode*&)Now);  //------------------------------------------
		if (Now) {
			L1:
			if (Now->seq == 0) {
				k = Now->id;
#ifdef _DEBUG
				TRACE("Head %d started -------------------------------------------->\n", k);
#endif
				//Idx = (CFace *)Now;
				Fdx = (CFace *)Now;
				Bdx = (CFace *)Now;
				index = 0;
				fdir = 1;
				bdir = -1;
				goto L3;
			} else {
				L2:
				if (Now->nxt == NULL) {
					goto PARK;
				} else {
					Now = (CFace*) Now->nxt;
					goto L1;
				}
			}
		} else {
			goto PARK;
		}

		L3:
		if (fdir != 0) {
			/**/
			ret=0;
			//Fdx->seq++;
			GoFirst((CNode*&)Idx);

			if (Idx) {
				L4:
				if (Idx->seq == 0 && Fdx->id != Idx->id) {
					if (fdir == 1) {
						/*
						*/
						if ((n[2]=(Fdx->d[0]*Idx->d[0] + Fdx->d[1]*Idx->d[1] + Fdx->d[2]*Idx->d[2])) < 0) rev = 1;  // --> <--   <-- -->
						else rev = 0;                                                                      // --> -->   <-- <--
						s0.Set(Fdx->v0->x, Fdx->v0->y, Fdx->v0->z);
						s1.Set(Fdx->v1->x, Fdx->v1->y, Fdx->v1->z);
						s2.Set(Fdx->v2->x, Fdx->v2->y, Fdx->v2->z);
						t0.Set(Idx->v0->x, Idx->v0->y, Idx->v0->z);
						t1.Set(Idx->v1->x, Idx->v1->y, Idx->v1->z);
						t2.Set(Idx->v2->x, Idx->v2->y, Idx->v2->z);
						t3.Set(Idx->mf[0], Idx->mf[1], Idx->mf[2]);
						ret = Near2(&s0, &s1, &s2, &t0, &t1, &t2);
						if (ret) {
							if (ret == 2 && rev) {

							} else if ((ret == 2 && !rev)) {
								if (180.0*acos(n[2])/3.1415926 < 80.0) {  // rad->angle
									frame->removeProgressCtrl();
									MessageBox(pParent->m_hWnd,"Loading STL failure -> Bad data","ERROR", MB_OK | MB_ICONEXCLAMATION);
									return 2;
								}
							} else if (ret == 3) {
								frame->removeProgressCtrl();
								MessageBox(pParent->m_hWnd,"Loading STL failure -> Multi data","ERROR", MB_OK | MB_ICONEXCLAMATION);
								return 2;
							} else {
								ret = 0;
								fdir = 1;
								err++;
							}
						}
					} else {  // -1
						while (1) {
							Sleep(1000);
						}
					}
					if (ret) {
						Fdx->seq = 1; Idx->seq = 1;
						Fdx->line = ii; Idx->line = ii;
						Fdx->linenext = Idx->id; Idx->linebefore = Fdx->id;
#ifdef _DEBUG
						k = Idx->id;
						TRACE("Forward %d at line %d --> OK\n", k, ii);
#endif
						goto L44;
					} else {
						goto L444;
					}
				} else {
					L444:
					if (Idx->nxt == NULL) {
						fdir = 0;
						if (Fdx->seq) {
							Fdx->linenext = -100;
						}
						//Fdx->seq--;
#ifdef _DEBUG
						k = Fdx->id;
						TRACE("Forward line %d finished ----> %d linenext=-100, fdir=%d\n", ii, k, fdir);
#endif
						goto L44;
					} else {
						Idx = (CFace*) Idx->nxt;
						k = Idx->id;

						goto L4;
					}
				}
			}
		}
		L44:
		if (fdir != 0) {  //------------------------------------------
			k = Idx->id;

			Fdx = (CFace *)Idx;

			goto L3;
		}                 //------------------------------------------

		L7:
		if (bdir != 0) {
			/**/
			ret=0;
			//Bdx->seq++;
			GoFirst((CNode*&)Idx);

			if (Idx) {
				L5:
				if (Idx->seq == 0 && Bdx->id != Idx->id) {
					if (bdir == 1) {
						while (1) {
							Sleep(1000);
						}
					} else {  // -1
						/*
						*/
						if ((n[2]=(Bdx->d[0]*Idx->d[0] + Bdx->d[1]*Idx->d[1] + Bdx->d[2]*Idx->d[2])) < 0) rev = 1;  // --> <--   <-- -->
						else rev = 0;                                                                      // --> -->   <-- <--
						s0.Set(Bdx->v0->x, Bdx->v0->y, Bdx->v0->z);
						s1.Set(Bdx->v1->x, Bdx->v1->y, Bdx->v1->z);
						s2.Set(Bdx->v2->x, Bdx->v2->y, Bdx->v2->z);
						t0.Set(Idx->v0->x, Idx->v0->y, Idx->v0->z);
						t1.Set(Idx->v1->x, Idx->v1->y, Idx->v1->z);
						t2.Set(Idx->v2->x, Idx->v2->y, Idx->v2->z);
						t3.Set(Idx->mf[0], Idx->mf[1], Idx->mf[2]);
						ret = Near2(&s0, &s1, &s2, &t0, &t1, &t2);
						if (ret) {
							if (ret == 2 && rev) {

							} else if ((ret == 2 && !rev)) {
								if (180.0*acos(n[2])/3.1415926 < 80.0) {  // rad->angle
									frame->removeProgressCtrl();
									MessageBox(pParent->m_hWnd,"Loading STL failure -> Bad data","ERROR", MB_OK | MB_ICONEXCLAMATION);
									return 2;
								}
							} else if (ret == 3) {
								frame->removeProgressCtrl();
								MessageBox(pParent->m_hWnd,"Loading STL failure -> Multi data","ERROR", MB_OK | MB_ICONEXCLAMATION);
								return 2;
							} else {
								ret = 0;
								bdir = -1;
								err++;
							}
						}
					}
					if (ret) {
						Bdx->seq = 1; Idx->seq = 1;
						Bdx->line = ii; Idx->line = ii;
						Bdx->linebefore = Idx->id; Idx->linenext = Bdx->id;
#ifdef _DEBUG
						k = Idx->id;
						TRACE("Backward %d at line %d --> OK\n", k, ii);
#endif
						goto L55;
					} else {
						goto L555;
					}
				} else {
					L555:
					if (Idx->nxt == NULL) {
						bdir = 0;
						if (Bdx->seq) {
							Bdx->linebefore = -100;
						}
						//Bdx->seq--;
#ifdef _DEBUG
						k = Bdx->id;
						TRACE("Backward line %d finished ----> %d linebefore=-100, bdir=%d\n", ii, k, bdir);
#endif
						goto L55;
					} else {
						Idx = (CFace*) Idx->nxt;
						k = Idx->id;

						goto L5;
					}
				}
			}
		}
		L55:
		if (bdir != 0) {  //------------------------------------------
			k = Idx->id;

			Bdx = (CFace *)Idx;

			goto L7;
		}                 //------------------------------------------

		if (fdir != 0 || bdir != 0) {  //------------------------------------------

			index = 0;
			goto L3;
		} else {
			ii++; count++;
			if (count > 10000) {
				frame->removeProgressCtrl();
				MessageBox(pParent->m_hWnd,"Loading STL failure -> Too many data","ERROR", MB_OK | MB_ICONEXCLAMATION);
				return 2;
			}
			goto L0;  // 새로운 시작점 찾기
		}                              //------------------------------------------

	}  // fnew          //----------------------------------------------------------------------------------------

	PARK:
	if (fnew == 0) {    //----------------------------------------------------------------------------------------

		index = 0; k = 0; err = 0;
		for (index = 1; index <= count; index++) {    //-----------------------------------------------
			ii = FindLine(index, &err);                         // 이중선 제거, subseq 번호 부여
			k += ii;
		}                                             //-----------------------------------------------
		ii = k;
#ifdef _DEBUG
		TRACE("Total Point ------> %d\n", k);
#endif
		k = 0;
		GoFirst((CNode*&)Solid->Face);  // 검사
		if (Solid->Face) {    //-----------------------------------------------
			rev = count;
			Solid->linenum = count;   // 등록
			/*
			CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
			//frame->m_ComboLine.Clear();
			frame->m_ComboLine.ResetContent();
			CString temp;
			temp.Format("Line All");
			frame->m_ComboLine.AddString(temp);
			for (index = 0; index < count; index++) {
				temp.Format("Line number -> %03d", index+1);
				frame->m_ComboLine.AddString(temp);
			}
			frame->m_ComboLine.SetCurSel(0);
			*/
			while (1) {
				/*
				if (Solid->Face->seq == 0) {
					while (1) {
						count = Solid->Face->id;
						Sleep(1000);
					}
				}
				*/
				k++;

				if (Solid->Face->nxt==NULL) break; // is it last ?
				else Solid->Face=(CFace*)Solid->Face->nxt;
			}
			((CMainFrame *)AfxGetMainWnd())->StatusBarMessage("%d Lines finished,  %d of %d Points used,  %d Sharp angles !!!!", count, ii, k, err);
		}                     //-----------------------------------------------

#ifdef _DEBUG
		TRACE("Total Point ------> %d,  %d\n", k, Solid->Face->id);
#endif

		if (count > 1) {      //-----------------------------------------------
			index = 1;
			SORT:
			FindEndPoint(index, &s0, &s1);  // s1 = end
			n[2] = 100000;
			k = index+1;
			for ( ; k <= count; k++) {
				FindEndPoint(k, &t0, &t1);
				n[0] = sqrt((s1.X()-t0.X())*(s1.X()-t0.X()) + (s1.Y()-t0.Y())*(s1.Y()-t0.Y()) + (s1.Z()-t0.Z())*(s1.Z()-t0.Z()));
				n[1] = sqrt((s1.X()-t1.X())*(s1.X()-t1.X()) + (s1.Y()-t1.Y())*(s1.Y()-t1.Y()) + (s1.Z()-t1.Z())*(s1.Z()-t1.Z()));
				if (n[0] < n[2]) {
					n[2] = n[0];
					ii = k; jj = 0;
				}
				if (n[1] < n[2]) {
					n[2] = n[1];
					ii = k; jj = 1;
				}
			}
			if (count > 1) {
				if (count-index > 1) {
					if (ii != index+1) {
						ChangeLines(index+1, ii);
#ifdef _DEBUG
						TRACE("%d 라인과 %d 라인을 바꾸었읍니다\n", index+1, ii);
#endif
					}
				}
				if (jj) {
					ChangeLine(index+1);
#ifdef _DEBUG
					TRACE("%d 라인의 가공 순서를 바꾸었읍니다\n", index+1);
#endif
				}
			}
			//index++;
			////////////////////////////
			//if (n[2] < 1000) {
			if (n[2] < mindist2) {
				if (SumLines(index, index+1, count)) {
					count--;
				} else {
					index++;
				}
			} else {
				index++;
			}
			////////////////////////////
			if (index <= count-1) goto SORT;
		}                     //-----------------------------------------------
		Solid->linenum = count;   // 재등록
		CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
		//frame->m_ComboLine.Clear();
		frame->m_ComboLine.ResetContent();
		CString temp;
		temp.Format("Line All");
		frame->m_ComboLine.AddString(temp);
		for (index = 0; index < count; index++) {
			temp.Format("Line number -> %03d", index+1);
			frame->m_ComboLine.AddString(temp);
		}
		frame->m_ComboLine.SetCurSel(0);
		if (rev != count) {
			MessageBox(pParent->m_hWnd,"Line is Combined","NOTICE", MB_OK);
		}
	}  // fnew          //----------------------------------------------------------------------------------------

	if (offset != 0) {    // Translate
		double x = -Solid->Min[0]+offset;
		double y = -Solid->Min[1];
		double z = -Solid->Min[2];
		GoFirst((CNode*&)Solid->Face);
		while (1) {
			Solid->Face->v0->Trans(x,y,z);
			Solid->Face->v1->Trans(x,y,z);
			Solid->Face->v2->Trans(x,y,z);
			if (Solid->Face->nxt == NULL) break;
			else Solid->Face = (CFace*)Solid->Face->nxt;
		}
	}
	SetMinMax();
	StlFileName = name;  // path+file name
	AutoScale();

	frame->removeProgressCtrl();
	return 0;
}

/************************************************
	모델을 Display 한다
************************************************/
void CCalc::ShowStl(void)
{
	float color[4]={0.7f,0.7f,0.3f,1.0f};
	int   count = 0;
	if (Solid==NULL) return;
	glPushMatrix();
	GoFirst((CNode*&)Solid);  // org                                   // ShowStl
	while (1) {
		//if (!stricmp(Solid->FileName,"support")) {
		//	glColor4fv(color_solid);
		//} else if (Solid->IS_SELECTED) {
		//	;
		//} else {
		//	glColor4fv(color_solid);
		//}
		if (Solid->base == 0) {
			color_select[0] = color_select1[0]; color_select[1] = color_select1[1];
			color_select[2] = color_select1[2]; color_select[3] = color_select1[3];  // green

			glPointSize(2.0);
			glLineWidth(1.0);

			glColor4fv(color_select);
		} else if (Solid->base == 1) {
			color_select[0] = color_select2[0]; color_select[1] = color_select2[1];
			color_select[2] = color_select2[2]; color_select[3] = color_select2[3];  // yellow
			glColor4fv(color_select);
		} else if (Solid->base == 2) {
			color_select[0] = color_select3[0]; color_select[1] = color_select3[1];
			color_select[2] = color_select3[2]; color_select[3] = color_select3[3];  // solid
			glColor4fv(color_select);
		}

		GoFirst((CNode*&)Solid->Face);
		
		if (Solid->Face) {
		while (1) {

			if (Solid->base == 0) {
				if (Solid->Face->seq < 0) goto TEST;
				if (Solid->Face->line < 1) goto TEST;
				if (Solid->Face->gsel < gsel) goto MID;

				glColor4fv(color_line);                                              // red
				glBegin(GL_POINTS); //
				if (gsel == 0) {
					glVertex3d(Solid->Face->mm[0], Solid->Face->mm[1], Solid->Face->mm[2]);  // 무게중심
				} else {
					glVertex3d(Solid->Face->m[0], Solid->Face->m[1], Solid->Face->m[2]);  // 무게중심
				}
				glEnd(); //
				glBegin(GL_LINES); //
				if (gsel == 0) {
					glVertex3d(Solid->Face->mm[0], Solid->Face->mm[1], Solid->Face->mm[2]);
					glVertex3d(Solid->Face->mm[0]+VSCALE*Solid->Face->n[0], Solid->Face->mm[1]+VSCALE*Solid->Face->n[1], Solid->Face->mm[2]+VSCALE*Solid->Face->n[2]);
					//glVertex3d(Solid->Face->mm[0]+VSCALE*Solid->Face->mn[0], Solid->Face->mm[1]+VSCALE*Solid->Face->mn[1], Solid->Face->mm[2]+VSCALE*Solid->Face->mn[2]);
				} else {
					glVertex3d(Solid->Face->m[0], Solid->Face->m[1], Solid->Face->m[2]);
					//glVertex3d(Solid->Face->m[0]+VSCALE*Solid->Face->n[0], Solid->Face->m[1]+VSCALE*Solid->Face->n[1], Solid->Face->m[2]+VSCALE*Solid->Face->n[2]);
					glVertex3d(Solid->Face->m[0]+VSCALE*Solid->Face->mn[0], Solid->Face->m[1]+VSCALE*Solid->Face->mn[1], Solid->Face->m[2]+VSCALE*Solid->Face->mn[2]);
				}
				glEnd(); //

					MID:
					glBegin(GL_LINES); //
					glVertex3d(Solid->Face->m[0],Solid->Face->m[1],Solid->Face->m[2]);
					glVertex3d(Solid->Face->mf[0],Solid->Face->mf[1],Solid->Face->mf[2]);
					glEnd(); //

				goto TEST;

			}
			glBegin(GL_TRIANGLES); //
			glNormal3d(Solid->Face->n[0],Solid->Face->n[1],Solid->Face->n[2]);
			glVertex3d(Solid->Face->v0->x,Solid->Face->v0->y,Solid->Face->v0->z);
			glVertex3d(Solid->Face->v1->x,Solid->Face->v1->y,Solid->Face->v1->z);
			glVertex3d(Solid->Face->v2->x,Solid->Face->v2->y,Solid->Face->v2->z);
			glEnd(); //
			TEST:
			if (Solid->Face->nxt==NULL) break; // is it last ?
			else Solid->Face=(CFace*)Solid->Face->nxt;
		}
		}

		if (Solid->nxt == NULL) break;
		else Solid = (CSolid*)Solid->nxt;
	}

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);  //

	if (DepartH) {
		glColor4fv(color_red);
		GoFirst((CNode*&)DepartH);
		while (1) {
			glBegin(GL_LINES); //
			glVertex3d(DepartH->v0->x,DepartH->v0->y,DepartH->v0->z);
			glVertex3d(DepartH->v1->x,DepartH->v1->y,DepartH->v1->z);
			glEnd(); //
			if (DepartH->nxt == NULL) break;
			else DepartH = (CHalfedge*)DepartH->nxt;
		}
	}
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);  //
	
	glPopMatrix();
}

void CCalc::ShowStlwire(void)
{
	static int only;
	int num = 0, num2 = 0;
	float color[4]={0.7f,0.7f,0.3f,1.0f};
	if (Solid == NULL) return;
	glPushMatrix();

	//GoFirst((CNode*&)Solid);
	GoLast((CNode*&)Solid);  // changed  // added               // ShowStlwire
	if (Solid->pre == NULL) only = 1;
	else only = 0;

	glDisable(GL_LIGHTING);  // --------> 선에서 안함
	while (1) {
		//if (!stricmp(Solid->FileName,"support")) {
		//	glColor4fv(color_solid);
		//}
		//else if (Solid->IS_SELECTED) {
		//}
		//else {
		//	glColor4fv(color_solid);
		//}
		if (Solid->base == 0) {
			color_select[0] = color_line[0]; color_select[1] = color_line[1];
			color_select[2] = color_line[2]; color_select[3] = color_line[3];

			glLineWidth(1.0);  // 4.0

			glColor4fv(color_select);
		} else if (Solid->base == 1) {
			color_select[0] = color_select2[0]; color_select[1] = color_select2[1];  // yellow
			color_select[2] = color_select2[2]; color_select[3] = color_select2[3];
			glLineWidth(1.0);
			glColor4fv(color_select);
		} else if (Solid->base == 2) {
			//color_select[0] = color_select3[0]; color_select[1] = color_select3[1];  // solid
			//color_select[2] = color_select3[2]; color_select[3] = color_select3[3];
			color_select[0] = color_select2[0]; color_select[1] = color_select2[1];  // yellow
			color_select[2] = color_select2[2]; color_select[3] = color_select2[3];
			glLineWidth(1.0);
			glColor4fv(color_select);
		}

		GoFirst((CNode*&)Solid->Face);
		
		if (Solid->Face) {
		while (1) {

			if (Solid->base == 0) {
				if (Solid->Face->line > 0) {                 // Solid->Face->line > 0    VIP
					if ((Solid->Face->line % 2) == 0) {
						glColor4fv(color_select3);
					} else {
						glColor4fv(color_select);
					}
					if (only) {
						glLineWidth(1.0);
					} else {
						glLineWidth(2.0);
					}
					glBegin(GL_LINES); //
					glVertex3d(Solid->Face->m[0],Solid->Face->m[1],Solid->Face->m[2]);
					glVertex3d(Solid->Face->mf[0],Solid->Face->mf[1],Solid->Face->mf[2]);
					glEnd(); //
					if (Solid->currentline == -1) goto ALL;
					if (Solid->Face->line == Solid->currentline) {
						if (Solid->Face->subseq > num) num = Solid->Face->subseq;
						ALL:

						//if (Solid->Face->subseq == seg_counter && Solid->currentline != -1) {
						if (Solid->Face->subseq == seg_counter) {
							glLineWidth(10.0);
							glPointSize(10.0);
							glColor3f(1.0, 1.0, 1.0);
							glDisable(GL_DEPTH_TEST);  //
							//glBegin(GL_LINES); //
							glBegin(GL_POINTS); //
								glVertex3d(Solid->Face->m[0],Solid->Face->m[1],Solid->Face->m[2]);
								glVertex3d(Solid->Face->mf[0],Solid->Face->mf[1],Solid->Face->mf[2]);
							glEnd(); //
							//glEnd(); //
							glEnable(GL_DEPTH_TEST);  //
						}
						if (Solid->Face->linebefore == -100) {
							glPointSize(5.0);
							glColor3f(1, 0, 0);
							glDisable(GL_DEPTH_TEST);  //
							glBegin(GL_POINTS); //
							glVertex3d(Solid->Face->mm[0], Solid->Face->mm[1], Solid->Face->mm[2]);
							glEnd(); //
							glEnable(GL_DEPTH_TEST);  //
						}
						/**/
						if (Solid->Face->linenext == -100) {
							glPointSize(5.0);
							glColor3f(0, 0, 1);
							glDisable(GL_DEPTH_TEST);  //
							glBegin(GL_POINTS); //
							glVertex3d(Solid->Face->mm[0], Solid->Face->mm[1], Solid->Face->mm[2]);
							glEnd(); //
							glEnable(GL_DEPTH_TEST);  //
						}
						/**/

					}
				} // if (Solid->Face->line > 0)
			} else {
				glBegin(GL_LINE_STRIP); //
				glVertex3d(Solid->Face->v0->x,Solid->Face->v0->y,Solid->Face->v0->z);
				glVertex3d(Solid->Face->v1->x,Solid->Face->v1->y,Solid->Face->v1->z);
				glVertex3d(Solid->Face->v2->x,Solid->Face->v2->y,Solid->Face->v2->z);
				glVertex3d(Solid->Face->v0->x,Solid->Face->v0->y,Solid->Face->v0->z);
				glEnd(); //
			} // if (Solid->base == 0)

			if (Solid->Face->nxt==NULL) break;
			else Solid->Face=(CFace*)Solid->Face->nxt;
		} // while
		} // if

		//if (Solid->nxt == NULL) break;
		//else Solid = (CSolid*)Solid->nxt;
		if (Solid->pre == NULL) break;
		else Solid = (CSolid*)Solid->pre;
	} // while
	((CMainFrame *)AfxGetMainWnd())->StatusBarMessage("%d Line, (%d : %d) !!!!", Solid->currentline, seg_counter, num);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);  //

	if (DepartH) {
		glColor4fv(color_red);
		GoFirst((CNode*&)DepartH);
		while (1) {
			glBegin(GL_LINES); //
			glVertex3d(DepartH->v0->x,DepartH->v0->y,DepartH->v0->z);
			glVertex3d(DepartH->v1->x,DepartH->v1->y,DepartH->v1->z);
			glEnd(); //
			if (DepartH->nxt == NULL) break;
			else DepartH = (CHalfedge*)DepartH->nxt;
		}
	}
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);  //
	
	glPopMatrix();
}


/*******************************************************
	Vertex List를 생성한다
	지정된 toleraance를 기준으로 Vertex들을 통합시킨다
*******************************************************/
void CCalc::MakeVertexList(double es, int mode)
{
	double tmp = ES;
	ES = es;
	int p,q,r;
	int count = 0, max_face = 0;

	CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
	GoLast((CNode*&)Solid);
	while (1) {
		max_face += CountNode((CNode*&)Solid->Face);
		if (Solid->pre == NULL) break;
		else Solid = (CSolid*)Solid->pre;
	}
	MakeId((CNode*)Solid);
	frame->setProgressCtrl(1,max_face/100,"Merging Vertex......");

	while (1) {

		if (!Solid->Vertex) {

			long temp_face_number = CountNode((CNode*)Solid->Face);
			long face_number=0;					// 각각의 갯수 저장
			int number =0, percent = 0;
		
			CVertex *v0 ,*v1,*v2;				//vertex 3
			
			GoFirst((CNode*&)Solid->Face);

			while (1) 						// 노드의 끝까지 반복
			{
				Solid->Face->v0->x = RoundOff(Solid->Face->v0->x);
				Solid->Face->v0->y = RoundOff(Solid->Face->v0->y);
				Solid->Face->v0->z = RoundOff(Solid->Face->v0->z);
				Solid->Face->v1->x = RoundOff(Solid->Face->v1->x);
				Solid->Face->v1->y = RoundOff(Solid->Face->v1->y);
				Solid->Face->v1->z = RoundOff(Solid->Face->v1->z);
				Solid->Face->v2->x = RoundOff(Solid->Face->v2->x);
				Solid->Face->v2->y = RoundOff(Solid->Face->v2->y);
				Solid->Face->v2->z = RoundOff(Solid->Face->v2->z);
				percent++;
				////////////////////////
				v0=FindVertex(Solid->vTable,Solid->Vertex,Solid->Face->v0->x,Solid->Face->v0->y,Solid->Face->v0->z,Solid->Min,Solid->Max);
				v1=FindVertex(Solid->vTable,Solid->Vertex,Solid->Face->v1->x,Solid->Face->v1->y,Solid->Face->v1->z,Solid->Min,Solid->Max);
				v2=FindVertex(Solid->vTable,Solid->Vertex,Solid->Face->v2->x,Solid->Face->v2->y,Solid->Face->v2->z,Solid->Min,Solid->Max);
				//solid의 vertex를 검색하여 리턴.
				////////////////////

				if (v0 == NULL)	{

					v0  = new CVertex(Solid->Face->v0->x,Solid->Face->v0->y,Solid->Face->v0->z,number++);

					p=int(((v0->x-Solid->Min[0])*VTABLE_SIZE)/(Solid->Max[0]-Solid->Min[0]));
					q=int(((v0->y-Solid->Min[1])*VTABLE_SIZE)/(Solid->Max[1]-Solid->Min[1]));
					r=int(((v0->z-Solid->Min[2])*VTABLE_SIZE)/(Solid->Max[2]-Solid->Min[2]));

					if (p<0) p = 0;
					else if (p>(VTABLE_SIZE-1)) p = (VTABLE_SIZE-1);
					if (q<0) q = 0;
					else if (q>(VTABLE_SIZE-1)) q = (VTABLE_SIZE-1);
					if (r<0) r = 0;
					else if (r>(VTABLE_SIZE-1)) r = (VTABLE_SIZE-1);

					Add((CNode*&)Solid->vTable[p][q][r],new CVertexIndex(v0));
					Add((CNode*&)Solid->Vertex,v0);
				} else {
					count++;
				}

				if (v1 == NULL)	{
					v1  = new CVertex(Solid->Face->v1->x,Solid->Face->v1->y,Solid->Face->v1->z,number++);
					p=int(((v1->x-Solid->Min[0])*VTABLE_SIZE)/(Solid->Max[0]-Solid->Min[0]));
					q=int(((v1->y-Solid->Min[1])*VTABLE_SIZE)/(Solid->Max[1]-Solid->Min[1]));
					r=int(((v1->z-Solid->Min[2])*VTABLE_SIZE)/(Solid->Max[2]-Solid->Min[2]));
					if (p<0) p = 0;
					else if (p>(VTABLE_SIZE-1)) p = (VTABLE_SIZE-1);
					if (q<0) q = 0;
					else if (q>(VTABLE_SIZE-1)) q = (VTABLE_SIZE-1);
					if (r<0) r = 0;
					else if (r>(VTABLE_SIZE-1)) r = (VTABLE_SIZE-1);
					Add((CNode*&)Solid->vTable[p][q][r],new CVertexIndex(v1));
					Add((CNode*&)Solid->Vertex,v1);
				} else {
					count++;
				}
				if (v2 == NULL) {
					v2 = new CVertex(Solid->Face->v2->x,Solid->Face->v2->y,Solid->Face->v2->z,number++);
					p=int(((v2->x-Solid->Min[0])*VTABLE_SIZE)/(Solid->Max[0]-Solid->Min[0]));
					q=int(((v2->y-Solid->Min[1])*VTABLE_SIZE)/(Solid->Max[1]-Solid->Min[1]));
					r=int(((v2->z-Solid->Min[2])*VTABLE_SIZE)/(Solid->Max[2]-Solid->Min[2]));
					if (p<0) p = 0;
					else if (p>(VTABLE_SIZE-1)) p = (VTABLE_SIZE-1);
					if (q<0) q = 0;
					else if (q>(VTABLE_SIZE-1)) q = (VTABLE_SIZE-1);
					if (r<0) r = 0;
					else if (r>(VTABLE_SIZE-1)) r = (VTABLE_SIZE-1);
					Add((CNode*&)Solid->vTable[p][q][r],new CVertexIndex(v2));
					Add((CNode*&)Solid->Vertex,v2);
				} else {
					count++;
				}
				if (percent > 100 ) {
					frame->stepProgress();
					percent = 0;
				}

				if (v0->z > Solid->Face->max_z) Solid->Face->max_z = v0->z;
				if (v1->z > Solid->Face->max_z) Solid->Face->max_z = v1->z;
				if (v2->z > Solid->Face->max_z) Solid->Face->max_z = v2->z;
				if (v0->z < Solid->Face->min_z) Solid->Face->min_z = v0->z;
				if (v1->z < Solid->Face->min_z) Solid->Face->min_z = v1->z;
				if (v2->z < Solid->Face->min_z) Solid->Face->min_z = v2->z;

				{
					delete Solid->Face->v0;
					delete Solid->Face->v1;
					delete Solid->Face->v2;
					Solid->Face->v0 = v0;
					Solid->Face->v1 = v1;
					Solid->Face->v2 = v2;
				}

				if (Solid->Face->nxt == NULL) break;
				else Solid->Face = (CFace*)Solid->Face->nxt;

			}
		}    //end of select if
		if (Solid->nxt == NULL) break;
		else Solid = (CSolid*)Solid->nxt;
	}     //end of solid while
	frame->removeProgressCtrl();
	
	GoFirst((CNode*&)Solid);
	while (1) {
		for (int p=0;p<VTABLE_SIZE;p++) {
			for (int q=0;q<VTABLE_SIZE;q++) {
				for (int r=0;r<VTABLE_SIZE;r++) {
					if (Solid->vTable[p][q][r]) {
						ClearNode((CNode*&)Solid->vTable[p][q][r], 15);    // CVertexIndex = 15
					}
				}
			}
		}
		if (Solid->nxt == NULL) break;
		else Solid = (CSolid*)Solid->nxt;
	}

	frame->setProgressCtrl(1,max_face,"Delete Invalid Facet......");
	GoFirst((CNode*&)Solid);
	while (1) {
		{
			GoFirst((CNode*&)Solid->Face);
			while (1) {
				if ((Solid->Face->v0 == Solid->Face->v1) ||
					(Solid->Face->v1 == Solid->Face->v2) ||
					(Solid->Face->v2 == Solid->Face->v0)) {
					DeleteFaceOnly(Solid->Face);
					GoFirst((CNode*&)Solid->Face);
				}
				if (Solid->Face->nxt == NULL) break;
				else Solid->Face = (CFace*)Solid->Face->nxt;
			}
		}
		if (Solid->nxt == NULL) break;
		else Solid = (CSolid*)Solid->nxt;
	}
	frame->removeProgressCtrl();

	ES = tmp;
	char s[80];
	sprintf(s,"Merging completed successfully : %d Vertices",count);
	if (mode) MessageBox(pParent->m_hWnd,s,"COMPLETED",MB_OK+MB_ICONINFORMATION);
}


/*******************************************************
	Edge List를 생성한다
	인접 Vertex, Face 의 정보를 저장한다
*******************************************************/
void CCalc::MakeEdgeList()
{
	if (Solid->Vertex == NULL) MakeVertexList(ES,0);

	CEdge *e0,*e1,*e2;
	long edge_number = 0, max_face = 0;
	CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();

	GoLast((CNode*&)Solid);
	while (1) {
		{
			max_face += CountNode((CNode*&)Solid->Face);
		}
		if (Solid->pre == NULL) break;
		else Solid = (CSolid*)Solid->pre;
	}
	frame->setProgressCtrl(1,max_face/100,"Building Edge List......");
	int percent = 0;
	nonmanifold_count = 0;
	
	while (1) {
		{
			if (Solid->Edge) {
				GoFirst((CNode*&)Solid->Edge);
				while (1) {
					Solid->Edge->f0 = NULL;
					Solid->Edge->f1 = NULL;
					if (Solid->Edge->nxt == NULL) break;
					else Solid->Edge = (CEdge*)Solid->Edge->nxt;
				}
			}
			GoFirst((CNode*&)Solid->Face);
			while (1) {
				Solid->Face->e0=Solid->Face->e1=Solid->Face->e2= NULL;
				percent++;
				e0=FindEdge(Solid->Face->v0,Solid->Face->v1);	//edge 찾기
				e1=FindEdge(Solid->Face->v1,Solid->Face->v2);
				e2=FindEdge(Solid->Face->v2,Solid->Face->v0);
				if (e0) {        //found the edge,
					if ((e0->f1) &&(e0->f1 != Solid->Face)) {
						e0->edge_type = EDGE_NON_MANIFOLD;
						Solid->Face->is_error=true;
						nonmanifold_count++;
					} else if (e0->f0 == NULL) {
						e0->f0=Solid->Face;
						Solid->Face->e0=e0;
					} else {
						e0->f1=Solid->Face;
						Solid->Face->e0=e0;
					}
				} else {       // 없으면 생성
					e0=new CEdge(Solid->Face->v0,Solid->Face->v1,edge_number++);
					e0->f0=Solid->Face , Solid->Face->e0=e0; Add((CNode*&)Solid->Edge,e0);
					Add((CNode*&)Solid->Face->v0->Indx,new CEdgeIndex(e0));
					Add((CNode*&)Solid->Face->v1->Indx,new CEdgeIndex(e0));
				}	
				if (e1) {  //found the edge
					if ((e1->f1)&&(e1->f1 != Solid->Face)) {
						e1->edge_type = EDGE_NON_MANIFOLD;
						Solid->Face->is_error=true;
						nonmanifold_count++;
					} else if (e1->f0 == NULL) {
						e1->f0=Solid->Face;
						Solid->Face->e1=e1;
					} else {
						e1->f1=Solid->Face;
						Solid->Face->e1=e1;
					}

				} else {
					e1=new CEdge(Solid->Face->v1,Solid->Face->v2,edge_number++);
					e1->f0=Solid->Face , Solid->Face->e1=e1; Add((CNode*&)Solid->Edge,e1);
					Add((CNode*&)Solid->Face->v1->Indx,new CEdgeIndex(e1));
					Add((CNode*&)Solid->Face->v2->Indx,new CEdgeIndex(e1));
				}
				if (e2) {
					if ((e2->f1)&&(e2->f1 != Solid->Face)) {
						e2->edge_type = EDGE_NON_MANIFOLD;
						Solid->Face->is_error=true;
						nonmanifold_count++;
					} else if (e2->f0 == NULL) {
						e2->f0=Solid->Face;
						Solid->Face->e2=e2;
					} else {
						e2->f1=Solid->Face;
						Solid->Face->e2=e2;
					}
				} else {
					e2=new CEdge(Solid->Face->v2,Solid->Face->v0,edge_number++);
					e2->f0=Solid->Face , Solid->Face->e2=e2; Add((CNode*&)Solid->Edge,e2);
					Add((CNode*&)Solid->Face->v2->Indx,new CEdgeIndex(e2));
					Add((CNode*&)Solid->Face->v0->Indx,new CEdgeIndex(e2));
				}
				if (percent > 100) {
					frame->stepProgress();
					percent =0;
				}
				
				if (Solid->Face->nxt == NULL) break;
				else Solid->Face = (CFace*)Solid->Face->nxt;
			}
		}
		if (Solid->nxt == NULL) break;
		else Solid = (CSolid*)Solid->nxt;
	}
	frame->removeProgressCtrl();
}

void CCalc::DeleteFaceOnly(CFace *&Face)
{
	if (Face == NULL) return;

	if (Face->pre == NULL) {
		CFace *FaceNxt = (CFace*)Face->nxt;
		Face = NULL;
		FaceNxt->pre = NULL;
		Face = FaceNxt;
	} else if (Face->nxt == NULL) {
		CFace *FacePre = (CFace*)Face->pre;
		FacePre->nxt = NULL;
		Face = FacePre;
	} else {    //pre - cur - nxt
		CFace *FacePre = (CFace*)Face->pre;
		CFace *FaceNxt = (CFace*)Face->nxt;
		FaceNxt->pre = FacePre;
		FacePre->nxt = FaceNxt;
		Face = NULL;
		Face = FaceNxt;
	}
}


//*******************************************************************
//	Solid의 Edge List에서 (v0,v1)인 Edge가 있는지 검색한다.
//	검색에	성공하면 그 Edge를 
//			실패하면 NULL을 리턴한다.
//*******************************************************************
CEdge* CCalc::FindEdge(CVertex* v0, CVertex* v1)
{
	if (v0 == NULL || v1 == NULL || Solid->Edge == NULL) return NULL;
	if (v0->Indx != NULL) {
		GoFirst((CNode*&)v0->Indx);
		while (1) {
			if ((v0->Indx->e->v0==v0 && v0->Indx->e->v1==v1) ||
				(v0->Indx->e->v1==v0 && v0->Indx->e->v0==v1)) {
				return v0->Indx->e;
			}
			if (v0->Indx->nxt == NULL) break;
			else v0->Indx = (CEdgeIndex*)v0->Indx->nxt;
		}
	}
	return NULL;
}