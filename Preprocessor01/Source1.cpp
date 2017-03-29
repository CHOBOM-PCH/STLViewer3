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

double VSCALE = 10.0;
double line_width = 0.0015;       // 0.001 = 1u -> 0.0015      0.0001 = 100n  0.00001 = 10n

int forward_check(CFace * Now, CFace * Idx, int * s_dir, int line)  // fdir = 1; far
{
	int ret=0;
	if (*s_dir == 0) return 0;
	GoFirst((CNode*&)Idx);

	if (Idx) {
		L4:
		if (Idx->seq == 0) {
			if (*s_dir == 1) {
				if (fabs(Now->mf[0]-Idx->m[0])<line_width && fabs(Now->mf[1]-Idx->m[1])<line_width && fabs(Now->mf[2]-Idx->m[2])<line_width) {
					ret = 1; *s_dir = 1;
				} else if (fabs(Now->mf[0]-Idx->mf[0])<line_width && fabs(Now->mf[1]-Idx->mf[1])<line_width && fabs(Now->mf[2]-Idx->mf[2])<line_width) {
					ret = 1; *s_dir = -1;
				}
			} else {
				if (fabs(Now->m[0]-Idx->m[0])<line_width && fabs(Now->m[1]-Idx->m[1])<line_width && fabs(Now->m[2]-Idx->m[2])<line_width) {
					ret = 1; *s_dir = 1;
				} else if (fabs(Now->m[0]-Idx->mf[0])<line_width && fabs(Now->m[1]-Idx->mf[1])<line_width && fabs(Now->m[2]-Idx->mf[2])<line_width) {
					ret = 1; *s_dir = -1;
				}
			}
			if (ret) {
				Now->seq++; Idx->seq++; //Now->seq = 1; Idx->seq = 1;
				Now->line = line; Idx->line = line;
				Now->linenext = Idx->id; Idx->linebefore = Now->id;
			}
			return 1;
		} else {
			if (Idx->nxt == NULL) {
				*s_dir = 0;
				Now->linenext = -100;
				return 0;
			} else {
				Idx = (CFace*) Idx->nxt;
				goto L4;
			}
		}
	}

	return ret;
}

int backward_check(CFace * Now, CFace * Idx, int * s_dir, int line)  // bdir = -1; middle
{
	int ret=0;
	if (*s_dir == 0) return 0;
	GoFirst((CNode*&)Idx);

	if (Idx) {
		L4:
		if (Idx->seq == 0) {
			if (*s_dir == 1) {
				if (fabs(Now->mf[0]-Idx->m[0])<line_width && fabs(Now->mf[1]-Idx->m[1])<line_width && fabs(Now->mf[2]-Idx->m[2])<line_width) {
					ret = 1; *s_dir = 1;
				} else if (fabs(Now->mf[0]-Idx->mf[0])<line_width && fabs(Now->mf[1]-Idx->mf[1])<line_width && fabs(Now->mf[2]-Idx->mf[2])<line_width) {
					ret = 1; *s_dir = -1;
				}
			} else {  // -1
				if (fabs(Now->m[0]-Idx->m[0])<line_width && fabs(Now->m[1]-Idx->m[1])<line_width && fabs(Now->m[2]-Idx->m[2])<line_width) {
					ret = 1; *s_dir = 1;
				} else if (fabs(Now->m[0]-Idx->mf[0])<line_width && fabs(Now->m[1]-Idx->mf[1])<line_width && fabs(Now->m[2]-Idx->mf[2])<line_width) {
					ret = 1; *s_dir = -1;
				}
			}
			if (ret) {
				Now->seq++; Idx->seq++; //Now->seq = 1; Idx->seq = 1;
				Now->line = line; Idx->line = line;
				Now->linebefore = Idx->id; Idx->linenext = Now->id;
			}
			return 1;
		} else {
			if (Idx->nxt == NULL) {
				*s_dir = 0;
				Now->linebefore = -100;
				return 0;
			} else {
				Idx = (CFace*) Idx->nxt;
				goto L4;
			}
		}
	}

	return ret;
}