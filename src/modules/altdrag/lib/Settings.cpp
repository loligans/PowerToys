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
    } m_configBools[4] = {
		{ L"altdrag_focusWindowWhenDragging", &m_settings.focusWindow_WhenDragging, 101 },
		{ L"altdrag_lowerWindowByMiddleClickingTitleBar", &m_settings.lowerWindow_MiddleClickTitleBar, 101 },
		{ L"altdrag_snapToScreenBorders", &m_settings.snapToScreenBorders, 101 },
		{ L"altdrag_snapToOutsideOtherWindows", &m_settings.snapToOutsideOtherWindows, 101 },
    };

	struct
	{
		PCWSTR name;
		Action* value;
		int resourceId;
	} m_configActions[3] = {
		{ L"altdrag_leftMouseButton", &m_settings.leftMouseButton, 101 },
		{ L"altdrag_middleMouseButton", &m_settings.middleMouseButton, 101 },
		{ L"altdrag_rightMouseButton", &m_settings.rightMouseButton, 101 }
	};

    const std::wstring m_activation_hotkey_name = L"altdrag_activation_hotkey";
};

IFACEMETHODIMP_(bool) AltDragSettings::GetConfig(_Out_ PWSTR buffer, _Out_ int* buffer_size) noexcept
{
    PowerToysSettings::Settings settings(m_hinstance, m_name);

    settings.set_description(ALTDRAG_SETTING_DESCRIPTION);
    settings.set_icon_key(L"pt-alt-drag");
    settings.set_overview_link(L"");
    settings.set_video_link(L"");

    settings.add_hotkey(m_activation_hotkey_name, 101, m_settings.activationHotkey);

    for (auto const& setting : m_configBools)
    {
        settings.add_bool_toogle(setting.name, setting.resourceId, *setting.value);
    }

    for (auto const& setting : m_configActions)
    {
        // settings.add_dropdown(setting.name, setting.resourceId, L"", { {"", ""} });
        /*
            TODO: Add action dropdown. 
            Need to figure out how to get enum class string presentation of actions.
        */
    }

	return settings.serialize_to_buffer(buffer, buffer_size);
}

IFACEMETHODIMP_(void) AltDragSettings::SetConfig(PCWSTR config) noexcept try
{
    LoadSettings(config, false);
    SaveSettings();
    m_callback->SettingsChanged();
    Trace::SettingsChanged(m_settings);
}
CATCH_LOG();

IFACEMETHODIMP_(void) AltDragSettings::CallCustomAction(PCWSTR action) noexcept
{

}

void AltDragSettings::LoadSettings(PCWSTR config, bool fromFile) noexcept try
{
    PowerToysSettings::PowerToyValues values = fromFile ?
        PowerToysSettings::PowerToyValues::load_from_settings_file(m_name) :
        PowerToysSettings::PowerToyValues::from_json_string(config);

    for (auto const& setting : m_configBools)
    {
        if (values.is_bool_value(setting.name))
        {
            *setting.value = values.get_bool_value(setting.name);
        }
    }

    for (auto const& setting : m_configActions)
    {
        if (values.is_int_value(setting.name))
        {
            *setting.value = static_cast<Action>(values.get_int_value(setting.name));
        }
    }

    if (values.is_object_value(m_activation_hotkey_name))
    {
        m_settings.activationHotkey = PowerToysSettings::HotkeyObject::from_json(values.get_json(m_activation_hotkey_name));
    }
}
CATCH_LOG();

void AltDragSettings::SaveSettings() noexcept try
{
    PowerToysSettings::PowerToyValues values(m_name);

    for (auto const& setting : m_configBools)
    {
        values.add_property(setting.name, *setting.value);
    }

    for (auto const& setting : m_configActions)
    {
        values.add_property(setting.name, static_cast<int>(*setting.value));
    }

    values.add_property(m_activation_hotkey_name, m_settings.activationHotkey);

    values.save_to_settings_file();
}
CATCH_LOG();

winrt::com_ptr<IAltDragSettings> MakeAltDragSettings(HINSTANCE hinstance, PCWSTR name) noexcept
{
	return winrt::make_self<AltDragSettings>(hinstance, name);
}