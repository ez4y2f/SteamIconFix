#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <tchar.h>
#include <Shlobj.h>
#include <format>
#include <vector>
#include <string>
#include <fstream>
#include <regex>
#include <map>
#include <algorithm>

#include <curl/curl.h>
#include <json/json.h>
#include <codecvt>
// for windows only :(

using namespace std;

BOOL isFileExists(const wstring &path) {
    DWORD dwAttr = GetFileAttributesW(path.c_str());
    if (dwAttr == INVALID_FILE_ATTRIBUTES) return FALSE;
    return TRUE;
}

size_t curlWriteFunc(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    return fwrite(ptr, size, nmemb, stream);
}

int downloadFile(const string &url, const wstring &path) {
    CURL *curl = curl_easy_init();
    long responseCode = 0;
    if (curl) {
        FILE *ofile = _wfopen(path.c_str(), L"wb");
        if (!ofile) {
            curl_easy_cleanup(curl);
            return -2;
        }
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, ofile);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteFunc);
        CURLcode res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &responseCode);

        fclose(ofile);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) return res;
        if (responseCode >= 400) return (int)responseCode;
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
    cout << str << endl;
    setclr(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void logerr(const char *str) {
    setclr(FOREGROUND_RED);
    cout << str << endl;
    setclr(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

// thanks @Zinc-in
string wstring2string(const wstring &wstr) {
    string result;
    int len = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS,
                                  wstr.c_str(), (int)wstr.size(),
                                  nullptr, 0, nullptr, nullptr);
    if (len <= 0) return {};
    char *buffer = new char[len + 1];
    WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS,
                        wstr.c_str(), (int)wstr.size(),
                        buffer, len, nullptr, nullptr);
    buffer[len] = '\0';
    result.append(buffer);
    delete[] buffer;
    return result;
}

bool getDirUrlFiles(const wstring &path, vector<wstring> &files) {
    WIN32_FIND_DATAW ffd{};
    wstring searchPath = path;
    if (!searchPath.empty() && searchPath.back() != L'\\')
        searchPath += L'\\';
    searchPath += L"*.url";

    HANDLE hFind = FindFirstFileW(searchPath.c_str(), &ffd);
    if (hFind == INVALID_HANDLE_VALUE) {
        cerr << "E[GetDirFiles] Failed open dir." << endl;
        return false;
    }

    do {
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;

        wstring fullPath = path;
        if (!fullPath.empty() && fullPath.back() != L'\\')
            fullPath += L'\\';
        fullPath += ffd.cFileName;

        bool result = false;
        for (auto &file : files) {
            auto pos1 = fullPath.find_last_of(L'\\');
            auto pos2 = file.find_last_of(L'\\');
            wstring name1 = (pos1 == wstring::npos) ? fullPath : fullPath.substr(pos1 + 1);
            wstring name2 = (pos2 == wstring::npos) ? file : file.substr(pos2 + 1);
            if (name1 == name2) {
                result = true;
                break;
            }
        }
        if (!result) files.push_back(fullPath);

    } while (FindNextFileW(hFind, &ffd));

    FindClose(hFind);
    return true;
}

static size_t WriteToStringCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    auto* str = static_cast<string*>(userdata);
    auto realSize = size * nmemb;
    str->append(ptr, realSize);
    return realSize;
}

string getClientIconHash(const string& appid) {
    CURL* curl = curl_easy_init();
    string url = "https://api.steamcmd.net/v1/info/" + appid;

    if (!curl) {
        logerr("E Curl init failed.");
        return "";
    }

    string resp;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteToStringCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);

    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        string err = curl_easy_strerror(res);
        curl_easy_cleanup(curl);
        logerr(("E Curl failed err: " + err).c_str());
        return "";
    }

    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    curl_easy_cleanup(curl);

    if (httpCode < 200 || httpCode >= 300) {
        logerr(("E Request clienticon failed: " + to_string(httpCode)).c_str());
        return "";
    }

    Json::CharReaderBuilder builder;
    builder["collectComments"] = false;

    Json::Value root;
    string errs;

    unique_ptr<Json::CharReader> reader(builder.newCharReader());
    const char* begin = resp.data();
    const char* end = resp.data() + resp.size();

    bool ok = reader->parse(begin, end, &root, &errs);
    if (!ok) {
        logerr(("E Json parse failed: " + errs).c_str());
        return "";
    }

    return root["data"][appid]["common"]["clienticon"].asString();
}

wstring readFileToWstring(const wstring& path) {
    FILE* fp = _wfopen(path.c_str(), L"rb");
    if (!fp) return L"";

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    string buffer;
    buffer.resize(size);

    fread(buffer.data(), 1, size, fp);
    fclose(fp);

    wstring_convert<codecvt_utf8_utf16<wchar_t>> conv;
    return conv.from_bytes(buffer);
}

