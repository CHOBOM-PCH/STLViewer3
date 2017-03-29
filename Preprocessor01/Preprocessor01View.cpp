
// Preprocessor01View.cpp : CPreprocessor01View 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "Preprocessor01.h"
#endif

#include "Preprocessor01Doc.h"
#include "Preprocessor01View.h"
#include "Calc.h"
#include "MainFrm.h"
#include <math.h>

#include <Windows.h>


#include "Transform.h"            // Transform.cpp 에서


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

mat4 A, B, BB, C, D, E, EE, F, G, H, S, T, U, GF, HGF, DC, V, W;
double focus = 0.15;
double focus2 = focus / sqrt(2.0);
double shift_1 = 0.15;
double down_2 = 0.15;
double shift_3 = 0.05;
double down_4 = 0.1;



double mindist = 1.0;                // 1.0
extern double mindist2;              // 0.5

double con_factor = 200.0;            //  이 곳의 1 이 200 mm  이고 따라서 VSCALE 도 200
double z_offset = 200.0;              //  이 값이 100 이면 z=0 이 100 mm 위에 있다 ---> ZOFF 는 -0.5

extern double line_width;
//double line_width = 0.0015;       // 0.001 = 1u -> 0.0015      0.0001 = 100n  0.00001 = 10n
extern double line_gap;

#define		INI_FILE_PATH		_T(".\\VCam.ini")
char gcode_file[50];

int near2(double x1, double x2, double y1, double y2, double z1, double z2, double * dist)
{
	*dist = sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) + (z1-z2)*(z1-z2));
	if (*dist < 0.0001) return 1;
	else return 0;
}
mat4 Trans(vec3& axis)
{
	mat4 res;
	res[0][0] = 1.0; res[0][1] = 0.0; res[0][2] = 0.0; res[0][3] = axis.x;
	res[1][0] = 0.0; res[1][1] = 1.0; res[1][2] = 0.0; res[1][3] = axis.y;
	res[2][0] = 0.0; res[2][1] = 0.0; res[2][2] = 1.0; res[2][3] = axis.z;
	res[3][0] = 0.0; res[3][1] = 0.0; res[3][2] = 0.0; res[3][3] = 1.0;
	return res;
}
mat4 Rotatez(double degree)
{
	mat4 res;
	double rad = (degree / 180.00) * 3.141592654f;
	double cos_ = cos(rad);
	double sin_ = sin(rad);
	res[0][0] = cos_; res[0][1] = -sin_; res[0][2] = 0.0; res[0][3] = 0.0;
	res[1][0] = sin_; res[1][1] =  cos_; res[1][2] = 0.0; res[1][3] = 0.0;
	res[2][0] =  0.0; res[2][1] =   0.0; res[2][2] = 1.0; res[2][3] = 0.0;
	res[3][0] =  0.0; res[3][1] =   0.0; res[3][2] = 0.0; res[3][3] = 1.0;
	return res;
}
mat4 Rotatey(double degree)
{
	mat4 res;
	double rad = (degree / 180.00) * 3.141592654f;
	double cos_ = cos(rad);
	double sin_ = sin(rad);
	res[0][0] =  cos_; res[0][1] = 0.0; res[0][2] = sin_; res[0][3] = 0.0;
	res[1][0] =   0.0; res[1][1] = 1.0; res[1][2] =  0.0; res[1][3] = 0.0;
	res[2][0] = -sin_; res[2][1] = 0.0; res[2][2] = cos_; res[2][3] = 0.0;
	res[3][0] =   0.0; res[3][1] = 0.0; res[3][2] =  0.0; res[3][3] = 1.0;
	return res;
}
mat4 Rotatex(double degree)
{
	mat4 res;
	double rad = (degree / 180.00) * 3.141592654f;
	double cos_ = cos(rad);
	double sin_ = sin(rad);
	res[0][0] = 1.0; res[0][1] =  0.0; res[0][2] =   0.0; res[0][3] = 0.0;
	res[1][0] = 0.0; res[1][1] = cos_; res[1][2] = -sin_; res[1][3] = 0.0;
	res[2][0] = 0.0; res[2][1] = sin_; res[2][2] =  cos_; res[2][3] = 0.0;
	res[3][0] = 0.0; res[3][1] =  0.0; res[3][2] =   0.0; res[3][3] = 1.0;
	return res;
}
mat4 Rotate(double degree, vec3& axis)                                          // axis 가 001 인경우
{
	mat4 res;
	double rad = (degree / 180.00) * 3.141592654f;
	double cos_ = cos(rad);
	double sin_ = sin(rad);
	double size = sqrt(axis.x*axis.x + axis.y*axis.y + axis.z*axis.z);
	double x = axis.x / size;
	double y = axis.y / size;
	double z = axis.z / size;         // 정규화
	double xs_ = x * sin_;
	double ys_ = y * sin_;
	double zs_ = z * sin_;
	double oneMinusCos = 1.0 - cos_;

	res[0][0] = (oneMinusCos * x * x) + cos_;                   // cos
	res[0][1] = (oneMinusCos * x * y) - (zs_);                  // -sin
	res[0][2] = (oneMinusCos * x * z) + (ys_);
	res[0][3] = 0;

	res[1][0] = (oneMinusCos * x * y) + (zs_);
	res[1][1] = (oneMinusCos * y * y) + cos_;
	res[1][2] = (oneMinusCos * y * z) - (xs_);
	res[1][3] = 0;

	res[2][0] = (oneMinusCos * x * z) - (ys_);
	res[2][1] = (oneMinusCos * y * z) + (xs_);
	res[2][2] = (oneMinusCos * z * z) + cos_;
	res[2][3] = 0;

	res[3][0] = 0.0; res[3][1] = 0.0; res[3][2] = 0.0; res[3][3] = 1.0;

	return res;
}
void mv_cross4(vec4& res, mat4& m, vec4& v)
{
	res.x = m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z + m[0][3]*v.w;
	res.y = m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z + m[1][3]*v.w;
	res.z = m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z + m[2][3]*v.w;
	res.w = m[3][0]*v.x + m[3][1]*v.y + m[3][2]*v.z + m[3][3]*v.w;
}
int mm_inverse4(mat4& p, mat4& q)         // column major
{
    double m[16], inv[16], det, tmp[12];
	double Result[4][4];
    int i, j;
	m[0]  = p[0][0]; m[4]  = p[0][1]; m[8]  = p[0][2]; m[12] = p[0][3];
	m[1]  = p[1][0]; m[5]  = p[1][1]; m[9]  = p[1][2]; m[13] = p[1][3];
	m[2]  = p[2][0]; m[6]  = p[2][1]; m[10] = p[2][2]; m[14] = p[2][3];
	m[3]  = p[3][0]; m[7]  = p[3][1]; m[11] = p[3][2]; m[15] = p[3][3];

	tmp[0]  = m[10]*m[15];
	tmp[1]  = m[11]*m[14];
	tmp[2]  = m[9] *m[15];
	tmp[3]  = m[11]*m[13];
	tmp[4]  = m[9] *m[14];
	tmp[5]  = m[10]*m[13];
	tmp[6]  = m[8] *m[15];
	tmp[7]  = m[11]*m[12];
	tmp[8]  = m[8] *m[14];
	tmp[9]  = m[10]*m[12];
	tmp[10] = m[8] *m[13];
	tmp[11] = m[9] *m[12];
    Result[0][0]  = tmp[0]*m[5] + tmp[3]*m[6] + tmp[4]*m[7];
    Result[0][0] -= tmp[1]*m[5] + tmp[2]*m[6] + tmp[5]*m[7];
    Result[0][1]  = tmp[1]*m[4] + tmp[6]*m[6] + tmp[9]*m[7];
    Result[0][1] -= tmp[0]*m[4] + tmp[7]*m[6] + tmp[8]*m[7];
    Result[0][2]  = tmp[2]*m[4] + tmp[7]*m[5] + tmp[10]*m[7];
    Result[0][2] -= tmp[3]*m[4] + tmp[6]*m[5] + tmp[11]*m[7];
    Result[0][3]  = tmp[5]*m[4] + tmp[8]*m[5] + tmp[11]*m[6];
    Result[0][3] -= tmp[4]*m[4] + tmp[9]*m[5] + tmp[10]*m[6];
    Result[1][0]  = tmp[1]*m[1] + tmp[2]*m[2] + tmp[5]*m[3];
    Result[1][0] -= tmp[0]*m[1] + tmp[3]*m[2] + tmp[4]*m[3];
    Result[1][1]  = tmp[0]*m[0] + tmp[7]*m[2] + tmp[8]*m[3];
    Result[1][1] -= tmp[1]*m[0] + tmp[6]*m[2] + tmp[9]*m[3];
    Result[1][2]  = tmp[3]*m[0] + tmp[6]*m[1] + tmp[11]*m[3];
    Result[1][2] -= tmp[2]*m[0] + tmp[7]*m[1] + tmp[10]*m[3];
    Result[1][3]  = tmp[4]*m[0] + tmp[9]*m[1] + tmp[10]*m[2];
    Result[1][3] -= tmp[5]*m[0] + tmp[8]*m[1] + tmp[11]*m[2];
    tmp[0]  = m[2]*m[7];
    tmp[1]  = m[3]*m[6];
    tmp[2]  = m[1]*m[7];
    tmp[3]  = m[3]*m[5];
    tmp[4]  = m[1]*m[6];
    tmp[5]  = m[2]*m[5];
    tmp[6]  = m[0]*m[7];
    tmp[7]  = m[3]*m[4];
    tmp[8]  = m[0]*m[6];
    tmp[9]  = m[2]*m[4];
    tmp[10] = m[0]*m[5];
    tmp[11] = m[1]*m[4];
    Result[2][0]  = tmp[0]*m[13]  + tmp[3]*m[14]  + tmp[4]*m[15];
    Result[2][0] -= tmp[1]*m[13]  + tmp[2]*m[14]  + tmp[5]*m[15];
    Result[2][1]  = tmp[1]*m[12]  + tmp[6]*m[14]  + tmp[9]*m[15];
    Result[2][1] -= tmp[0]*m[12]  + tmp[7]*m[14]  + tmp[8]*m[15];
    Result[2][2]  = tmp[2]*m[12]  + tmp[7]*m[13]  + tmp[10]*m[15];
    Result[2][2] -= tmp[3]*m[12]  + tmp[6]*m[13]  + tmp[11]*m[15];
    Result[2][3]  = tmp[5]*m[12]  + tmp[8]*m[13]  + tmp[11]*m[14];
    Result[2][3] -= tmp[4]*m[12]  + tmp[9]*m[13]  + tmp[10]*m[14];
    Result[3][0]  = tmp[2]*m[10]  + tmp[5]*m[11]  + tmp[1]*m[9];
    Result[3][0] -= tmp[4]*m[11]  + tmp[0]*m[9]   + tmp[3]*m[10];
    Result[3][1]  = tmp[8]*m[11]  + tmp[0]*m[8]   + tmp[7]*m[10];
    Result[3][1] -= tmp[6]*m[10]  + tmp[9]*m[11]  + tmp[1]*m[8];
    Result[3][2]  = tmp[6]*m[9]   + tmp[11]*m[11] + tmp[3]*m[8];
    Result[3][2] -= tmp[10]*m[11] + tmp[2]*m[8]   + tmp[7]*m[9];
    Result[3][3]  = tmp[10]*m[10] + tmp[4]*m[8]   + tmp[9]*m[9];
    Result[3][3] -= tmp[8]*m[9]   + tmp[11]*m[10] + tmp[5]*m[8];
	det = m[0]*Result[0][0] + m[1]*Result[0][1] + m[2]*Result[0][2] + m[3]*Result[0][3];

    if (det == 0)
        return 0;

    det = (1.0 / det);

    for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			q[i][j] = Result[i][j]*det;
		}
	}
    return 1;
}
vec4 Calc_inv(double ang1, double ang2, vec4& vec)
{
	/*
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			V[i][j] = 0;
		}
	}
	V[0][0] = V[1][1] = V[2][2] = V[3][3] = 0.1;
	mm_inverse4(V, W);
	*/
	/*
	vec3 ax = vec3(1, 0, 0);
	B = Rotate(90, ax);
	B = Rotatex(90);
	ax = vec3(0, 1, 0);
	B = Rotate(90, ax);
	B = Rotatey(90);
	ax = vec3(0, 0, 1);
	B = Rotate(90, ax);
	B = Rotatez(90);
	*/

	vec4 targ, targ2;
	vec4 org = vec4(0.0, 0.0, 0.0, 1.0);
	vec3 axis = vec3(0.0, 0.0, 1.0); // z
	B = Rotate(ang1, axis);

	axis = vec3(1.0, 0.0, 0.0); // x
	E = Rotate(ang2, axis);

	V = GF*E*DC*B;           // 추가

	/*
	//W = V._inverse();       // a
	W = glm::inverse(V);      // b

	//double a = V[0][0];
	//double b = V[0][1];
	//double c = V[0][2];
	//double d = V[0][3];
	//mm_inverse4(V, W);      // c
	S = W*V;
#ifdef _DEBUG
	TRACE("V\n");
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			TRACE("m[%d][%d] = %.4f   ", i, j, V[i][j]);
		}
		TRACE("\n");
	}
	TRACE("W\n");
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			TRACE("m[%d][%d] = %.4f   ", i, j, W[i][j]);
		}
		TRACE("\n");
	}
#endif
	mv_cross4(targ, W, vec);          //W = K_inv
	mv_cross4(targ2, V, targ);        //V = K          targ2 == vec  여기까지 맞음
	axis = vec3(targ.x, targ.y, targ.z);
	A = Trans(axis);
#ifdef _DEBUG
	TRACE("A\n");
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			TRACE("m[%d][%d] = %.4f   ", i, j, A[i][j]);
		}
		TRACE("\n");
	}
#endif
	S = V*A;       // 원하는 연산
	S = A*V;       // 이렇게 해야 원하는 연산
#ifdef _DEBUG
	TRACE("S\n");
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			TRACE("m[%d][%d] = %.4f   ", i, j, S[i][j]);
		}
		TRACE("\n");
	}
#endif
	mv_cross4(targ, S, org);
	*/

	mv_cross4(targ, V, org);
	targ = vec - targ;

	//axis = vec3(targ.x, targ.y, targ.z);
	//A = Trans(axis);
	//S = V*A;
	//mv_cross4(targ, S, org);
	return targ;

	//mm_inverse4(V, W);
	////vec.w = 0;
	//mv_cross4(targ, W, vec);       // 안되는 이유를 모르겟음  ---> 알 것 같음
	//return targ;

	// vec      = V * A * org     ---> A * org = W * vec
	// targ     = V * I * org
	// vec-targ = 
}
//
//            | x |                       | fix.x |
// A * org =  | y |  =  inv(GF*E*DC*B) *  | fix.y |
//            | z |       (  W  )         | fix.z |
//            | 1 |                       |  1    |
//
vec4 Calc_noz(double vxxx, double vyyy, double vzzz, double ang1, double ang2)
{
	vec3 axis = vec3(vxxx, vyyy, vzzz);
	A = Trans(axis);

	axis = vec3(0.0, 0.0, 1.0); // z
	B = Rotate(ang1, axis);

	axis = vec3(1.0, 0.0, 0.0); // x
	E = Rotate(ang2, axis);

	U = GF*E*DC*B*A;

	vec4 org = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 target;

	mv_cross4(target, U, org);
	return target;
}
vec4 Calc_foc(double vxxx, double vyyy, double vzzz, double ang1, double ang2)
{
	//mat4 A, B, C, D, E, F, G, H, S, T;

	vec3 axis = vec3(vxxx, vyyy, vzzz);
	A = Trans(axis);

	axis = vec3(0.0, 0.0, 1.0); // z
	B = Rotate(ang1, axis);

	/*
	axis = vec3(0.15, 0.0, 0.0);
	C = Trans(axis);

	axis = vec3(0.0, 0.0, -0.15);
	D = Trans(axis);
	*/

	axis = vec3(1.0, 0.0, 0.0); // x
	E = Rotate(ang2, axis);

	/*
	axis = vec3(0.05, 0.0, 0.0);
	F = Trans(axis);

	axis = vec3(0.0, 0.0, -0.1);
	G = Trans(axis);

	axis = vec3(0.0, 0.0, -focus);          // 0.15
	H = Trans(axis);
	*/

	T = HGF*E*DC*B*A;
	U = GF*E*DC*B*A;
	V = GF*E*DC*B;           // 추가
	//W = inverse(V);
	//mm_inverse4(V, W);

	S = DC*A;

	vec4 org = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 target, target2, org2;

// 같은 결과
#if 0
	mv_cross4(target, S, org);
	org2 = target;                     // B 를 제외하고 E 전       org2 =          DCB(0)Au

	mv_cross4(target, HGF, org2);      // B 를 제외하고 E 후       target = HGFE(0)DCB(0)Au

	mv_cross4(target2, E, (target-org2));    // E 적용             target2 = E(s) * (HGFE(0)DCB(0)Au - DCB(0)Au)

	target = target2 + org2;           // final                    target = E(s) * (HGFE(0)DCB(0)Au - DCB(0)Au) + DCB(0)Au
                                       //                                 = E(s)HGFDCAu - E(s)DCAu + DCAu
	                                   //                                 = (E(s)HGF - E(s) + I) * DCAu
	//return target;

	mv_cross4(target2, A, org);

	org = target2;                     // B 전                     org = Au

	mv_cross4(target2, B, (target-org));    //                     target2 = B(t) * ( (EHGF-E+I)*DCAu - Au )
                                            //                             = 
	target = target2 + org;                 //                     target = 
#else
	//T = H*G*F*E*D*C*B*A;
	//T = HGF*E*DC*B*A;
	mv_cross4(target, T, org);
#endif

	return target;

	//mv_cross4(target, glm::affineInverse(T), org);
	//mv_cross4(target, glm::inverseTranspose(T), org);
	//org = vec4(0.0, 0.0, 0.0, 1.0);
}
void Calc_init()
{
	vec3 axis;

	axis = vec3(shift_1*con_factor, 0.0, 0.0);      // 0.15
	C = Trans(axis);

	axis = vec3(0.0, 0.0, -down_2*con_factor);      // 0.15   결국  -0.15
	D = Trans(axis);

	axis = vec3(shift_3*con_factor, 0.0, 0.0);       // 0.05
	F = Trans(axis);

	axis = vec3(0.0, 0.0, -down_4*con_factor);        // 0.1  결국  -0.1
	G = Trans(axis);

	axis = vec3(0.0, 0.0, -focus*con_factor);         // 0.15  결국  -0.15
	H = Trans(axis);

	DC = D*C;
	GF = G*F;
	HGF = H*GF;  // H*G*F


	//
	//vec4 target = Calc(0, 0, 0, 0, 0);
	//target = Calc(0, 0, 0, 0, 180);
	//target = Calc(1, 0, 0, 0, 0);
	//axis = vec3(1.0, 1.0, 0.0); // z
	//B = Rotate(180, axis);
	//vec4 org = vec4(1.0, 0.0, 0.0, 1.0), target;
	//mv_cross4(target, B, org);
}





