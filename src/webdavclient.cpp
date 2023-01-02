#include <errno.h>
#include <unistd.h>
#include <cstring>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <inttypes.h>
#include <errno.h>
#include <fcntl.h>
#include "lang.h"
#include "webdav/client.hpp"
#include "webdavclient.h"
#include "windows.h"
#include "util.h"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

static const char *months[12] = {
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec",
};
namespace WebDAV
{

	static int DownloadCallback(void *context, uint64_t dltotal, uint64_t dlnow, uint64_t ultotal, uint64_t ulnow)
	{
		uint64_t *bytes_transfered = (uint64_t *)context;
		*bytes_transfered = dlnow;
		return 0;
	}

	static int UploadCallback(void *context, uint64_t dltotal, uint64_t dlnow, uint64_t ultotal, uint64_t ulnow)
	{
		uint64_t *bytes_transfered = (uint64_t *)context;
		*bytes_transfered = ulnow;
		return 0;
	}

	int WebDavClient::Connect(const char *host, const char *user, const char *pass)
	{
		std::string url = std::string(host);
		std::size_t scheme_pos = url.find_first_of("://");
		std::string root_folder = "/";
		if (scheme_pos != std::string::npos)
		{
			std::size_t root_folder_pos = url.find_first_of("/", scheme_pos+3);
			if (root_folder_pos != std::string::npos)
			{
				root_folder = url.substr(root_folder_pos);
				url = url.substr(0, root_folder_pos);
			}
		}

		WebDAV::dict_t options = {
			{"webdav_hostname", url},
			{"webdav_root", root_folder},
			{"webdav_username", user},
			{"webdav_password", pass}};
		client = new WebDAV::Client(options);
		connected = true;

		return 1;
	}

	/*
	 * LastResponse - return a pointer to the last response received
	 */
	const char *WebDavClient::LastResponse()
	{
		return (const char *)response;
	}

	/*
	 * IsConnected - return true if connected to remote
	 */
	bool WebDavClient::IsConnected()
	{
		return connected;
	}

	/*
	 * Ping - return true if connected to remote
	 */
	bool WebDavClient::Ping()
	{
		connected = client->check();
		return connected;
	}

	/*
	 * Quit - disconnect from remote
	 *
	 * return 1 if successful, 0 otherwise
	 */
	int WebDavClient::Quit()
	{
		if (client != NULL)
			delete (client);
		client = NULL;
		connected = false;
		return 1;
	}

	/*
	 * Mkdir - create a directory at server
	 *
	 * return 1 if successful, 0 otherwise
	 */
	int WebDavClient::Mkdir(const char *ppath)
	{
		return client->create_directory(ppath);
	}

	/*
	 * Rmdir - remove directory and all files under directory at remote
	 *
	 * return 1 if successful, 0 otherwise
	 */
	int WebDavClient::_Rmdir(const char *ppath)
	{
		return client->clean(ppath);
	}

	/*
	 * Rmdir - remove directory and all files under directory at remote
	 *
	 * return 1 if successful, 0 otherwise
	 */
	int WebDavClient::Rmdir(const char *path, bool recursive)
	{
		if (stop_activity)
			return 1;

		std::vector<FsEntry> list = ListDir(path);
		int ret;
		for (int i = 0; i < list.size(); i++)
		{
			if (stop_activity)
				return 1;

			if (list[i].isDir && recursive)
			{
				if (strcmp(list[i].name, "..") == 0)
					continue;
				ret = Rmdir(list[i].path, recursive);
				if (ret == 0)
				{
					sprintf(status_message, "%s %s", lang_strings[STR_FAIL_DEL_DIR_MSG], list[i].path);
					return 0;
				}
			}
			else
			{
				sprintf(activity_message, "%s %s\n", lang_strings[STR_DELETING], list[i].path);
				ret = Delete(list[i].path);
				if (ret == 0)
				{
					sprintf(status_message, "%s %s", lang_strings[STR_FAIL_DEL_FILE_MSG], list[i].path);
					return 0;
				}
			}
		}
		ret = _Rmdir(path);
		if (ret == 0)
		{
			sprintf(status_message, "%s %s", lang_strings[STR_FAIL_DEL_DIR_MSG], path);
			return 0;
		}

		return 1;
	}