std::vector<std::string> getInstalledSteamAppIds(const std::wstring& steamRoot) {
    std::vector<std::string> result;

    std::wstring steamappsRoot = steamRoot;
    if (!steamappsRoot.empty() && steamappsRoot.back() != L'\\')
        steamappsRoot += L'\\';
    steamappsRoot += L"steamapps";

    std::vector<std::wstring> libraries;
    libraries.push_back(steamappsRoot);

    std::wstring vdfPath = steamappsRoot;
    if (!vdfPath.empty() && vdfPath.back() != L'\\')
        vdfPath += L'\\';
    vdfPath += L"libraryfolders.vdf";

    wstring vdfContent = readFileToWstring(vdfPath);
    if (!vdfContent.empty()) {
        wregex re(L"\"path\"\\s*\"([^\"]+)\"");
        for (wsregex_iterator it(vdfContent.begin(), vdfContent.end(), re), end; it != end; ++it) {
            wstring libPath = (*it)[1].str();
            if (!libPath.empty() && libPath.back() != L'\\')
                libPath += L'\\';
            libPath += L"steamapps";
            libraries.push_back(libPath);
        }
    }


    std::sort(libraries.begin(), libraries.end());
    libraries.erase(std::unique(libraries.begin(), libraries.end()), libraries.end());

    for (const auto& lib : libraries) {
        WIN32_FIND_DATAW fd{};
        std::wstring pattern = lib;
        if (!pattern.empty() && pattern.back() != L'\\')
            pattern += L'\\';
        pattern += L"appmanifest_*.acf";

        HANDLE hFind = FindFirstFileW(pattern.c_str(), &fd);
        if (hFind == INVALID_HANDLE_VALUE) continue;

        do {
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;

            std::wstring fullPath = lib;
            if (!fullPath.empty() && fullPath.back() != L'\\')
                fullPath += L'\\';
            fullPath += fd.cFileName;

            wstring content = readFileToWstring(fullPath);
            if (content.empty()) continue;

            wregex appidRe(L"\"appid\"\\s*\"([0-9]+)\"");
            wsmatch m;
            if (regex_search(content, m, appidRe)) {
                int id = _wtoi(m[1].str().c_str());
                if (id > 0) {
                    result.push_back(to_string(id));
                    cout << "Found " << to_string(id) << endl;
                }
            }

        } while (FindNextFileW(hFind, &fd));

        FindClose(hFind);
    }
    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());

    return result;
}

