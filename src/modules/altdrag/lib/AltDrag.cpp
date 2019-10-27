#include "pch.h"
#include "common/dpi_aware.h"

struct AltDrag : public winrt::implements<AltDrag, IAltDrag, IAltDragCallback>
{
public:
    AltDrag(HINSTANCE hinstance, IAltDragSettings* settings) noexcept
        : m_hinstance(hinstance)
    {
        m_settings.attach(settings);
        m_settings->SetCallback(this);
    }

    // IAltDrag
    IFACEMETHODIMP_(void) Run() noexcept;
    IFACEMETHODIMP_(void) Destroy() noexcept;

    // IAltDrag
    IFACEMETHODIMP_(bool) InMoveSize() noexcept { std::shared_lock readLock(m_lock); return m_inMoveSize; }
    IFACEMETHODIMP_(void) MoveSizeStart(HWND window, HMONITOR monitor, POINT const& ptScreen) noexcept;
    IFACEMETHODIMP_(void) MoveSizeUpdate(HMONITOR monitor, POINT const& ptScreen) noexcept;
    IFACEMETHODIMP_(void) MoveSizeEnd(HWND window, POINT const& ptScreen) noexcept;
    IFACEMETHODIMP_(void) WindowCreated(HWND window) noexcept;
	IFACEMETHODIMP_(bool) OnKeyDown(PKBDLLHOOKSTRUCT info) noexcept;
	IFACEMETHODIMP_(bool) OnKeyUp(PKBDLLHOOKSTRUCT info) noexcept;
    IFACEMETHODIMP_(void) SettingsChanged() noexcept;

	LRESULT WndProc(HWND, UINT, WPARAM, LPARAM) noexcept;

protected:
	static LRESULT CALLBACK s_WndProc(HWND, UINT, WPARAM, LPARAM) noexcept;

private:
    struct require_read_lock
    {
        template<typename T>
        require_read_lock(const std::shared_lock<T>& lock) { lock; }

        template<typename T>
        require_read_lock(const std::unique_lock<T>& lock) { lock; }
    };

    struct require_write_lock
    {
        template<typename T>
        require_write_lock(const std::unique_lock<T>& lock) { lock; }
    };

    void MoveSizeStartInternal(HWND window, HMONITOR monitor, POINT const& ptScreen, require_write_lock) noexcept;
    void MoveSizeEndInternal(HWND window, POINT const& ptScreen, require_write_lock) noexcept;
    void MoveSizeUpdateInternal(HMONITOR monitor, POINT const& ptScreen, require_write_lock) noexcept;

    const HINSTANCE m_hinstance{};
    
    mutable std::shared_mutex m_lock;
    HWND m_window{};
    HWND m_windowMoveSize{};
    bool m_inMoveSize{};
    winrt::com_ptr<IAltDragSettings> m_settings;
};

// IAltDrag
IFACEMETHODIMP_(void) AltDrag::Run() noexcept
{
    std::unique_lock writeLock(m_lock);

    WNDCLASSEXW wcex{};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.lpfnWndProc = s_WndProc;
    wcex.hInstance = m_hinstance;
    wcex.lpszClassName = L"AltDrag";
    RegisterClassExW(&wcex);

    BufferedPaintInit();

    m_window = CreateWindowExW(
        WS_EX_TOOLWINDOW,
        L"AltDrag",
        L"",
        WS_POPUP,
        0, 0, 0, 0,
        nullptr,
        nullptr,
        m_hinstance, 
        this);
    if (!m_window) return;

    RegisterHotKey(
        m_window,
        1,
        m_settings->GetSettings().activationHotkey.get_modifiers(),
        m_settings->GetSettings().activationHotkey.get_code());
}

// IAltDrag
IFACEMETHODIMP_(void) AltDrag::Destroy() noexcept
{
    std::unique_lock writeLock(m_lock);

    BufferedPaintUnInit();
    if (m_window)
    {
        DestroyWindow(m_window);
        m_window = nullptr;
    }
}

