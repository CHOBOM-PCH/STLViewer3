/********************************************************
	Implementation of Halfedge Solid Modeling Class Library
*********************************************************/

#include "stdafx.h"
#include "solid.h"
#include <math.h>

int add_count = 0;
int del_count = 0;
int del_count_face = 0;
int del_count_vertex = 0;
//double line_width = 0.0015;       // 0.001 = 1u -> 0.0015      0.0001 = 100n  0.00001 = 10n
extern double line_width;

/********************************************************
	External Function
	리스트들을 관리한다.
********************************************************/
extern void log(char* format, ...) //log file for report and debugging
{
	char buffer[300];
	va_list ap;
	va_start(ap,format);
	vsprintf(buffer,format,ap);
	va_end(ap);
    
	FILE* a = fopen("c:\\STLlog.txt", "at");
	fprintf(a,"%s",buffer);
	fprintf(stdout,"%s",buffer);
	fclose(a);
}

extern void MakeRing(CNode* CUR) 
{
	CNode* first;
	CNode* last;
	CNode* TMP;

	TMP=CUR;
	while (1) {  //find the first node
	if (TMP->pre==NULL) break;
	TMP=TMP->pre;
	}
	first=TMP;

	TMP=CUR;
	while (1) {
	if (TMP->nxt==NULL) break; //find the last node
	TMP=TMP->nxt;
	}
	last=TMP;

	//first    last
	first->pre=last;
	last->nxt=first;
}

extern void GoLast(CNode * & Node)
{
	if (Node==NULL) return ;// nothing to search

	while (1) {
		if (Node->nxt==NULL) break;
		Node=Node->nxt;
	}
}

extern void Add(CNode*& CUR, CNode* NEW)   //CURRENT   NEW        이곳에 구현
{
	if (NEW==NULL) return;  // nothing to insert..
	if (CUR==NULL) {
		CUR=NEW;
		add_count++;
		return;
	} //create

	//go to the last node
	while (1) {
		if (CUR->nxt==NULL) break;
		CUR=CUR->nxt;
	}

	//CUR   NEW
	CUR->nxt=NEW;
	NEW->pre=CUR;
	NEW->nxt = NULL;
	CUR=NEW;
	add_count++;
}

extern void Insert(CNode*& CUR, CNode* NEW)   //CURRENT   NEW  after
{
	if (NEW==NULL) return;  // nothing to insert..
	if (CUR==NULL) {CUR=NEW; return;} //create

	//CUR   NEW
	CNode* AFTER = CUR->nxt;
	NEW->pre=CUR;
	NEW->nxt = AFTER;
	CUR->nxt=NEW;
	if (AFTER != NULL) AFTER->pre=NEW;
	CUR=NEW;
}

extern bool Go(CNode*& TMP,int id) 
{
	CNode* CUR=TMP;
	//go to the first node
	while (1) {
	if (CUR->pre==NULL) break;
	CUR=CUR->pre;
	}

	//check
	int success=0;
	while (1) {
	if (CUR->id == id) {success=1;break;}
	if (CUR->nxt==NULL) {success=0;break;}
	CUR=CUR->nxt;
	};

	if (success) TMP=CUR;
	else return false;
	return true;
}

extern void PrintAll(CNode* CUR)
{
	int num=0;

	if (CUR==NULL) {
		log("nothing to print");
		return;
	}
	while (1) {    //go to the first node
		if (CUR->pre==NULL) break;
		CUR=CUR->pre;
	}

	while (1) { //print all
		if (CUR==NULL) break;
		log("%d \n",CUR->id);
		CUR=CUR->nxt;
		num++;
	}
	log("%d nodes printed\n",num);
}

extern void Print(CNode* CUR)
{
	log("%d \n",CUR->id);
}

extern void GoFirst(CNode*& Node)
{
	if (Node==NULL) return ;// nothing to search
	while (1) {
		if (Node->pre==NULL) break;
		Node=Node->pre;
	}
}

