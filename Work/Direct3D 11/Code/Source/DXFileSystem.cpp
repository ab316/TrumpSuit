#include "DXStdafx.h"
#include "DXFileSystem.h"

namespace DXFramework
{ // DXFramework namespace begin

	CDXFileSystem::CDXFileSystem()
	{
		// Application's working directory is taken initially.
		GetCurrentDirectory(DX_MAX_FILE_PATH, m_szWorkingDirectory);
		// No additional dirctory initially.
		ClearAdditionalDirectories();

		DX_DEBUG_OUTPUT0(L"ctor");
	}


	CDXFileSystem::~CDXFileSystem()
	{
		FreeAllFiles(false);

		DX_DEBUG_OUTPUT0(L"dtor");
	}


	void CDXFileSystem::SetWorkingDirectory(LPCWSTR szNewDirectory)
	{
		wcscpy_s(m_szWorkingDirectory, DX_MAX_FILE_PATH, szNewDirectory);
	}


	void CDXFileSystem::AddAdditionalDirectory(LPCWSTR szDirectory)
	{
		if (m_uiNumAdditionalDirs >= DX_MAX_ADDITIONAL_DIRECTORIES)
		{
			return;
		}

		wcscpy_s(m_szAdditionalDirs[m_uiNumAdditionalDirs], DX_MAX_FILE_PATH, szDirectory);
		m_uiNumAdditionalDirs++;
	}


	void CDXFileSystem::ClearAdditionalDirectories()
	{
		m_uiNumAdditionalDirs = 0;
		for (int i=0; i<DX_MAX_ADDITIONAL_DIRECTORIES; i++)
		{
			m_szAdditionalDirs[i][0] = 0;
		}
	}


	WCHAR* CDXFileSystem::GetFileName(LPCWSTR szInFile)
	{
		// Search for the last black slash.
		WCHAR* pLastSlash = (WCHAR*)wcsrchr(szInFile, '\\');

		// Slash found! Set the out file name to our new file name
		// consisting of only the file name and extension
		if (pLastSlash)
		{
			// pLastSlash points at the last slash. Incrementing it would make
			// it point to the first character of the file name.
			pLastSlash++;
			return pLastSlash;
		}
		// No slashes?
		else
		{			
			return (WCHAR*)szInFile;
		}
	}


	bool CDXFileSystem::FileExists(LPCWSTR szInFile)
	{
		return (GetFileAttributes(szInFile) != INVALID_FILE_ATTRIBUTES);
	}


