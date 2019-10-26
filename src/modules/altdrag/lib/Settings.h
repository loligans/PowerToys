#pragma once

#include <common/settings_objects.h>

enum class Action
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
    /* Center a window on the screen. */
    Center,
    /* Close a window. */
    Close
};
struct Settings
{
    bool focusWindow_WhenDragging = false;
    bool lowerWindow_MiddleClickTitleBar = false;
    bool snapToScreenBorders = false;
    bool snapToOutsideOtherWindows = false;
    Action leftMouseButton = Action::Nothing;
	Action middleMouseButton = Action::Nothing;
	Action rightMouseButton = Action::Nothing;
    PowerToysSettings::HotkeyObject activationHotkey = PowerToysSettings::HotkeyObject::from_settings(false, false, true, false, VK_MENU, L"");
};

interface __declspec(uuid("{C47261B6-EC9B-4E5C-9192-1D138CB97E03}")) IAltDragSettings : public IUnknown
{
    IFACEMETHOD_(void, SetCallback)(interface IAltDragCallback* callback) = 0;
    IFACEMETHOD_(bool, GetConfig)(_Out_ PWSTR buffer, _Out_ int* buffer_size) = 0;
    IFACEMETHOD_(void, SetConfig)(PCWSTR config) = 0;
    IFACEMETHOD_(Settings, GetSettings)() = 0;
};

winrt::com_ptr<IAltDragSettings> MakeAltDragSettings(HINSTANCE hinstance, PCWSTR config) noexcept;