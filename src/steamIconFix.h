//
// Created by -Zinc- on 2024/3/10.
//
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <vector>
#include <dirent.h>
#include <winreg.h>
#include <tchar.h>
#include <sys/stat.h>
#include <shlobj.h>
#define CURL_STATICLIB
#include "curl.h"

#ifndef STEAMICONFIX_STEAMICONFIX_H
#define STEAMICONFIX_STEAMICONFIX_H
using namespace std;

BOOL isFileExists(const string& path);

bool getDirFiles(const string& path, vector<string> &files,char folderFlag);//Use 's' indicate the icon from Start menu folder,'d' from desktop
//BOOL downloadFile(const string& url, const string& path);
size_t curlWriteFunc(void *ptr, size_t size, size_t nmemb, FILE *stream);
int downloadFile(const string& url, const string& path);
void flushIcon();
void setclr(unsigned short clr);
void logsuc(const char* str);
void logwrn(const char* str);
void logerr(const char* str);
bool getDesktopDir(string &path);
bool getSteamStartMenuDir(string &path);
string wstring2string(wstring wstr);
#endif //STEAMICONFIX_STEAMICONFIX_H
