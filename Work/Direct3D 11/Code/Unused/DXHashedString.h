#pragma once

namespace DXFramework
{ // DXFramework namespace begin

	class CDXHashedString
	{
	private:
		char*				m_szString;
		unsigned long		m_pHash;

	public:
		unsigned long		GetHash()	const		{ return (unsigned long)m_pHash;	};
		const char* 		GetString()	const		{ return m_szString;				};
		static	unsigned long		Hash(const char* szString);

	public:
		CDXHashedString(char* szString);
		~CDXHashedString();
	};

} // DXFramework namespace end