// CPreprocessor01View

IMPLEMENT_DYNCREATE(CPreprocessor01View, CView)
//IMPLEMENT_DYNCREATE(CPreprocessor01View, CPreviewView)

BEGIN_MESSAGE_MAP(CPreprocessor01View, CView)
//BEGIN_MESSAGE_MAP(CPreprocessor01View, CPreviewView)

	// OpenGL 및 initializer
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	

	// Mainframe and toolbar
	ON_COMMAND(ID_FILE_OPEN,               &CPreprocessor01View::OnFileOpen)
	ON_COMMAND(ID_FILE_NEW,                &CPreprocessor01View::OnFileNew)
	ON_COMMAND(ID_VIEW_GRID,               &CPreprocessor01View::OnViewGrid)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GRID,     &CPreprocessor01View::OnUpdateViewGrid)

	ON_COMMAND(ID_ROTATION_MODE,           &CPreprocessor01View::OnRotationMode)
	ON_UPDATE_COMMAND_UI(ID_ROTATION_MODE, &CPreprocessor01View::OnUpdateRotationMode)
	ON_COMMAND(ID_TRANS_MODE,              &CPreprocessor01View::OnTransMode)
	ON_UPDATE_COMMAND_UI(ID_TRANS_MODE,    &CPreprocessor01View::OnUpdateTransMode)



	ON_COMMAND(ID_FILE_PRINT,              &CView::OnFilePrint)                  // 표준 인쇄 명령입니다.   // 여기에 있음
	ON_COMMAND(ID_FILE_PRINT_DIRECT,       &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW,      &CView::OnFilePrintPreview)
	//ON_COMMAND(ID_FILE_PRINT,              &CPreviewView::OnFilePrint)                  // 표준 인쇄 명령입니다.   // 여기에 있음
	//ON_COMMAND(ID_FILE_PRINT_DIRECT,       &CPreviewView::OnFilePrint)
	//ON_COMMAND(ID_FILE_PRINT_PREVIEW,      &CPreprocessor01View::OnFilePrintPreview)     // ID_FILE_PRINT_SETUP 는 App에



	// User Interface	
	ON_WM_LBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEWHEEL()

	ON_COMMAND(ID_FILL_POLYGON,            &CPreprocessor01View::OnFillPolygon)
	ON_COMMAND(ID_TRANSPARENCY,            &CPreprocessor01View::OnTransparency)
	ON_COMMAND(ID_WIRE_FRAME,              &CPreprocessor01View::OnWireFrame)
	ON_UPDATE_COMMAND_UI(ID_FILL_POLYGON,  &CPreprocessor01View::OnUpdateFillPolygon)
	ON_UPDATE_COMMAND_UI(ID_TRANSPARENCY,  &CPreprocessor01View::OnUpdateTransparency)
	ON_UPDATE_COMMAND_UI(ID_WIRE_FRAME,    &CPreprocessor01View::OnUpdateWireFrame)

	ON_COMMAND(ID_DEC,              &CPreprocessor01View::OnDecLine)
	ON_COMMAND(ID_INC,              &CPreprocessor01View::OnIncLine)
	ON_COMMAND(ID_CHANGE,           &CPreprocessor01View::OnChangeOrder)
	ON_COMMAND(ID_GMAKE,            &CPreprocessor01View::OnMakeG)
	ON_COMMAND(ID_GVIEW,            &CPreprocessor01View::OnViewG)