// IAltDragCallback
IFACEMETHODIMP_(void) AltDrag::MoveSizeStart(HWND window, HMONITOR monitor, POINT const& ptScreen) noexcept
{
    std::unique_lock writeLock(m_lock);
    MoveSizeStartInternal(window, monitor, ptScreen, writeLock);
}

// IAltDragCallback
IFACEMETHODIMP_(void) AltDrag::MoveSizeUpdate(HMONITOR monitor, POINT const& ptScreen) noexcept
{
    std::unique_lock writeLock(m_lock);
    MoveSizeUpdateInternal(monitor, ptScreen, writeLock);
}

// IAltDragCallback
IFACEMETHODIMP_(void) AltDrag::MoveSizeEnd(HWND window, POINT const& ptScreen) noexcept
{
    std::unique_lock writeLock(m_lock);
    MoveSizeEndInternal(window, ptScreen, writeLock);
}

// IAltDragCallback
IFACEMETHODIMP_(void) AltDrag::WindowCreated(HWND window) noexcept
{
	
}

// IAltDragCallback
IFACEMETHODIMP_(bool) AltDrag::OnKeyDown(PKBDLLHOOKSTRUCT info) noexcept
{
	bool const shift = GetAsyncKeyState(VK_SHIFT) & 0x8000;
	bool const win = GetAsyncKeyState(VK_LWIN) & 0x8000;
	// MessageBox(nullptr, L"Key Pressed", L"OnKeyDown", MB_OK);
	return false;
}

// IAltDragCallback
IFACEMETHODIMP_(bool) AltDrag::OnKeyUp(PKBDLLHOOKSTRUCT info) noexcept
{
	bool const shift = GetAsyncKeyState(VK_SHIFT) & 0x8000;
	bool const win = GetAsyncKeyState(VK_LWIN) & 0x8000;
	// MessageBox(nullptr, L"Key Pressed", L"OnKeyDown", MB_OK);
	return false;
}

// IAltDragCallback
IFACEMETHODIMP_(void) AltDrag::SettingsChanged() noexcept
{
	UnregisterHotKey(m_window, 1);
	RegisterHotKey(m_window, 1,
		m_settings->GetSettings().activationHotkey.get_modifiers(),
		m_settings->GetSettings().activationHotkey.get_code());
}

void AltDrag::MoveSizeStartInternal(HWND window, HMONITOR monitor, POINT const& ptScreen, require_write_lock writeLock) noexcept
{
	RECT windowRect{};
	::GetWindowRect(window, &windowRect);
}

void AltDrag::MoveSizeUpdateInternal(HMONITOR monitor, POINT const& ptScreen, require_write_lock writeLock) noexcept
{

}

void AltDrag::MoveSizeEndInternal(HWND window, POINT const& ptScreen, require_write_lock) noexcept
{

}

LRESULT AltDrag::WndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) noexcept
{
	return DefWindowProc(window, message, wparam, lparam);
}

LRESULT CALLBACK AltDrag::s_WndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) noexcept
{
	auto thisRef = reinterpret_cast<AltDrag*>(GetWindowLongPtr(window, GWLP_USERDATA));
	if (!thisRef && (message == WM_CREATE))
	{
		const auto createStruct = reinterpret_cast<LPCREATESTRUCT>(lparam);
		thisRef = reinterpret_cast<AltDrag*>(createStruct->lpCreateParams);
		SetWindowLongPtr(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(thisRef));
	}

	return thisRef ?
		thisRef->WndProc(window, message, wparam, lparam) :
		DefWindowProc(window, message, wparam, lparam);

}

winrt::com_ptr<IAltDrag> MakeAltDrag(HINSTANCE hinstance, IAltDragSettings* settings) noexcept
{
	return winrt::make_self<AltDrag>(hinstance, settings);
}