extern void DeleteAll(CNode* CUR)    // ???
{
	if (CUR==NULL) 	return;
	GoFirst((CNode*&) CUR);

	CNode* TMP=CUR;

	while (1) { //delete all
		if (CUR==NULL) break;
		TMP=CUR;
		CUR=CUR->nxt;
		delete TMP;
		del_count++;
	}
}
void real_delete(CNode*& CUR, int mode)
{
	if (mode != CUR->type) {
		while (1) {
			Sleep(1000);
		}
	}
	if (mode == 1) {
		CSolid * TMP = (CSolid *) CUR->body;
		delete TMP;
	}
	else if (mode == 2) {CSlice * TMP = (CSlice *) CUR->body; delete TMP;}
	else if (mode == 3) {CHalfedge * TMP = (CHalfedge *) CUR->body; delete TMP;}
	else if (mode == 4) {CHalfedge * TMP = (CHalfedge *) CUR->body; delete TMP;}  //
	else if (mode == 11) {CFace * TMP = (CFace *) CUR->body; delete TMP;}
	else if (mode == 12) {CVertex * TMP = (CVertex *) CUR->body; delete TMP;}
	else if (mode == 13) {CEdge * TMP = (CEdge *) CUR->body; delete TMP;}
	else if (mode == 14) {CEdgeIndex * TMP = (CEdgeIndex *) CUR->body; delete TMP;}
	else if (mode == 15) {CVertexIndex * TMP = (CVertexIndex *) CUR->body; delete TMP;}
	else if (mode == 16) {CEdgeLoop * TMP = (CEdgeLoop *) CUR->body; delete TMP;}
	else if (mode == 17) {CLoop * TMP = (CLoop *) CUR->body; delete TMP;}
	else if (mode == 18) {CFaceIndex * TMP = (CFaceIndex *) CUR->body; delete TMP;}
}
extern void Delete(CNode*& CUR, int mode, bool after = false)          // 여기
{
	if (CUR==NULL) return;


	if (CUR->pre==NULL && CUR->nxt==NULL) {
		real_delete(CUR, mode);
		//delete CUR;
		CUR=NULL;                                    //  Add 시 CUR = NEW
		del_count++;
		return;
	}

	if (CUR->pre==NULL) {   //the first node :       <CUR>  NEXT  ....
		CNode* NXT=CUR->nxt;
		NXT->pre=NULL;
		real_delete(CUR, mode);
		//delete CUR;
		CUR=NXT;
		del_count++;
		return;
	} else if (CUR->nxt==NULL) { //the last node    ......   PREVIOUS <CUR>
		CNode* PRE=CUR->pre;
		PRE->nxt=NULL;
		real_delete(CUR, mode);
		//delete CUR;
		CUR = PRE;
		del_count++;
		return;
	} else if (CUR->nxt == CUR->pre) {
		CNode* PRE=CUR->pre;
		real_delete(CUR, mode);
		//delete CUR;
		CUR = PRE;
		CUR->nxt = NULL;
		CUR->pre = NULL;
		del_count++;
		return;
	} else {  //the middle node             PRE  <CUR>   NEXT
		CNode* PRE=CUR->pre;
		CNode* NXT=CUR->nxt;
		real_delete(CUR, mode);
		//delete CUR;
		PRE->nxt=NXT;
		NXT->pre=PRE;
		if (after) CUR=NXT; 
		else CUR=PRE;
		del_count++;
		return;
	}
}

extern void MakeId(CNode* Node)
{
	if (Node==NULL) return;
	int id=0;
	while (1) {     //go to the first node
		if (Node->pre==NULL) break;
		Node=Node->pre;
	}
	while (1) {
		Node->id=id++;
		Node=Node->nxt;
		if (Node==NULL) break;
	}
	//printf("%d\n", id);	// Solid의 id 확인
}

