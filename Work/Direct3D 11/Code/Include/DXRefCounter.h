#pragma once

namespace DXFramework
{ // DXFramework namespace begin

	class IDXUnknown;

	// A reference counter class that will store pointer to all CDXUnkown
	// objects made. This is useful for holding record of all objects made by
	// the DXFramework. Curently the RefCounter is destroyed when the program
	// exits.
	class CDXRefCounter
	{
	private:
		// The array of objects made by DXFramework.
		std::vector <IDXUnknown*>	m_vecpObjects;

	public:
		// Adds a CDXUnkown object to the RefCounter record.
		void	AddObject(IDXUnknown* pObject);
		// Removes a CDXUnkown object from the RefCounter record.
		// Also checks for the object's ref count. It only removes the object.
		// Freeing dynamically allocated memory is the job of callee.
		void	RemoveObject(IDXUnknown* pUnknown);
		// Calls RemoveObject() on each of the UNKNOWNS in the counter's
		// record.
		void	Clear();
		// This prints all the info of the objects stored on the output window.
		// For debugging purpose only.
		void	DebugDumpObjectList();

	public:
		// Gets the singleton instance of the ref counter.
		static	CDXRefCounter*		GetInstance();

	private:
		// ctor. Private to make the class singleton.
		CDXRefCounter();

	public:
		// dtor. Calls Clear().
		~CDXRefCounter();
	};

} // DXFramework namespace end