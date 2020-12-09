#include "utils.h"

#include <stdlib.h>

string to_string(int val) {
  char buf[32];
  snprintf(buf, sizeof(buf), "%d", val);
  return string(buf);
}

int to_int(string val) {
  return atoi(val.c_str());
}

double to_double(string val) {
  return atof(val.c_str());
}

string capitalize(string str) {
  int i = 0;
  while (str[i]) {
    while (isspace(str[i]))
      i++;
    str[i] = toupper(str[i]);
    while (str[i] && !isspace(str[i]))
      i++;
  }
  return str;
}

#ifdef WIN32
#define BB_HKEY "Software\\BlingBling\\" PACKAGE_VERSION
#define WIN32_DEFAULT_DATADIR "C:\\Program Files\\BlingBling\\data\\"

void set_data_dir(string dir) {
  HKEY key;
  DWORD lpdw;

  if (RegCreateKeyEx(
          HKEY_CURRENT_USER, BB_HKEY,
          0,   // reserved
          "",  // ptr to null-term string specifying the object type of this key
          REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key,
          &lpdw) == ERROR_SUCCESS) {
    const char* tmp = dir.c_str();
    RegSetValueEx(key, "datadir", 0, REG_SZ, (BYTE*)tmp, dir.length() + 1);
  }
}

string get_data_dir() {
  HKEY key;
  char buf[256];

  if (RegOpenKeyEx(HKEY_CURRENT_USER, BB_HKEY,
                   0,  // reserved
                   KEY_QUERY_VALUE, &key) == ERROR_SUCCESS) {
    DWORD dsize = sizeof(int);
    DWORD dwtype = REG_SZ;
    char* tmp;

    if (RegQueryValueEx(key, "datadir", 0, &dwtype, (BYTE*)&tmp, &dsize) == 0)
      return string(tmp);
  }

  set_data_dir(WIN32_DEFAULT_DATADIR);
  return WIN32_DEFAULT_DATADIR;
}

#else
string get_data_dir() {
  return DATADIR;
}
#endif
