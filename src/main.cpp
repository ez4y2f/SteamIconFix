#include <iostream>
#include <windows.h>
#include <vector>
#include <dirent.h>
#include <winreg.h>
#include <tchar.h>

#define CURL_STATICLIB

#include "steamIconFix.h"
#include "lnkReader.h"
// for windows only :(

using namespace std;

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
    char *userProfileVar;
    if (!(progFilesVar = getenv("programfiles(x86)"))) {
        logerr("E Cannot find programfiles(x86), exiting...");
        system("pause");
        exit(0);
    }
    if (!(userProfileVar = getenv("UserProfile"))) {
        logerr("E Cannot find UserProfile, exiting...");
        system("pause");
        exit(0);
    }

//    string progFilesStr;
//    progFilesStr.assign(progFilesVar, strlen(progFilesVar));
//    string userProfileStr;
//    userProfileStr.assign(userProfileVar, strlen(userProfileVar));
    string steamStartMenuDir = "\0";
    getSteamStartMenuDir(steamStartMenuDir);
    lnkReader lnkReader;
    lnkReader.run(steamStartMenuDir + R"(\Steam.lnk)");
    string SteamFolderDir = lnkReader.getPath().substr(0, lnkReader.getPath().length() - 10);//not include "\Steam.exe"
    string steamIconDir = SteamFolderDir + R"(\steam\games)";
    string desktopDir = "\0";
    getDesktopDir(desktopDir);

    if (isFileExists(steamIconDir)) {
        cout << "Found Steam icon dir in " << steamIconDir << endl;
    } else {
        logwrn("W Cannot find steam icon dir, manual input>_");
        getline(cin, steamIconDir);
        if (!isFileExists(steamIconDir)) {
            logerr("E Path not exist. Exiting...");
            system("pause");
            exit(0);
        }
    }

    if (isFileExists(desktopDir)) {
        cout << "Fount Desktop in " << desktopDir << endl;
    } else {
        logwrn("W Cannot find desktop dir, manual input>_");
        getline(cin, desktopDir);
        if (!isFileExists(desktopDir)) {
            logerr("E Path not exist. Exiting...");
            system("pause");
            exit(0);
        }
    }

    cout << endl;

    vector<string> files;
    string temp;
    getDirFiles(desktopDir, files, 'd');
    getDirFiles(steamStartMenuDir, files, 's');
    char urlbuf[256]; // vars in ini(.url)
    char iconbuf[MAX_PATH];

    string iconurl = "http://cdn.cloudflare.steamstatic.com/steamcommunity/public/images/apps/";

    for (auto &file: files) {
        //if (file.find_last_of('.') > file.length()) continue;
        //if(strcmp(file.substr(file.find_last_of('.') + 1, file.length() - 1).c_str(), "url") != 0) continue; // not an url file
        if (file.substr(file.length() - 4, 1) == "d") {
            file.replace(file.length() - 4, 1,".");
            GetPrivateProfileString(_T("InternetShortcut"), _T("URL"), nullptr, urlbuf, sizeof(urlbuf),
                                    (desktopDir + "\\" + file).c_str());
            GetPrivateProfileString(_T("InternetShortcut"), _T("IconFile"), nullptr, iconbuf, sizeof(iconbuf),
                                    (desktopDir + "\\" + file).c_str());
        } else {
            file.replace(file.length() - 4, 1,".");
            GetPrivateProfileString(_T("InternetShortcut"), _T("URL"), nullptr, urlbuf, sizeof(urlbuf),
                                    (steamStartMenuDir + "\\" + file).c_str());
            GetPrivateProfileString(_T("InternetShortcut"), _T("IconFile"), nullptr, iconbuf, sizeof(iconbuf),
                                    (steamStartMenuDir + "\\" + file).c_str());
        }
        temp = urlbuf;
        if (temp.find(':') > temp.length() || temp.find_last_of('/') > temp.length()) continue;
        if (strcmp(temp.substr(0, temp.find(':')).c_str(), "steam") != 0) continue; // not a steam shortcut
        temp = temp.substr(temp.find_last_of('/') + 1, temp.length() - 1); // appid;
        cout << "Find Steam Shortcut " << file << " with appid " << temp << endl;


        string iconfile = iconbuf;
        if (iconfile.find_last_of('\\') > iconfile.length()) {
            logerr("E invalid shortcut");
            continue;
        }
        string dwnurl = iconurl + temp + '/' + iconfile.substr(iconfile.find_last_of('\\') + 1, iconfile.length() - 1);
        cout << "- try to download icon from " << dwnurl << " to " << iconfile << endl;
        int res = downloadFile(dwnurl, iconfile);
        if (res) {
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
