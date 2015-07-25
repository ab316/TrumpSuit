#include "DXStdafx.h"
#include "DXShader.h"
#include "DXShaderManager.h"

namespace DXFramework
{ // DXFramework namespace begin

	CDXShaderManager::CDXShaderManager()
	{
		DX_SET_UNKNOWN_NAME(this, L"SHADER MANAGER");
	}


	CDXShaderManager::~CDXShaderManager()
	{
	}


	HRESULT CDXShaderManager::Create(void* pNull)
	{
		UNREFERENCED_PARAMETER(pNull);
		return S_OK;
	}


	DXResourceHandle CDXShaderManager::CreateResource(void* pShaderDesc, WCHAR* szCustomKey)
	{
		DXResourceKey key;
		DXResourceMapInsertResult insertResult;
		DXResourceHandle returnValue;
		DXShaderDesc* pDesc;
		CDXShader* pShader;

		pDesc = (DXShaderDesc*)pShaderDesc;
		pShader = nullptr;

		// Custom key given? Well and good. But it's better not to provide a
		// custom key for shaders.
		if (szCustomKey)
		{
			wcscpy_s(key.m_szName, DX_MAX_RESOURCE_KEY_NAME, szCustomKey);
		}
		else // No? Process a key name of format %FILE%_%ENTRY%_%PROFILE%
		{
			WCHAR szFileName[DX_MAX_FILE_NAME];
			WCHAR* pFileName;
			WCHAR* pDot;

			// Find the last slash.
			pFileName = wcsrchr(pDesc->szFileName, '\\');
			// If not found, then only the file name is present.
			if (!pFileName)
			{
				pFileName = pDesc->szFileName;
			}
			// Copy the file name to a local variable.
			wcscpy_s(szFileName, DX_MAX_FILE_NAME, pFileName);
			// Find the extension full stop.
			pDot = wcsrchr(szFileName, '.');
			// We don't need the file extension.
			if (pDot)
			{
				*pDot = 0;
			}

			// Format our key name: %FILE%_%ENTRY%_%PROFILE%
			// So keep file and function names small.
			wsprintf(key.m_szName, L"%s_%s_%s", szFileName, pDesc->szEntryPoint, pDesc->szShaderLevel);
		}

		// Try to insert the shader with the key into the map.
		insertResult = InsertResource(key, pShader);
		if (insertResult.second) // New resource inserted?
		{
			HRESULT hr = S_OK;

			// New shader resource on the heap.
			pShader = new CDXShader();
			// Create the resource. NOTE: This does not create the resource
			// in the graphics memory. It only prepares the resource so that it
			// can be created anytime using Recreate() method.
			DX_V(pShader->Create(pShaderDesc));
			if (FAILED(hr)) // Failed?
			{
				DX_DEBUG_OUTPUT2(L"Failed on a shader: FILE:%s, ENTRY: %s.", pDesc->szFileName, pDesc->szEntryPoint);
				// Free the memory.
				DX_SAFE_DELETE(pShader);
				return returnValue;
			}

			// Success!
			insertResult.first->second = pShader;
			AddMemoryUsage(pShader->GetSize());
		}
		else // Existing element returned.
		{
			pShader = (CDXShader*)insertResult.first->second;
		}

		pShader->AddRef();
		returnValue.key = key;
		returnValue.pResource = pShader;

		return returnValue;
	}

} // DXFramework namespace end