END_MESSAGE_MAP()

// CPreprocessor01View 생성/소멸

CPreprocessor01View::CPreprocessor01View()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	MouseMode = MOUSE_NONE;
	DrawMode = NO_MODE;
	ModelStyle = MODEL_SHADE;

	IS_DRAW_NET = FALSE;

	CenterX = CenterY = 100.0;
	WinSize = 100;
	
	X_Rotate = -50; Y_Rotate = 0; Z_Rotate = 10;
	fTransX = 115; fTransY = 80;
	fTransZ = 0.0;

	ZoomScale = 0.9f;
	StartX = StartY = 0.0f;

	platform_x_size = 210;
	platform_y_size = 210;
	platform_z_size = 200;
	fnew = 0;                  // (4)
	m_hDib = NULL;
	m_pBitmapBits = NULL;
	m_hDib = NULL;
	m_hOldDC = NULL;
	m_hMemDC = NULL;
	m_hOldRC = NULL;
	m_hMemRC = NULL;
	m_hOldDib = NULL;

	ReadINI();
	Calc_init();
}

CPreprocessor01View::~CPreprocessor01View()
{
}
void CPreprocessor01View::ReadINI()
{
	TCHAR strChar[100];
	GetPrivateProfileString(_T("CONFIG"), _T("PATTERN_LINE_WIDTH"), _T("0.0015"), strChar, 20, INI_FILE_PATH );
	line_width = _tcstod(strChar, NULL);
	line_gap = line_width*10.0;
	GetPrivateProfileString(_T("CONFIG"), _T("MIN_PATTERN_LINE_GAP"), _T("0.3"), strChar, 20, INI_FILE_PATH );
	mindist2 = _tcstod(strChar, NULL);
	GetPrivateProfileString(_T("CONFIG"), _T("MIN_MOTION_INTERVAL"), _T("0.5"), strChar, 20, INI_FILE_PATH );
	mindist = _tcstod(strChar, NULL);
	GetPrivateProfileString(_T("CONFIG"), _T("GCODE_FILE"), _T("./vgcode.txt"), gcode_file, 50, INI_FILE_PATH );

	GetPrivateProfileString(_T("INTERFACE"), _T("CONVERSION_UNIT_LENGTH"), _T("1000.0"), strChar, 20, INI_FILE_PATH );
	con_factor = _tcstod(strChar, NULL);
	GetPrivateProfileString(_T("INTERFACE"), _T("Z_ORIGIN_HEIGHT"), _T("1000.0"), strChar, 20, INI_FILE_PATH );
	z_offset = _tcstod(strChar, NULL);

	GetPrivateProfileString(_T("MODEL"), _T("ROT_ARM_POS"), _T("0.15"), strChar, 20, INI_FILE_PATH );
	shift_1 = _tcstod(strChar, NULL);
	GetPrivateProfileString(_T("MODEL"), _T("ROT_ARM_LENGTH"), _T("0.15"), strChar, 20, INI_FILE_PATH );
	down_2 = _tcstod(strChar, NULL);
	GetPrivateProfileString(_T("MODEL"), _T("NOZZLE_POS"), _T("0.05"), strChar, 20, INI_FILE_PATH );
	shift_3 = _tcstod(strChar, NULL);
	GetPrivateProfileString(_T("MODEL"), _T("NOZZLE_LENGTH"), _T("0.1"), strChar, 20, INI_FILE_PATH );
	down_4 = _tcstod(strChar, NULL);
	GetPrivateProfileString(_T("MODEL"), _T("FOCUS_LENGTH"), _T("0.15"), strChar, 20, INI_FILE_PATH );
	focus = _tcstod(strChar, NULL);
	focus2 = focus / sqrt(2.0);
}

BOOL CPreprocessor01View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.
	cs.style |= CS_OWNDC;                         // (4-1)
	return CView::PreCreateWindow(cs);
	//return CPreviewView::PreCreateWindow(cs);
}

// CPreprocessor01View 그리기

void CPreprocessor01View::OnDraw(CDC* pDC)     // override
{
	//CPreprocessor01Doc* pDoc = GetDocument();
	//ASSERT_VALID(pDoc);
	//if (!pDoc)
	//	return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.

}


// CPreprocessor01View 인쇄

BOOL CPreprocessor01View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CPreprocessor01View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CPreprocessor01View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CPreprocessor01View 진단

#ifdef _DEBUG
void CPreprocessor01View::AssertValid() const
{
	CView::AssertValid();
	//CPreviewView::AssertValid();
}

void CPreprocessor01View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
	//CPreviewView::Dump(dc);
}

CPreprocessor01Doc* CPreprocessor01View::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPreprocessor01Doc)));
	return (CPreprocessor01Doc*)m_pDocument;
}
#endif //_DEBUG


// CPreprocessor01View 메시지 처리기


int CPreprocessor01View::OnCreate(LPCREATESTRUCT lpCreateStruct)  // message WM_CREATE
{
	if (CView::OnCreate(lpCreateStruct) == -1)         // (4-2)
	//if (CPreviewView::OnCreate(lpCreateStruct) == -1)         // (4-2)
		return -1;
	
	m_pDC = new CClientDC(this);
	hDC = m_pDC->GetSafeHdc();               // OpenGL rendering context를 설정하고 초기화함
	SetDCPixelFormat(hDC);		
	hRC = wglCreateContext(hDC);
	InitOpenGL();

	//칼크의 부모 윈도우 주소 결정 
	Calc.pParent=this;
	
	path = (GetCommandLine());

	return 0;
}

extern int add_count, del_count, del_count_face, del_count_vertex;
void CPreprocessor01View::OnDestroy()    // message WM_DESTROY
{
	//오픈지엘 콘텍스트 제거		
	CView::OnDestroy();
	//CPreviewView::OnDestroy();

	//OnFileNew();        // 모든 메모리 크리어
	int k = add_count+del_count;
	k = del_count_face+del_count_vertex;

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRC);

	delete m_pDC;
	
	//ClearNode((CNode*&)Calc.Solid, 1);    // CSolid = 1
	//ClearNode((CNode*&)Calc.Slice, 2);    // CSlice = 2
	//if (Calc.Solid) delete Calc.Solid;
	//if (Calc.Slice) delete Calc.Slice;
}

void CPreprocessor01View::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	//CPreviewView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	wglMakeCurrent(hDC, hRC);	//
	width=cx;
	height=cy;

	if (cx && cy) {                      // 만약 윈도우의 가로나 세로 길이가 0 가 아니라면
	                                     // 그에 맞춰 OpenGL의 ViewPort를 재정립한다.
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		InitProjection();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		//glViewport(0, 0, cx, cy);
		glViewport(0, 0, width, height);
	}

	wglMakeCurrent(0,0);         //

}

void CPreprocessor01View::OnPaint()    // message WM_PAINT
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CView::OnPaint()을(를) 호출하지 마십시오.

	// 새로은 scene을 그리기 위하여 color buffer와 depth buffer를 clear한다.
	DrawScene();
    CPen pen;
	pen.CreatePen(PS_DOT, 2, RGB(0,0,80));
	CPen* pOldPen = dc.SelectObject( &pen );
	ValidateRect(NULL);
}

void CPreprocessor01View::SetDCPixelFormat(HDC hDC)
{
	int nPixelFormat;

	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	// Size of this structure
		1,					// Version of this structure	
		PFD_DRAW_TO_WINDOW |		// Draw to Window (not to bitmap)
		PFD_SUPPORT_OPENGL |		// Support OpenGL calls in window
		PFD_DOUBLEBUFFER,			// Double buffered mode
		PFD_TYPE_RGBA,			// RGBA Color mode
		24,					// Want 16 bit color 
		0,0,0,0,0,0,				// Not used to select mode
		0,0,					// Not used to select mode
		0,0,0,0,0,			// Not used to select mode
		128,					// Size of depth buffer   128 / 16
		0, 0,               // Not used to select mode
		PFD_MAIN_PLANE,			// Draw in main plane
		0,					// Not used to select mode
		0,0,0 };					// Not used to select mode

	nPixelFormat = ChoosePixelFormat(hDC, &pfd);

	SetPixelFormat(hDC, nPixelFormat, &pfd);
}

void CPreprocessor01View::InitOpenGL(void)
{
	wglMakeCurrent(hDC, hRC);         //
	glEnable(GL_DEPTH_TEST);     // depth buffering(z-buffering) 가동
	glEnable(GL_NORMALIZE);      // 앞으로 쓰게될 vector들을 자동으로 unit vector로 변환함
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_CULL_FACE);		 //후향면 제거(은선을 제거한다).
	InitLight();
	glClearColor(Calc.color_back[0],Calc.color_back[1],Calc.color_back[2],Calc.color_back[3]);
	InitShadingModel();
	glLoadIdentity();
	InitProjection();
	glMatrixMode(GL_MODELVIEW);             // modelview matrix의 초기화
	glLoadIdentity();
	wglMakeCurrent(0,0);              //
}

void CPreprocessor01View::InitProjection(void)
{
	glOrtho(CenterX-WinSize*width/height,
			CenterX+WinSize*width/height,
			CenterY-WinSize,
			CenterY+WinSize,-1000.0f, 1000.0f );  

	// 시야각이 45도인 입체적인 projection
	// gluPerspective( 45.0f, (float)width/height, 2.0f, 100.0f );      
	// 2,4,17에서 0,0,0을 y축 orientation으로 바라봄
	// 2,4,17에서 0,0,0을 y축 orientation으로 바라봄
}

void CPreprocessor01View::InitLight(void)
{
	glLightfv(GL_LIGHT0, GL_POSITION, Calc.position0);
	glLightfv(GL_LIGHT2, GL_POSITION, Calc.position1);
	glLightfv(GL_LIGHT1, GL_POSITION, Calc.position2);
	glEnable(GL_LIGHTING);  // -------> 처음
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Calc.ambient);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);
	
	glMaterialfv(GL_FRONT, GL_SPECULAR, Calc.specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, Calc.shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, Calc.emission);
}

