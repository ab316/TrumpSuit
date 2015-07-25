#pragma once
#pragma warning (disable : 4005)

#include <Windows.h>
#include <D3D11.h>
#include <D3DX11.h>
#include <d3dcompiler.h>
#include <DxErr.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <vector>
#include <map>
#include <algorithm>

#ifdef _DEBUG
#define DX_DEBUG
#endif

// Need for setting markers and events for PIX debugging.
#ifdef DX_DEBUG
#include <d3d9.h>
#endif

// Set if compiling for x64 platform.
#ifdef _WIN64
#define DX_X64
#endif



namespace DXFramework
{ // DXFramework namespace begin

	using namespace DirectX;
	using namespace DirectX::PackedVector;

	//////////////// GLOBAL CONSTANTS ////////////////
#define		DX_WINDOW_CLASS_NAME		L"D3DFrameworkWndClass"
#define		DX_WINDOW_DEFAULT_NAME		L"DX Application"
#define		DX_MAX_FILE_PATH			_MAX_PATH
#define		DX_MAX_FILE_NAME			30
#define		DX_ERROR_BUFFER_SIZE		512
#define		DX_MAX_NAME					40
#define		DX_INVALID_CLASS_ID			-1
#define		DX_DEFAULT_NAME				L"UNNAMED"
#define		DX_MEMORY_ALIGNMENT			16

	//////////////////////////////////////////////////


	//////////////////////////// ERROR DEBUGGING MACROS //////////////////////////////
	// Buffer to store debug error messages. Provides the uniform name for the error message
	// buffer.
#define		DX_DEFINE_ERROR_BUFFER				WCHAR sz_DX_ErrorBuffer[DX_ERROR_BUFFER_SIZE]
#define		DX_ERROR_BUFFER						sz_DX_ErrorBuffer

	// Displays an error message box with the specified message (str).
#define		DX_ERROR_MSGBOX						CDXHelper::ErrorMessage
#define		DX_POINT_IN_RECT					CDXHelper::PointInRect
#define		DX_OFFSET_RECT						CDXHelper::OffsetRect
#define		DX_GET_FORMAT_SIZE					CDXHelper::GetFormatSize
#define		DX_SCALE_RECT						CDXHelper::ScaleRect