	/*
	 * Get - issue a GET command and write received data to output
	 *
	 * return 1 if successful, 0 otherwise
	 */

	int WebDavClient::Get(const char *outputfile, const char *ppath)
	{
		return client->download(ppath, outputfile, &bytes_transfered, DownloadCallback);
	}

	bool WebDavClient::FileExists(const char *ppath)
	{
		std::string path = ppath;
		path = Util::Ltrim(path, "/");
		return client->check(path);
	}

	/*
	 * Put - issue a PUT command and send data from input
	 *
	 * return 1 if successful, 0 otherwise
	 */
	int WebDavClient::Put(const char *inputfile, const char *ppath)
	{
		return client->upload(ppath, inputfile, &bytes_transfered, UploadCallback);
	}

	int WebDavClient::Rename(const char *src, const char *dst)
	{
		return client->move(src, dst);
	}

	int WebDavClient::Delete(const char *ppath)
	{
		return client->clean(ppath);
	}

	int WebDavClient::Size(const char *ppath, uint64_t *size)
	{
		WebDAV::dict_t file_info = client->info(ppath);
		*size = std::stoll(WebDAV::get(file_info, "size"));
		return 1;
	}

	std::vector<FsEntry> WebDavClient::ListDir(const char *path)
	{
		std::vector<FsEntry> out;
		FsEntry entry;
		memset(&entry, 0, sizeof(FsEntry));
		if (strlen(path) > 1 && path[strlen(path) - 1] == '/')
		{
			strlcpy(entry.directory, path, strlen(path) - 1);
		}
		else
		{
			sprintf(entry.directory, "%s", path);
		}
		sprintf(entry.name, "..");
		sprintf(entry.path, "%s", entry.directory);
		sprintf(entry.display_size, "%s", lang_strings[STR_FOLDER]);
		entry.file_size = 0;
		entry.isDir = true;
		out.push_back(entry);

		WebDAV::dict_items_t files = client->list(path);
		for (int i = 0; i < files.size(); i++)
		{
			FsEntry entry;
			memset(&entry, 0, sizeof(entry));
			sprintf(entry.directory, "%s", path);
			sprintf(entry.name, WebDAV::get(files[i], "name").c_str());
			sprintf(entry.display_date, WebDAV::get(files[i], "modified").c_str());

			if (strlen(path) == 1 and path[0] == '/')
			{
				sprintf(entry.path, "%s%s", path, WebDAV::get(files[i], "name").c_str());
			}
			else
			{
				sprintf(entry.path, "%s/%s", path, WebDAV::get(files[i], "name").c_str());
			}

			std::string resource_type = WebDAV::get(files[i], "type");
			entry.isDir = resource_type == "d:collection" || resource_type == "D:collection";
			entry.file_size = 0;
			if (!entry.isDir)
			{
				entry.file_size = std::stoll(WebDAV::get(files[i], "size"));
				if (entry.file_size < 1024)
				{
					sprintf(entry.display_size, "%dB", entry.file_size);
				}
				else if (entry.file_size < 1024 * 1024)
				{
					sprintf(entry.display_size, "%.2fKB", entry.file_size * 1.0f / 1024);
				}
				else if (entry.file_size < 1024 * 1024 * 1024)
				{
					sprintf(entry.display_size, "%.2f%MB", entry.file_size * 1.0f / (1024 * 1024));
				}
				else
				{
					sprintf(entry.display_size, "%.2fGB", entry.file_size * 1.0f / (1024 * 1024 * 1024));
				}
			}
			else
			{
				sprintf(entry.display_size, "%s", lang_strings[STR_FOLDER]);
			}

			out.push_back(entry);
		}

		return out;
	}

	std::string WebDavClient::GetPath(std::string ppath1, std::string ppath2)
	{
		std::string path1 = ppath1;
		std::string path2 = ppath2;
		path1 = Util::Rtrim(Util::Trim(path1, " "), "/");
		path2 = Util::Rtrim(Util::Trim(path2, " "), "/");
		path1 = path1 + "/" + path2;
		return path1;
	}

}