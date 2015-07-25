#include "DXStdafx.h"
#include "DXBaseApplication.h"
#include "DXResource.h"


namespace DXFramework
{ // DXFramework namespace begin

	void CDXHelper::OutputDebugString(LPCSTR strMsgFormat, ...)
	{
		char strOutBuffer[DX_ERROR_BUFFER_SIZE];
		char strInFormat[DX_ERROR_BUFFER_SIZE] = "\tDXFramework: ";
		
		strcat_s(strInFormat, strMsgFormat);
		strcat_s(strInFormat, DX_ERROR_BUFFER_SIZE, "\n");

		va_list args;
		va_start(args, strMsgFormat);
		vsprintf_s(strOutBuffer, DX_ERROR_BUFFER_SIZE, strInFormat, args);
		va_end(args);

		OutputDebugStringA(strOutBuffer);
	}


	void CDXHelper::OutputDebugString(LPCWSTR strMsgFormat, ...)
	{
		WCHAR strOutBuffer[DX_ERROR_BUFFER_SIZE];
		WCHAR strInFormat[DX_ERROR_BUFFER_SIZE] = L"\tDXFramework: ";

		wcscat_s(strInFormat, DX_ERROR_BUFFER_SIZE, strMsgFormat);
		wcscat_s(strInFormat, DX_ERROR_BUFFER_SIZE, L"\n");

		va_list args;
		va_start(args, strMsgFormat);
		vswprintf_s(strOutBuffer, DX_ERROR_BUFFER_SIZE, strInFormat, args);

		va_end(args);

		::OutputDebugString(strOutBuffer);
	}


	void CDXHelper::ErrorMessage(LPCWSTR strMsgFormat, ...)
	{
		WCHAR strOutBuffer[DX_ERROR_BUFFER_SIZE];

		va_list args;
		va_start(args, strMsgFormat);
		vswprintf_s(strOutBuffer, DX_ERROR_BUFFER_SIZE, strMsgFormat, args);

		va_end(args);

		MessageBox(NULL, strOutBuffer, L"DXFramework", MB_ICONERROR);
	}


	size_t CDXHelper::GetFormatSize(DX_FORMAT format)
	{
		switch (format)
		{
		case DX_FORMAT_D24_UNORM_S8_UINT:
		case DX_FORMAT_R8G8B8A8_UNORM:
			return 4;
			break;

		case DX_FORMAT_R32G32B32A32_FLOAT:
		case DX_FORMAT_R32G32B32A32_UINT:
			return 16;
			break;

		case DX_FORMAT_R32G32B32_FLOAT:
		case DX_FORMAT_R32G32B32_UINT:
			return 12;
			break;

		case DX_FORMAT_R32G32_FLOAT:
		case DX_FORMAT_R32G32_UINT:
			return 8;
			break;

		default:
			return (size_t)-1;
		}
	}


	void CDXHelper::ScaleRect(DXRect* pRect, float fX, float fY, bool bCentered)
	{
		float fOldWidth = pRect->fRight - pRect->fLeft;
		float fOldHeight = pRect->fBottom - pRect->fTop;
		float fDiffWidth = (fOldWidth * fX) - fOldWidth;
		float fDiffHeight = (fOldHeight * fY) - fOldHeight;
		if (bCentered)
		{
			float fHalfWidth = fDiffWidth / 2.0f;
			float fHalfHeight = fDiffHeight / 2.0f;
			pRect->fLeft -= fHalfWidth;
			pRect->fRight += fHalfWidth;
			pRect->fTop -= fHalfHeight;
			pRect->fBottom += fHalfHeight;
		}
		else
		{
			pRect->fRight += fDiffWidth;
			pRect->fBottom += fDiffHeight;
		}
	}

} // DXFramework namespace end