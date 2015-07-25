#include <ctype.h>
#include <string>
#include "DXHashedString.h"

namespace DXFramework
{ // DXFramework namespace begin

	CDXHashedString::CDXHashedString(char* szString)
	{
		m_szString = szString;
		m_pHash = Hash(szString);
	}


	CDXHashedString::~CDXHashedString()
	{
	}


	unsigned long CDXHashedString::Hash(const char* szString)
	{
		unsigned long BASE = 65521;
		unsigned long NMAX = 5552;

#define DO1(buf, i)		{ s1 += tolower(buf[i]);	s2 += s1;	};
#define DO2(buf, i)		DO1(buf, i);	DO1(buf, i+1);
#define DO4(buf, i)		DO2(buf, i);	DO2(buf, i+2);
#define DO8(buf, i)		DO4(buf, i);	DO4(buf, i+4);
#define DO16(buf)		DO8(buf, 0);	DO8(buf, 8);

		if (!szString)
		{
			return 0;
		}

		unsigned long s1 = 0;
		unsigned long s2 = 0;

		for (unsigned long i=strlen(szString); i>0;)
		{
			unsigned long k = i < NMAX ? i : NMAX;
			i -= k;
			while (k >= 16)
			{
				DO16(szString);
				szString += 16;
				k -= 16;
			}

			if (k != 0)
			{
				do
				{
					s1 += *szString;
					szString++;
					s2 += s1;
					k--;
				} 
				while (k);
			}
		}

		s1 %= BASE;
		s2 %= BASE;

		return ( (s2 << 16) | s1 );
	}

} // DXFramework namespace end