void CPreprocessor01View::InitShadingModel(void)
{
	glShadeModel(GL_FLAT);     // Smooth shading을 선택
}

BOOL CPreprocessor01View::OnEraseBkgnd(CDC* pDC)
{
	return false;
}


void CPreprocessor01View::DrawScene(int dir)
{
	if (dir) {
		wglMakeCurrent(hDC,hRC);	//
	}

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glPushMatrix();
	//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );  
	
	glMatrixMode(GL_PROJECTION);     // projection matrix의 초기화
	glLoadIdentity();
	InitProjection();
	glMatrixMode(GL_MODELVIEW);      // modelview matrix의 초기화
	glLoadIdentity();
	
	InitLight();
	
	glPushMatrix();
	gluLookAt(fTransX, fTransY,10, fTransX, fTransY, 0, 0.0f, 1.0f, 0.0f);
	
	glTranslated(CenterX,CenterY,0);
	glRotated(X_Rotate,1.0f,0.0f,0.0f);
	glRotated(Y_Rotate,0.0f,1.0f,0.0f);
	glRotated(Z_Rotate,0.0f,0.0f,1.0f);
	
	glPushMatrix();
	DrawMainaxis();
	glScalef(ZoomScale, ZoomScale, ZoomScale);
	
	if (IS_DRAW_NET) DrawPlatform();
	
	if (DrawMode == STL_MODE) {
		if (ModelStyle == MODEL_WIRE_FRAME) {
			Calc.ShowStlwire();
			Invalidate();
		} else {
			Calc.ShowStl();    // MODEL_SHADE
			Invalidate();
		}
	}

	if (ModelStyle == MODEL_WIRE_FRAME) {
		;                                                  // 이 처리를 함
	} else if (ModelStyle == MODEL_SHADE) {
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);            // 이 처리를 함
	} else if ((ModelStyle == MODEL_TRANSPARENCY) ){
		glDisable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT, GL_FILL);                // 이 처리를 함
		glEnable( GL_BLEND );
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	}
	
	glPopMatrix();

	glPopMatrix();

	glPopMatrix();
	
	glPushMatrix();

	glTranslated(-25.f*width/height,5.0f*width/height,500.0f);
	DrawAxis();

	glPopMatrix();
	
	glFlush();			// 위의 API에 의하여 구성된 instruction들을 buffer
	if (dir) {
		SwapBuffers(hDC);	// 로부터 flush시킨다. 그후 page를 swap시켜서 화면을 갱신한다.
		wglMakeCurrent(0,0);        //
		Invalidate();
	}
}

void CPreprocessor01View::OnViewStl()
{
	// TODO: Add your control notification handler code here
	DrawMode = STL_MODE;
	Invalidate();
}

void CPreprocessor01View::DrawPlatform(void)
{
	float x,y;
	glPushMatrix();
	glColor3fv(Calc.color_net);
	glDisable(GL_LIGHTING);  //
	glDisable( GL_BLEND );

	glLineWidth(3);
		glBegin(GL_LINE_STRIP);
		glVertex3d(0,0, 0.);
		glVertex3d(0,210, 0.);
		glVertex3d(210,210, 0.);
		glVertex3d(210,0, 0.);
		glVertex3d(0,0, 0.);
		glEnd();
		glLineWidth(1);
		for (x=0;x<211;x+=10) {
			glBegin(GL_LINES);
			glVertex3d(x, 0, 0.);
			glVertex3d(x, 210, 0.);
			glEnd();
		}
		for (y=0;y<211;y+=10) {
			glBegin(GL_LINES);
			glVertex3d(0,y,0.);
			glVertex3d(210,y,0.);
			glEnd();
		}
	glEnable(GL_LIGHTING);  //
	glPopMatrix();
}


void CPreprocessor01View::OnLButtonDown(UINT nFlags, CPoint point)
{
	MouseMode = MOUSE_ROTATION1;
	SetCapture();
	StartX = point.x;
	StartY = point.y;
	Invalidate();
	CView::OnLButtonDown(nFlags, point);
}

void CPreprocessor01View::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	MouseMode = MOUSE_TRANSLATION;
	SetCapture();
	StartX = point.x;
	StartY = point.y;
	Invalidate();
	CView::OnMButtonDown(nFlags, point);
}

void CPreprocessor01View::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	MouseMode = MOUSE_ROTATION2;
	SetCapture();
	StartX = point.x;
	StartY = point.y;
	Invalidate();
	CView::OnRButtonDown(nFlags, point);
}

void CPreprocessor01View::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nFlags|MK_LBUTTON|MK_MBUTTON)
	{
		if (MouseMode == MOUSE_ROTATION1){
			X_Rotate += (float)((point.y - StartY)/5.6);
			Z_Rotate -= (float)((point.x - StartX)/5.6);

			StartX = point.x;
			StartY = point.y;
			Invalidate();
		}
		else if (MouseMode == MOUSE_ROTATION2){
			Y_Rotate += (float)((point.x - StartX)/5.6);

			StartX = point.x;
			StartY = point.y;
			Invalidate();
		}
		else if (MouseMode == MOUSE_TRANSLATION){
			double x = (StartX-point.x)*WinSize/310.0;
			double y = (StartY-point.y)*WinSize/310.0;
			StartX = point.x;
			StartY = point.y;
			fTransX += x;
			fTransY -= y;
			Invalidate();
		}
  	}
	CView::OnMouseMove(nFlags, point);
}

void CPreprocessor01View::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	MouseMode = MOUSE_NONE;
	ReleaseCapture();
	CView::OnLButtonUp(nFlags, point);
	((CMainFrame *)AfxGetMainWnd())->StatusBarMessage("%.2f %.2f, %.2f %.2f %.2f, %.2f %.2f, %.2f", CenterX, CenterY, X_Rotate, Y_Rotate, Z_Rotate, fTransX, fTransY, ZoomScale);
}

void CPreprocessor01View::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	MouseMode = MOUSE_NONE;
	ReleaseCapture();
	CView::OnMButtonUp(nFlags, point);
	((CMainFrame *)AfxGetMainWnd())->StatusBarMessage("%.2f %.2f, %.2f %.2f %.2f, %.2f %.2f, %.2f", CenterX, CenterY, X_Rotate, Y_Rotate, Z_Rotate, fTransX, fTransY, ZoomScale);
}

void CPreprocessor01View::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	MouseMode = MOUSE_NONE;
	ReleaseCapture();
	CView::OnRButtonUp(nFlags, point);
	((CMainFrame *)AfxGetMainWnd())->StatusBarMessage("%.2f %.2f, %.2f %.2f %.2f, %.2f %.2f, %.2f", CenterX, CenterY, X_Rotate, Y_Rotate, Z_Rotate, fTransX, fTransY, ZoomScale);
}
BOOL CPreprocessor01View::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (ZoomScale<0) {
		ZoomScale += (float)zDelta/1000;
	}
	else ZoomScale += (float)zDelta/1000;
	Invalidate(TRUE);
	return TRUE;
	//return CView::OnMouseWheel(nFlags, zDelta, pt);

}





void CPreprocessor01View::OnRotationMode()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if (MouseMode == MOUSE_ROTATION1 && MouseMode == MOUSE_ROTATION2) MouseMode = MOUSE_NONE;
	else if (MouseMode = MOUSE_NONE) MouseMode == MOUSE_ROTATION1 && MouseMode == MOUSE_ROTATION2;
	Invalidate();
}
void CPreprocessor01View::OnUpdateRotationMode(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	pCmdUI->SetCheck(MouseMode == MOUSE_ROTATION1 && MouseMode == MOUSE_ROTATION2);
}





void CPreprocessor01View::OnTransMode()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if (MouseMode==MOUSE_TRANSLATION) MouseMode = MOUSE_NONE;
	else MouseMode=MOUSE_TRANSLATION;
	Invalidate();
}
void CPreprocessor01View::OnUpdateTransMode(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	pCmdUI->SetCheck(MouseMode == MOUSE_TRANSLATION);
}


void CPreprocessor01View::OnFileNew()        // pkc new
{
	int k;
	//CSolid * VSolid;
	//CSolid * TSolid;

	ReadINI();
	Calc_init();

	fnew = 0;
	Calc.gsel = 0;

	DrawMode = NO_MODE;
	CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
	frame->setProgressCtrl(1,100,"Deleting......");

	////////////////
	//ClearNode((CNode*&)Calc.Solid, 1);     // 이것으로 메모리 누수가 있었음


	// CSolid::~CSolid(void) 를 막았을 때 먹었음
	/*
	GoFirst((CNode*&)Calc.Solid);
	RE:
	k = add_count+del_count;
	k = del_count_face+del_count_vertex;
	k = Calc.Solid->base;                         // fnew
	GoFirst((CNode*&)Calc.Solid->Face);
	ClearNode((CNode*&)Calc.Solid->Face, 11);     // CFace = 11
	if (Calc.Solid->nxt != NULL) {
		Calc.Solid = (CSolid*) Calc.Solid->nxt;
		goto RE;
	}

	ClearNode((CNode*&)Calc.Solid, 1);
	*/
	////////////////

	ClearNode((CNode*&)Calc.Solid, 1);  // CSolid::~CSolid(void) 에서 작용하게 됨

	k = add_count+del_count;
	k = del_count_face+del_count_vertex;
	ClearNode((CNode*&)Calc.Slice, 2);

	Calc.Solid = new CSolid;

	frame->removeProgressCtrl();
	Invalidate();
}
void CPreprocessor01View::OnFileOpen()        // pkc open
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if (fnew >= 3) {
		//((CMainFrame *)AfxGetMainWnd())->StatusBarMessage("%s", "새로 만들기를 한 후에 파일 열기를 할 수 있읍니다");
		AfxMessageBox("새로 만들기를 한 후에 파일 열기를 할 수 있읍니다");
		return;
	}
	if (fnew == 0) {
		((CMainFrame *)AfxGetMainWnd())->StatusBarMessage("%s", "<<<<<  패턴 파일을 선택해 주세요  >>>>>");
		Calc.gsel = 0;
	}
	if (fnew == 1) ((CMainFrame *)AfxGetMainWnd())->StatusBarMessage("%s", "<<<<<  지그 파일을 선택해 주세요  >>>>>");
	if (fnew == 2) ((CMainFrame *)AfxGetMainWnd())->StatusBarMessage("%s", "<<<<<  모델 파일을 선택해 주세요  >>>>>");

	CString sFileName,sPathName,sExtName;

	CFileDialog    *pdlg;
	static char szFilter[] = "VCam Files (*.stl) | *.stl | All Files (*.*) | *.* ||";
	pdlg  =  new CFileDialog(true,"stl | asc",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
							szFilter);
						//	(BOOL bOpenFileDialog,
						//LPCTSTR lpszDefExt = NULL, 
						//LPCTSTR lpszFileName = NULL, 
						// DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
						//LPCTSTR lpszFilter = NULL, CWnd* pParentWnd = NULL );

	if (pdlg->DoModal() != IDOK) { 
		delete pdlg;
		return ;
	}
	
	sFileName = pdlg->GetFileTitle();        // "패턴 2"
	sPathName = pdlg->GetPathName();         // "K:\       \패턴 2.stl"
	sExtName = pdlg->GetFileExt();           // "stl"
	delete pdlg;

	if (!stricmp(sExtName.GetBuffer(3),"stl")) {
		
		CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
		frame->UpdateWindow();

		Calc.fnew = fnew;
		int ret = Calc.ReadStl(sPathName.GetBuffer(_MAX_PATH), fnew);  // pkc
		if (ret == 1) return;
		else if (ret == 2) {
			if (fnew == 0) ClearNode((CNode*&)Calc.Solid->Face, 11);
			else Delete((CNode*&)Calc.Solid, 1);         // 여기
			return;
		}
		
		strcpy(Calc.Solid->FileName , sFileName.GetBuffer(_MAX_PATH));  // file title (no extension)
		strcpy(Calc.Solid->PathName , sPathName.GetBuffer(_MAX_PATH));  // path+file name

		Calc.Solid->base = fnew;
		fnew++;

		Calc.Solid->hDC =hDC;
		Calc.Solid->hRC =hRC;
		
		DrawMode = STL_MODE;

		X_Rotate = -50; Y_Rotate = 0; Z_Rotate = 10;    // pkc
		fTransX = 115; fTransY = 80;                   // pkc

		MouseMode = MOUSE_NONE;
		CPreprocessor01Doc* pDoc = GetDocument();
		pDoc->SetTitle(sPathName);
	}
	Invalidate();
}