extern long CountNode(CNode * Node)	 //리스트의 아이템 갯수
{
	if (Node == NULL) return 0;
	long num=0;
	while (1) {	   
		if (Node->pre==NULL) break;
		Node=Node->pre;
	}
	while (1) {
		num++;
		if (Node->nxt==NULL) break;
		else Node=Node->nxt;
	}
	return num;
	//printf("%d\n", num);
}

extern void ClearNode(CNode * & Node, int mode)         //리스트를 삭제             이곳에 구현
{
	if (Node==NULL) return;

	while (1) {  
		Delete(Node, mode);         // 여기
		if (Node==NULL) break;
	}
}

extern void CancelRing(CNode* Node)
{
		 //링을 푼다
	CNode* s=Node;
	CNode* e;
	CNode* tmp=s;

	while (1) {
		if (tmp->nxt==s || tmp->nxt==NULL) {e=tmp; break;}
		tmp=tmp->nxt;
	}

	e->nxt=NULL;
	s->pre=NULL;
}


/********************************************************
*********************************************************
	IMPLEMENTATION of Class
*********************************************************
********************************************************/
//U X V
//Uy*Vz - Uz*Vy
//Uz*Vx - Ux*Vz
//Ux*Vy - Uy*Vx
CVector CVector::cross(CVector A)
{
	CVector B;
	B.a[0]= a[1] * A.a[2] - a[2] * A.a[1];
	B.a[1]= a[2] * A.a[0] - a[0] * A.a[2];
	B.a[2]= a[0] * A.a[1] - a[1] * A.a[0];
	return B;
}

void CFace::Set()
{
	Set(v0,v1,v2);
}
void CFace::Set2()
{
	Set2(v0,v1,v2);
}
void CFace::Set2(CVertex* v0, CVertex* v1, CVertex* v2)
{
	CVector a1(v1->x- v0->x, v1->y - v0->y, v1->z - v0->z);
	CVector b1(v2->x- v0->x, v2->y - v0->y, v2->z - v0->z);
	CVector c1(v2->x- v1->x, v2->y - v1->y, v2->z - v1->z);
	double a = sqrt(a1.a[0]*a1.a[0] + a1.a[1]*a1.a[1] + a1.a[2]*a1.a[2]);
	double b = sqrt(b1.a[0]*b1.a[0] + b1.a[1]*b1.a[1] + b1.a[2]*b1.a[2]);
	double c = sqrt(c1.a[0]*c1.a[0] + c1.a[1]*c1.a[1] + c1.a[2]*c1.a[2]);
	mm[0] = (v0->x + v1->x + v2->x) / 3.0;
	mm[1] = (v0->y + v1->y + v2->y) / 3.0;
	mm[2] = (v0->z + v1->z + v2->z) / 3.0;
	if (a < b && a < c && a < line_width*3) {
		m[0] = (v1->x + v0->x)/2.0;
		m[1] = (v1->y + v0->y)/2.0;
		m[2] = (v1->z + v0->z)/2.0;
		mf[0] = v2->x; mf[1] = v2->y; mf[2] = v2->z;
		mb[0] = v0->x; mb[1] = v0->y; mb[2] = v0->z;
		mb2[0] = v1->x; mb[1] = v1->y; mb[2] = v1->z;
		seq = 0; line=linebefore=linenext = -1; 
	} else if (b < a && b < c && b < line_width*3) {
		m[0] = (v2->x + v0->x)/2.0;
		m[1] = (v2->y + v0->y)/2.0;
		m[2] = (v2->z + v0->z)/2.0;
		mf[0] = v1->x; mf[1] = v1->y; mf[2] = v1->z;
		mb[0] = v0->x; mb[1] = v0->y; mb[2] = v0->z;
		mb2[0] = v2->x; mb[1] = v2->y; mb[2] = v2->z;
		seq = 0; line=linebefore=linenext = -1; 
	} else if (c < a && c < b && c < line_width*3) {
		m[0] = (v1->x + v2->x)/2.0;
		m[1] = (v1->y + v2->y)/2.0;
		m[2] = (v1->z + v2->z)/2.0;
		mf[0] = v0->x; mf[1] = v0->y; mf[2] = v0->z;
		mb[0] = v1->x; mb[1] = v1->y; mb[2] = v1->z;
		mb2[0] = v2->x; mb[1] = v2->y; mb[2] = v2->z;
		seq = 0; line=linebefore=linenext = -1; 
	} else {
		seq = -1; line=linebefore=linenext = -1; 
	}
	d[0] = mf[0]-m[0];
	d[1] = mf[1]-m[1];
	d[2] = mf[2]-m[2];
	a = sqrt(d[0]*d[0]+d[1]*d[1]+d[2]*d[2]);
	d[0] /= a;
	d[1] /= a;
	d[2] /= a;
	d[3] = -d[0]*m[0] - d[1]*m[1] - d[2]*m[2];  // m 평면
	d[4] = -d[0]*mf[0] - d[1]*mf[1] - d[2]*mf[2];  // mf 평면
}
CFace::~CFace(void) 
{

	if (e0) {delete (CEdge *)e0; e0=NULL;};
	if (e1) {delete (CEdge *)e1; e1=NULL;};
	if (e2) {delete (CEdge *)e2; e2=NULL;};
	if (v0) {delete (CVertex *)v0; v0=NULL;};
	if (v1) {delete (CVertex *)v1; v1=NULL;};
	if (v2) {delete (CVertex *)v2; v2=NULL;};

	del_count_face++;
}

