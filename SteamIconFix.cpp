#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <vector>
#include <dirent.h>
#include <wininet.h>
#include <tchar.h>
#include <sys/stat.h>
#include <Shlobj.h>
#include <algorithm>
#include <format>
#define CURL_STATICLIB
#include "include/curl.h"
// for windows only :(

using namespace std;

BOOL isFileExists(const string& path) {
    DWORD dwAttr = GetFileAttributes((LPCSTR)path.c_str());
    if (dwAttr == 0xFFFFFFFF) return false;
    return true;
}

bool getDirFiles(const string& path, vector<string> &files) {
    DIR *dir;
    dirent *ptr;
    struct stat s{};

    if((dir = opendir(path.c_str())) == nullptr) {
        cerr << "E[GetDirFiles] Failed open dir." << endl;
        return false;
    }

    while((ptr = readdir(dir)) != nullptr) {
        if(strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) continue; // cur or pat dir
        stat(ptr->d_name, &s);
        if(s.st_mode & S_IFDIR) continue;

        files.emplace_back(ptr->d_name);
    }

    closedir(dir);
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

int downloadFile(const string& url, const string& path) {
    CURL *curl = curl_easy_init();
    if(curl) {
        FILE *ofile = fopen(path.c_str(), "wb");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, ofile);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteFunc);
        CURLcode res = curl_easy_perform(curl);

        fclose(ofile);
        curl_easy_cleanup(curl);

        if(res != CURLE_OK) return res;
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
    HANDLE hCon =GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hCon,clr);
}

