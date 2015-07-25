#include "DXStdafx.h"
#include "DXProcess.h"

namespace DXFramework
{ // DXFramework namespace begin

	IDXProcess::IDXProcess()	:	m_bInitialized(false),
									m_bActive(false),
									m_bPaused(true),
									m_bKill(false),
									m_pDependent(nullptr),
									m_uiType(0)
	{
	}


	IDXProcess::~IDXProcess()
	{
		SetChildProcess(nullptr);
	}

} // DXFramework namespace end