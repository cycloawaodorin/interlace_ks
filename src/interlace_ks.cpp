//----------------------------------------------------------------------------------
//		インターレース化フィルタ
//----------------------------------------------------------------------------------
#include <windows.h>
#include "filter.h"
#include "version.h"
#include <cmath>

//---------------------------------------------------------------------
//		フィルタ構造体定義
//---------------------------------------------------------------------
static constexpr size_t CHECK_N = 1;
static const TCHAR *check_name[CHECK_N] = {"疑似インターレース化"};
static int check_default[CHECK_N]  = {0};
#define FILTER_NAME "インターレース化"

FILTER_DLL filter = {
	FILTER_FLAG_INTERLACE_FILTER|FILTER_FLAG_EX_INFORMATION,
	0, 0, // 設定ウィンドウサイズ - 設定しない
	const_cast<TCHAR *>(FILTER_NAME),
	0, // トラックバーの数
	NULL, NULL, NULL, NULL, // その他トラックバー関連
	CHECK_N,
	const_cast<TCHAR **>(check_name),
	check_default,
	func_proc,
	NULL, // func_init
	NULL, // func_exit
	NULL, NULL, NULL, NULL, NULL, 0,
	const_cast<TCHAR *>(FILTER_NAME " " VERSION " by KAZOON"),
	NULL, NULL, NULL, NULL, NULL, NULL,
	func_is_saveframe,
	NULL, NULL, NULL, NULL,
	0, 0 // reserve
};

EXTERN_C FILTER_DLL __declspec(dllexport) * __stdcall
GetFilterTable(void)
{
	return &filter;
}

static constexpr float weights[] = {
	0.0036891354301046677f, 0.015056142680948828f, -0.03399863151427591f, -0.06663731776798075f, 0.13550528412853946f, 0.4463853870426637f,
	0.4463853870426637f, 0.13550528412853946f, -0.06663731776798075f, -0.03399863151427591f, 0.015056142680948828f, 0.0036891354301046677f
};
// ycp_tb の y の前後の色を補間して fpip->ycp_edit に書き込む
static void
interpolate(FILTER_PROC_INFO *fpip, PIXEL_YC *ycp_tb, int y)
{
	int start=y-5-(y&1), end=y+7-(y&1);
	if ( start < 0 || fpip->h < end ) {
		int skip=0;
		if ( start < 0 ) {
			skip=-start;
		}
		if ( fpip->h < end ) {
			end = fpip->h;
		}
		for (int x=0; x<fpip->w; x++) {
			float yy=0.0f, cb=0.0f, cr=0.0f, w=0.0f;
			for ( int sy=start+skip; sy<end; sy++ ) {
				const PIXEL_YC *px=ycp_tb+(sy*fpip->max_w+x);
				float wi=weights[sy-start];
				yy += wi*px->y;
				cb += wi*px->cb;
				cr += wi*px->cr;
				w += wi;
			}
			PIXEL_YC *px=fpip->ycp_edit+(y*fpip->max_w+x);
			px->y = static_cast<short>(std::round(yy/w));
			px->cb = static_cast<short>(std::round(cb/w));
			px->cr = static_cast<short>(std::round(cr/w));
		}
	} else {
		for (int x=0; x<fpip->w; x++) {
			float yy=0.0f, cb=0.0f, cr=0.0f;
			for ( int sy=start; sy<end; sy++ ) {
				const PIXEL_YC *px=ycp_tb+(sy*fpip->max_w+x);
				float wi=weights[sy-start];
				yy += wi*px->y;
				cb += wi*px->cb;
				cr += wi*px->cr;
			}
			PIXEL_YC *px=fpip->ycp_edit+(y*fpip->max_w+x);
			px->y = static_cast<short>(std::round(yy));
			px->cb = static_cast<short>(std::round(cb));
			px->cr = static_cast<short>(std::round(cr));
		}
	}
}

#include <cstdio>
// 本体
BOOL
func_proc(FILTER *fp, FILTER_PROC_INFO *fpip)
{
	PIXEL_YC *ycp_top, *ycp_bottom;
	int frame = fpip->frame - fpip->frame%2;
	
	if ( (!(fpip->flag&FILTER_PROC_INFO_FLAG_INVERT_FIELD_ORDER))^(!(fpip->flag&FILTER_PROC_INFO_FLAG_INVERT_INTERLACE)) ) {
		// TFF
		ycp_top = static_cast<PIXEL_YC *>(fp->exfunc->get_ycp_source_cache(fpip->editp, frame, 0));
		ycp_bottom = static_cast<PIXEL_YC *>(fp->exfunc->get_ycp_source_cache(fpip->editp, frame+1, 0));
	} else {
		// BFF
		ycp_bottom = static_cast<PIXEL_YC *>(fp->exfunc->get_ycp_source_cache(fpip->editp, frame, 0));
		ycp_top = static_cast<PIXEL_YC *>(fp->exfunc->get_ycp_source_cache(fpip->editp, frame+1, 0));
	}
	if ( ycp_top == nullptr ) {
		ycp_top = fpip->ycp_temp;
		memset(ycp_top, 0, sizeof(PIXEL_YC)*((fpip->max_w)*(fpip->h)));
	}
	if ( ycp_bottom == nullptr ) {
		ycp_bottom = fpip->ycp_temp;
		memset(ycp_bottom, 0, sizeof(PIXEL_YC)*((fpip->max_w)*(fpip->h)));
	}
	
	if ( fp->check[0] ) {
		for (int y=0; y<fpip->h; y++) {
			if ( y&1 ) {
				interpolate(fpip, ycp_bottom, y);
			} else {
				interpolate(fpip, ycp_top, y);
			}
		}
	} else {
		for (int y=0; y<fpip->h; y++) {
			if ( y&1 ) {
				memcpy(fpip->ycp_edit+y*fpip->max_w, ycp_bottom+y*fpip->max_w, sizeof(PIXEL_YC)*fpip->w);
			} else {
				memcpy(fpip->ycp_edit+y*fpip->max_w, ycp_top+y*fpip->max_w, sizeof(PIXEL_YC)*fpip->w);
			}
		}
	}
	
	return TRUE;
}

BOOL
func_is_saveframe(FILTER *fp, void *editp, int saveno, int frame, int fps, int edit_flag, int inter)
{
	return ( !!( saveno*fps/30 - (saveno+1)*fps/30 ) );
}