	// Output a formatted string in the debug window. This is defined only in
	// debug mode. In Release mode it would be replaced by no statement.
#ifdef DX_DEBUG
#define		DX_DEBUG_OUTPUT						CDXHelper::OutputDebugString
//#define		DX_DEBUG_STRING_FORMAT(f)		L"FILE:%s LINE:%d, %s(),\n\t"##f
//#define		DX_DEBUG_STRING_ARGS			(wcsrchr(__FILEW__, '\\')+1), __LINE__, (wcschr(__FUNCTIONW__, ':') + 2)
#define		DX_DEBUG_STRING_FORMAT(f)			L"%s(): "##f
#define		DX_DEBUG_STRING_ARGS				(wcschr(__FUNCTIONW__, ':') + 2)
#define		DX_DEBUG_OUTPUT0(f)							DX_DEBUG_OUTPUT(DX_DEBUG_STRING_FORMAT(f), DX_DEBUG_STRING_ARGS)
#define		DX_DEBUG_OUTPUT1(f, a1)						DX_DEBUG_OUTPUT(DX_DEBUG_STRING_FORMAT(f), DX_DEBUG_STRING_ARGS, a1)
#define		DX_DEBUG_OUTPUT2(f, a1, a2)					DX_DEBUG_OUTPUT(DX_DEBUG_STRING_FORMAT(f), DX_DEBUG_STRING_ARGS, a1, a2)
#define		DX_DEBUG_OUTPUT3(f, a1, a2, a3)				DX_DEBUG_OUTPUT(DX_DEBUG_STRING_FORMAT(f), DX_DEBUG_STRING_ARGS, a1, a2, a3)
#define		DX_DEBUG_OUTPUT4(f, a1, a2, a3, a4)			DX_DEBUG_OUTPUT(DX_DEBUG_STRING_FORMAT(f), DX_DEBUG_STRING_ARGS, a1, a2, a3, a4)
#define		DX_DEBUG_OUTPUT5(f, a1, a2, a3, a4, a5)		DX_DEBUG_OUTPUT(DX_DEBUG_STRING_FORMAT(f), DX_DEBUG_STRING_ARGS, a1, a2, a3, a4, a5)
#define		DX_DEBUG_OUTPUT6(f, a1, a2, a3, a4, a5, a6)	DX_DEBUG_OUTPUT(DX_DEBUG_STRING_FORMAT(f), DX_DEBUG_STRING_ARGS, a1, a2, a3, a4, a5, a6)
#else
#define DX_DEBUG_OUTPUT
#define		DX_DEBUG_OUTPUT0(f)
#define		DX_DEBUG_OUTPUT1(f, a1)
#define		DX_DEBUG_OUTPUT2(f, a1, a2)
#define		DX_DEBUG_OUTPUT3(f, a1, a2, a3)
#define		DX_DEBUG_OUTPUT4(f, a1, a2, a3, a4)
#define		DX_DEBUG_OUTPUT5(f, a1, a2, a3, a4, a5)
#define		DX_DEBUG_OUTPUT6(f, a1, a2, a3, a4, a5, a6)
#endif


#define DX_SAFE_RELEASE(x)			if(x)	{ x->Release(); x = 0;	}
	// Only use to free memory allocated using new.
#define DX_SAFE_DELETE(x)			if(x)	{ delete x; x = 0;		}
	// Only use to free memory allocated using new.
#define DX_SAFE_DELETE_ARRAY(x)		if(x)	{ delete [] x; x = 0;	}
	// Memory allocated with this macro MUST be free with DX_FREE macro.
#define DX_MALLOC(size)						_aligned_malloc(size, DX_MEMORY_ALIGNMENT)
	// Only use this macro to free memory allocated with DX_MALLOC
#define DX_FREE(pBlock)			if (pBlock) { _aligned_free(pBlock); pBlock = 0;	}
#define DX_V_RETURN(x)						{ hr=x; if (FAILED(hr)) { return hr; } }
#define DX_V(x)								{ hr=x; }
#define DX_ALIGN							__declspec(align(DX_MEMORY_ALIGNMENT))
	
#ifdef DX_DEBUG
#define DX_SET_UNKNOWN_NAME(p, sz)			p->SetName(sz);
#define DX_SET_D3D_DEBUG_NAME(obj, strName)	try { obj->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)strlen(strName), strName); } catch (...) { DX_DEBUG_OUTPUT1(L"WARNING!! Exception raised in setting debug name: %s", L#strName); }
#define DX_PIX_EVENT_BEGIN(dwColor, sz)		D3DPERF_BeginEvent(dwColor, sz)
#define DX_PIX_EVENT_END					D3DPERF_EndEvent()
#define DX_PIX_MARKER(dwColor, sz)			D3DPERF_SetMarker(dwColor, sz)
#else
#define DX_SET_UNKNOWN_NAME(p, sz)
#define DX_SET_D3D_DEBUG_NAME(obj, name) 
#define DX_PIX_EVENT_BEGIN(dwColor, sz) 
#define DX_PIX_EVENT_END 
#define DX_PIX_MARKER(dwColor, sz)
#endif
	///////////////////////////////////////////////////////////////////////////////////



