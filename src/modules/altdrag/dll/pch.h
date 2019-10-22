#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Unknwn.h>
#include <winrt/base.h>
#include <ProjectTelemetry.h>
#include <TraceLoggingActivity.h>
#include <wil/common.h>
#include <wil/result.h>

#include "lib/trace.h"
#include "lib/resource.h"
#include "lib/Settings.h"

namespace winrt
{
	using namespace ::winrt;
}