//
// Created by -Zinc- on 2024/3/10.
//
#include "tools.h"

string wstring2string(wstring wstr) {
    string result;
    int len = WideCharToMultiByte(CP_ACP,WC_NO_BEST_FIT_CHARS,
                                  wstr.c_str(),wstr.size(),
                                  nullptr,0, nullptr, nullptr);
    char* buffer=new char [len+1];
    WideCharToMultiByte(CP_ACP,WC_NO_BEST_FIT_CHARS,
                        wstr.c_str(),wstr.size(),
                        buffer,len, nullptr, nullptr);
    buffer[len]='\0';
    result.append(buffer);
    delete[] buffer;
    return result;
}
