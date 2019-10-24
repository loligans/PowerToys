#include "pch.h"
#include <common/settings_objects.h>

struct AltDragSettings : winrt::implements<AltDragSettings, IAltDragSettings>
{
public:
	AltDragSettings(HINSTANCE hinstance, PCWSTR name)
		: m_hinstance(hinstance)
		, m_name(name)
	{
		LoadSettings(name, true /*fromFile*/);
	}

    IFACEMETHODIMP_(void) SetCallback(interface IAltDragCallback* callback) { m_callback.attach(callback); }
	IFACEMETHODIMP_(bool) GetConfig(_Out_ PWSTR buffer, _Out_ int* buffer_sizeg) noexcept;
	IFACEMETHODIMP_(void) SetConfig(PCWSTR config) noexcept;
	IFACEMETHODIMP_(void) CallCustomAction(PCWSTR action) noexcept;
	IFACEMETHODIMP_(Settings) GetSettings() noexcept { return m_settings; }

private:
	void LoadSettings(PCWSTR config, bool fromFile) noexcept;
	void SaveSettings() noexcept;

    winrt::com_ptr<IAltDragCallback> m_callback;
	const HINSTANCE m_hinstance;
	PCWSTR m_name{};

	Settings m_settings;

    struct
    {
        PCWSTR name;
        bool* value;
        int resourceId;
    } m_configBools[1] = {
        { L"", &m_settings.focusWindowWhenDragging, 101 }
    };
};

IFACEMETHODIMP_(bool) AltDragSettings::GetConfig(_Out_ PWSTR buffer, _Out_ int* buffer_sizeg) noexcept
{
    PowerToysSettings::Settings settings(m_hinstance, m_name);

    settings.set_description(ALTDRAG_DESCRIPTION);
    settings.set_icon_key(L"pt-alt-drag");
    settings.set_overview_link(L"");
    settings.set_video_link(L"");

    settings.add_hotkey(L"", L"", m_settings.activationHotkey);

    // for (auto const& setting : )

	return false;
}

IFACEMETHODIMP_(void) AltDragSettings::SetConfig(PCWSTR config) noexcept
{

}

IFACEMETHODIMP_(void) AltDragSettings::CallCustomAction(PCWSTR action) noexcept
{

}

void AltDragSettings::LoadSettings(PCWSTR config, bool fromFile) noexcept
{

}

void AltDragSettings::SaveSettings() noexcept
{

}

winrt::com_ptr<IAltDragSettings> MakeAltDragSettings(HINSTANCE hinstance, PCWSTR name) noexcept
{
	return winrt::make_self<AltDragSettings>(hinstance, name);
}