/********************************************************
	<Class of Halfedge Solid Modeling>

*********************************************************/

#ifndef SOLID

class CNode;									  
class CVecter;
class CMatrix;
class CVertex;
class CEdge;
class CFace;
class CHalfedge;
class CSolid;
class CCalc;
class CLoop;
class CSlice;
class CFaceIndex;
class CEdgeIndex;
class CVertexIndex;
class CStep;

/********************************************************
	Define Edge Type
*********************************************************/
#define EDGE_NORMAL				1	//정상(f=2)
#define EDGE_HOLE					2	//홀(f=1)
#define EDGE_NONE_MANIFOLD	3   //Non-Manifold
#define EDGE_AUX					4	//평면
#define EDGE_ORIENTATION		5	//노말 에러
#define EDGE_OVERLAB				6	//겹침
#define EDGE_INTERSECT			7	//십자형 NONE_MANIFOLD
#define VTABLE_SIZE				16


#define pi_value 3.141597

/********************************************************
	Double Linked List
*********************************************************/
class CNode 
{
public:
	long id;
	CNode* pre;
	CNode* nxt;
	void * body;    // pkc add
	int    type;    // pkc add
	CNode(void) {id=-1, pre=NULL, nxt=NULL;}
//	~CNode(void) {delete pre;delete nxt;}
	CNode(long ID) {id=ID, pre=NULL, nxt=NULL;}
};

/********************************************************
	Vector Class
*********************************************************/
class CVector 
{
public:
	void Set(double x, double y, double z);
	CVector cross(CVector A);
	double a[4];
	CVector(double x,double y,double z) {a[0]=x, a[1]=y, a[2]=z, a[3]=1.0;}
	CVector(void) {a[0]=a[1]=a[2]=0.0; a[3]=1.0;}

	double DotP(CVector v);
	double DotP(CVector *v);
	double DotP(CVertex *v);
	CVector operator=(CVector A);
	bool operator!=(CVector A);
	void Normalize(void);

	void RotateX(double angle);
	void RotateY(double angle);
	void RotateZ(double angle);
	double X(void) {return a[0];}
	double Y(void) {return a[1];}
	double Z(void) {return a[2];}
};

/********************************************************
	4 by 4  Matrix Class
*********************************************************/
class CMatrix 
{
public:
	double a[4][4];
};

/********************************************************
	Index Table of Edge
*********************************************************/
class CEdgeIndex:public CNode                                      // 14
{
public:
	CEdge *e;
	CEdgeIndex(CEdge *E=NULL,int ID =-1) {
		body = this; // 14
		type = 14;
		id=ID;e=E;
	};
};

/********************************************************
	Index Table of Face
*********************************************************/
class CFaceIndex:public CNode                                          // 18
{
public:
	CFace *f;
	CFaceIndex(CFace *F=NULL,int ID =-1) {
		body = this; // 18
		type = 18;
		id=ID;f=F;};
};

/********************************************************
	Edge based Loop Class 
*********************************************************/
class CEdgeLoop:public CNode                                        // 16
{
public:
	bool IS_SELECTED;
	CVector normal;
	CEdgeIndex *eindx;
	CEdgeLoop(CEdgeIndex *E=NULL,int ID =-1) {
		body = this; // 16
		type = 16;
		id=ID;eindx=E;IS_SELECTED=false;};
	~CEdgeLoop();
};

/********************************************************
	Index Table of Vertex
*********************************************************/
class CVertexIndex:public CNode                                      // 15
{
public:
	CVertex *v;
	CVertexIndex(CVertex *V=NULL,int ID =-1) {
		body = this; // 15
		type = 15;
		id=ID;v=V;
	};
};

/********************************************************
	Class Vertex 
	Included Index Table of Edge 
*********************************************************/
class CVertex:public CNode                                           // 12
{
public:
	CFace *f;
	bool IsEqual(CVertex *v,double es);
	bool IS_SELECTED;
	double x,y,z;
	CEdgeIndex *Indx;
	CVertex(double X,double Y,double Z,long ID=-1,CFace *FACE=NULL) {
		body = this; // 12
		type = 12;
		x=X,y=Y,z=Z,id=ID;Indx=NULL;IS_SELECTED=false;f=FACE;
	}
	CVertex(void) {
		body = this; // 12
		type = 12;
		x=y=z=-1.0;Indx=NULL;IS_SELECTED = false;
	}
	~CVertex(void);
	void Trans(double tx,double ty, double tz, double a = 1.0);
	void RotateX(double angle, double a = 1.0);
	void RotateY(double angle, double a = 1.0);
	void RotateZ(double angle, double a = 1.0);
	void Scale(int isx,int isy, int isz,double a);
	void Print(void);
	CVertex operator=(CVertex A);
};

/********************************************************
	Class Halfedge
	v0 -> v1  direction
*********************************************************/
class CHalfedge:public CNode                // 3
{
public:
	CVertex* v0;   
	CVertex* v1;
//	CSolid* Solid;//back polonger
	CHalfedge(CVertex* V0 = NULL, CVertex* V1 = NULL,long ID=-1) {
		body = this; // 3
		type = 3;
		v0=V0, v1=V1,id=ID/*,Solid=NULL*/;
	}
	~CHalfedge(void);
	void Print(void);
};

