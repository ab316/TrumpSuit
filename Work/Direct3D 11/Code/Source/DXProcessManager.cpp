#include "DXStdafx.h"
#include "DXProcess.h"
#include "DXProcessManager.h"

namespace DXFramework
{ // DXFramework namespace begin

	CDXProcessManager::CDXProcessManager()
	{
	}


	CDXProcessManager::~CDXProcessManager()
	{
		DXProcessVectorIterator iter = m_vecpProcesses.begin();
		DXProcessVectorIterator end = m_vecpProcesses.end();

		while (iter != end)
		{
			Detach(*iter);
			++iter;
		}
	}


	void CDXProcessManager::Attach(IDXProcess* pProcess)
	{
		if (!pProcess->IsActive())
		{
			pProcess->SetAttached(true);
			m_vecpProcesses.push_back(pProcess);
		}
		else
		{
			DX_DEBUG_OUTPUT0(L"Process already attached.");
		}
	}


	void CDXProcessManager::Detach(IDXProcess* pProcess)
	{
		if (pProcess->IsAttached())
		{
			DXProcessVectorIterator iter = m_vecpProcesses.begin();
			DXProcessVectorIterator end = m_vecpProcesses.end();
			while (iter != end)
			{
				if (pProcess == (*iter)) // Process matched?
				{
					pProcess->SetAttached(false);
					m_vecpProcesses.erase(iter); // Remove it.
					return; // And we are done!
				}
				++iter; // No? continue till we reach the end.
			}
		}
		DX_DEBUG_OUTPUT0(L"Process not attached.");
	}


	void CDXProcessManager::UpdateProcesses(float fDeltaTime)
	{
		DXProcessVectorIterator iter = m_vecpProcesses.begin();
		DXProcessVectorIterator end = m_vecpProcesses.end();
		IDXProcess* pProcess;

		while (iter != end)
		{
			pProcess = (*iter);
			// Process dead?
			if (pProcess->IsDead())
			{
				// Look for a child process.
				IDXProcess* pChild = pProcess->GetChildProcess();
				if (pChild) // Found?
				{
					// It's no longer your child!
					pProcess->SetChildProcess(nullptr);
					// Now the process manager directly owns the process.
					Attach(pChild);
				}
				// Dead process needs to be detached.
				Detach(pProcess);
			}
			// Otherwise check to see if the process is active and not paused.
			else if (pProcess->IsActive() && !pProcess->IsPaused())
			{
				// Update it.
				pProcess->Update(fDeltaTime);
			}
			// Next process please!
			++iter;
		}
	}

} // DXFramework namespace end