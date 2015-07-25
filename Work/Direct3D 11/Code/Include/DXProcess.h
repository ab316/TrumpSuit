#pragma once

#include "DXUnknown.h"

namespace DXFramework
{ // DXFramework namespace begin

	// A DXProcess class generalizes a task so that is easier to maintain and 
	// instantiate on multiple objects. It can also have a child process which
	// takes the place of it's parent process when it is killed.
	// A single process instance can
	// only be attached to ONLY ONE process manager.
	class IDXProcess : public IDXUnknown
	{
		friend class CProcessManager;

	protected:
		// A child process. The child process begin when the parent process
		// is killed.
		IDXProcess*			m_pDependent;
		UINT				m_uiType;
		bool				m_bActive;
		bool				m_bPaused;
		// When this flag is set to true. The ProcessManager will call Kill() method of the
		// process and frees its memory.
		bool				m_bKill;
		bool				m_bInitialized;
		// True if the process is attached to a process manager.
		bool				m_bAttached;

	public:
		bool		IsActive()			const			{ return m_bActive;			};
		bool		IsPaused()			const			{ return m_bPaused;			};
		bool		IsDead()			const			{ return m_bKill;			};
		bool		IsInitialized()		const			{ return m_bInitialized;	};
		bool		IsAttached()		const			{ return m_bAttached;		};
		UINT		GetType()			const			{ return m_uiType;			};
		IDXProcess*	GetChildProcess()	const			{ return m_pDependent;		};
		
		void		SetActive(bool bActive)				{ m_bActive = bActive;		};
		void		SetType(UINT uiNewType)				{ m_uiType = uiNewType;		};
		void		SetAttached(bool bAttached)			{ m_bAttached = bAttached;	};
		void		SetChildProcess(IDXProcess* p)
		{
			// If this process already depends on another process. We should first
			// remove that dependency.
			DX_SAFE_RELEASE(m_pDependent);
			m_pDependent = p;
			m_pDependent->AddRef();
		};

	public:
		virtual void	Initialize()					{ m_bInitialized = true;	};
		virtual void	Pause(bool bPaused)				{ m_bPaused = bPaused;		};
		virtual void	Kill()							{							};
		// Default behaviour: If the process is not already initialized. It will be
		// on the first call to update.
		virtual	void	Update(float fDelta)
		{
			UNREFERENCED_PARAMETER(fDelta);
			if (!m_bInitialized)
			{
				Initialize();
				m_bInitialized = false;
			}
		}

	public:
		IDXProcess();
		virtual ~IDXProcess();
	};

} // DXFramework namespace end