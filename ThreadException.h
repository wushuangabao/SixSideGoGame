#ifndef THREADEXCEPTION_H
#define THREADEXCEPTION_H

#include <exception>
#include <windows.h>

namespace ThreadLib
{
	enum Error
	{
		Unspecified,                // δ�������
		InitFailure,                // �߳̿�û�г�ʼ��
		CreationFailure             // �̲߳����Ա�����
	};

	class Exception : public std::exception//�쳣
	{
	public:
		Exception(Error p_error = Unspecified)
		{
			m_error = p_error;
		}

		Error GetError() const //����쳣
		{
			return m_error;
		}

	protected:
		Error m_error;
	};
}

#endif