int main() {

    ios::sync_with_stdio(false);

    setclr(11);
    cout << "========Steam Icon Fix========" << endl;
    setclr(6);
    cout << ">_D3bug the w0r1d.    Visit https://y2f.xyz for more information." << endl;
    setclr(15);

    wstring progFilesVar;
    DWORD size = MAX_PATH * sizeof(WCHAR);
    auto *buffer = new WCHAR[MAX_PATH];
    if (!SUCCEEDED(RegGetValueW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\Valve\\Steam", L"InstallPath", RRF_RT_ANY,
                                nullptr, (PVOID) buffer, &size))) {
        logerr("E Steam is not installed, exiting...");
        system("pause");
        delete[] buffer;
        return 0;
    }

    progFilesVar = buffer;
    delete[] buffer;


    PWSTR desktopVar = nullptr;
    if (!SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Desktop, 0, nullptr, &desktopVar))) {
        logerr("E Cannot find Desktop, exiting...");
        system("pause");
        return 0;
    }

    PWSTR startMenuVar = nullptr;
    HRESULT hrStart = SHGetKnownFolderPath(FOLDERID_Programs, 0, nullptr, &startMenuVar);
    if (!SUCCEEDED(hrStart)) {
        logwrn("W Cannot find Start menu, skipped");
    }

    wstring desktopDir = desktopVar;
    CoTaskMemFree(desktopVar);

    wstring startMenuDir;
    if (SUCCEEDED(hrStart)) {
        startMenuDir = startMenuVar;
        CoTaskMemFree(startMenuVar);
        if (!startMenuDir.empty() && startMenuDir.back() != L'\\')
            startMenuDir += L'\\';
        startMenuDir += L"Steam";
    }

    wstring steamiconDir = progFilesVar;
    if (!steamiconDir.empty() && steamiconDir.back() != L'\\')
        steamiconDir += L'\\';
    steamiconDir += L"steam\\games";

    if (isFileExists(steamiconDir)) {
        cout << "Found Steam icon dir in " << wstring2string(steamiconDir) << endl;
    } else {
        logwrn("W Cannot find steam icon dir, manual input>_");
        string input;
        getline(cin, input);
        if (input.empty()) {
            logerr("E Path not exist. Exiting...");
            system("pause");
            return 0;
        }
        steamiconDir = wstring(input.begin(), input.end());
        if (!isFileExists(steamiconDir)) {
            logerr("E Path not exist. Exiting...");
            system("pause");
            return 0;
        }
    }

    if (isFileExists(desktopDir)) {
        cout << "Found Desktop in " << wstring2string(desktopDir) << endl;
    } else {
        logwrn("W Cannot find desktop dir, manual input>_");
        string input;
        getline(cin, input);
        if (input.empty()) {
            logerr("E Path not exist. Exiting...");
            system("pause");
            return 0;
        }
        desktopDir = wstring(input.begin(), input.end());
        if (!isFileExists(desktopDir)) {
            logerr("E Path not exist. Exiting...");
            system("pause");
            return 0;
        }
    }

    if (!startMenuDir.empty() && isFileExists(startMenuDir)) {
        cout << "Found Steam Start Menu Shortcut in " << wstring2string(startMenuDir) << endl;
    } else {
        logwrn("W Cannot find StartMenu Dir, skipping...");
    }

    int cdnChoose = -1;

    cout << endl;
    logui("==Type what CDN u wanna use==");
    logui("0 -> fastly");
    logui("1 -> cloudflare");

    while ((cdnChoose != 0) && (cdnChoose != 1)) {
        cout << "CDN[enter for 0]";
        string cdnChooseStr;
        getline(cin, cdnChooseStr);
        if (cdnChooseStr.length() > 1) {
            cout << "Input Invalid" << endl;
            continue;
        }
        if (cdnChooseStr.empty()) cdnChooseStr = "0";
        char cdnChooseChar = cdnChooseStr[0];
        if (!(cdnChooseChar < '2' && cdnChooseChar >= '0')) {
            cout << "Input Invalid" << endl;
            continue;
        }
        cdnChoose = cdnChooseChar - '0';
    }
    cout << endl;

    vector<wstring> files;
    getDirUrlFiles(desktopDir, files);
    if (!startMenuDir.empty() && isFileExists(startMenuDir)) {
        getDirUrlFiles(startMenuDir, files);
    }

    wchar_t urlbuf[256]; // vars in ini(.url)
    wchar_t iconbuf[MAX_PATH];

    string iconurl;
    if (cdnChoose == 0) iconurl = "https://shared.fastly.steamstatic.com/community_assets/images/apps/";
    if (cdnChoose == 1) iconurl = "https://cdn.cloudflare.steamstatic.com/steamcommunity/public/images/apps/";

    vector<string> appIds = getInstalledSteamAppIds(progFilesVar);
    map<string, string> ciHashs;
    for (const auto &file : files) {
        GetPrivateProfileStringW(L"InternetShortcut", L"URL", nullptr, urlbuf, sizeof(urlbuf), file.c_str());

        GetPrivateProfileStringW(L"InternetShortcut", L"IconFile", nullptr, iconbuf, sizeof(iconbuf),

                                file.c_str());

        wstring urlW = urlbuf;
        wstring iconW = iconbuf;

        if (urlW.empty()) continue;

        auto colonPos = urlW.find(L':');
        auto slashPos = urlW.find_last_of(L'/');
        if (colonPos == wstring::npos || slashPos == wstring::npos) continue;

        if (urlW.substr(0, colonPos) != L"steam") continue; // not a steam shortcut

        if (iconW.find_last_of(L'\\') == wstring::npos) {
            logerr("E invalid shortcut");
            continue;
        }

        wstring appIdW = urlW.substr(slashPos + 1);
        wstring iconFileNameW = iconW.substr(iconW.find_last_of(L'\\') + 1);

        string appId = wstring2string(appIdW);
        string iconFileName = wstring2string(iconFileNameW);

        cout << "Find Steam Shortcut "
             << wstring2string(file.substr(file.find_last_of(L'\\') + 1))
             << " with appid " << appId << endl;
        appIds.push_back(appId);
        ciHashs[appId] = iconFileName;
    }
    sort(appIds.begin(), appIds.end());
    appIds.erase(unique(appIds.begin(), appIds.end()), appIds.end());
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    for (const auto &appId : appIds) {
        string iconFileName = ciHashs.contains(appId) ? ciHashs[appId] : getClientIconHash(appId);
        if (iconFileName.empty()) {
            logwrn(("W iconFileName with appid " + appId + " is Empty, Skipped.").c_str());
            continue;
        }
        string dwnurl = iconurl + appId + "/" + iconFileName;

        wstring dstPath = steamiconDir;
        if (!dstPath.empty() && dstPath.back() != L'\\')
            dstPath += L'\\';
        dstPath += converter.from_bytes(iconFileName);

        cout << "- try to download icon from " << dwnurl << " to " << wstring2string(dstPath) << endl;
        int res = downloadFile(dwnurl, dstPath);

        bool terminateFlag = false; // terminate for error not-to-skip

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
                terminateFlag = true;
                break;
            default:
                logerr(("E download failed. Check your network! Error " + to_string(res)).c_str());
                terminateFlag = true;
        }
        if (terminateFlag){
            logerr("E deleting downloaded files...");
            DeleteFileW(dstPath.c_str());
            system("pause");
            return 0;
        }
    }

    cout << "Finish Fixing, Flushing icon cache..." << endl;
    flushIcon();
    logsuc("Success!");
    system("pause");
    return 0;
}