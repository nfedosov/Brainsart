#pragma once


__declspec(dllimport)
bool GetFirstLslStream(
    _In_  DWORD  nBufferSize,
    _Out_ PCHAR szCuccentLlsStreamName);

__declspec(dllimport)
bool GetNextLslStream(
    _In_  DWORD  nBufferSize,
    _Out_ PCHAR szCuccentLlsStreamName);