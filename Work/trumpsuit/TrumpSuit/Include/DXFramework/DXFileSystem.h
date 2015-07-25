#pragma once

namespace DXFramework
{ // DXFramework namespace begin

#define DX_MAX_ADDITIONAL_DIRECTORIES 10

	// A structure to hold information regarding a file read in memory
	// by DXFileSystem.
	struct DXMemoryFile
	{
		WCHAR		szFileName[DX_MAX_FILE_PATH];
		// Size of file data.
		size_t		iDataSize;
		// How many times the file has been requested using ReadFile. For
		// keeping record.
		UINT16		iUsage;
		// Pointer to the file data.
		void*		pFileData;
	};

	typedef std::vector<DXMemoryFile*> DXFileVector;
	typedef std::vector<DXMemoryFile*>::iterator DXFileVectorIterator;

	// A singleton class for handling files for the DXFramework. This class
	// does not inherit from IDXUnknown.
	class CDXFileSystem
	{
	private:
		// A vector of all files read in memory by the file system.
		DXFileVector	m_vecOpenedFiles;
		UINT			m_uiNumAdditionalDirs;
		// Absolute path of the framework's working directory.
		WCHAR			m_szWorkingDirectory[DX_MAX_FILE_PATH];
		// Relative path for the addtional relative directories for searching files.
		WCHAR			m_szAdditionalDirs[DX_MAX_ADDITIONAL_DIRECTORIES][DX_MAX_FILE_PATH];
	
	private:
		// Helper function for FindFile method. Performs the steps 1 to 4 for it.
		bool			LookInTypicalDirectories(LPCWSTR szInFile, LPWSTR szOutAbsolutePath);

	public:
		// Returns a pointer to start of the file name in the given file path string.
		WCHAR*			GetFileName(LPCWSTR szInFile);
		// A valid absolute path must be given. There must not be a slash
		// at the end except when the working directory is a drive's root e.g "C:\"
		// "C:\Hardrive\" in not valid. "C:\Hardrive" is valid.
		void			SetWorkingDirectory(LPCWSTR szNewDirectory);
		// Do not place slashes at the begining and the end. Up to DX_MAX_ADDITIONAL_DIRECTORIES
		// are available. Further calling this method will do nothing.
		void			AddAdditionalDirectory(LPCWSTR szDirectory);
		void			ClearAdditionalDirectories();
		inline bool		FileExists(LPCWSTR szInFile);
		// szInFile must be relative. The functions first straight away
		// tries to open the file. If unable, it then look for it as follows:
		// 1- %WORKING_DIRECTORY%
		// 2- %WORKING_DIRETORY% \\ %ADDITIONAL_DIRECTORY[i]%
		// 3- Parent directory of %WORKING_DIRECTORY%
		// 4- Parent directory of %WORKING_DIRECTORY% \\ %ADDITIONAL_DIRECTORY[i]%
		// 5- Parent's parent directory of %WORKING_DIRECTORY%
		// 6- "Parent's parent directory of %WORKING_DIRECTORY%" \\ %ADDITIONAL_DIRECTORY[i]%
		// 7- "Parent directory parent's parent directory of %WORKING_DIRECTORY%"
		// 8- "Parent directory parent's parent directory of %WORKING_DIRECTORY%" \\ %ADDITIONAL_DIRECTORY[i]%
		// 9- Repeats the above pattern using the file name only if szInFile
		//    has a relative path.
		// 10- returns false.
		// Note: if the file is found straight away, szOutAbsolutePath may not be
		// absolute.
		bool			FindFile(LPCWSTR szInFile, LPWSTR szOutAbsolutePath);
		// Reads a file into the memory. Automatically searches for the file by calling
		// FindFile() method. Avoid absolute path names. If the file was already read by 
		// the file system its usage will be increased. Remember to call DecreaseFileUsage()
		// once the file is no longer need by the object. The buffer is allocated by the file
		// system, therefore is should not be destroyed by the caller.
		HRESULT			ReadFile(LPCWSTR szInFile, void** ppDataBytes, size_t* pOutFileSize);
		// Decreases the file usage. This should be called when an object no longer requires
		// the file data. The file name must be the same which was passed into the ReadFile()
		// method.
		void			DecreaseFileUsage(LPCWSTR szFile);
		// Frees the memory occupied by the file and removes it from the record.
		void			FreeFile(LPCWSTR szFile);
		void			FreeFile(DXFileVectorIterator iter);
		// If Zero usage is true, Only those files whose usage is 0 will be freed.
		// Otherwise all with warning messages.
		void			FreeAllFiles(bool bZeroUsageOnly);
		// Gets the iterator to the file with the specified name in the vector. .end() if
		// not found.
		DXFileVectorIterator	FindFileInVector(LPCWSTR szFileName);
		bool					ValidIterator(DXFileVectorIterator iter)	{ return (iter != m_vecOpenedFiles.end()); };
		
	public:
		inline static CDXFileSystem* GetInstance()
		{
			// Can't use new here as destroying it would then be the developer's
			// responsibility. The file system should be freed when the application exits.
			static CDXFileSystem fileSystem;

			return &fileSystem;
		}

	private:
		CDXFileSystem();

	public:
		~CDXFileSystem();
	};

} // DXFramewrk namespace end