	bool CDXFileSystem::LookInTypicalDirectories(LPCWSTR szInFile, LPWSTR szOutAbsolutePath)
	{
		WCHAR szSearchPath[DX_MAX_FILE_PATH];
		WCHAR szTempPath[DX_MAX_FILE_PATH];

#define DX_FILE_EXISTS if (FileExists(szSearchPath)) { wcscpy_s(szOutAbsolutePath, DX_MAX_FILE_PATH, szSearchPath); return true; }

		// Step 1
		// Look in %WORKING_DIR%
		wsprintf(szSearchPath, L"%s\\%s", m_szWorkingDirectory, szInFile);
		DX_FILE_EXISTS;

		// Step 2
		// Look in %WORKING_DIR%\\%ADDITIONAL_DIRECTORY[i]%
		for (UINT i=0; i<m_uiNumAdditionalDirs; i++)
		{
			wsprintf(szSearchPath, L"%s\\%s\\%s", m_szWorkingDirectory, m_szAdditionalDirs[i], szInFile);
			DX_FILE_EXISTS;
		}

		// Step 3
		// Truncate the last directory in the working directory so we
		// get its parent directory.
		wcscpy_s(szTempPath, DX_MAX_FILE_PATH, m_szWorkingDirectory);
		WCHAR* pLastSlash = wcsrchr(szTempPath, '\\');
		// If no last slash then there's no parent directory.
		if (!pLastSlash)
		{
			// Then file not found.
			return false;
		}
		// Truncate the last directory.
		*pLastSlash = 0;
		// Now look again for a last slash.
		pLastSlash = wcsrchr(szTempPath, '\\');
		// After truncating the last directory, if there's no slash (e.g. for "C:\"
		// first truncation will give "C:", this is not a valid path, so we again
		// check for a last slash, if it is not there, then our directory is invalid.
		if (!pLastSlash)
		{
			// File not found.
			return false;
		}

		// Now repeat with the NEW working directory.
		wsprintf(szSearchPath, L"%s\\%s", szTempPath, szInFile);
		DX_FILE_EXISTS;

		// Step 4
		// Try with the additional directories.
		for (UINT i=0; i<m_uiNumAdditionalDirs; i++)
		{
			wsprintf(szSearchPath, L"%s\\%s\\%s", szTempPath, m_szAdditionalDirs[i], szInFile);
			DX_FILE_EXISTS;
		}

		// Step 5
		pLastSlash = wcsrchr(szTempPath, '\\');
		if (!pLastSlash)
		{
			// File not found.
			return false;
		}
		// Remove the last directory.
		*pLastSlash = 0;
		// Like step 3
		pLastSlash = wcsrchr(szTempPath, '\\');
		if (!pLastSlash)
		{
			// No a valid directory.
			return false;
		}
		
		wsprintf(szSearchPath, L"%s\\%s", szTempPath, szInFile);
		DX_FILE_EXISTS;

		// Step 6
		// Try with the additional directories.
		for (UINT i=0; i<m_uiNumAdditionalDirs; i++)
		{
			wsprintf(szSearchPath, L"%s\\%s\\%s", szTempPath, m_szAdditionalDirs[i], szInFile);
			DX_FILE_EXISTS;
		}


		// Step 7
		pLastSlash = wcsrchr(szTempPath, '\\');
		if (!pLastSlash)
		{
			// File not found.
			return false;
		}
		// Remove the last directory.
		*pLastSlash = 0;
		// Like step 3
		pLastSlash = wcsrchr(szTempPath, '\\');
		if (!pLastSlash)
		{
			// No a valid directory.
			return false;
		}

		// Step 8
		// Try with the additional directories.
		for (UINT i=0; i<m_uiNumAdditionalDirs; i++)
		{
			wsprintf(szSearchPath, L"%s\\%s\\%s", szTempPath, m_szAdditionalDirs[i], szInFile);
			DX_FILE_EXISTS;
		}

		return false;
	}


	bool CDXFileSystem::FindFile(LPCWSTR szInFile, LPWSTR szOutAbsolutePath)
	{
		bool bFound = false;

		szOutAbsolutePath[0] = 0;
		// Search the file as given.
		if (GetFileAttributes(szInFile) != INVALID_FILE_ATTRIBUTES)
		{
			// Found?
			bFound = true;
			wcscpy_s(szOutAbsolutePath, DX_MAX_FILE_PATH, szInFile);
		}
		// Search steps 1 to 8.
		else if (LookInTypicalDirectories(szInFile, szOutAbsolutePath))
		{
			// Found?
			bFound = true;
		}
		else
		{
			// Get the file name.
			WCHAR* szFileNameOnly = GetFileName(szInFile);

			// If the acquired file name is not in the begining of szInFile then
			// we were given a relative path. So no we only use the file name and leave
			// the relative path.
			if (szFileNameOnly != szInFile)
			{
				// Step 9;
				bFound = LookInTypicalDirectories(szFileNameOnly, szOutAbsolutePath);
			}
		}

		return bFound;
	}


	DXFileVectorIterator CDXFileSystem::FindFileInVector(LPCWSTR szFileName)
	{	
		DXFileVectorIterator iter = m_vecOpenedFiles.begin();
		while (iter != m_vecOpenedFiles.end())
		{
			DXMemoryFile* pIterFile = *iter;
			if (_wcsicmp(szFileName, pIterFile->szFileName) == 0)
			{
				return iter;
			}
			iter++;
		}

		return m_vecOpenedFiles.end();
	}


	void CDXFileSystem::DecreaseFileUsage(LPCWSTR szFile)
	{
		DXFileVectorIterator iter = FindFileInVector(szFile);
		DXMemoryFile* pFile = (DXMemoryFile*)*iter;
		if (!pFile->iUsage)
		{
			DX_DEBUG_OUTPUT1(L"WARNING!!! File usage already 0 : %s.", szFile);
			return;
		}
		pFile->iUsage--;
	}