void CPreprocessor01View::MakeG(FILE *fpp)
{
	int con_test = 1, special = 0;

	CPreprocessor01Doc* pDoc = GetDocument();
	pDoc->g_make = 0;

	if (fpp == NULL) return;

	GoFirst((CNode*&)Calc.Solid);
	if (Calc.Solid->linenum == 0) return;

	FILE *fp;
	if (con_test) {
		fp = fopen(gcode_file, "w");
	}

	int index, k, init = 1, closed = 0, check = 0;
	double x, y, z, w, u, x1, y1, z1, w1, u1;
	double xp, yp, zp, wp, up, dist, vtheta, vphi, vphi2, ssize = 1.0, err;
	vec4 dtarget, newtarget, target1, targetp;
	vec3 vecp, norp, dir, dirp;
	double a, b, c, tv[3], ttheta, tphi, tphi2, tvv[3];

	fprintf(fpp, "#1->200X\n");        // 200 pulse 일 때  1mm
	fprintf(fpp, "#2->200Y\n");
	fprintf(fpp, "#3->1000Z\n");       // 1000 pulse 일 때  1mm
	fprintf(fpp, "#4->100W\n");        // 100 pulse 일 때  1degree
	fprintf(fpp, "#5->100U\n");

	fprintf(fpp, "\nclose\n");
	fprintf(fpp, "end gat\n");
	fprintf(fpp, "del gat\n");

	fprintf(fpp, "\nOPEN PROG 5 CLEAR\n");
	fprintf(fpp, "F100.000\n");

	fprintf(fpp, "\nfrax(X,Y,Z,W,U)\n");
	fprintf(fpp, "dwell 0\n");

	fprintf(fpp, "\nG90\n");

	GoFirst((CNode*&)Calc.Solid->Face);
	CFace * Now = Calc.Solid->Face;

	for (index = 1; index <= Calc.Solid->linenum; index++) {
		init = 1; closed = 0;
		GoFirst((CNode*&)Now);
		if (Now) {
			L1:
			if (Now->line == index) {
				if (Now->linebefore == -100) {  // 처음
					while (1) {
						special = 0;
						Now->gsel = 0;
						k = Now->linenext;

						//x = Now->mm[0]   +    50.0;        //  x, y, z 모두 양수
						//y = 400.0   -  Now->mm[1];           // 무게중심
						//z = 81.0    -  Now->mm[2];
						////z = 81;
						x = Now->m[0];
						y = Now->m[1];
						z = Now->m[2];
						vtheta = 180.0 * acos(Now->mn[2]/ssize) / 3.141592654f;
						vphi   = 180.0 * atan2(Now->mn[1], Now->mn[0]) / 3.141592654f;
						if (vphi < -90.0) vphi2 = vphi + 360.0;
						else vphi2 = vphi;

						if (init) {
							init = 0;
							if (k == -100) {
								break;
							}
							u = vtheta;
							if (u < 0.001) u = 0.0;
							if (u > 135.0) u = 135.0;
							if (u == 0.0) {  // Tz 90
								w = +90.;   //270;			// -90
							} else {
								w = vphi2 + 90.;
							}
							x1=x; y1=y; z1=z;
							w1=w; u1=u;
							xp=x; yp=y; zp=z;
							wp=w; up=u;
							vecp = vec3(Now->mn[0], Now->mn[1], Now->mn[2]);
							norp = vec3(Now->n[0], Now->n[1], Now->n[2]);

							dtarget.x = x + Now->mn[0]*focus*con_factor;      // 여기서
							dtarget.y = y + Now->mn[1]*focus*con_factor;
							dtarget.z = z + Now->mn[2]*focus*con_factor;
							dtarget.w = 1.0;
							target1 = dtarget;
							targetp = dtarget;
							newtarget = Calc_inv(w, u, dtarget);
							x = newtarget.x;
							y = newtarget.y;
							z = newtarget.z;
							fprintf(fpp, "\n; Line %d --> started\n", index);
							fprintf(fpp, "G01 X%.3f, Y%.3f, Z%.3f, W%.3f, U%.3f\n", x, y, z-z_offset, w, u);
							if (con_test)
								fprintf(fp,  "%.3f  %.3f  %.3f  %.3f  %.3f\n", x, y, z-z_offset, w, u);
							fprintf(fpp, "M91\n");
							////////
							dtarget = Calc_foc(x, y, z, w, u);
							near2(dtarget.x , xp, dtarget.y , yp, dtarget.z , zp, &err);
							if (err > 0.0005) {
								while (1) {
									Sleep(1000);
								}
							}
							Now->gsel = 1;
						} else {
							//////////////////////////////////////
							if (check) {
								err = sqrt((x-xp)*(x-xp) + (y-yp)*(y-yp) + (z-zp)*(z-zp));
								dir = vec3((x-xp)/err, (y-yp)/err, (z-zp)/err);
							}
							dist = sqrt((xp-x)*(xp-x) + (yp-y)*(yp-y) + (zp-z)*(zp-z));

							u = vtheta;
							if (u < 0.001) u = 0.0;
							if (u > 135.0) u = 135.0;
							if (u == 0.0) {  // Tz 90
								w = +90.;   //270;			// -90
							} else {
								w = vphi2 + 90.;
							}
							if (fabs(wp-w) > 90. && fabs(wp-w) < 270.) {
								u *= -1.0;
								w -= 180.0;
								if (w < 0.0) w += 360.0;
							}
							//////////////////////////////////////
							if (fabs(up-u) > 30.) {
								if (dist < line_width) {
									if (k == -100) {
										goto MID;      // MID
									}
									goto PASS;
								} else {
									if (dist > (mindist * 10.0)) {
										special = 1;
									}
									goto CHECK;
								}
							}
							if (dist < mindist) {  // 1.0
								if (k != -100) {
									if (dist < line_width) {
										goto PASS;
									} else
									if (check) {
										err = 180.0 * acos(dir.x*dirp.x+dir.y*dirp.y+dir.z*dirp.z) / 3.141592654f;
										if (err > 15.) {
											goto CHECK;    // mindist*sin(10) = 0.174
										}
									}
									goto PASS;
								} else {    //  k == -100
									;    // CHECK
								}
							} else {
								closed = 1;
								if (dist > (mindist * 10.0)) {
									special = 2;
								}
							}

							CHECK:            // xp yp zp 는 끝점, targetp.x 는 노즐, vecp 는 수직벡터  (이전의)
							if (special == 1) {
								tvv[0] = vecp.x + Now->mn[0]; tvv[1] = vecp.y + Now->mn[1]; tvv[2] = vecp.z + Now->mn[2];
								err = sqrt(tvv[0]*tvv[0] + tvv[1]*tvv[1] + tvv[2]*tvv[2]);
								tvv[0] /= err; tvv[1] /= err; tvv[2] /= err;

								a = (3.0*xp + x)/4.0; b = (3.0*yp + y)/4.0; c = (3.0*zp + z)/4.0;
								tv[0] = vecp.x + tvv[0]; tv[1] = vecp.y + tvv[1]; tv[2] = vecp.z + tvv[2];
								{
									err = sqrt(tv[0]*tv[0] + tv[1]*tv[1] + tv[2]*tv[2]);
									tv[0] /= err; tv[1] /= err; tv[2] /= err;
									ttheta = 180.0 * acos(tv[2]/ssize) / 3.141592654f;
									tphi   = 180.0 * atan2(tv[1], tv[0]) / 3.141592654f;
									if (tphi < -90.0) tphi2 = tphi + 360.0;
									else tphi2 = tphi;

									u = ttheta;
									if (u < 0.001) u = 0.0;
									if (u > 135.0) u = 135.0;
									if (u == 0.0) {  // Tz 90
										w = +90.;   //270;			// -90
									} else {
										w = tphi2 + 90.;
									}
									if (fabs(wp-w) > 90. && fabs(wp-w) < 270.) {
										u *= -1.0;
										w -= 180.0;
										if (w < 0.0) w += 360.0;
									}

									dtarget.x = a + tv[0]*focus*con_factor;      // 여기서
									dtarget.y = b + tv[1]*focus*con_factor;
									dtarget.z = c + tv[2]*focus*con_factor;
									dtarget.w = 1.0;
									newtarget = Calc_inv(w, u, dtarget);
									a = newtarget.x;
									b = newtarget.y;
									c = newtarget.z;
									fprintf(fpp, "G01 X%.3f, Y%.3f, Z%.3f, W%.3f, U%.3f\n", a, b, c-z_offset, w, u);
									if (con_test)
										fprintf(fp,  " %.3f  %.3f  %.3f  %.3f  %.3f\n", a, b, c-z_offset, w, u); //1
									wp=w; up=u;
								}
								a = (xp + x)/2.0; b = (yp + y)/2.0; c = (zp + z)/2.0;
								tv[0] = vecp.x + Now->mn[0]; tv[1] = vecp.y + Now->mn[1]; tv[2] = vecp.z + Now->mn[2];
								{
									err = sqrt(tv[0]*tv[0] + tv[1]*tv[1] + tv[2]*tv[2]);
									tv[0] /= err; tv[1] /= err; tv[2] /= err;
									ttheta = 180.0 * acos(tv[2]/ssize) / 3.141592654f;
									tphi   = 180.0 * atan2(tv[1], tv[0]) / 3.141592654f;
									if (tphi < -90.0) tphi2 = tphi + 360.0;
									else tphi2 = tphi;

									u = ttheta;
									if (u < 0.001) u = 0.0;
									if (u > 135.0) u = 135.0;
									if (u == 0.0) {  // Tz 90
										w = +90.;   //270;			// -90
									} else {
										w = tphi2 + 90.;
									}
									if (fabs(wp-w) > 90. && fabs(wp-w) < 270.) {
										u *= -1.0;
										w -= 180.0;
										if (w < 0.0) w += 360.0;
									}

									dtarget.x = a + tv[0]*focus*con_factor;      // 여기서
									dtarget.y = b + tv[1]*focus*con_factor;
									dtarget.z = c + tv[2]*focus*con_factor;
									dtarget.w = 1.0;
									newtarget = Calc_inv(w, u, dtarget);
									a = newtarget.x;
									b = newtarget.y;
									c = newtarget.z;
									fprintf(fpp, "G01 X%.3f, Y%.3f, Z%.3f, W%.3f, U%.3f\n", a, b, c-z_offset, w, u);
									if (con_test)
										fprintf(fp,  " %.3f  %.3f  %.3f  %.3f  %.3f\n", a, b, c-z_offset, w, u); //1
									wp=w; up=u;
								}
								a = (3.0*x + xp)/4.0; b = (3.0*y + yp)/4.0; c = (3.0*z + zp)/4.0;
								tv[0] = tvv[0] + Now->mn[0]; tv[1] = tvv[1] + Now->mn[1]; tv[2] = tvv[2] + Now->mn[2];
								{
									err = sqrt(tv[0]*tv[0] + tv[1]*tv[1] + tv[2]*tv[2]);
									tv[0] /= err; tv[1] /= err; tv[2] /= err;
									ttheta = 180.0 * acos(tv[2]/ssize) / 3.141592654f;
									tphi   = 180.0 * atan2(tv[1], tv[0]) / 3.141592654f;
									if (tphi < -90.0) tphi2 = tphi + 360.0;
									else tphi2 = tphi;

									u = ttheta;
									if (u < 0.001) u = 0.0;
									if (u > 135.0) u = 135.0;
									if (u == 0.0) {  // Tz 90
										w = +90.;   //270;			// -90
									} else {
										w = tphi2 + 90.;
									}
									if (fabs(wp-w) > 90. && fabs(wp-w) < 270.) {
										u *= -1.0;
										w -= 180.0;
										if (w < 0.0) w += 360.0;
									}

									dtarget.x = a + tv[0]*focus*con_factor;      // 여기서
									dtarget.y = b + tv[1]*focus*con_factor;
									dtarget.z = c + tv[2]*focus*con_factor;
									dtarget.w = 1.0;
									newtarget = Calc_inv(w, u, dtarget);
									a = newtarget.x;
									b = newtarget.y;
									c = newtarget.z;
									fprintf(fpp, "G01 X%.3f, Y%.3f, Z%.3f, W%.3f, U%.3f\n", a, b, c-z_offset, w, u);
									if (con_test)
										fprintf(fp,  " %.3f  %.3f  %.3f  %.3f  %.3f\n", a, b, c-z_offset, w, u); //1
									wp=w; up=u;
								}
							} else if (special == 2) {
								a = (xp + x)/2.0; b = (yp + y)/2.0; c = (zp + z)/2.0;
								//tv[0] = norp.x + Now->n[0]; tv[1] = norp.y + Now->n[1]; tv[2] = norp.z + Now->n[2];
								tv[0] = vecp.x + Now->mn[0]; tv[1] = vecp.y + Now->mn[1]; tv[2] = vecp.z + Now->mn[2];
								{
									err = sqrt(tv[0]*tv[0] + tv[1]*tv[1] + tv[2]*tv[2]);
									tv[0] /= err; tv[1] /= err; tv[2] /= err;
									ttheta = 180.0 * acos(tv[2]/ssize) / 3.141592654f;
									tphi   = 180.0 * atan2(tv[1], tv[0]) / 3.141592654f;
									if (tphi < -90.0) tphi2 = tphi + 360.0;
									else tphi2 = tphi;

									u = ttheta;
									if (u < 0.001) u = 0.0;
									if (u > 135.0) u = 135.0;
									if (u == 0.0) {  // Tz 90
										w = +90.;   //270;			// -90
									} else {
										w = tphi2 + 90.;
									}
									if (fabs(wp-w) > 90. && fabs(wp-w) < 270.) {
										u *= -1.0;
										w -= 180.0;
										if (w < 0.0) w += 360.0;
									}

									dtarget.x = a + tv[0]*focus*con_factor;      // 여기서
									dtarget.y = b + tv[1]*focus*con_factor;
									dtarget.z = c + tv[2]*focus*con_factor;
									dtarget.w = 1.0;
									newtarget = Calc_inv(w, u, dtarget);
									a = newtarget.x;
									b = newtarget.y;
									c = newtarget.z;
									fprintf(fpp, "G01 X%.3f, Y%.3f, Z%.3f, W%.3f, U%.3f\n", a, b, c-z_offset, w, u);
									if (con_test)
										fprintf(fp,  "  %.3f  %.3f  %.3f  %.3f  %.3f\n", a, b, c-z_offset, w, u); //2
									wp=w; up=u;
								}
							}
							u = vtheta;
							if (u < 0.001) u = 0.0;
							if (u > 135.0) u = 135.0;
							if (u == 0.0) {  // Tz 90
								w = +90.;   //270;			// -90
							} else {
								w = vphi2 + 90.;
							}
							if (fabs(wp-w) > 90. && fabs(wp-w) < 270.) {
								u *= -1.0;
								w -= 180.0;
								if (w < 0.0) w += 360.0;
							}
							if (!check) {
								err = sqrt((x-xp)*(x-xp) + (y-yp)*(y-yp) + (z-zp)*(z-zp));
								dirp = vec3((x-xp)/err, (y-yp)/err, (z-zp)/err); check = 1;
							} else {
								dirp = dir;
							}
							xp=x; yp=y; zp=z;
							wp=w; up=u;
							vecp = vec3(Now->mn[0], Now->mn[1], Now->mn[2]);
							norp = vec3(Now->n[0], Now->n[1], Now->n[2]);

							dtarget.x = x + Now->mn[0]*focus*con_factor;      // 여기서
							dtarget.y = y + Now->mn[1]*focus*con_factor;
							dtarget.z = z + Now->mn[2]*focus*con_factor;
							dtarget.w = 1.0;
							targetp = dtarget;
							newtarget = Calc_inv(w, u, dtarget);
							x = newtarget.x;
							y = newtarget.y;
							z = newtarget.z;
							fprintf(fpp, "G01 X%.3f, Y%.3f, Z%.3f, W%.3f, U%.3f\n", x, y, z-z_offset, w, u);
							if (con_test)
								fprintf(fp,  "%.3f  %.3f  %.3f  %.3f  %.3f\n", x, y, z-z_offset, w, u);
							////////
							dtarget = Calc_foc(x, y, z, w, u);
							near2(dtarget.x , xp, dtarget.y , yp, dtarget.z , zp, &err);
							if (err > 0.0005) {
								while (1) {
									Sleep(1000);
								}
							}
							Now->gsel = 1;
						}
						MID:
						if (k == -100) {  // linenext
							if (closed) {
								dist = sqrt((xp-x1)*(xp-x1) + (yp-y1)*(yp-y1) + (zp-z1)*(zp-z1));  // xp = x
								if (dist < line_width) {
									fprintf(fpp, "M90\n");
									fprintf(fpp, "; Line %d --> closed (perfect)\n", index);
								} else
								if (dist < mindist2) {  // 0.5
									if (fabs(wp-w1) > 90. && fabs(wp-w1) < 270.) {
										u1 *= -1.0;
										w1 -= 180.0;
										if (w1 < 0.0) w1 += 360.0;
									}
									newtarget = Calc_inv(w1, u1, target1);
									x = newtarget.x;
									y = newtarget.y;
									z = newtarget.z;
									fprintf(fpp, "G01 X%.3f, Y%.3f, Z%.3f, W%.3f, U%.3f\n", x, y, z-z_offset, w1, u1);
									if (con_test)
										fprintf(fp,  "%.3f  %.3f  %.3f  %.3f  %.3f\n", x, y, z-z_offset, w1, u1);
									fprintf(fpp, "M90\n");
									fprintf(fpp, "; Line %d --> closed\n", index);
									////////
									dtarget = Calc_foc(x, y, z, w1, u1);
									near2(dtarget.x , x1, dtarget.y , y1, dtarget.z , z1, &err);
									if (err > 0.0005) {
										while (1) {
											Sleep(1000);
										}
									}
									//Now->gsel = 1;
								} else {
									fprintf(fpp, "M90\n");
									fprintf(fpp, "; Line %d --> open\n", index);
								}
							} else {
								fprintf(fpp, "M90\n");
								fprintf(fpp, "; Line %d --> open\n", index);
							}
							break;
						}
						PASS:
						Go((CNode*&) Now, k); // pkc
					} // while (1)
				} else {
					goto L3;
				} // if (Now->linebefore == -100) 
			} else {
				L3:
				if (Now->nxt == NULL) {
					continue;
				} else {
					Now = (CFace*) Now->nxt;
					goto L1;
				}
			}  // if (Now->line == index) {
		}  // if (Now) {
	}  // for

	//fprintf(fpp, "G01 X%.3f, Y%.3f, Z%.3f\n", x1, y1, z1);

	fprintf(fpp, "dwell 0\n");
	fprintf(fpp, "CLOSE\n");
	((CMainFrame *)AfxGetMainWnd())->StatusBarMessage("%s", "G-Code 를 생성했읍니다");
	if (con_test) {
		fclose(fp);
	}
}
void CPreprocessor01View::OnViewG()
{
	if (Calc.gsel) Calc.gsel = 0;
	else Calc.gsel = 1;
	Invalidate();
}
void CPreprocessor01View::OnMakeG()
{
	if (AfxMessageBox(_T("G-Code 를 생성합니다."), MB_ICONSTOP | MB_OKCANCEL) == IDCANCEL) {
		//pDoc->g_make = 0;
		return;
	}

	CPreprocessor01Doc* pDoc = GetDocument();
	pDoc->g_make = 1;
	pDoc->OnFileSave();
}
void CPreprocessor01View::OnChangeOrder()
{
	if (Calc.Solid->currentline == -1) {
		AfxMessageBox("가공 순서를 바꿀 Line 을 선택해 주세요.");
		return;
	} else {
		if (AfxMessageBox(_T("선택된 라인의 가공 순서를 역으로 바꿉니다."), MB_ICONSTOP | MB_OKCANCEL) == IDOK) {
			Calc.ChangeLine(Calc.Solid->currentline);
		}
	}
	Invalidate();
}
void CPreprocessor01View::OnDecLine()
{
	Calc.seg_counter--;
	Calc.seg_counter--;
	if (Calc.seg_counter < 1) Calc.seg_counter = 1;
	Invalidate();
}
void CPreprocessor01View::OnIncLine()
{
	Calc.seg_counter++;
	Calc.seg_counter++;
	Invalidate();
}