void logui(const char* str) {
    setclr(FOREGROUND_BLUE);
    cout << str << endl;
    setclr(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void logsuc(const char* str) {
    setclr(FOREGROUND_GREEN);
    cout << str << endl;
    setclr(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void logwrn(const char* str) {
    setclr(FOREGROUND_RED | FOREGROUND_GREEN);
    cerr << str << endl;
    setclr(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void logerr(const char* str) {
    setclr(FOREGROUND_RED);
    cerr << str << endl;
    setclr(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

// thanks @Zinc-in
string wstring2string(const wstring& wstr) {
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
    char dirarr[MAX_PATH];
    getcwd(dirarr, MAX_PATH);
    string dir = dirarr;
    dir = dir.substr(dir.find_last_of('\\') + 1, dir.length() - 1);
    cout << "Current running in dir " << dir << endl;

    char *progFilesVar;
    if(!(progFilesVar = getenv("programfiles(x86)"))) {
        logerr("E Cannot find programfiles(x86), exiting...");
        system("pause");
        exit(0);
    }

    WCHAR *desktopVar;
    if(!SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Desktop, 0, nullptr, &desktopVar))) {
        logerr("E Cannot find Desktop, exiting...");
        system("pause");
        exit(0);
    }

    WCHAR *startMenuVar;
    if(!SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Programs, 0, nullptr, &startMenuVar))) {
        logwrn("W Cannot find Start menu, skipped");
    }

    string progFilesStr;
    progFilesStr.assign(progFilesVar, strlen(progFilesVar));
    string desktopDir = wstring2string(desktopVar);
    string startMenuDir = wstring2string(startMenuVar) + R"(\Steam)";
    string steamiconDir = progFilesStr + R"(\Steam\steam\games)";

    if(isFileExists(steamiconDir)) {
        cout << "Found Steam icon dir in " << steamiconDir << endl;
    }else {
        logwrn("W Cannot find steam icon dir, manual input>_");
        getline(cin, steamiconDir);
        if(!isFileExists(steamiconDir)) {
            logerr("E Path not exist. Exiting...");
            system("pause");
            exit(0);
        }
    }

    if(isFileExists(desktopDir)) {
        cout << "Found Desktop in " << desktopDir << endl;
    }else {
        logwrn("W Cannot find desktop dir, manual input>_");
        getline(cin, desktopDir);
        if(!isFileExists(desktopDir)) {
            logerr("E Path not exist. Exiting...");
            system("pause");
            exit(0);
        }
    }

    string cdnChoose;

    cout << endl;
    logui("==Type what CDN u wanna use==");
    logui("0 -> akamai");
    logui("1 -> cloudflare");

    while ((cdnChoose != "0") && (cdnChoose != "1")) {
        cout << "CDN[enter for 0]";
        getline(cin, cdnChoose);
        if(cdnChoose.empty()) {
            cdnChoose = "0";
            break;
        }
        if((cdnChoose != "0") && (cdnChoose != "1")) cout << "Input invalid." << endl;
    }

    cdnChoose = cdnChoose == "0" ? "akamai" : "cloudflare";
    logui(vformat("Current using CDN {}", make_format_args(cdnChoose)).c_str());

    vector<string> files;
    vector<string> downloadedAppid; // program find .url files in both desktop and start menu, that makes it no repeating

    string temp;
    getDirFiles(desktopDir, files);

    if(isFileExists(startMenuDir)) {
        cout << endl << "Found Steam Start Menu Shortcut in " << startMenuDir << endl;
        getDirFiles(startMenuDir, files);
    }else {
        logwrn("W Cannot find StartMenu Dir, skipping...");
    }

    char urlbuf[256]; // vars in ini(.url)
    char iconbuf[MAX_PATH];

    string iconurl = "http://cdn.{}.steamstatic.com/steamcommunity/public/images/apps/";
    iconurl = vformat(iconurl, make_format_args(cdnChoose));

    for(const auto & file : files) {
        if(file.find_last_of('.') > file.length()) continue;
        if(strcmp(file.substr(file.find_last_of('.') + 1, file.length() - 1).c_str(), "url") != 0) continue; // not an url file
        GetPrivateProfileString(_T("InternetShortcut"), _T("URL"), nullptr, urlbuf, sizeof(urlbuf), (desktopDir + "\\" + file).c_str());
        GetPrivateProfileString(_T("InternetShortcut"),  _T("IconFile"), nullptr, iconbuf, sizeof(iconbuf), (desktopDir + "\\" + file).c_str());
        temp = urlbuf;
        if(temp.find(':') > temp.length() || temp.find_last_of('/') > temp.length()) continue;
        if(strcmp(temp.substr(0, temp.find(':')).c_str(), "steam") != 0) continue; // not a steam shortcut
        temp = temp.substr(temp.find_last_of('/') + 1, temp.length() - 1); // appid;
        cout << "Find Steam Shortcut " << file << " with appid " << temp << endl;
        if(find(downloadedAppid.begin(), downloadedAppid.end(), temp) != downloadedAppid.end()) {
            cout << "Have dealt with before, skip..." << endl;
            continue;
        }
        downloadedAppid.emplace_back(temp);

        string iconfile = iconbuf;
        if(iconfile.find_last_of('\\') > iconfile.length()) {
            logerr("E invalid shortcut");
            continue;
        }
        string dwnurl = iconurl + temp + '/' + iconfile.substr(iconfile.find_last_of('\\') + 1, iconfile.length() - 1);

        if(iconfile.find(steamiconDir) == string::npos || strcmp(iconfile.substr(iconfile.find_last_of('.') + 1, iconfile.length() - 1).c_str(), "ico") != 0){
            cout << "Needn't re-download, skip..." << endl;
            continue; // icon file is not "clienticon"
        }

        cout << "- try to download icon from " << dwnurl << " to " << iconfile << endl;
        int res = downloadFile(dwnurl, iconfile);
        if(res) {
            if(res == 3) logwrn("Skipped, error 3, needn't redownload");

            logerr(("E download failed. Check your network! Error " + to_string(res)).c_str());
            logerr("E deleting downloaded files...");
            system(("del \"" + iconfile + "\"").c_str());
            system("pause");
            exit(0);
        }
        else logsuc("- Successful Fixed.");
    }

    cout << "Finish Fixing, Flushing icon cache..." << endl;
    flushIcon();
    logsuc("Success!");
    system("pause");
    return 0;
}