	void CDXFileSystem::FreeFile(DXFileVectorIterator iter)
	{
		DXMemoryFile* pFile = (DXMemoryFile*)*iter;
		
		if (pFile->iUsage)
		{
			DX_DEBUG_OUTPUT2(L"WARNING!!! Called on file with non-zero usage : FILE: %s USAGE: %d.", pFile->szFileName, pFile->iUsage);
		}

		// Delete the file data.
		DX_FREE(pFile->pFileData);
		// Delete the DXMemoryFile object.
		DX_FREE(pFile);
		// Remove the file object from the vector.
		m_vecOpenedFiles.erase(iter);
	}


	void CDXFileSystem::FreeFile(LPCWSTR szFile)
	{
		DXFileVectorIterator iter = FindFileInVector(szFile);
		FreeFile(iter);
	}


	void CDXFileSystem::FreeAllFiles(bool bZeroUsageOnly)
	{
		DXFileVectorIterator iter = m_vecOpenedFiles.begin();
		while (iter != m_vecOpenedFiles.end())
		{
			DXMemoryFile* pFile = (DXMemoryFile*)*iter;
			if (bZeroUsageOnly && pFile->iUsage)
			{
				continue;
			}
			FreeFile(iter);
			iter = m_vecOpenedFiles.begin();
		}
	}


	HRESULT CDXFileSystem::ReadFile(LPCWSTR szInFile, void** ppDataBytes, size_t* pOutFileSize)
	{
		HRESULT hr = S_OK;

		// First find the file in the files vector to see if it is already
		// loaded. If it is then we don't need to open it again.
		DXFileVectorIterator iter = FindFileInVector(szInFile);
		if (iter != m_vecOpenedFiles.end())
		{
			DXMemoryFile* pFile = (DXMemoryFile*)*iter;

			*ppDataBytes = pFile->pFileData;
			*pOutFileSize = pFile->iDataSize;
			// Increase the file usage.
			pFile->iUsage++;

			return S_OK;
		}

		WCHAR szFilePath[DX_MAX_FILE_PATH];
		bool bFound = FindFile(szInFile, szFilePath);
		if (!bFound)
		{
			hr = E_FAIL;
			DX_DEBUG_OUTPUT1(L"ERROR!!! File not found: %s.", szInFile);
			return hr;
		}


		HANDLE file;
		file = CreateFile(szFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (file == INVALID_HANDLE_VALUE)
		{
			hr = E_FAIL;
			DX_DEBUG_OUTPUT1(L"ERROR!!! Unable to open file: %s.", szFilePath);
			return hr;
		}
		
		DWORD dwFileSize = GetFileSize(file, nullptr);
		DWORD dwBytesRead;

		char* pBuffer = (char*)DX_MALLOC(dwFileSize);
		if (!pBuffer)
		{
			hr = E_OUTOFMEMORY;
			return hr;
		}
		
		BOOL bSuccess = ::ReadFile(file, pBuffer, dwFileSize, &dwBytesRead, nullptr);
		if (!bSuccess)
		{
			hr = E_FAIL;
			DX_DEBUG_OUTPUT1(L"Unable to read file: %s.", szFilePath);
			return hr;
		}
		// Close the file.
		CloseHandle(file);

		if (dwBytesRead != dwFileSize)
		{
			DX_DEBUG_OUTPUT1(L"WARNING!!! Bytes read from file not equal to file size. File: %s.", szInFile);
		}

		// Create a memory file object to store in the vector.
		DXMemoryFile* pMemoryFile = (DXMemoryFile*)DX_MALLOC(sizeof(DXMemoryFile));
		pMemoryFile->iDataSize = dwBytesRead;
		pMemoryFile->pFileData = pBuffer;
		pMemoryFile->iUsage = 1;
		wcscpy_s(pMemoryFile->szFileName, DX_MAX_FILE_PATH, szInFile);
		m_vecOpenedFiles.push_back(pMemoryFile);

		*ppDataBytes = pBuffer;
		*pOutFileSize = dwBytesRead;

		return hr;
	}

} // DXFramewrk namespace end