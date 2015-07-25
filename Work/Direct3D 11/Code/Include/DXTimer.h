#pragma once

#include "DXUnknown.h"

namespace DXFramework
{ // DXFramework namespace begin

	typedef void DXTimeExpireEvent();

	// A high resolution timer that can also act as a stop watch which can
	// fire an event when the time expires.
	class CDXTimer : public IDXUnknown
	{
	private:
		LARGE_INTEGER		m_liLastCount;
		LARGE_INTEGER		m_liFrequency;
		double				m_fTime;
		// The event function to call when the stop clock expires.
		// It takes no args and its return type is void.
		DXTimeExpireEvent*	m_pTimeExpireEvent;
		// The absolute time of the timer.
		// The time elapsed since the clocked ticked last time.
		float				m_fElapsedTime;
		// A stop clock advances backwards i.e. towards 0.
		bool				m_bStopClock;
		bool				m_bPaused;
		bool				m_bUnregisterOnTimeExpire;


	public:
		// Clock won't do anything when paused. Remember to call SetTime() first
		// for stop clocks.
		void				Tick();
		// Resets the time parameters and pauses it. Does not change the name.
		void				Reset();
		// Timer is initially paused.
		void				Initialize(LARGE_INTEGER liFrequency, LPCWSTR szName=DX_DEFAULT_NAME, bool bStopClock=false, DXTimeExpireEvent timeExpireEvent=0);


	public:
		// If this is set to true, the timer will be unregistered from the base
		// application timer vector.
		bool			IsStopClock()				const			{ return m_bStopClock;			};
		bool			IsPaused()					const			{ return m_bPaused;				};
		float			GetTimeSinceLastTick()		const			{ return m_fElapsedTime;		};
		double			GetAbsoluteTime()			const			{ return m_fTime;				};
		LARGE_INTEGER	GetFrequency()				const			{ return m_liFrequency;			};
		void			SetPause(bool bPause)						{ m_bPaused = bPause;			};
		// Set the time. For stop clocks.
		void			SetTime(float fTime)						{ m_fTime = fTime;				};
		void			SetStopClock(bool bStopClock)				{ m_bStopClock = bStopClock;	};
		void			SetFrequency(LARGE_INTEGER liFrequency)		{ m_liFrequency = liFrequency;	};
		// For stop clocks, the function to call when the timer ticks 0.
		// Takes a functions of return type void and 0 args.
		void			SetTimeExpireEvent(DXTimeExpireEvent eventFunc)		{ m_pTimeExpireEvent = eventFunc;			};
		void			UnregisterOnTimeExpire(bool bUnregister)			{ m_bUnregisterOnTimeExpire = bUnregister;	};

	public:
		virtual UINT		GetTypeId()		const	{ return DX_TIMER_TYPE_ID;		};
		virtual LPCWSTR		GetTypeName()	const	{ return DX_TIMER_TYPE_NAME;	};

	public:
		CDXTimer();
		~CDXTimer();
	};

} // DXFramework namespace end