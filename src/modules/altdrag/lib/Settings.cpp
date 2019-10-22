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

	IFACEMETHODIMP_(bool) GetConfig(_Out_ PWSTR buffer, _Out_ int* buffer_sizeg) noexcept;
	IFACEMETHODIMP_(void) SetConfig(PCWSTR config) noexcept;
	IFACEMETHODIMP_(void) CallCustomAction(PCWSTR action) noexcept;
	IFACEMETHODIMP_(Settings) GetSettings() noexcept { return m_settings; }

private:
	void LoadSettings(PCWSTR config, bool fromFile) noexcept;
	void SaveSettings() noexcept;

	const HINSTANCE m_hinstance;
	PCWSTR m_name{};

	Settings m_settings;
};

IFACEMETHODIMP_(bool) AltDragSettings::GetConfig(_Out_ PWSTR buffer, _Out_ int* buffer_sizeg) noexcept
{
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