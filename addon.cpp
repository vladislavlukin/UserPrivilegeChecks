#include <napi.h>
#include <windows.h>
#include <lm.h>
#include <iostream>
#include <string>
#pragma comment(lib, "Netapi32.lib")

std::string Utf16ToUtf8(const std::wstring& utf16Str) {
    std::string utf8Str;
    int utf8Length = WideCharToMultiByte(CP_UTF8, 0, utf16Str.c_str(), -1, NULL, 0, NULL, NULL);
    if (utf8Length > 0) {
        utf8Str.resize(utf8Length);
        WideCharToMultiByte(CP_UTF8, 0, utf16Str.c_str(), -1, &utf8Str[0], utf8Length, NULL, NULL);
    }
    return utf8Str;
}

Napi::String CheckPrivilege(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    DWORD dwLevel = 2;
    DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
    LPUSER_INFO_2 pBuf = NULL;
    LPUSER_INFO_2 pTmpBuf;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    NET_API_STATUS nStatus;

    nStatus = NetUserEnum(NULL, dwLevel, FILTER_NORMAL_ACCOUNT, (LPBYTE*)&pBuf, dwPrefMaxLen, &dwEntriesRead, &dwTotalEntries, NULL);

    if (nStatus == NERR_Success) {
        if ((pTmpBuf = pBuf) != NULL) {
            std::string result;
            for (DWORD i = 0; i < dwEntriesRead; i++) {
                std::wstring currentUsername = pTmpBuf->usri2_name;

                result += " " + Utf16ToUtf8(currentUsername);

                LPUSER_INFO_3 pUserInfo3;
                NET_API_STATUS nStatusUserInfo = NetUserGetInfo(NULL, pTmpBuf->usri2_name, 3, (LPBYTE*)&pUserInfo3);

                if (nStatusUserInfo == NERR_Success) {
                    DWORD userPrivilege = pUserInfo3->usri3_priv;

                    result += ":" + std::to_string(userPrivilege);

                    NetApiBufferFree(pUserInfo3);
                } else {
                    result += ":3"; // Если произошла ошибка, присваиваем значение 3
                }

                pTmpBuf++;
            }
            return Napi::String::New(env, result);
        }
    } else {
        return Napi::String::New(env, "error");
    }

    if (pBuf != NULL) {
        NetApiBufferFree(pBuf);
    }

    return Napi::String::New(env, "error");
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "checkPrivilege"), Napi::Function::New(env, CheckPrivilege));
    return exports;
}

NODE_API_MODULE(addon, Init)