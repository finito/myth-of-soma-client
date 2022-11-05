#ifndef ASYNC_FILE_LOAD_H
#define ASYNC_FILE_LOAD_H
#include <windows.h>
#include <string>

namespace Framework
{

struct AsyncLoadItem
{
	OVERLAPPED overlapped;
	void* buffer;
	DWORD size;
};

void PollAsyncLoads();

AsyncLoadItem* AsyncLoad(const std::string& filename);

} // namespace Framework

#endif // ASYNC_FILE_LOAD_H