#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <dirent.h>
#include <wininet.h>
#include <tchar.h>
#include <sys/stat.h>
#include <Shlobj.h>
#include <format>
#include <vector>

#define CURL_STATICLIB

#include "include/curl.h"
// for windows only :(

using namespace std;

BOOL isFileExists(const string &path) {
    DWORD dwAttr = GetFileAttributes((LPCSTR) path.c_str());
    if (dwAttr == 0xFFFFFFFF) return false;
    return true;
}

/*
BOOL downloadFile(const string& url, const string& path) {
    if(!isFileExists(path)) return false;
    if(URLDownloadToFile(nullptr, (LPCSTR)url.c_str(), (LPCSTR)path.c_str(), 0, nullptr) == S_OK) return true;
    return false;
}
*/

size_t curlWriteFunc(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    return fwrite(ptr, size, nmemb, stream);
}

int downloadFile(const string &url, const string &path) {
    CURL *curl = curl_easy_init();
    long responseCode;
    if (curl) {
        FILE *ofile = fopen(path.c_str(), "wb");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, ofile);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteFunc);
        CURLcode res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &responseCode);

        fclose(ofile);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) return res;
        if (responseCode >=400)
            return responseCode;
        return 0;
    }
    return -1;
}

void flushIcon() {
    system("taskkill /f /im explorer.exe");
    system(R"(attrib -h -s -r "%userprofile%\AppData\Local\IconCache.db")");
    system(R"(del /f "%userprofile%\AppData\Local\IconCache.db")");
    system(R"(attrib /s /d -h -s -r "%userprofile%\AppData\Local\Microsoft\Windows\Explorer\*")");
    system(R"(del /f "%userprofile%\AppData\Local\Microsoft\Windows\Explorer\thumbcache_32.db")");
    system(R"(del /f "%userprofile%\AppData\Local\Microsoft\Windows\Explorer\thumbcache_96.db")");
    system(R"(del /f "%userprofile%\AppData\Local\Microsoft\Windows\Explorer\thumbcache_102.db")");
    system(R"(del /f "%userprofile%\AppData\Local\Microsoft\Windows\Explorer\thumbcache_256.db")");
    system(R"(del /f "%userprofile%\AppData\Local\Microsoft\Windows\Explorer\thumbcache_1024.db")");
    system(R"(del /f "%userprofile%\AppData\Local\Microsoft\Windows\Explorer\thumbcache_idx.db")");
    system(R"(del /f "%userprofile%\AppData\Local\Microsoft\Windows\Explorer\thumbcache_sr.db")");
    system("echo y　reg delete \"HKEY_CLASSES_ROOT\\Local Settings\\Software\\Microsoft\\Windows\\CurrentVersion\\TrayNotify\" /v IconStreams");
    system("echo y　reg delete \"HKEY_CLASSES_ROOT\\Local Settings\\Software\\Microsoft\\Windows\\CurrentVersion\\TrayNotify\" /v PastIconsStream");
    system("start explorer");
}

void setclr(unsigned short clr) {
    HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hCon, clr);
}

