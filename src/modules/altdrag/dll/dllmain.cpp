#include "pch.h"
#include <common/settings_objects.h>
#include <interface/powertoy_module_interface.h>
#include <interface/lowlevel_keyboard_event_data.h>
#include <interface/win_hook_event_data.h>
#include <interface/lowlevel_mouse_event_data.h>

extern "C" IMAGE_DOS_HEADER __ImageBase;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		Trace::RegisterProvider();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		Trace::UnregisterProvider();
		break;
	}
	return TRUE;
}

// Implement the PowerToy Module Interface and all the required methods.
class AltDragModule : public PowertoyModuleIface
{
public:
    // Return the display name of the powertoy, this will be cached
	virtual PCWSTR get_name() override
	{
		return L"AltDrag";
	}

	// Return array of the names of all events that this powertoy listens for, with
	// nullptr as the last element of the array. Nullptr can also be retured for empty
	// list.
	virtual PCWSTR* get_events() override
	{
		static const wchar_t* events[] = { ll_keyboard, ll_mouse, nullptr };
		return events;
	}

	// Return JSON with the configuration options.
	virtual bool get_config(_Out_ PWSTR buffer, _Out_ int* buffer_size) override
	{
        return m_settings->GetConfig(buffer, buffer_size);
	}

	// Called by the runner to pass the updated settings values as a serialized JSON.
	virtual void set_config(const wchar_t* config) override
	{
        m_settings->SetConfig(config);
	}

	// Enable the powertoy
	virtual void enable()
	{
        if (!m_app)
        {
            // TODO: Trace event
            m_app = MakeAltDrag(reinterpret_cast<HINSTANCE>(&__ImageBase), m_settings.get());
            if (m_app)
            {
                m_app->Run();
            }
        }
	}

	// Disable the powertoy
	virtual void disable()
	{
        if (m_app)
        {
            // TODO: Trace event
            m_app->Destroy();
            m_app = nullptr;
        }
	}

	// Returns if the powertoys is enabled
	virtual bool is_enabled() override
	{
		return (m_app != nullptr);
	}

	// Handle incoming event, data is event-specific
	virtual intptr_t signal_event(const wchar_t* name, intptr_t data)  override
	{
        if (m_app)
        {
		    if (wcscmp(name, ll_keyboard) == 0)
		    {
                // Return 1 if the keypress is to be suppressed (not forwarded to Windows), otherwise return 0.
                return HandleKeyboardHookEvent(reinterpret_cast<LowlevelKeyboardEvent*>(data));
		    }
		    else if (wcscmp(name, win_hook_event) == 0)
		    {
                // Return value is ignored
                // HandleWinHookEvent(reinterpret_cast<WinHookEvent*>(data));
		    }
			else if (wcscmp(name, ll_mouse) == 0)
			{
				return HandleMouseHookEvent(reinterpret_cast<LowlevelMouseEvent*>(data));
			}
        }
		return 0;
	}

    // Destroy the powertoy and free memory
    virtual void destroy() override
    {
        disable();
        delete this;
    }

    AltDragModule()
    {
        m_settings = MakeAltDragSettings(reinterpret_cast<HINSTANCE>(&__ImageBase), AltDragModule::get_name());
    };

private:
    winrt::com_ptr<IAltDrag> m_app;
    winrt::com_ptr<IAltDragSettings> m_settings;

    intptr_t HandleKeyboardHookEvent(LowlevelKeyboardEvent* data) noexcept;
	intptr_t HandleMouseHookEvent(LowlevelMouseEvent* data) noexcept;
    void HandleWinHookEvent(WinHookEvent* data) noexcept;
    void MoveSizeStart(HWND window, POINT const& ptScreen) noexcept;
    void MoveSizeEnd(HWND window, POINT const& ptScreen) noexcept;
    void MoveSizeUpdate(POINT const& ptScreen) noexcept;
};

intptr_t AltDragModule::HandleKeyboardHookEvent(LowlevelKeyboardEvent* data) noexcept
{
	if (data->wParam == WM_KEYDOWN || data->wParam == WM_SYSKEYDOWN)
	{
        return m_app.as<IAltDragCallback>()->OnKeyDown(data->lParam) ? 1 : 0;
	}
	return 0;
}

intptr_t AltDragModule::HandleMouseHookEvent(LowlevelMouseEvent* data) noexcept
{
	return 0;
}

void AltDragModule::HandleWinHookEvent(WinHookEvent* data) noexcept
{
	POINT ptScreen;
	GetPhysicalCursorPos(&ptScreen);

	switch (data->event)
	{
	case EVENT_SYSTEM_MOVESIZESTART:
		MoveSizeStart(data->hwnd, ptScreen);
		break;
	case EVENT_SYSTEM_MOVESIZEEND:
		MoveSizeEnd(data->hwnd, ptScreen);
		break;
	case EVENT_OBJECT_LOCATIONCHANGE:
        /*if (m_app.as<IAltDragCallback>()->InMoveSize())
        {
		    MoveSizeUpdate(ptScreen);
        }*/
		break;
    default:
        break;
	}
}

void AltDragModule::MoveSizeStart(HWND window, POINT const& ptScreen) noexcept
{
    // TODO: Look into IsInterestingWindow should be used here.
    if (auto monitor = MonitorFromPoint(ptScreen, MONITOR_DEFAULTTONULL))
    {
        m_app.as<IAltDragCallback>()->MoveSizeStart(window, monitor, ptScreen);
    }
}

void AltDragModule::MoveSizeEnd(HWND window, POINT const& ptScreen) noexcept
{
    // TODO: Look into IsInterestingWindow should be used here.
    m_app.as<IAltDragCallback>()->MoveSizeEnd(window, ptScreen);
}

void AltDragModule::MoveSizeUpdate(POINT const& ptScreen) noexcept
{
    if (auto monitor = MonitorFromPoint(ptScreen, MONITOR_DEFAULTTONULL))
    {
        m_app.as<IAltDragCallback>()->MoveSizeUpdate(monitor, ptScreen);
    }
}

extern "C" __declspec(dllexport) PowertoyModuleIface * __cdecl powertoy_create()
{
	return new AltDragModule();
}
