#include "DXStdafx.h"
#include "DXFileSystem.h"
#include "DXTimer.h"
#include "DXRefCounter.h"
#include "DX3DDriver.h"
#include "DXGraphicsDriver.h"
#include "DXUIManager.h"
#include "DXBaseApplication.h"

#define		DX_DEFAULT_MAX_TEXTURES					10
#define		DX_DEFAULT_MAX_HARDWARE_BUFFERS			10
#define		DX_DEFAULT_MAX_SHADERS					10
#define		DX_DEFAULT_HARDWARE_RESOURCE_MEMORY		1024*1024*1024

namespace DXFramework
{ // DXFramework namespace begin

	WCHAR CDXBaseApplication::m_sszDirectory[DX_MAX_FILE_PATH] = L"";

	CDXBaseApplication::CDXBaseApplication() :	m_bStarted(false),
												m_bActive(false),
												m_pFileSystem(nullptr),
												m_pFuncFrameRender(nullptr),
												m_pFuncResize(nullptr),
												m_pFuncWindowMessage(nullptr),
												m_pGraphicsDriver(nullptr),
												m_pHighResTimer(nullptr),
												m_pRefCounter(nullptr),
												m_hInstance(nullptr),
												m_hWindow(nullptr),
												m_fFPS(0.0f),
												m_uiFrame(0)
	{
		DX_ERROR_BUFFER[0] = 0;

		MEMORYSTATUSEX status;
		status.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&status);
		
		m_dwPhysicalMemory = status.ullTotalPhys;
		m_dwAvailableMemory = status.ullAvailPhys;
		m_dwVirtualMemory = status.ullAvailVirtual;

		QueryPerformanceFrequency(&m_liProcessorClock);