/********************************************************
	Class Edge
	Linked 2 Face and 2 Vertex
	edge_type로 Edge의 타입을 구분한다
********************************************************/
class CEdge:public CNode                                             // 13
{
public:
	bool IS_SELECTED;
	 ~CEdge(void);
	CVertex *v0, *v1;
	CFace *f0, *f1;
//	CFaceIndex *ErrorFace;
	int edge_type;
//	CSolid* Solid;	//back polonger 
	CEdge(long ID=-1) {
		body = this; // 13
		type = 13;
		v0=v1=NULL;f0=f1=NULL,id=ID;edge_type=0;IS_SELECTED=false;}
	CEdge(CVertex* V0, CVertex* V1,long ID=-1) {
		body = this; // 13
		type = 13;
		v0=V0, v1=V1, id=ID;f0=NULL;f1=NULL;edge_type=0;IS_SELECTED=false;};
	void Print(void);
};

/********************************************************
	Class Face have Normal Vector
	Linked 3 Edge and 3 Vertex
	Set()으로 Normal Vector 계산
********************************************************/
class CFace : public CNode                                               // 11
{
public:
	void Set();
	long loop_num;
	void Set(CVertex* v0, CVertex* v1, CVertex* v2);
	void Set2();
	void Set2(CVertex* v0, CVertex* v1, CVertex* v2);
	CVertex *v0, *v1, *v2; // loop
	CEdge *e0, *e1 , *e2;
	double max_z,min_z;
	bool is_error;
	int  seq, line, subseq, linenext, linebefore, gsel;

	CFace(CVertex *V0,CVertex *V1,CVertex *V2,long ID=-1)           // CFace::CFace
	{
		body = this; // 11
		type = 11;
		v0 = V0, v1 = V1, v2 = V2, id = ID;
		e0 = e1 = e2 = NULL;
		max_z=-99999,min_z=99999;
		is_error=false;
		line = -1; gsel = 0;
	}
	CFace(long ID=-1) {
		body = this; // 11
		type = 11;
		v0=v1=v2=NULL; e0 = e1 = e2 = NULL; id=ID,max_z=-99999,min_z=99999;is_error=false;
		line = -1; gsel = 0;
	}

	~CFace(void);
	double n[3];             // normal 

	double m[3];             // 짧은 선분의 중점
	double mf[3];            // front 점
	double mb[3];            // back 점
	double mb2[3];           // back 점
	double mn[3];
	double mm[3];           // 무게중심
	double d[5];
	void Print(void);
};

/********************************************************
	Class Loop 
	dir : Direction CCW = 1 (External)
					CW = 0 (Internal)
	Halfedge로 구성됨
********************************************************/
class CLoop:public CNode                                        // 17
{
public:
	CVector n;
	int dir;
	CHalfedge *h;
	CLoop(CHalfedge* H,int Dir = 0,long ID=-1) {
		body = this; // 17
		type = 17;
		h=H;dir=Dir;id = ID;}
	CLoop(long ID = -1) {
		body = this; // 17
		type = 17;
		h=NULL;dir=1;id=ID;}
	~CLoop(void);
};
/********************************************************
	Class Slice 
	각 층의 단면 정보를 링크드 리스트의 형태로 갖고 있다
	각층마다 여러개의 루프를 loop 리스트에 갖고 있다
********************************************************/
class CSlice:public CNode                   // 2
{
public:
	double n[3];
	CLoop* loop;
	CSlice(CLoop *l = NULL, long ID = -1) {
		body = this; // 2
		type = 2;
		loop=l;id=ID;
	}
	~CSlice(void);
};

/********************************************************
	Class Solid
	STL로 부터 솔리드 데이타 스트럭쳐를 저장한다.
	Face List / Edge List / Vertex List
	Index Table of Vertex 로 구성
********************************************************/
class CSolid :public CNode                // 1           // pkc   public CNode
{
public:
	HDC hDC;
	HGLRC hRC;
	bool Nonmanifold; // Non-Manifold 인가
	void MoveToCenter(); //모델을 원점에 위치
	
	long face_number, vertex_number, edge_number;
	CVertexIndex* vTable[VTABLE_SIZE][VTABLE_SIZE][VTABLE_SIZE];
	double Max[3],Min[3];
	CFace* Face;//Face 모음
	CEdge* Edge;// Edge 모음
	CVertex* Vertex;// Vertex 모음
	CEdgeIndex* errorEdge; // 에러 있는 edge 모음
	CEdgeIndex* errorNormalEdge; // 에러 있는 edge 모음
	CEdgeLoop* Hole;// Hole Loop 모음
	char FileName[256], PathName[256];
	CSolid(void);
	~CSolid(void);
	bool IS_SELECTED;
	int  base, linenum, currentline;  // pkc
};

class CStep
{
public:
	CSlice *hole;
	CLoop *polygon;
	CStep(CSlice *HOLE = NULL, CLoop *POLYGON = NULL) {hole = HOLE, polygon = POLYGON;};
	~CStep();
};

#define SOLID
#endif
