#pragma once
#include "resource.h"

#define WIN32_LEAN_AND_MEAN
#include <Unknwn.h>
#include <winrt/base.h>
#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <ProjectTelemetry.h>
#include <shellapi.h>
#include <strsafe.h>
#include <TraceLoggingActivity.h>
#include <wil\resource.h>
#include <wil\result.h>
#include <windows.foundation.h>
#include <psapi.h>

#include "trace.h"
#include "Settings.h"
#include "common/common.h"

#pragma comment(lib, "windowsapp")

namespace winrt
{
    using namespace ::winrt;
}
