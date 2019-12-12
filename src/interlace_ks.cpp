//----------------------------------------------------------------------------------
//		インターレース化フィルタ
//----------------------------------------------------------------------------------
#include <windows.h>
#include <stdlib.h>
#include <limits.h>
#include "filter.h"
#include "version.h"

//---------------------------------------------------------------------
//		フィルタ構造体定義
//---------------------------------------------------------------------
static constexpr size_t CHECK_N = 1;
static TCHAR check_name0[] = "疑似インターレース化";
static TCHAR *check_name[CHECK_N] = {check_name0};
static int check_default[CHECK_N]  = {0};
#define FILTER_NAME "インターレース化"
static TCHAR filter_name[] = FILTER_NAME;
static TCHAR filter_information[] = FILTER_NAME " " VERSION " by KAZOON";

FILTER_DLL filter = {
	FILTER_FLAG_INTERLACE_FILTER|FILTER_FLAG_EX_INFORMATION,
	0, 0, // 設定ウィンドウサイズ - 設定しない
	filter_name,
	0, // トラックバーの数
	NULL, NULL, NULL, NULL, // その他トラックバー関連
	CHECK_N,
	check_name,
	check_default,
	func_proc,
	NULL, // func_init
	NULL, // func_exit
	NULL, NULL, NULL, NULL, NULL, 0,
	filter_information,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	0, 0 // reserve
};

EXTERN_C FILTER_DLL __declspec(dllexport) * __stdcall
GetFilterTable(void)
{
	return &filter;
}

// 本体
BOOL
func_proc(FILTER *fp, FILTER_PROC_INFO *fpip)
{
	return TRUE;
}
