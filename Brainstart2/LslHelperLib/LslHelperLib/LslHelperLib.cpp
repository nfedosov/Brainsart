#include <Windows.h>

#include "../../liblsl-master/include/lsl_cpp.h"


extern "C"
__declspec(dllexport)
bool GetFirstLslStream(
    _In_  DWORD  nBufferSize,
    _Out_ PCHAR szCuccentLlsStreamName);

extern "C"
__declspec(dllexport)
bool GetNextLslStream(
    _In_  DWORD  nBufferSize,
    _Out_ PCHAR szCuccentLlsStreamName);

std::vector<lsl::stream_info> g_Steams;
std::vector<lsl::stream_info>::iterator g_SteamsIterator;

bool GetFirstLslStream(
    _In_  DWORD  nBufferSize,
    _Out_ PCHAR szCuccentLlsStreamName)
{
    g_Steams = lsl::resolve_streams();

    if (g_Steams.empty())
    {
        return FALSE;
    }

    g_SteamsIterator = g_Steams.begin();

    strncpy_s(
        szCuccentLlsStreamName,
        nBufferSize,
        (*g_SteamsIterator).name().c_str(),
        (*g_SteamsIterator).name().length());
     
    return TRUE;
}

bool GetNextLslStream(
    _In_  DWORD  nBufferSize,
    _Out_ PCHAR szCuccentLlsStreamName)
{
    if (!szCuccentLlsStreamName)
    {
        return FALSE;
    }

    ++g_SteamsIterator;

    if (g_SteamsIterator == g_Steams.end())
    {
        return FALSE;
    }

    strncpy_s(
        szCuccentLlsStreamName,
        nBufferSize,
        (*g_SteamsIterator).name().c_str(),
        (*g_SteamsIterator).name().length());

    return TRUE;
}

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpvReserved)  // reserved
{
    return TRUE;
}