		DX_DEBUG_OUTPUT0(L"ctor");
	}


	CDXBaseApplication::~CDXBaseApplication()
	{
		DX_DEBUG_OUTPUT0(L"dtor");
	}


	void CDXBaseApplication::SetInitialMemory(uintptr_t uiMaxTextures, uintptr_t uiMaxHardwareBuffers, uintptr_t uiMaxShaders, uintptr_t uiHardwareResourceMemory)
	{
		m_uiMaxTextures = uiMaxTextures ? uiMaxTextures : DX_DEFAULT_MAX_TEXTURES;
		m_uiMaxHardwareBuffers = uiMaxHardwareBuffers ? uiMaxHardwareBuffers : DX_DEFAULT_MAX_HARDWARE_BUFFERS;
		m_uiMaxShaders = uiMaxShaders ? uiMaxShaders : DX_DEFAULT_MAX_SHADERS;
		m_uiHardwareResourcesMemory = uiHardwareResourceMemory ? uiHardwareResourceMemory : DX_DEFAULT_HARDWARE_RESOURCE_MEMORY;
	}


	HRESULT CDXBaseApplication::Startup(HINSTANCE hInstance, UINT uiWidth, UINT uiHeight, bool bWindowed, LPCWSTR szTitle, LPCWSTR szWorkingDirectory)
	{	
		HRESULT hr = S_OK;

		if (m_bStarted)
		{
			hr = E_ILLEGAL_METHOD_CALL;
			DX_DEBUG_OUTPUT0(L"Already Started.");
			return hr;
		}

		m_hInstance = hInstance;

		// Create the RefCounter singleton class instance.
		m_pRefCounter = CDXRefCounter::GetInstance();
		// Create the FileSystem singleton class instance.
		m_pFileSystem = CDXFileSystem::GetInstance();
		m_pFileSystem->AddAdditionalDirectory(L"Shaders");
		m_pFileSystem->AddAdditionalDirectory(L"Code\\Shaders");
		m_pFileSystem->AddAdditionalDirectory(L"Textures");
		m_pFileSystem->AddAdditionalDirectory(L"Fonts");

		// If we are given a working directory. Use it.
		if (szWorkingDirectory)
		{
			m_pFileSystem->SetWorkingDirectory(szWorkingDirectory);
		}

		// Create our window.
		hr = CreateAppWindow(uiWidth, uiHeight, szTitle);
		if (FAILED(hr))
		{
			DX_ERROR_MSGBOX(L"DXApplication: Failed to create App window.");
			Shutdown();
			return hr;
		}

		// Start the graphics driver.
		m_pGraphicsDriver = CDXGraphicsDriver::GetInstance();
		DX_V(m_pGraphicsDriver->Startup(m_hWindow));
		if (FAILED(hr))
		{
			DX_ERROR_MSGBOX(L"DXApplication: Failed to start graphics driver.");
			Shutdown();
			return hr;
		}

		m_pGraphicsDriver->CreateWindowDependentResources();
		m_pUIManager = CDXUIManager::GetInstance();;

		m_pHighResTimer = new CDXTimer();
		m_pHighResTimer->Initialize(m_liProcessorClock, L"Default Clock");
		RegisterTimer(m_pHighResTimer);

		m_bStarted = true;
		DX_DEBUG_OUTPUT0(L"Started");

		return hr;
	}


	void CDXBaseApplication::Shutdown()
	{
		m_pRefCounter->DebugDumpObjectList();
		
		SendMessage(m_hWindow, WM_CLOSE, 0, 0);
		UnregisterClass(DX_WINDOW_CLASS_NAME, m_hInstance);
		
		m_hWindow = nullptr;
		m_hInstance = nullptr;

		m_pGraphicsDriver->Shutdown();

		UnregisterAllTimers();
		DX_SAFE_DELETE(m_pHighResTimer);

		// Clear the ref counter. By this time all the framework
		// resources should have been freed.
		m_pRefCounter->Clear();

		m_bStarted = false;
		m_bActive = false;

		DX_DEBUG_OUTPUT0(L"Shutdown.");
	}


	HRESULT CDXBaseApplication::CreateAppWindow(UINT uiWidth, UINT uiHeight, LPCWSTR szTitle)
	{
		HRESULT hr = S_OK;

		// Create a window class.
		WNDCLASSEX windowClass;
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hInstance = m_hInstance;
		windowClass.hIcon = NULL;
		windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		windowClass.lpszClassName = DX_WINDOW_CLASS_NAME;
		windowClass.lpszMenuName = NULL;
		windowClass.hIconSm = NULL;
		windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW);
		windowClass.lpfnWndProc = (WNDPROC)WindowProc;

		// Register the window class.
		if (!RegisterClassEx(&windowClass))
		{
			hr = E_FAIL;
			DX_DEBUG_OUTPUT0(L"ERROR!!! Unable to register window class.");
			return hr;
		}

		DWORD dwStyle = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

		// Calculate the actual width and height needed to obtain a client area
		// of the given size.
		RECT rect = { 0, 0, uiWidth, uiHeight };
		AdjustWindowRect(&rect, dwStyle, FALSE);

		// Create the window.
		m_hWindow = CreateWindow(windowClass.lpszClassName, szTitle ? szTitle : DX_WINDOW_DEFAULT_NAME, dwStyle, CW_USEDEFAULT,
			CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, 0, 0, m_hInstance, 0);

		if (!m_hWindow)
		{
			hr = E_FAIL;
			DX_DEBUG_OUTPUT0(L"ERROR!!! Unable to create window.");
			return hr;
		}

		return hr;
	}


	LRESULT CALLBACK CDXBaseApplication::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		static CDXGraphicsDriver* pGraphics = nullptr;
		static CDXBaseApplication* pApp = GetInstance();
		static CDXUIManager* pUIMan = CDXUIManager::GetInstance();
		static bool bActive;

		// Allow the UI manager to handle messages.
		if (pUIMan->ProcessMessage(hWnd, uMsg, wParam, lParam))
		{
			return 0;
		}

		// Allow the user to handle messages.
		if (pApp->m_pFuncWindowMessage)
		{
			if (pApp->m_pFuncWindowMessage(hWnd, uMsg, wParam, lParam))
			{
				return 0;
			}
		}

		switch (uMsg)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_ACTIVATE:
			bActive = (LOWORD(wParam) == WA_INACTIVE) ? false : true;
			pApp->SetActive(bActive);
			pApp->PauseTimers(!bActive);
			DX_DEBUG_OUTPUT1(L"Application %s.", bActive ? L"activated" : L"deactivated");
			break;

		case WM_SIZE:
			if (wParam == SIZE_MINIMIZED)
			{
				pApp->SetActive(false);
			}

			if (pApp->GetActive())
			{
				pGraphics = pApp->GetGraphicsDriver();
				if (pGraphics)
				{
					if (FAILED(pGraphics->HandleWindowResize()))
					{
						DestroyWindow(hWnd);
						DX_DEBUG_OUTPUT0(L"ERROR!!! WM_SIZE failed.");
						return 0;
					}

					// Call the application provided callback function.
					if (pApp->m_pFuncResize)
					{
						CDX3DDriver* p3d = pGraphics->Get3DDriver();
						pApp->m_pFuncResize((float)p3d->GetBackbufferWidth(), (float)p3d->GetBackbufferHeight());
					}

					DX_DEBUG_OUTPUT0(L"Window resized.");
				}
			}
			break;

		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
			break;
		}

		return 0;
	}


	void CDXBaseApplication::Run()
	{
		if (!m_bStarted)
		{
			DX_DEBUG_OUTPUT0(L"Called when not started.");
			return;
		}

		// Show the window. This should be done after creating the graphics driver because
		// calling this function passes a WM_SIZE message to the window which need the
		// graphics driver.
		ShowWindow(m_hWindow, SW_SHOW);

		// Now switch to full-screen mode if user has demanded it. We
		// always begin in windowed mode.
		CDX3DDriver::GetInstance()->SetWindowedMode(true);


		m_pHighResTimer->SetPause(false);
		MSG msg = { 0 };
		while (msg.message != WM_QUIT)
		{
			// If we have message, process it.
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				// Tick the registered timers. If the window is deactivated, the
				// timers will get paused. But they must be allowed to Tick() for
				// correct working.
				DXTimerContainerIterator iter = m_containerpTimers.begin();
				DXTimerContainerIterator end = m_containerpTimers.end();
				while (iter != end)
				{
					(*iter)->Tick();
					++iter;
				}

				if (m_bActive)
				{
					++m_uiFrame;
					static float fElapsedTime = 0.0f;
					double fTime = m_pHighResTimer->GetAbsoluteTime();
					fElapsedTime += m_pHighResTimer->GetTimeSinceLastTick();
					m_fFPS = 1.0f / fElapsedTime;
					if (fElapsedTime < 1.0f/30.0f)
					{
						//continue;
					}
					
					m_pUIManager->Update(fElapsedTime);
					m_pGraphicsDriver->BeginRendering();
					if (m_pFuncFrameRender)
					{
						m_pFuncFrameRender(fTime, fElapsedTime);
					}
					m_pGraphicsDriver->EndRendering();
					fElapsedTime = 0.0f;
				}
			}
		}
	}


	void CDXBaseApplication::RegisterTimer(CDXTimer* pTimer)
	{
		DXTimerContainerIterator iter = m_containerpTimers.begin();
		DXTimerContainerIterator end = m_containerpTimers.end();
		
		while (iter != end)
		{
			if (*iter == pTimer)
			{
				return;
			}
			++iter;
		}

		m_containerpTimers.push_back(pTimer);
	}


	void CDXBaseApplication::UnregisterTimer(CDXTimer* pTimer)
	{
		DXTimerContainerIterator iter = m_containerpTimers.begin();
		DXTimerContainerIterator end = m_containerpTimers.end();
		
		while (iter != end)
		{
			if (*iter == pTimer)
			{
				m_containerpTimers.erase(iter);
				return;
			}
			++iter;
		}
	}


	void CDXBaseApplication::PauseTimers(bool bPause)
	{
		DXTimerContainerIterator iter = m_containerpTimers.begin();
		DXTimerContainerIterator end = m_containerpTimers.end();

		while (iter != end)
		{
			(*iter)->SetPause(bPause);
			++iter;
		}
	}

} // DXFramework namespace end