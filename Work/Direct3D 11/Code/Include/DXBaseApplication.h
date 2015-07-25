#pragma once

#include "DXUnknown.h"

namespace DXFramework
{ // DXFramework namespace begin

	// These classes are not declared here therefore the prototypes.
	// IDXUnknown's header is included because CDXBaseApplication requires
	// its definition to inherit from it.
	class CDXRefCounter;
	class CDXFileSystem;
	class CDXTimer;
	class CDXGraphicsDriver;
	class CDXUIManager;

	typedef void	DXOnFrameRenderFunc(double fTime, float fElapsedTime);
	typedef void	DXOnResize(float fBackBufferWidth, float fBackBufferHeight);
	typedef bool	DXOnWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// This class handles the DXFramework application. It is responsible for creating a
	// window, processing window messages, running the message loop and directing other
	// objects (e.g. Graphics driver) to do their jobs. Name of this class is still not
	// confirmed. How it is going to allow application specific contents is again undecided.
	class CDXBaseApplication : public IDXUnknown
	{
		typedef std::vector<CDXTimer*>		DXTimerContainer;
		typedef DXTimerContainer::iterator	DXTimerContainerIterator;

		// Private members.
	private:
		DXTimerContainer		m_containerpTimers;
		LARGE_INTEGER			m_liProcessorClock;
		DWORD64					m_dwPhysicalMemory;
		DWORD64					m_dwAvailableMemory;
		DWORD64					m_dwVirtualMemory;
		uintptr_t				m_uiMaxTextures;
		uintptr_t				m_uiMaxHardwareBuffers;
		uintptr_t				m_uiMaxShaders;
		uintptr_t				m_uiHardwareResourcesMemory;
		// A pointer to the ref counter. The ref counter is of global scope
		// and is not a part of this class.
		CDXRefCounter*			m_pRefCounter;
		// The high resolution timer.
		CDXTimer*				m_pHighResTimer;
		CDXFileSystem*			m_pFileSystem;
		CDXGraphicsDriver*		m_pGraphicsDriver;
		CDXUIManager*			m_pUIManager;
		
		// Callback function.
		DXOnFrameRenderFunc*	m_pFuncFrameRender;
		DXOnResize*				m_pFuncResize;
		DXOnWindowMessage*		m_pFuncWindowMessage;

		// Current instance of our program. Should be acquired from WinMain.
		HINSTANCE				m_hInstance;
		// The window that would be created by this class when CreateWindow() method
		// is called.
		HWND					m_hWindow;
		float					m_fFPS;
		UINT32					m_uiFrame;
		// Working directory. All file searches will be made in this directory.
		static WCHAR			m_sszDirectory[DX_MAX_FILE_PATH];
		// Buffer to store text of errors.
		DX_DEFINE_ERROR_BUFFER;
		// Boolean to store whether the app is started.
		bool					m_bStarted;
		// Whether the app window is active or not.
		bool					m_bActive;

		// Public system tasks.
	public:
		// Setting any arguments to 0 will cause it to use default value.
		void		SetInitialMemory(uintptr_t uiMaxTextures=0, uintptr_t uiMaxHardwareBuffers=0, uintptr_t uiMaxShaders=0, uintptr_t uiHardwareResourceMemory=0);
		// Initializes the application. Starting the reference counter, registering
		// default framework classes types, Creating a window and starting the graphics
		// driver.
		HRESULT		Startup(HINSTANCE hInstance, UINT uiWidth=800, UINT uiHeight=600, bool bWindowed=true, LPCWSTR szTitle=0, LPCWSTR szWorkingDirectory=0);
		// Starts the application loop.
		void		Run();
		// Should be called after the application loop ends.
		void		Shutdown();
		void		RegisterTimer(CDXTimer* pTimer);
		void		UnregisterTimer(CDXTimer*pTimer);
		// Pauses/Unpauses all the registered timers.
		void		PauseTimers(bool bPause);
		void		UnregisterAllTimers()				{ m_containerpTimers.clear();	};

		// Private system tasks.
	private:
		HRESULT		CreateAppWindow(UINT uiWidth, UINT uiHeight, LPCWSTR szTitle=0);
		// The window's message handler procedure.
		static LRESULT CALLBACK	WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		// Public gets sets.
	public:
		// void (double fTime, float fElapsedTime)
		inline void SetCallbackOnFrameRender(DXOnFrameRenderFunc* pProc)	{ m_pFuncFrameRender = pProc;	};
		// void (float fWidth, float fHeight)
		inline void	SetCallbackOnResize(DXOnResize* pProc)					{ m_pFuncResize = pProc;		};
		// bool (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam_
		inline void	SetCallbackOnWindowMessage(DXOnWindowMessage* pProc)	{ m_pFuncWindowMessage = pProc;	};
		
	public:
		inline void					SetActive(bool bActive)			{ m_bActive = bActive;			};
		inline bool					GetActive()				const	{ return m_bActive;				};
		inline CDXGraphicsDriver*	GetGraphicsDriver()		const	{ return m_pGraphicsDriver;		};
		inline LARGE_INTEGER		GetProcessorClock()		const	{ return m_liProcessorClock;	};
		inline DWORD64				GetPhysicalMemory()		const	{ return m_dwPhysicalMemory;	};
		inline DWORD64				GetAvailableMemory()	const	{ return m_dwAvailableMemory;	};
		inline DWORD64				GetVirtualMemory()		const	{ return m_dwVirtualMemory;		};
		inline float				GetFPS()				const	{ return m_fFPS;				};
		inline UINT					GetFrame()				const	{ return m_uiFrame;				};

		// Public static get sets.
	public:
		// Returns: The one and only instance of this class.
		inline static CDXBaseApplication* GetInstance()
		{
			// Must NOT use new to create the instance. Because
			// it must be freed by the application using delete.
			static CDXBaseApplication pInstance;
			return &pInstance;
		}

		// Returns: Working directory.
		inline static LPCWSTR GetWorkingDirectory()		{ return (LPCWSTR)m_sszDirectory; }

	public:
		virtual UINT		GetTypeId()		const	{ return DX_APPLICATION_TYPE_ID;	};
		virtual LPCWSTR		GetTypeName()	const	{ return DX_APPLICATION_TYPE_NAME;	};

	private:
		// The default ctor. Made private to make this a Singleton class.
		CDXBaseApplication();
	public:
		// dtor. Made virtual so that if delete is called on a pointer (of type
		// CDXBaseApplication) to a derived class, then the dtor of the derived
		// class is also called.
		virtual ~CDXBaseApplication();
	};

} // DXFramework namespace end