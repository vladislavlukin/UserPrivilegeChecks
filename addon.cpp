#include <napi.h>
#include <windows.h>

Napi::String CheckPrivilege(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    std::string username = info[0].As<Napi::String>();

    HANDLE hToken;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        return Napi::String::New(env, "Error opening process token");
    }

    PTOKEN_USER ptu = NULL;
    DWORD cb = 0;
    GetTokenInformation(hToken, TokenUser, NULL, 0, &cb);

    ptu = (PTOKEN_USER)GlobalAlloc(GPTR, cb);

    if (!GetTokenInformation(hToken, TokenUser, ptu, cb, &cb)) {
        GlobalFree(ptu);
        CloseHandle(hToken);
        return Napi::String::New(env, "Error getting token information");
    }

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, GetCurrentProcessId());
    HANDLE hAccessToken = NULL;
    if (!OpenProcessToken(hProcess, TOKEN_DUPLICATE | TOKEN_QUERY, &hAccessToken)) {
        GlobalFree(ptu);
        CloseHandle(hToken);
        CloseHandle(hProcess);
        return Napi::String::New(env, "Error opening process token");
    }

    BOOL bResult = FALSE;
    DWORD dwSize = 0;
    TOKEN_ELEVATION_TYPE elevationType;
    if (!GetTokenInformation(hAccessToken, TokenElevationType, &elevationType, sizeof(elevationType), &dwSize)) {
        CloseHandle(hAccessToken);
        CloseHandle(hProcess);
        GlobalFree(ptu);
        CloseHandle(hToken);
        return Napi::String::New(env, "Error getting token elevation type");
    }

    CloseHandle(hAccessToken);
    CloseHandle(hProcess);
    GlobalFree(ptu);
    CloseHandle(hToken);

    if (elevationType == TokenElevationTypeFull) {
        return Napi::String::New(env, username + " имеет привилегию Администратора");
    } else if (elevationType == TokenElevationTypeLimited) {
        return Napi::String::New(env, username + " имеет привилегию Пользователя");
    } else {
        return Napi::String::New(env, username + " имеет привилегию Гостя");
    }
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "checkPrivilege"), Napi::Function::New(env, CheckPrivilege));
    return exports;
}

NODE_API_MODULE(addon, Init)