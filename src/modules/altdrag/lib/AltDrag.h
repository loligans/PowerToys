#pragma once

interface IAltDragSettings;

interface __declspec(uuid("{C62903CC-9096-41D0-A9A5-1E4100B1C417}")) IAltDrag : public IUnknown
{
    IFACEMETHOD_(void, Run)() = 0;
    IFACEMETHOD_(void, Destroy)() = 0;
};

interface __declspec(uuid("{965BC508-EE6D-42CE-9255-81F5BA076661}")) IAltDragCallback : public IUnknown
{
    IFACEMETHOD_(void, MoveSizeStart)(HWND window, HMONITOR monitor, POINT const& ptScreen) = 0;
    IFACEMETHOD_(void, MoveSizeUpdate)(HMONITOR monitor, POINT const& ptScreen) = 0;
    IFACEMETHOD_(void, MoveSizeEnd)(HWND window, POINT const& ptScreen) = 0;
    IFACEMETHOD_(void, WindowCreated)(HWND window) = 0;
	IFACEMETHOD_(bool, OnKeyDown)(PKBDLLHOOKSTRUCT info) = 0;
    IFACEMETHOD_(void, SettingsChanged)() = 0;
};

winrt::com_ptr<IAltDrag> MakeAltDrag(HINSTANCE hinstance, IAltDragSettings* settings) noexcept;
