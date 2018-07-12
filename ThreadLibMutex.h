#ifndef THREADLIBMUTEX_H //������
#define THREADLIBMUTEX_H

#include <windows.h>
#include <string>

namespace ThreadLib
{
	class Mutex
	{
	public:
		Mutex()
		{
			InitializeCriticalSection(&m_mutex);
		}

		~Mutex()
		{
			DeleteCriticalSection(&m_mutex);
		}

		inline void Lock() //��ס��Դ
		{
			EnterCriticalSection(&m_mutex);
		}

		inline void Unlock() //�ͷ���Դ
		{
			LeaveCriticalSection(&m_mutex);
		}

	protected:
		CRITICAL_SECTION m_mutex;
	};
}

#endif