void CPreprocessor01View::OnFillPolygon()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ModelStyle = MODEL_SHADE;
	Invalidate();
}
void CPreprocessor01View::OnTransparency()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ModelStyle = MODEL_TRANSPARENCY;
	//DrawMode = STL_MODE;
	Invalidate();
}
void CPreprocessor01View::OnWireFrame()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ModelStyle = MODEL_WIRE_FRAME;
	Invalidate();
}



void CPreprocessor01View::OnViewGrid()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	IS_DRAW_NET = (!IS_DRAW_NET);
	Invalidate();;
}
void CPreprocessor01View::OnUpdateViewGrid(CCmdUI *pCmdUI)    // pkc add
{
	pCmdUI->SetCheck(IS_DRAW_NET);
}




void CPreprocessor01View::OnUpdateFillPolygon(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	pCmdUI->SetCheck(ModelStyle == MODEL_SHADE);
}


void CPreprocessor01View::OnUpdateTransparency(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	pCmdUI->SetCheck(ModelStyle == MODEL_TRANSPARENCY);	
}


void CPreprocessor01View::OnUpdateWireFrame(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	pCmdUI->SetCheck(ModelStyle == MODEL_WIRE_FRAME);
}





void CPreprocessor01View::DrawMainaxis(void)
{
		glPushMatrix();				
		
		glDisable(GL_LIGHTING);  //
		glDisable( GL_BLEND );

		glLineWidth(2);
				
		glPushMatrix();
		GLfloat x_Div		= WinSize	/ 5.0f;
		GLfloat y_Div		= WinSize	/ 5.0f;
		GLfloat z_Div		= WinSize	/ 2.0f;
		GLfloat gl_Line	= WinSize	/ 5.0f;
		GLfloat z_Offset	= WinSize	/ 5.0f;

		//glTranslatef(CenterX + x_Div, CenterY + y_Div, -WinSize + z_Div + z_Offset);

		GLUquadricObj *pObj;	// Temporary, used for quadrics

		// Measurements
		GLfloat fAxisRadius	= 0.025f	*10.0f;
		GLfloat fAxisHeight	= 1.0f	*20.0f;
		GLfloat fArrowRadius	= 0.06f	*20.0f;
		GLfloat fArrowHeight	= 0.1f	*50.0f;

		// Setup the quadric object
		pObj = gluNewQuadric();
			gluQuadricDrawStyle(pObj, GLU_FILL);
			gluQuadricNormals(pObj, GLU_SMOOTH);
			gluQuadricOrientation(pObj, GLU_OUTSIDE);
			gluQuadricTexture(pObj, GLU_FALSE);
		
		
		
		///////////////////////////////////////////////////////
		// Draw the Red X axis 2nd, with arrowed head
		glColor3f(1.0f, 0.0f, 0.0f);
		glPushMatrix();
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
			gluCylinder(pObj, fAxisRadius, fAxisRadius, fAxisHeight, 10, 1);
			glPushMatrix();
				glTranslatef(0.0f, 0.0f, fAxisHeight);
				gluCylinder(pObj, fArrowRadius, 0.0f, fArrowHeight, 10, 1);
				glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
				gluDisk(pObj, fAxisRadius, fArrowRadius, 10, 1);
			glPopMatrix();
		glPopMatrix();
		
		///////////////////////////////////////////////////////
		// Draw the Green Y axis 3rd, with arrowed head
		glColor3f(0.0f, 1.0f, 0.0f);
		glPushMatrix();
			glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
			gluCylinder(pObj, fAxisRadius, fAxisRadius, fAxisHeight, 10, 1);
			glPushMatrix();
				glTranslatef(0.0f, 0.0f, fAxisHeight);
				gluCylinder(pObj, fArrowRadius, 0.0f, fArrowHeight, 10, 1);
				glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
				gluDisk(pObj, fAxisRadius, fArrowRadius, 10, 1);
			glPopMatrix();
		glPopMatrix();
		
		// Draw the blue Z axis first, with arrowed head
		glColor3f(0.0f, 0.0f, 1.0f);
		gluCylinder(pObj, fAxisRadius, fAxisRadius, fAxisHeight, 10, 1);
		glPushMatrix();
			glTranslatef(0.0f, 0.0f, fAxisHeight);
		gluCylinder(pObj, fArrowRadius, 0.0f, fArrowHeight, 10, 1);
		glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
		gluDisk(pObj, fAxisRadius, fArrowRadius, 15, 1);
		glPopMatrix();

		// White Sphere at origin
		glColor3f(0.3f, 0.3f, 0.3f);
		gluSphere(pObj, 1.0f, 15, 15);

		// pkc 제거
		/*
		glLineWidth(2.0f);
		glColor3f(0.6f,0.6f,0.6f);
		//glTranslated(8.,8.,8.);
		//glutWireCube(16.0f);

		glBegin(GL_LINE_STRIP);			// xy평면
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(16.0f, 0.0f, 0.0f);
		glVertex3f(16.0f, 16.0f, 0.0f);
		glVertex3f(0.0f, 16.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glEnd();

		glBegin(GL_LINE_STRIP);			// yz평면
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 16.0f, 0.0f);
		glVertex3f(0.0f, 16.0f, 16.0f);
		glVertex3f(0.0f, 0.0f, 16.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glEnd();

		glBegin(GL_LINE_STRIP);			// zx평면
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(16.0f, 0.0f, 0.0f);
		glVertex3f(16.0f, 0.0f, 16.0f);
		glVertex3f(0.0f, 0.0f, 16.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glEnd();
		*/

		// Delete the quadric
		gluDeleteQuadric(pObj);
		
		glPopMatrix();
	glPopMatrix();
	glLineWidth(1);
	glEnable(GL_LIGHTING);  //

}

