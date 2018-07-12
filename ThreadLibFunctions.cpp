#include "stdafx.h"
#include "ThreadLib.h"

namespace ThreadLib
{
	std::map< DWORD, HANDLE > g_handlemap;

	DWORD WINAPI DummyRun(void* p_data)
	{
		DummyData* data = (DummyData*)p_data;
		data->m_func(data->m_data);
		delete data;
		return 0;
	}

	DWORD WINAPI GoRun(void* p_data)
	{
		DummyData* data = (DummyData*)p_data;
		data->m_func(NULL);
		delete data;
		return 0;
	}
}
