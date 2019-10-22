#pragma once

#include <common/settings_objects.h>

namespace AltDragActions
{
    enum Action
    {
        /* No action will be performed. */
        Nothing,
        /* Move a window. */
        Move,
        /* Resizes a window. */
        Resize,
        /* Minimize a window. */
        Minimize,
        /* Lower the window. */
        Lower,
        /* Make a window topmost. */
        AlwaysOnTop,
        /* Center a window on the screen. */
        Center,
        /* Close a window. */
        Close
    };
}
struct Settings
{
    bool focusWindowWhenDragging = false;
    bool lowerWindowByMiddleClickingTitleBar = false;
    bool snapToScreenBorders = false;
    bool snapToOutsideOtherWindows = false;
    INT8 leftMouseButton_Action = AltDragActions::Nothing;
    INT8 middleMouseButton_Action = AltDragActions::Nothing;
    INT8 rightMouseButton_Action = AltDragActions::Nothing;
    PowerToysSettings::HotkeyObject activationHotkey;
};

__interface __declspec(uuid("{C47261B6-EC9B-4E5C-9192-1D138CB97E03}")) IAltDragSettings : public IUnknown
{
    IFACEMETHOD_(void, SetCallback)(interface IFancyZonesCallback* callback) = 0;
    IFACEMETHOD_(bool, GetConfig)(_Out_ PWSTR buffer, _Out_ int* buffer_size) = 0;
    IFACEMETHOD_(void, SetConfig)(PCWSTR config) = 0;
    IFACEMETHOD_(void, CallCustomAction)(PCWSTR action) = 0;
    IFACEMETHOD_(Settings, GetSettings)() = 0;
};

winrt::com_ptr<IAltDragSettings> MakeAltDragSettings(HINSTANCE hinstance, PCWSTR config) noexcept;