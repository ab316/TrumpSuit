#include "DXStdafx.h"
#include "DXTimer.h"
#include "DXBaseApplication.h"

namespace DXFramework
{ // DXFramework namespace begin

	CDXTimer::CDXTimer()
	{
	}

	
	CDXTimer::~CDXTimer()
	{
	}


	void CDXTimer::Initialize(LARGE_INTEGER liFrequency, LPCWSTR szName, bool bStopClock, DXTimeExpireEvent timeExpireEvent)
	{
		Reset();
		SetName(szName);
		SetStopClock(bStopClock);
		SetTimeExpireEvent(timeExpireEvent);
		m_liFrequency.QuadPart = liFrequency.QuadPart;
	}


	void CDXTimer::Reset()
	{
		m_bPaused = true;
		m_fTime = 0.0f;
		m_fElapsedTime = 0.0f;
		// We only play with the quad part (the 64 bit part). So
		// no need to worry about the other members.
		m_liLastCount.QuadPart = 0;
		m_bUnregisterOnTimeExpire = false;
	}


	void CDXTimer::Tick()
	{
		LARGE_INTEGER liNewCount;
		// Acquire the new processor counter.
		QueryPerformanceCounter(&liNewCount);
		if (!m_bPaused)
		{
			LARGE_INTEGER liDifference;
			// If the last counter is not zero. Tick. Last counter will be
			// zero on the first call to tick.
			if (m_liLastCount.QuadPart)
			{
				// Get the difference b/w the old and new count.
				liDifference.QuadPart = liNewCount.QuadPart - m_liLastCount.QuadPart;
				// The time elapsed in seconds.
				m_fElapsedTime = (float)((double)liDifference.QuadPart / (double)m_liFrequency.QuadPart);
				// To solve a bug which may arise if the processor goes into a
				// power saver mode.
				if (m_fElapsedTime < 0.0f)
				{
					m_fElapsedTime = 0.0f;
				}
				// Absolute time of the timer.
				m_fTime += m_bStopClock ? -m_fElapsedTime : m_fElapsedTime;
				// If it is a stop clock and the clock ticks 0. Reset and
				// Fire the event if provided.
				if (m_bStopClock && m_fTime <= 0.0f)
				{
					Reset();
					if (m_pTimeExpireEvent)
					{
						m_pTimeExpireEvent();
					}
					if (m_bUnregisterOnTimeExpire)
					{
						CDXBaseApplication::GetInstance()->UnregisterTimer(this);
					}
					DX_DEBUG_OUTPUT1(L"%s timer expired.", GetName());
				}
			}
		}
		// Set the old count to the new count.
		m_liLastCount = liNewCount;
	}

} // DXFramework namespace end