void CPreprocessor01View::DrawAxis(void)
{
	glPushMatrix();
	glDisable(GL_LIGHTING);  //
	
	GLUquadricObj *pObj;
	pObj = gluNewQuadric();
	gluQuadricDrawStyle(pObj, GLU_FILL);
	gluQuadricNormals(pObj, GLU_SMOOTH);
	gluQuadricOrientation(pObj, GLU_OUTSIDE);
	gluQuadricTexture(pObj, GLU_FALSE);
	
	glColor3f(0.f,0.f,0.f);
	gluSphere(pObj, 0.5, 10, 10);
	

	glRotated(90.f,1.0f,0.0f,0.0f);	// 작은 축의 방향을 화면의 방향과 일치
	
	glRotated(X_Rotate,1.0f,0.0f,0.0f);
	glRotated(Y_Rotate,0.0f,0.0f,-1.0f);
	glRotated(Z_Rotate,0.0f,1.0f,0.0f);
	
	glLineWidth(1.0);
	glBegin (GL_LINES);
	glColor3f(1,0,0); // X axis is red.
	glVertex3f(0,0,0);
	glVertex3f(10,0,0); 
	
	glColor3f(0,0,1); // Y axis is green.
	glVertex3f(0,0,0);
	glVertex3f(0,10,0);
	
	glColor3f(0,1,0); // z axis is blue.
	glVertex3f(0,0,0);
	glVertex3f(0,0,-10);
	glEnd();

	glLineWidth(0.1f);
	glColor3f(0.f,0.f,0.f);
	glTranslated(3.,3.,-3.);
	glutWireCube(6.0f);
	gluDeleteQuadric(pObj);
	glPopMatrix();
	glEnable(GL_LIGHTING);  //
}

BOOL CPreprocessor01View::PreTranslateMessage(MSG* pMsg)
{
    switch(pMsg->message) { 
        //case WM_CHAR:
        case WM_KEYDOWN:
            if (pMsg->wParam==VK_LEFT) { 
		        fTransX += 5;
                Invalidate();
            } else if (pMsg->wParam==VK_RIGHT) { 
		        fTransX -= 5;
		        Invalidate();
            } else if (pMsg->wParam==VK_DOWN) { 
		        fTransY += 5;
		        Invalidate();
	        } else if (pMsg->wParam==VK_UP) { 
		        fTransY -= 5;
		        Invalidate();
	        } else if (pMsg->wParam==VK_HOME) { 
				OnDecLine();
			} else if (pMsg->wParam==VK_END) { 
				OnIncLine();
			}

	        if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) {
				// 위 VK_RETURN은 Enter, VK_ESCAPE는 ESC을 의미함. 필요시 하나만 사용.
				return true;
			}

			break;
    } 

	return CView::PreTranslateMessage(pMsg);
	//return CPreviewView::PreTranslateMessage(pMsg);
}

void CPreprocessor01View::OnFilePrintPreview()
{

	//AFXPrintPreview(this);
	//return;

	// ON_COMMAND(ID_FILE_PRINT_PREVIEW,      &CView::OnFilePrintPreview) 로 call 하지 않음

	/**/
	// In derived classes, implement special window handling here
    // Be sure to Unhook Frame Window close if hooked.

    // must not create this on the frame. Must outlive this function
    CPrintPreviewState* pState = new CPrintPreviewState;

    //if (!DoPrintPreview(AFX_IDD_PREVIEW_TOOLBAR, this, RUNTIME_CLASS(CPreviewView), pState))    // view class
	if (!DoPrintPreview(AFX_IDD_PREVIEW_TOOLBAR, this, RUNTIME_CLASS(CPreprocessor01View), pState))    // view class
    {
        // In derived classes, reverse special window handling
        // here for Preview failure case

        TRACE0("Error: DoPrintPreview failed");
        AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
        delete pState;      // preview failed to initialize, 
                    // delete State now
    }
	/**/

}

