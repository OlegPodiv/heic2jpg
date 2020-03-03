#pragma once 

#ifdef WIN32
#include <Windows.h>
#else
#include <stdio.h>
#include <pthread.h>
#endif

class CEuLock
{
public:
	CEuLock()
	{
#ifdef WIN32
		 m_hEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
#else
		//m_pMutext = PTHREAD_MUTEX_INITIALIZER;
		pthread_mutex_init(&m_pMutext, NULL); //³õÊ¼»¯»¥³âËø 	
#endif 
	}

	~CEuLock()
	{
#ifdef WIN32
		CloseHandle(m_hEvent);
#else
		pthread_mutex_destroy(&m_pMutext); //Ïú»Ù»¥³âËø 
#endif
	}

public:
	void lock()
	{
#ifdef WIN32
        WaitForSingleObject(m_hEvent, INFINITE);
#else
		pthread_mutex_lock( &m_pMutext );
#endif
	}

	void unlock()
	{
#ifdef WIN32
		 SetEvent(m_hEvent);
#else
		pthread_mutex_unlock( &m_pMutext );
#endif
	}

private:
#ifdef WIN32
	HANDLE m_hEvent;
#else
    pthread_mutex_t m_pMutext;
#endif
};

template<typename T>
class CLock
{
public:
    CLock(T& t):m_t(t)
    {
        m_t.lock();
    }

    ~CLock()
    {
        m_t.unlock();
    }
private:
    T& m_t;
};

typedef CLock<CEuLock> CCritLock;