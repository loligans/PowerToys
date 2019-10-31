#pragma once
#include <Windows.h>

namespace
{
	const wchar_t* ll_mouse = L"ll_mouse";
}

struct LowlevelMouseEvent
{
	MSLLHOOKSTRUCT* lParam;
	WPARAM wParam;
};