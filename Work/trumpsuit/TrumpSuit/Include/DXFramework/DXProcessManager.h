#pragma once


namespace DXFramework
{ // DXFramework namespace begin

	class IDXProcess;

	// A DXProcessManager managers processes attached to it, updating them, and killing
	// them when there kill flag is set.
	class CDXProcessManager
	{
		typedef std::vector<IDXProcess*>		DXProcessVector;
		typedef DXProcessVector::iterator		DXProcessVectorIterator;

	protected:
		DXProcessVector			m_vecpProcesses;

	public:
		void		Attach(IDXProcess* pProcess);
		void		Detach(IDXProcess* pProcess);
		void		UpdateProcesses(float fDeltaTime);

	public:
		CDXProcessManager();
		~CDXProcessManager();
	};

} // DXFramework namespace end