/*
void CGLObj::RenderScene()
{	
	::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	::glPushMatrix();
	
	// Position
	::glTranslatef(m_xTranslation, m_yTranslation, m_zTranslation - m_fRadius);
	::glRotatef(m_xRotation, 1.0, 0.0, 0.0);
	::glRotatef(m_yRotation, 0.0, 1.0, 0.0);
	::glRotatef(m_zRotation, 0.0, 0.0, 1.0);

	::glCallList(1);

	::glPopMatrix();
	::glFinish();
}
*/
void CPreprocessor01View::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: Add your specialized code here and/or call the base class

	//CGLObj::OnPrint(pDC, pInfo, this);
	OnPrint1(pDC, pInfo, this);
	DrawScene(0);                   //RenderScene();
	OnPrint2(pDC);


	/**/
	__super::OnPrint(pDC, pInfo);  // vc2010 이 만듬
	/**/
}
void CPreprocessor01View::OnPrint1(CDC* pDC, CPrintInfo* pInfo, CView* pView) 
{
	CRect rcClient;
	pView->GetClientRect(&rcClient); 
	float fClientRatio = float(rcClient.Height())/rcClient.Width();

	// Get page size
	m_szPage.cx  = pDC->GetDeviceCaps(HORZRES);
	m_szPage.cy = pDC->GetDeviceCaps(VERTRES);

	CSize szDIB;
		szDIB.cx = rcClient.Width();
		szDIB.cy = rcClient.Height()+500;
		goto PASS;
	if (pInfo->m_bPreview)
	{
		// Use screen resolution for preview.
		szDIB.cx = rcClient.Width();
		szDIB.cy = rcClient.Height();
	}
	else  // Printing
	{
		// Use higher resolution for printing.
		// Adjust size according screen's ratio.
		if (m_szPage.cy > fClientRatio*m_szPage.cx)
		{
			// View area is wider than Printer area
			szDIB.cx = m_szPage.cx;
			szDIB.cy = long(fClientRatio*m_szPage.cx);
		}
		else
		{
			// View area is narrower than Printer area
			szDIB.cx = long(float(m_szPage.cy)/fClientRatio);
			szDIB.cy = m_szPage.cy;
		}
		// Reduce the Resolution if the Bitmap size is too big.
		// Ajdust the maximum value, which is depends on printer's memory.
		// I use 20 MB. 

		//goto PASS;
		while (szDIB.cx*szDIB.cy > 20e6)   
		{
			szDIB.cx = szDIB.cx/2;
			szDIB.cy = szDIB.cy/2;
		}
	}

	PASS:
	TRACE("Buffer size: %d x %d = %6.2f MB\n", szDIB.cx, szDIB.cy, szDIB.cx*szDIB.cy*0.000001);
	
	// Initialize the bitmap header info.
	memset(&m_bmi, 0, sizeof(BITMAPINFO));
	m_bmi.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	m_bmi.bmiHeader.biWidth			= szDIB.cx;
	m_bmi.bmiHeader.biHeight		= szDIB.cy;
	m_bmi.bmiHeader.biPlanes		= 1;
	m_bmi.bmiHeader.biBitCount		= 24;
	m_bmi.bmiHeader.biCompression	= BI_RGB;
	m_bmi.bmiHeader.biSizeImage		= szDIB.cx * szDIB.cy * 3;

	// Create DIB
	HDC	hDC = ::GetDC(pView->m_hWnd);
	m_hDib = ::CreateDIBSection(hDC, &m_bmi, DIB_RGB_COLORS, &m_pBitmapBits, NULL, (DWORD)0);
	::ReleaseDC(pView->m_hWnd, hDC);

	// Create memory DC
	m_hMemDC = ::CreateCompatibleDC(NULL);
	if (!m_hMemDC)
	{
		DeleteObject(m_hDib);
		m_hDib = NULL;
		return;
	}

	m_hOldDib = SelectObject(m_hMemDC, m_hDib);

	// Setup memory DC's pixel format.
	if (!SetDCPixelFormat(m_hMemDC, PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL | PFD_STEREO_DONTCARE))
	{
		SelectObject(m_hMemDC, m_hOldDib);
		DeleteObject(m_hDib);
		m_hDib = NULL;
		DeleteDC(m_hMemDC);
		m_hMemDC = NULL;
		return;
	}

	// Create memory RC
	m_hMemRC = ::wglCreateContext(m_hMemDC);
	if (!m_hMemRC)
	{
		SelectObject(m_hMemDC, m_hOldDib);
		DeleteObject(m_hDib);
		m_hDib = NULL;
		DeleteDC(m_hMemDC);
		m_hMemDC = NULL;
		return;
	}

	// Store old DC and RC
	m_hOldDC = ::wglGetCurrentDC();
	m_hOldRC = ::wglGetCurrentContext(); 

	// Make m_hMemRC the current RC.
	::wglMakeCurrent(m_hMemDC, m_hMemRC);

		// InitOpenGL
		glEnable(GL_DEPTH_TEST);     // depth buffering(z-buffering) 가동
		glEnable(GL_NORMALIZE);      // 앞으로 쓰게될 vector들을 자동으로 unit vector로 변환함
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_CULL_FACE);		 //후향면 제거(은선을 제거한다).
		InitLight();
		glClearColor(1.0f,1.0f,1.0f,1.0f);  // white
		InitShadingModel();
		glLoadIdentity();
		InitProjection();
		glMatrixMode(GL_MODELVIEW);             // modelview matrix의 초기화
		glLoadIdentity();

		// OnSize
		::glMatrixMode(GL_PROJECTION);
		::glLoadIdentity();
		InitProjection();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		//glViewport(0, 0, cx, cy);
		glViewport(0, 0, width, height);



	//SetOpenGLState();
	//::glViewport(0, 0, szDIB.cx, szDIB.cy);
	//SetFrustum();
	//CreateDisplayList(1);
}
/*
void CGLObj::SetOpenGLState()
{
	// Default mode
	::glEnable(GL_NORMALIZE);
	::glEnable(GL_DEPTH_TEST);

	// Lights, material properties
	::glClearColor(m_ClearColorRed, m_ClearColorGreen, m_ClearColorBlue, 1.0f);
	::glClearDepth(1.0);
	::glShadeModel(GL_SMOOTH);

	// Default polygonmode
	::glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
*/
/*
void CGLObj::SetFrustum()
{
	::glMatrixMode(GL_PROJECTION);
	::glLoadIdentity();
	::gluPerspective(45.0, m_dAspect, m_dNearPlane, m_dFarPlane); 
	::glMatrixMode(GL_MODELVIEW);
}
*/
/*
void CGLObj::CreateDisplayList(UINT nList)
{
	::glNewList(nList, GL_COMPILE);
	float fLen(3);
	// Color-Triangle
	::glBegin(GL_TRIANGLES);
		::glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		::glVertex3f(-fLen, fLen, 0);
		::glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
        ::glVertex3f(-fLen, -fLen, 0);
		::glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        ::glVertex3f(fLen, -fLen, 0);
	::glEnd();
	// Mono-Triangle
	::glBegin(GL_TRIANGLES);
		::glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
		::glVertex3f(-fLen, fLen, 0);
		::glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        ::glVertex3f(fLen, fLen, 0);
		::glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        ::glVertex3f(fLen, -fLen, 0);
	::glEnd();
	::glEndList();
}
*/
void CPreprocessor01View::OnPrint2(CDC* pDC) 
{
	::wglMakeCurrent(NULL, NULL);	
	::wglDeleteContext(m_hMemRC);

	// Restore last DC and RC
	::wglMakeCurrent(m_hOldDC, m_hOldRC);	

	// Size of printing image
	float fBmiRatio = float(m_bmi.bmiHeader.biHeight) / m_bmi.bmiHeader.biWidth;
	CSize szImageOnPage;  
	if (m_szPage.cx > m_szPage.cy)	 // Landscape page
	{
		if (fBmiRatio<1) {	  // Landscape image
			szImageOnPage.cx = m_szPage.cx;
			szImageOnPage.cy = long(fBmiRatio * m_szPage.cy);
		} else {		  // Portrait image
			szImageOnPage.cx = long(m_szPage.cy/fBmiRatio);
			szImageOnPage.cy = m_szPage.cy;
		}
	} else {	    // Portrait page
		if (fBmiRatio<1) {	  // Landscape image
			szImageOnPage.cx = m_szPage.cx;
			szImageOnPage.cy = long(fBmiRatio * m_szPage.cx);
		} else {			  // Portrait image
			szImageOnPage.cx = long(m_szPage.cx/fBmiRatio);
			szImageOnPage.cy = m_szPage.cy;
		}
	}

	CSize szOffset((m_szPage.cx - szImageOnPage.cx) / 2, (m_szPage.cy - szImageOnPage.cy) / 2);

	// Stretch the Dib to fit the image size.
	int nRet = ::StretchDIBits(pDC->GetSafeHdc(),
							  szOffset.cx, szOffset.cy,
							  szImageOnPage.cx, szImageOnPage.cy,
							  0, 0,
							  m_bmi.bmiHeader.biWidth, m_bmi.bmiHeader.biHeight,
							  (GLubyte*) m_pBitmapBits,
							  &m_bmi, DIB_RGB_COLORS, SRCCOPY);

	if (nRet == GDI_ERROR) {
		TRACE0("Failed in StretchDIBits()");
	}

	if (m_hOldDib) {
		SelectObject(m_hMemDC, m_hOldDib);
	}

	DeleteObject(m_hDib);
	m_hDib = NULL;
	DeleteDC(m_hMemDC);	
	m_hMemDC = NULL;
	m_hOldDC = NULL;
}
BOOL CPreprocessor01View::SetDCPixelFormat(HDC hDC, DWORD dwFlags)
{
	PIXELFORMATDESCRIPTOR pixelDesc;
	
	pixelDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pixelDesc.nVersion = 1;
	
	pixelDesc.dwFlags = dwFlags;
	pixelDesc.iPixelType = PFD_TYPE_RGBA;
	pixelDesc.cColorBits = 24;
	pixelDesc.cRedBits = 8;
	pixelDesc.cRedShift = 16;
	pixelDesc.cGreenBits = 8;
	pixelDesc.cGreenShift = 8;
	pixelDesc.cBlueBits = 8;
	pixelDesc.cBlueShift = 0;
	pixelDesc.cAlphaBits = 0;
	pixelDesc.cAlphaShift = 0;
	pixelDesc.cAccumBits = 64;
	pixelDesc.cAccumRedBits = 16;
	pixelDesc.cAccumGreenBits = 16;
	pixelDesc.cAccumBlueBits = 16;
	pixelDesc.cAccumAlphaBits = 0;
	pixelDesc.cDepthBits = 32;
	pixelDesc.cStencilBits = 8;
	pixelDesc.cAuxBuffers = 0;
	pixelDesc.iLayerType = PFD_MAIN_PLANE;
	pixelDesc.bReserved = 0;
	pixelDesc.dwLayerMask = 0;
	pixelDesc.dwVisibleMask = 0;
	pixelDesc.dwDamageMask = 0;
	
	int nPixelIndex = ::ChoosePixelFormat(hDC, &pixelDesc);
	if (nPixelIndex == 0) // Choose default
	{
		nPixelIndex = 1;
		if (::DescribePixelFormat(hDC, nPixelIndex, 
			sizeof(PIXELFORMATDESCRIPTOR), &pixelDesc) == 0)
			return false;
	}

	if (!::SetPixelFormat(hDC, nPixelIndex, &pixelDesc))
		return false;

	return true;
}