	///////////////////////////////// ENUMERATIONS ////////////////////////////////////
	// R G B A don't mean that only color values can be used. They are
	// only used as names for the channels. Each letter is followed by the number of bits
	// that are used to store value in that channel. The end of the format name specifies
	// the how the value in each channel will be interpreted.
	// U		-	unsigned
	// S		-	signed
	// FLOAT	-	floating point
	// NORM		-	normalized. integer in resource, normalized float ([0, 1] for UNORM
	//				and [-1, 1] for SNORM) in shader.
	enum DX_FORMAT
	{
		// A 32 bit unsigned normalized integer. In resource it is 
		// interpreted as an unsigned integer. In a shader it is interpreted
		// as an unsigned normalized float value. [0.0f, 1.0f]. This format is 
		// mainly for the output Render Target.
		DX_FORMAT_R8G8B8A8_UNORM		=	DXGI_FORMAT_R8G8B8A8_UNORM,
		// 24 bit unsigned normalized integer for depth. 8 bit unsigned integer
		// for stencil
		DX_FORMAT_D24_UNORM_S8_UINT		=	DXGI_FORMAT_D24_UNORM_S8_UINT,
		DX_FORMAT_R32G32B32A32_FLOAT	=	DXGI_FORMAT_R32G32B32A32_FLOAT,
		DX_FORMAT_R32G32B32A32_UINT		=	DXGI_FORMAT_R32G32B32A32_UINT,
		DX_FORMAT_R32G32B32_FLOAT		=	DXGI_FORMAT_R32G32B32_FLOAT,
		DX_FORMAT_R32G32B32_UINT		=	DXGI_FORMAT_R32G32B32_UINT,
		DX_FORMAT_R32G32_FLOAT			=	DXGI_FORMAT_R32G32_FLOAT,
		DX_FORMAT_R32G32_UINT			=	DXGI_FORMAT_R32G32_UINT,
		DX_FORMAT_R32_FLOAT				=	DXGI_FORMAT_R32_FLOAT,
		DX_FORMAT_R32_UINT				=	DXGI_FORMAT_R32_UINT,
		DX_FORMAT_B8G8R8A8_UNORM		=	DXGI_FORMAT_B8G8R8A8_UNORM,

		// Below are Block Compressed formats. The texture width and height
		// for these formats must be multiples of 4.
		DX_FORMAT_R5G6B5A1_BC_UNORM		=	DXGI_FORMAT_BC1_UNORM,
		DX_FORMAT_R5G6B5A4_BC_UNORM		=	DXGI_FORMAT_BC2_UNORM,
		DX_FORMAT_R5G6B5A8_BC_UNORM		=	DXGI_FORMAT_BC3_UNORM,
		DX_FORMAT_R8_BC_UNORM			=	DXGI_FORMAT_BC4_UNORM,
		DX_FORMAT_R8G8_BC_UNORM			=	DXGI_FORMAT_BC5_UNORM,
	};
	///////////////////////////////////////////////////////////////////////////////////



	///////////////////////////////// STRUCTS /////////////////////////////////////////
	struct DXWindowState
	{
		HWND	hWnd;
		UINT	uiWidth;
		UINT	uiHeight;
		bool	bWindowed;
	};


	struct DXRect
	{
		float	fLeft;
		float	fTop;
		float	fRight;
		float	fBottom;

		inline DXRect()
		{
		}


		inline DXRect(float left, float top, float right, float bottom)
		{
			fLeft = left;
			fTop = top;
			fRight = right;
			fBottom = bottom;
		}
	};

	
	struct DXPoint
	{
		float x;
		float y;

		inline DXPoint()
		{
		}

		inline DXPoint(float fX, float fY)	:	x(fX),
												y(fY)
		{
		}
	};
	///////////////////////////////////////////////////////////////////////////////////


	//////////////////////////// HELPER CLASS //////////////////////////////////

	// Contains some helper functions.
	class CDXHelper
	{
	public:
	// These functions output a formatted string to the debug output and also copy it
	// in the outString if provided ( != NULL ).
	// Two function to support both ASCII and WIDE character sets as msgFormat.
	// Make sure that strMsgFormat and any supplied strings as arguments are all 
	// either ASCII or Unicode mixed input will result in undefined output.
		static void			OutputDebugString(LPCSTR strMsgFormat, ...);
		static void			OutputDebugString(LPCWSTR strMsgFormat, ...);
		static void			ErrorMessage(LPCWSTR strMsgFormat, ...);
		static size_t		GetFormatSize(DX_FORMAT format);
		static void			ScaleRect(DXRect* pRect, float fX, float fY, bool bCentered);


		inline static bool PointInRect(DXPoint point, DXRect rect)
		{
			return ( (point.x >= rect.fLeft && point.x <= rect.fRight) && (point.y >= rect.fTop && point.y <= rect.fBottom) );
		}


		inline static void OffsetRect(DXRect* pRect, float fX, float fY)
		{
			pRect->fLeft += fX;
			pRect->fRight += fX;
			pRect->fTop += fY;
			pRect->fBottom += fY;
		}
	};

	//////////////////////////////////////////////////////////////////////////////////////////


} // DXFramework namespace end