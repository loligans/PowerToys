#pragma once

struct Settings;

class Trace {
public:
  static void RegisterProvider();
  static void UnregisterProvider();
  static void SettingsChanged(const Settings& settings) noexcept;
};