void CFace::Set(CVertex* v0, CVertex* v1, CVertex* v2)
{
	CVector vec0(v1->x- v0->x, v1->y - v0->y, v1->z - v0->z);	    // pkc 벡터연산
	CVector vec1(v2->x- v0->x, v2->y - v0->y, v2->z - v0->z);
	CVector nor = vec0.cross(vec1);
	n[0]= nor.X();
	n[1]= nor.Y();
	n[2]= nor.Z();
	double sqr = sqrt(n[0]*n[0]+n[1]*n[1]+n[2]*n[2]);
	n[0] /= sqr;
	n[1] /= sqr;
	n[2] /= sqr;
	mn[0] = n[0]; mn[1] = n[1]; mn[2] = n[2];
}

void CFace::Print(void) 
{
	log ("< face id %d \n",id);
}

CEdge::~CEdge()
{
	{delete (CVertex*)v0;v0=NULL;}
	{delete (CVertex*)v1;v1=NULL;}
	{delete (CFace*)f0;f0=NULL;}
	{delete (CFace*)f1;f1=NULL;}
	//if (Solid){delete Solid;Solid = NULL;}
}

void CEdge::Print(void)
{
	log("%f,%f,%f ~ %f,%f,%f \n",v0->x,v0->y,v0->z,v1->x,v1->y,v1->z);
}

void CVertex::Trans(double tx,double ty, double tz, double a)
{
	x += (tx);
	y += (ty);
	z += (tz);
}

void CVertex::RotateX(double angle, double a)
{
	angle = 3.1415926*angle/180.0;  // angle->rad
	double fz = z, fy = y;
	double fcos = cos(angle);
	double fsin = sin(angle);
	y = fy*fcos - fz*fsin;
	z = fy*fsin + fz*fcos;
}

void CVertex::RotateY(double angle, double a)
{
	angle = 3.1415926*angle/180.0;
	double fx = x, fz = z;
	double fcos = cos(angle);
	double fsin = sin(angle);
	x = fx*fcos + fz*fsin;
	z = -1.0*fx*fsin + fz*fcos;
}

void CVertex::RotateZ(double angle, double a)
{
	angle = 3.1415926*angle/180.0;
	double fx = x, fy = y;
	double fcos = cos(angle);
	double fsin = sin(angle);
	x = fx*fcos - fy*fsin;
	y = fx*fsin + fy*fcos;
}

void CVertex::Scale(int isx,int isy, int isz,double a)
{
	if (isx) x *= a;
	if (isy) y *= a;
	if (isz) z *= a;
}

