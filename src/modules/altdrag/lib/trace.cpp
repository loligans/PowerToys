#include "pch.h"
#include "trace.h"

TRACELOGGING_DEFINE_PROVIDER(
  g_hProvider,
  "Microsoft.PowerToys",
  // {38e8889b-9731-53f5-e901-e8a7c1753074}
  (0x38e8889b, 0x9731, 0x53f5, 0xe9, 0x01, 0xe8, 0xa7, 0xc1, 0x75, 0x30, 0x74),
  TraceLoggingOptionProjectTelemetry());

void Trace::RegisterProvider() {
  TraceLoggingRegister(g_hProvider);
}

void Trace::UnregisterProvider() {
  TraceLoggingUnregister(g_hProvider);
}

void Trace::SettingsChanged(const Settings& settings) noexcept
{
    TraceLoggingWrite(
        g_hProvider,
        "AltDrag::Event::SettingsChanged",
        ProjectTelemetryPrivacyDataTag(ProjectTelemetryTag_ProductAndServicePerformance),
        TraceLoggingKeyword(PROJECT_KEYWORD_MEASURE),
        TraceLoggingBoolean(settings.focusWindow_WhenDragging, "Focus Window When Dragging"),
        TraceLoggingBoolean(settings.lowerWindow_MiddleClickTitleBar, "Lower Window By Middle Clicking Title Bar"),
        TraceLoggingBoolean(settings.snapToScreenBorders, "Snap to the Screen Borders"),
        TraceLoggingBoolean(settings.snapToOutsideOtherWindows, "Snap to the Outside of Other Windows"),
        TraceLoggingValue(static_cast<int>(settings.leftMouseButton), "Left Mouse Button Action."),
        TraceLoggingValue(static_cast<int>(settings.middleMouseButton), "Middle Mouse Button Action."),
        TraceLoggingValue(static_cast<int>(settings.rightMouseButton), "Right Mouse Button Action."));
}
