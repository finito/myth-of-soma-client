
#include "AsyncFileLoad.h"
#include <iostream>

VOID WINAPI OnAsyncLoadComplete(DWORD errorCode, DWORD numberOfBytesTransfered, OVERLAPPED* overlapped)
{
	overlapped->hEvent = 0; // Signals to users that the file has finished loading.
}

namespace Framework
{

void PollAsyncLoads()
{
	SleepEx(0, TRUE); // Calls OnAsyncLoadComplete when the asynchronous load has completed.
}

AsyncLoadItem* AsyncLoad(const std::string& filename)
{
	HANDLE file = CreateFileA(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
	if (file == INVALID_HANDLE_VALUE)
	{
		return nullptr;
	}

	AsyncLoadItem* item = (AsyncLoadItem*)(malloc(sizeof(AsyncLoadItem)));
	DWORD fileSize = 0, fileSizeHigh = 0;
	fileSize = GetFileSize(file, &fileSizeHigh);
	if (fileSize == INVALID_FILE_SIZE)
	{
		//free(item);
		return nullptr;
	}

	item->size = fileSize;
	item->buffer = malloc(fileSize);
	item->overlapped.Offset = 0;
	item->overlapped.OffsetHigh = 0;
	item->overlapped.Pointer = 0;
	item->overlapped.Internal = item->overlapped.InternalHigh = 0;
	item->overlapped.hEvent = (HANDLE)(filename.c_str());
	BOOL ret = ReadFileEx(file, item->buffer, fileSize, &item->overlapped, &OnAsyncLoadComplete);
	if (!ret)
	{
		DWORD error = GetLastError(); void* errText = 0;
		DWORD errLength = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM, 0, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&errText, 0, 0);
		std::cerr << (char*)errText << std::endl;
		LocalFree(errText);
		//free(item->buffer);
		//free(item);
		return nullptr;
	}

	return item;
}

} // namespace Framework