bool CVertex::IsEqual(CVertex *v,double es)
{
	double tmp;
	tmp = x - v->x; 
	if (tmp > -es &&  tmp < es) {
		tmp = y - v->y;
		if (tmp > -es &&  tmp < es) {
			tmp = z - v->z;
			if (tmp > -es &&  tmp < es) return true;
		}
	}
	return false;
}

CVertex CVertex::operator=(CVertex A)
{
	x = A.x;
	y = A.y;
	z = A.z;
	return *this;
}

CVertex::~CVertex(void)
{
	if (Indx) ClearNode((CNode*&)Indx, 14);   // CEdgeIndex = 14
	del_count_vertex++;
}

void CVertex::Print(void)
{
	log("Vertex id %d : %lf %lf %lf \n",id,x,y,z);
}

CSolid::CSolid()
{
	body = this; // 1
	type = 1;
	Face=NULL; Edge=NULL; Vertex=NULL; Hole=NULL; errorEdge=NULL;
	Nonmanifold = false;
	//FileName = PathName = "";
	for (int i=0;i<VTABLE_SIZE;i++) {
		for (int j=0;j<VTABLE_SIZE;j++) {
			for (int k=0;k<VTABLE_SIZE;k++) {
				vTable[i][j][k] = NULL;
			}
		}
	}
	Face = NULL;            // Face 모음
	Edge = NULL;            // Edge 모음
	Vertex = NULL;          // Vertex 모음
	errorEdge = NULL;       // 에러 있는 edge 모음
	errorNormalEdge = NULL; // 에러 있는 edge 모음
	Hole = NULL;            // Hole Loop 모음
	linenum = 0;
	currentline = -1;
}

CSolid::~CSolid(void)
{
	/**/
	for (int i=0;i<VTABLE_SIZE;i++) {
		for (int j=0;j<VTABLE_SIZE;j++) {
			for (int k=0;k<VTABLE_SIZE;k++) {
				ClearNode((CNode*&)vTable[i][j][k], 15);    // CVertexIndex = 15
			}
		}
	}
	//FileName = PathName = "";

	if (Vertex) ClearNode((CNode*&)Vertex, 12);	           // CVertex = 12
	if (Edge) ClearNode((CNode*&)Edge, 13);	               // CEdge = 13
	if (Face) ClearNode((CNode*&)Face, 11);	               // CFace = 11
	if (Hole) ClearNode((CNode*&)Hole, 16);                // CEdgeLoop = 16
	if (errorEdge) ClearNode((CNode*&)errorEdge, 14);         // CEdgeIndex = 14
	if (errorNormalEdge) ClearNode((CNode*&)errorNormalEdge, 14);   // CEdgeIndex = 14
	/**/
}

void CSolid::MoveToCenter()
{
	if (Face == NULL) return;
	double x,y,z;
	//x = (Max[0]+Min[0])/-2.0;
	//y = (Max[1]+Min[1])/-2.0;
	x = -Min[0];
	y = -Min[1];
	z = -Min[2];
	
	if (Vertex == NULL) {
		GoFirst((CNode*&)Face);
		while (1) {
			Face->v0->Trans(x,y,z);
			Face->v1->Trans(x,y,z);
			Face->v2->Trans(x,y,z);
			if (Face->nxt == NULL) break;
			else Face = (CFace*)Face->nxt;
		}
	} else {
		GoFirst((CNode*&)Vertex);
		while (1) {
			Vertex->Trans(x,y,z);
			if (Vertex->nxt == NULL) break;
			else Vertex = (CVertex*)Vertex->nxt;
		}
	}
	Max[0] += x;
	Min[0] += x;
	Max[1] += y;
	Min[1] += y;
	Max[2] += z;
	Min[2] += z;
}

CSlice::~CSlice(void)
{
	//if (loop) ClearNode((CNode*&)loop);
	if (loop) {
		CLoop *tmp;
		GoLast((CNode*&)loop);
		while (1) {
			tmp = (CLoop*)loop->pre;
			delete (CLoop*)loop;
			loop = NULL;
			loop = tmp;
			if (loop == NULL) break;
		}
	}
}

