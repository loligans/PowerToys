#include "pch.h"
#include "shortcut_guide.h"
#include "target_state.h"
#include "trace.h"
#include <common/settings_objects.h>

extern "C" IMAGE_DOS_HEADER __ImageBase;

OverlayWindow* instance = nullptr;

OverlayWindow::OverlayWindow() {
  init_settings();
}

const wchar_t * OverlayWindow::get_name() {
  return L"Shortcut Guide";
}

const wchar_t ** OverlayWindow::get_events() {
  static const wchar_t* events[2] = { ll_keyboard, 0 };
  return events;
}

bool OverlayWindow::get_config(wchar_t* buffer, int *buffer_size) {
  HINSTANCE hinstance = reinterpret_cast<HINSTANCE>(&__ImageBase);

  PowerToysSettings::Settings settings(hinstance, get_name());
  settings.set_description(L"Shows a help overlay with Windows shortcuts when the Windows key is pressed.");
  settings.set_overview_link(L"https://github.com/microsoft/PowerToys/blob/master/src/modules/shortcut_guide/README.md");
  settings.set_icon_key(L"pt-shortcut-guide");

  settings.add_int_spinner(
    pressTime.name,
    pressTime.resourceId,
    pressTime.value,
    100,
    10000,
    100
  );

  settings.add_int_spinner(
    overlayOpacity.name,
    overlayOpacity.resourceId,
    overlayOpacity.value,
    0,
    100,
    1
  );

  settings.add_choice_group(
    theme.name,
    theme.resourceId,
    theme.value,
    theme.keys_and_texts
  );

  return settings.serialize_to_buffer(buffer, buffer_size);
}

void OverlayWindow::set_config(const wchar_t * config) {
  try {
    PowerToysSettings::PowerToyValues _values =
      PowerToysSettings::PowerToyValues::from_json_string(config);
    if (_values.is_int_value(pressTime.name)) {
      int press_delay_time = _values.get_int_value(pressTime.name);
      pressTime.value = press_delay_time;
      if (target_state) {
        target_state->set_delay(press_delay_time);
      }
    }
    if (_values.is_int_value(overlayOpacity.name)) {
      int overlay_opacity = _values.get_int_value(overlayOpacity.name);
      overlayOpacity.value = overlay_opacity;
      if (winkey_popup) {
        winkey_popup->apply_overlay_opacity(((float)overlayOpacity.value) / 100.0f);
      }
    }
    if (_values.is_string_value(theme.name)) {
      theme.value = _values.get_string_value(theme.name);
      winkey_popup->set_theme(theme.value);
    }
    _values.save_to_settings_file();
  }
  catch (std::exception&) {
    // Improper JSON.
  }
}

void OverlayWindow::enable() {
  if (!_enabled) {
    Trace::EnableShortcutGuide(true);
    winkey_popup = new D2DOverlayWindow();
    winkey_popup->apply_overlay_opacity(((float)overlayOpacity.value)/100.0f);
    winkey_popup->set_theme(theme.value);
    target_state = new TargetState(pressTime.value);
    winkey_popup->initialize();
  }
  _enabled = true;
}

void OverlayWindow::disable(bool trace_event) {
  if (_enabled) {
    if (trace_event) {
      Trace::EnableShortcutGuide(false);
    }
    winkey_popup->hide();
    target_state->exit();
    delete target_state;
    delete winkey_popup;
    target_state = nullptr;
    winkey_popup = nullptr;
  }
  _enabled = false;
}

void OverlayWindow::disable() {
  this->disable(true);
}

bool OverlayWindow::is_enabled() {
  return _enabled;
}

intptr_t OverlayWindow::signal_event(const wchar_t * name, intptr_t data) {
  if (_enabled && wcscmp(name, ll_keyboard) == 0) {
    auto& event = *(reinterpret_cast<LowlevelKeyboardEvent*>(data));
    if (event.wParam == WM_KEYDOWN ||
        event.wParam == WM_SYSKEYDOWN ||
        event.wParam == WM_KEYUP ||
        event.wParam == WM_SYSKEYUP) {
      bool supress = target_state->signal_event(event.lParam->vkCode, 
                                                event.wParam == WM_KEYDOWN || event.wParam == WM_SYSKEYDOWN);
      return supress ? 1 : 0;
    }
  }
  return 0;
}

void OverlayWindow::on_held() {
  auto active_window = get_filtered_active_window();
  winkey_popup->show(active_window);
}

void OverlayWindow::on_held_press(DWORD vkCode) {
  winkey_popup->animate(vkCode);
}

void OverlayWindow::was_hidden() {
  target_state->was_hiden();
}

void OverlayWindow::destroy() {
  this->disable(false);
  delete this;
  instance = nullptr;
}

void OverlayWindow::init_settings() {
  try {
    PowerToysSettings::PowerToyValues settings =
      PowerToysSettings::PowerToyValues::load_from_settings_file(OverlayWindow::get_name());
    if (settings.is_int_value(pressTime.name)) {
      pressTime.value = settings.get_int_value(pressTime.name);
    }
    if (settings.is_int_value(overlayOpacity.name)) {
      overlayOpacity.value = settings.get_int_value(overlayOpacity.name);
    }
    if (settings.is_string_value(theme.name)) {
      theme.value = settings.get_string_value(theme.name);
    }
  }
  catch (std::exception&) {
    // Error while loading from the settings file. Just let default values stay as they are.
  }
}