void logui(const char *str) {
    setclr(FOREGROUND_BLUE);
    cout << str << endl;
    setclr(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void logsuc(const char *str) {
    setclr(FOREGROUND_GREEN);
    cout << str << endl;
    setclr(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void logwrn(const char *str) {
    setclr(FOREGROUND_RED | FOREGROUND_GREEN);
    cerr << str << endl;
    setclr(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void logerr(const char *str) {
    setclr(FOREGROUND_RED);
    cerr << str << endl;
    setclr(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

bool getDirUrlFiles(const string &path, vector<string> &files) {
    DIR *dir;
    dirent *ptr;
    struct stat s{};
    string fullDir;
    bool result;

    if ((dir = opendir(path.c_str())) == nullptr) {
        cerr << "E[GetDirFiles] Failed open dir." << endl;
        return false;
    }

    while ((ptr = readdir(dir)) != nullptr) {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) continue; // cur or pat dir
        fullDir = path + '\\' + ptr->d_name;
        stat(fullDir.c_str(), &s);//use full dir is true
        if (s.st_mode & S_IFDIR) continue;
        if (strcmp(ptr->d_name + strlen(ptr->d_name) - 4, ".url") != 0) continue;//not url file
        result = false;
        for (auto &file: files) {
            if (fullDir.substr(fullDir.find_last_of('\\') + 1) == file.substr(file.find_last_of('\\') + 1)) {
                result = true;
                break;
            }
        }
        if (!result) files.push_back(fullDir);
    }
    closedir(dir);
    return true;
}

// thanks @Zinc-in
string wstring2string(const wstring &wstr) {
    string result;
    int len = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS,
                                  wstr.c_str(), wstr.size(),
                                  nullptr, 0, nullptr, nullptr);
    char *buffer = new char[len + 1];
    WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS,
                        wstr.c_str(), wstr.size(),
                        buffer, len, nullptr, nullptr);
    buffer[len] = '\0';
    result.append(buffer);
    delete[] buffer;
    return result;
}

int main() {
    setclr(11);
    cout << "========Steam Icon Fix by ez4y2f========" << endl;
    setclr(6);
    cout << ">_D3bug the w0r1d.    Visit https://y2f.xyz for more information." << endl;
    setclr(15);
    /*
    char dirarr[MAX_PATH];
    getcwd(dirarr, MAX_PATH);
    string dir = dirarr;
    dir = dir.substr(dir.find_last_of('\\') + 1, dir.length() - 1);
    cout << "Current running in dir " << dir << endl;*///no need

    string progFilesVar;
    DWORD size = MAX_PATH;
    auto *buffer = new WCHAR[MAX_PATH];
    if (!SUCCEEDED(RegGetValueW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\Valve\\Steam", L"InstallPath", RRF_RT_ANY,
                                nullptr, (PVOID) buffer, &size))) {
        logerr("E Steam is not installed, exiting...");
        system("pause");
        exit(0);
    }

    progFilesVar = wstring2string(buffer);
    delete[] buffer;


    WCHAR *desktopVar;
    if (!SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Desktop, 0, nullptr, &desktopVar))) {
        logerr("E Cannot find Desktop, exiting...");
        system("pause");
        exit(0);
    }

    WCHAR *startMenuVar;
    if (!SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Programs, 0, nullptr, &startMenuVar))) {
        logwrn("W Cannot find Start menu, skipped");
    }

    string desktopDir = wstring2string(desktopVar);
    string startMenuDir = wstring2string(startMenuVar) + R"(\Steam)";
    string steamiconDir = progFilesVar + R"(\steam\games)";

    if (isFileExists(steamiconDir)) {
        cout << "Found Steam icon dir in " << steamiconDir << endl;
    } else {
        logwrn("W Cannot find steam icon dir, manual input>_");
        getline(cin, steamiconDir);
        if (!isFileExists(steamiconDir)) {
            logerr("E Path not exist. Exiting...");
            system("pause");
            exit(0);
        }
    }

    if (isFileExists(desktopDir)) {
        cout << "Found Desktop in " << desktopDir << endl;
    } else {
        logwrn("W Cannot find desktop dir, manual input>_");
        getline(cin, desktopDir);
        if (!isFileExists(desktopDir)) {
            logerr("E Path not exist. Exiting...");
            system("pause");
            exit(0);
        }
    }
    if (isFileExists(startMenuDir)) {
        cout << "Found Steam Start Menu Shortcut in " << startMenuDir << endl;
    } else {
        logwrn("W Cannot find StartMenu Dir, skipping...");
    }

    string cdnChoose;

    cout << endl;
    logui("==Type what CDN u wanna use==");
    logui("0 -> akamai");
    logui("1 -> cloudflare");

    while ((cdnChoose != "0") && (cdnChoose != "1")) {
        cout << "CDN[enter for 0]";
        getline(cin, cdnChoose);
        if (cdnChoose.empty()) {
            cdnChoose = "0";
            break;
        }
        if ((cdnChoose != "0") && (cdnChoose != "1")) cout << "Input invalid." << endl;
    }
    cout << endl;

    cdnChoose = cdnChoose == "0" ? "akamai" : "cloudflare";
    logui(vformat("Current using CDN {}", make_format_args(cdnChoose)).c_str());

    vector<string> files;
    getDirUrlFiles(desktopDir, files);
    getDirUrlFiles(startMenuDir, files);
    string url;
    string iconfile;
    char urlbuf[256]; // vars in ini(.url)
    char iconbuf[MAX_PATH];

    string iconurl = "http://cdn.{}.steamstatic.com/steamcommunity/public/images/apps/";
    iconurl = vformat(iconurl, make_format_args(cdnChoose));

    for (const auto &file: files) {
        GetPrivateProfileString(_T("InternetShortcut"), _T("URL"), nullptr, urlbuf, sizeof(urlbuf), file.c_str());
        GetPrivateProfileString(_T("InternetShortcut"), _T("IconFile"), nullptr, iconbuf, sizeof(iconbuf),
                                file.c_str());
        url = urlbuf;
        if (url.find(':') > url.length() || url.find_last_of('/') > url.length()) continue;
        if (strcmp(url.substr(0, url.find(':')).c_str(), "steam") != 0) continue; // not a steam shortcut
        iconfile = iconbuf;
        if (iconfile.find_last_of('\\') > iconfile.length()) {
            logerr("E invalid shortcut");
            continue;
        }
        url = url.substr(url.find_last_of('/') + 1);
        iconfile = iconfile.substr(iconfile.find_last_of('\\') + 1);
        cout << "Find Steam Shortcut " << file.substr(file.find_last_of('\\') + 1) << " with appid " << url << endl;
        string dwnurl = iconurl + url + "/" + iconfile;

        cout << "- try to download icon from " << dwnurl << " to " << steamiconDir + "\\" + iconfile << endl;
        int res = downloadFile(dwnurl, steamiconDir + "\\" + iconfile);
        switch (res) {
            case 0:
                logsuc("- Successful Fixed.");
                break;
            case 3:
                logwrn("Skipped, error 3, URL error");
                break;
            case 404:
                logwrn("Skipped, error 404, File Not Found");
                break;
            case 500 ...599:
                logwrn(("Error" + to_string(res) + ", Server Error,please change cdn").c_str());
                break;
            default:
                logerr(("E download failed. Check your network! Error " + to_string(res)).c_str());
                logerr("E deleting downloaded files...");
                system(("del \"" + iconfile + "\"").c_str());
                system("pause");
                exit(0);

        }
        if (res) {
            if (res == 3) {
                logwrn("Skipped, error 3, URL error");
                continue;
            }
            if (res == 404) {
                logwrn("Skipped, error 404, File Not Found");
                continue;
            }
            if (res > 500) logwrn(("Error" + to_string(res) + ", Server Error,please change cdn").c_str());

            logerr(("E download failed. Check your network! Error " + to_string(res)).c_str());
            logerr("E deleting downloaded files...");
            system(("del \"" + iconfile + "\"").c_str());
            system("pause");
            exit(0);
        } else logsuc("- Successful Fixed.");
    }

    cout << "Finish Fixing, Flushing icon cache..." << endl;
    flushIcon();
    logsuc("Success!");
    system("pause");
    return 0;
}