CLoop::~CLoop(void)
{
	//if (h) ClearNode((CNode*&)h);
	if (h) {
		CHalfedge *tmp;
		GoLast((CNode*&)h);
		while (1) {
			tmp = (CHalfedge*)h->pre;
			delete (CHalfedge*)h;h=NULL;
			h = tmp;
			if (h == NULL) break;
		}
	}
}

CHalfedge::~CHalfedge(void)
{
	//if (Solid) {delete Solid; Solid=NULL;}
	if (v0) {delete (CVertex*)v0; v0=NULL;}
	if (v1) {delete (CVertex*)v1; v1=NULL;}
}

CEdgeLoop::~CEdgeLoop(void)
{
	if (eindx) ClearNode((CNode*&)eindx, 14);   // CEdgeIndex = 14
}

void CHalfedge::Print(void)
{
	log("Halfedge id %d : %d , %d \n", id,v0->id, v1->id);
}

double CVector::DotP(CVector v)
{
	return (a[0]*v.a[0] +a[1]*v.a[1]+a[2]*v.a[2]);
}

double CVector::DotP(CVector *v)
{
	return (a[0]*v->a[0] + a[1]*v->a[1] + a[2]*v->a[2]);
}

double CVector::DotP(CVertex *v)
{
	return (a[0]*v->x + a[1]*v->y + a[2]*v->z);
}

CVector CVector::operator=(CVector A)
{
	a[0] = A.a[0];
	a[1] = A.a[1];
	a[2] = A.a[2];
	a[3] = A.a[3];
	return *this;
}

void CVector::Normalize(void)
{
	double dist = sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]);
	if (dist!=0) {
		a[0] /= dist;
		a[1] /= dist;
		a[2] /= dist;
		a[3] = 1;
	}
}

void CVector::Set(double x, double y, double z)
{
	a[0] = x;
	a[1] = y;
	a[2] = z;
}

void CVector::RotateX(double angle)
{
	angle = 3.1415926*angle/180.0;
	double fz = a[2], fy = a[1];
	double fcos = cos(angle);
	double fsin = sin(angle);
	a[1] = fy*fcos - fz*fsin;
	a[2] = fy*fsin + fz*fcos;
}

void CVector::RotateY(double angle)
{
	angle = 3.1415926*angle/180.0;
	double fx = a[0], fz = a[2];
	double fcos = cos(angle);
	double fsin = sin(angle);
	a[0] = fx*fcos + fz*fsin;
	a[2] = -1.0*fx*fsin + fz*fcos;
}

void CVector::RotateZ(double angle)
{
	angle = 3.1415926*angle/180.0;
	double fx = a[0], fy = a[1];
	double fcos = cos(angle);
	double fsin = sin(angle);
	a[0] = fx*fcos - fy*fsin;
	a[1] = fx*fsin + fy*fcos;
}

CStep::~CStep(void)
{
	if (polygon) ClearNode((CNode*&)polygon, 17);    // CLoop = 17
	if (hole) ClearNode((CNode*&)hole, 2);       // CSlice = 2
}

bool CVector::operator!=(CVector A)
{
	CVector tmpv1(a[0],a[1],a[2]);
	CVector tmpv2(A.a[0],A.a[1],A.a[2]);
	tmpv1.Normalize();
	tmpv2.Normalize();
	if (tmpv1.a[0] - tmpv2.a[0] < 0.0000001 &&
		tmpv1.a[0] - tmpv2.a[0] > -0.0000001)
		if (tmpv1.a[1] - tmpv2.a[1] < 0.0000001 &&
			tmpv1.a[1] - tmpv2.a[1] > -0.0000001)
			if (tmpv1.a[2] - tmpv2.a[2] < 0.0000001 &&
				tmpv1.a[2] - tmpv2.a[2] > -0.0000001)
				return false;
	return true;
}
