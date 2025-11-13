#include <Arduino.h>
#include <Preferences.h>
#include <libstorage.h>

static const char* kNamespace = "cred";
static const char* kWiFiSsidKey = "wifi_ssid";
static const char* kWiFiPwdKey  = "wifi_pwd";

bool saveWiFiCredentials(const String &ssid, const String &password) {
  if (ssid.length() == 0) return false;
  Preferences prefs;
  if (!prefs.begin(kNamespace, false)) return false;
  bool ok = prefs.putString(kWiFiSsidKey, ssid) > 0;
  ok = ok && (prefs.putString(kWiFiPwdKey, password) >= 0);
  prefs.end();
  return ok;
}

bool loadWiFiCredentials(String &outSsid, String &outPassword) {
  Preferences prefs;
  if (!prefs.begin(kNamespace, true)) return false;
  String s = prefs.getString(kWiFiSsidKey, "");
  String p = prefs.getString(kWiFiPwdKey,  "");
  prefs.end();
  if (s.length() == 0) return false;
  outSsid = s;
  outPassword = p;
  return true;
}

bool clearWiFiCredentials() {
  Preferences prefs;
  if (!prefs.begin(kNamespace, false)) return false;
  bool ok = prefs.remove(kWiFiSsidKey);
  ok = prefs.remove(kWiFiPwdKey) || ok;
  prefs.end();
  return ok;
}

bool hasWiFiCredentials() {
  Preferences prefs;
  if (!prefs.begin(kNamespace, true)) return false;
  String s = prefs.getString(kWiFiSsidKey, "");
  prefs.end();
  return s.length() > 0;
}
