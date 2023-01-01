#ifndef LAUNCHER_CONFIG_H
#define LAUNCHER_CONFIG_H

#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include "fs.h"

#define APP_ID "WDVCLI001"
#define DATA_PATH "ux0:data/" APP_ID
#define APP_PATH "ux0:app/" APP_ID
#define CONFIG_INI_FILE  DATA_PATH "/config.ini"

#define WEBDAV_CLIENT_VPK_URL "https://github.com/cy33hc/vita-webdav-client/releases/download/latest/webdavclient.vpk"
#define WEBDAV_CLIENT_VERSION_URL "https://github.com/cy33hc/vita-webdav-client/releases/download/latest/version.txt"
#define WEBDAV_CLIENT_VERSION_PATH APP_PATH "/version.txt"
#define WEBDAV_CLIENT_VERSION_UPDATE_PATH DATA_PATH "/tmp/version.txt"
#define WEBDAV_CLIENT_VPK_UPDATE_PATH DATA_PATH "/tmp/webdavclient.vpk"

#define CONFIG_GLOBAL "Global"

#define CONFIG_DEFAULT_STYLE_NAME "Default"
#define CONFIG_SWAP_XO "swap_xo"

#define CONFIG_BACKGROUD_MUSIC "backgroud_music"
#define CONFIG_ENABLE_BACKGROUND_MUSIC "enable_backgroud_music"

#define CONFIG_WEBDAV_SERVER_NAME "webdav_server_name"
#define CONFIG_WEBDAV_SERVER "webdav_server"
#define CONFIG_WEBDAV_SERVER_USER "webdav_server_user"
#define CONFIG_WEBDAV_SERVER_PASSWORD "webdav_server_password"

#define CONFIG_LAST_SITE "last_site"

#define CONFIG_LOCAL_DIRECTORY "local_directory"
#define CONFIG_REMOTE_DIRECTORY "remote_directory"
#define CONFIG_UPDATE_WARN_MISSING "warn_missing_installs"

#define CONFIG_LANGUAGE "language"

struct FtpSettings {
    char site_name[32];
    char server[256];
    char username[33];
    char password[25];
};

extern bool swap_xo;
extern std::vector<std::string> sites;
extern std::map<std::string,FtpSettings> site_settings;
extern char local_directory[MAX_PATH_LENGTH];
extern char remote_directory[MAX_PATH_LENGTH];
extern char app_ver[6];
extern char last_site[32];
extern char display_site[32];
extern char language[32];
extern FtpSettings *webdav_settings;
extern bool warn_missing_installs;

namespace CONFIG {
    void LoadConfig();
    void SaveConfig();
    void RemoveFromMultiValues(std::vector<std::string> &multi_values, std::string value);
    void ParseMultiValueString(const char* prefix_list, std::vector<std::string> &prefixes, bool toLower);
    std::string GetMultiValueString(std::vector<std::string> &multi_values);
}
#endif
