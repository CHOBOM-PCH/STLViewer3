//#pragma once
#include "stdafx.h"
#include "Solid.h"
#ifndef CALC


#define	COMPLETED					1
#define SOLID_NOT_EXIST			-1
#define CANCEL						-2
#define STEP_SIZE_ZERO			2
#define EDGE_NO_ERROR			0
#define EDGE_NORMAL_ERROR	1
#define EDGE_HOLE					2
#define EDGE_NON_MANIFOLD	3

// Default Color ����
const	float color_red[]={1, 0, 0, 1};
const	float color_green[]={0 , 1, 0, 1.};
const	float color_blue[]={0, 0, 1, 1.0};
const	float color_wire[]={1.0f, 0.0f, 0.0f, 1.0f};
const	float color_yellow[]={1, 1, 0, 1.0};
const	float local_view[] = { 1.0 };


//CNode Class�� ���� �ܺ� �Լ� �����
extern void log(char* format, ...);							// log ���� �����
extern void MakeRing(CNode* CUR) ;						// Ring ������ �����
extern void Add(CNode*& CUR, CNode* NEW);			//����Ʈ�߰�
extern void Insert(CNode*& CUR, CNode* NEW);		//����Ʈ�߰�
extern bool Go(CNode*& TMP,int id);						// id�� �̵�
extern void Delete(CNode*& CUR, int mode, bool after=false);		//����
extern void PrintAll(CNode* CUR);							//log ��ο� ���
extern void Print(CNode* CUR);								//log�� ���
extern void GoFirst(CNode*& Node);						//����Ʈ�� ó������
extern void DeleteAll(CNode* CUR);							//��� ����
extern void MakeId(CNode* Node);							//����Ʈ�� id�ű�	
extern long CountNode(CNode * Node);					//����Ʈ�� ����
extern void ClearNode(CNode * & Node, int mode);					//��λ���
extern void CancelRing(CNode* Node);						//Ring���� ����
extern void GoLast(CNode*& Node);						//����Ʈ�� ����������


class CCalc
{
public:
	CWnd *pParent;

	CString  StlFileName;

	CSolid *Solid; //, *Upper, *Lower;    // pkc
	//CSolid *Edge, *Vertex, *Node;
	CSlice *Slice;
	//CStep *Step;

	CHalfedge *DepartH,*ScanH;

	CEdge* FindEdge(CVertex* v0, CVertex* v1);
	CVertex* FindVertex(CVertexIndex *vTable[VTABLE_SIZE][VTABLE_SIZE][VTABLE_SIZE],CVertex *Vertex,double x,double y,double z,double min[3],double max[3]);

	float color_net[4], color_cli[4], color_solid[4], color_back[4], color_select[4], color_line[4];
	float color_select1[4], color_select2[4], color_select3[4];
	float color_x[4],color_y[4],color_z[4],color_layout[4];
	float color_int[4];
	float ambient[4];
	float diffuse[4];
 	float specular[4];
	float position0[4],position1[4],position2[4];
	float emission[4];
	float shininess[1];
	float noshininess[1];

	
	int hole_count, normal_count,nonmanifold_count, over_count; //number of errors
	
	long edge_number;
	long vertex_number;
	long face_number;
	long stl_vertex_number;
	double ES, thickness, position;//View Section
	
	int stl_number;

	int fnew;    // pkc
	int seg_counter;
	int gsel;
	
	double max_size;
	double Min[3],Max[3],thin;
	CVertex Intersec[4],IntersecSave[4];
	
	CCalc(void);
	~CCalc(void);
	void SetDefaultColor(void);
	double RoundOff(double v);
	bool IsEqual(double a,double b);
	void SetMinMax();
	void AutoScale();

	void MakeVertexList(double es,int mode =1);
	void MakeEdgeList();

	void DeleteFaceOnly(CFace*& Face);

	int ReadStl(char* name, int mode = 0, double offset = 0);
	void ShowStl(void);
	void ShowStlwire(void);
	int  CCalc::FindLine(int line, int * sharp, int remove = 0);
	int  CCalc::FindEndPoint(int line, CVector* start, CVector* end);
	int  CCalc::ChangeLine(int line);
	int  CCalc::ChangeLines(int line1, int line2);
	int  CCalc::SumLines(int line1, int line2, int final);
	void forward_check(CFace * Fdx, CFace * Idx, int * fdir, int ii);
	void backward_check(CFace * Bdx, CFace * Idx, int * bdir, int ii);
	int  CCalc::Near(CVector * s, CVector * t0, CVector * t1, CVector * t2, CVector * t3, int first=1);
	int  CCalc::Near2(CVector * s0, CVector * s1, CVector * s2, CVector * t0, CVector * t1, CVector * t2, int first=0);
};

#define CALC
#endif
