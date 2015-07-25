#include "DXStdafx.h"
#include "DXBaseApplication.h"
#include "DX3DDriver.h"	
#include "DXGraphicsDriver.h"
#include "DXHardwareBuffer.h"
#include "DXUIButton.h"
#include "DXUIEditBox.h"
#include "DXUIRadioButton.h"
#include "DXUIScrollBar.h"
#include "DXUIManager.h"
#include "DXTextRenderer.h"
#include "DXBitmapFontFile.h"

#include "DXStackAllocator.h"
#include "DXPoolAllocator.h"
#include "DXHeapAllocator.h"
#include "DXMemoryManager.h"

using namespace DXFramework;


CDXBaseApplication* pApp = CDXBaseApplication::GetInstance();

#define DX_PERFORMANCE_DECLARE LARGE_INTEGER dxFreq, dxC1, dxC2; QueryPerformanceFrequency(&dxFreq)
#define DX_PERFORMANCE_BEGIN QueryPerformanceCounter(&dxC1)
#define DX_PERFORMANCE_END QueryPerformanceCounter(&dxC2)
#define DX_PERFORMANCE_TIME (((double)dxC2.QuadPart - (double)dxC1.QuadPart)/(double)dxFreq.QuadPart)

void OnFrameRender(double fTime, float fElapsedTime)
{
	UNREFERENCED_PARAMETER(fElapsedTime);

	static CDXGraphicsDriver* pGraphics = CDXGraphicsDriver::GetInstance();
	static float fFps = 0.0f;
	static float fAccumlatedTime = 0.0f;
	static WCHAR szDimensions[40] = { 0 };
	static WCHAR szTime[40] = { 0 };

	fAccumlatedTime += fElapsedTime;

	if (fAccumlatedTime > 0.5f)
	{
		fAccumlatedTime = 0.0f;
		fFps = pApp->GetFPS();
	}
	
	CDX3DDriver* p3D = CDX3DDriver::GetInstance();
	swprintf_s(szDimensions, 40, L"%d x %d", p3D->GetBackbufferWidth(), p3D->GetBackbufferHeight());
	swprintf_s(szTime, 40, L"%.2fs FPS: %.0f\n", fTime, fFps);

	//float fBackColor[4] = { sin(fTime), 0.4f, cos(fTime), 1.0f };
	float fBackColor[4] = { .5f, .5f, .5f, 1.0f };

	pGraphics->Clear(true, fBackColor);

	//IDXUIElement* pB1 = CDXUIManager::GetInstance()->GetElement(2);
	//pB1->Move(200 - sin(fTime*2.0f)*100, 100);

	//WCHAR coord[25];
	float x, y;
	x = (float)sin(fTime);
	y = (float)cos(fTime);
	//swprintf_s(coord, L"(%f, %f)", x, y);

	CDXTextRenderer* pText = CDXTextRenderer::GetInstance();
	pText->SetFontColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	pText->BeginText(5, 5, true);
	pText->SetFontSize(24);
	//pText->RenderText(L"Aa");
	pText->RenderText(szDimensions, 0);
	pText->RenderText(szTime, 0);
	pText->EndText();
}


struct test
{
	int	 a;
	char szText1[6];
	char szText2[4];

	test()
	{
		a = 0xFFFFFFFF;
		strcpy_s(szText1, 6, "HELLO");
		strcpy_s(szText2, 4, "HOW");
	}
};


//_declspec(align(8))
struct test2
{
	char szText1[6];
	//int e;
	char szText2[4];

	test2()
	{
		strcpy_s(szText1, 6, "START");
		strcpy_s(szText2, 4, "END");
		//e = 316;
	}
};


int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	UNREFERENCED_PARAMETER(nShowCmd);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(hPrevInstance);


	//SetThreadAffinityMask(GetCurrentThread(), 0x4);
	//SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	//SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);



//	DX_PERFORMANCE_DECLARE;
//
//	CDXMemoryManager* pMemMan = CDXMemoryManager::GetInstance();
//#if !defined (DX_MEMORY_DEBUG)
//	CDXPoolAllocator* poolId = pMemMan->AddPool(sizeof(test), __alignof(test), 101);
//	CDXStackAllocator* pStack = pMemMan->AddStack(sizeof(test)*2000);
//#else
//	CDXPoolAllocator* poolId = pMemMan->AddPool(sizeof(test), __alignof(test), 101, __FILEW__, __LINE__);
//	CDXStackAllocator* pStack = pMemMan->AddStack(sizeof(test)*2000, __FILEW__, __LINE__);
//#endif
//
//	test* t;
//
//	CDXPoolAllocator pool;
//	void* p = malloc(1024);
//	pool.Initialize(sizeof(test), __alignof(test), 1024, p);
//
//	DX_PERFORMANCE_BEGIN;
//	for (int i=0; i<50; i++)
//	{
//		//t = pool.AllocateNew<test>();
//		//pool.DeallocateDelete<test>(t);
//#if defined (DX_MEMORY_DEBUG)
//		t = new(pMemMan->AllocateFromStack(pStack, sizeof(test), __FILEW__, __LINE__)) test();
//#else
//		t = new(pMemMan->AllocateFromStack(pStack, sizeof(test))) test();
//#endif
//		//pMemMan->DeallocateFromStack(t);
//	}
//	DX_PERFORMANCE_END;
//
//	double tCustom = DX_PERFORMANCE_TIME;
//
//
//	DX_PERFORMANCE_BEGIN;
//	for (int i=0; i<50; i++)
//	{
//		t = new test();
//		//delete t;
//	}
//	DX_PERFORMANCE_END;
//
//	double tNew = DX_PERFORMANCE_TIME;
//
//	double factor = tNew / tCustom;
//	DX_ERROR_MSGBOX(L"%f", factor);
//
//	//return 0;
//
//
//	//DX_PERFORMANCE_BEGIN;
//	//for (int i=0; i<100000; i++)
//	//{
//	//	test* a = new test();
//	//	delete a;
//	//}
//	//DX_PERFORMANCE_END;
//
//	//double t1 = DX_PERFORMANCE_TIME;
//
//	//DX_PERFORMANCE_BEGIN;
//	//for (int i=0; i<100000; i++)
//	//{
//	//	test* a = pool.AllocateNew<test>();
//	//	pool.DeallocateDelete<test>(a);;
//	//	//test* a = (test*)pool.Allocate(sizeof(test));
//	//	//pool.Deallocate(a);
//	//}
//	//DX_PERFORMANCE_END;
//	//double t2 = DX_PERFORMANCE_TIME;
//
//	//double factor = t1 / t2;
//	//DX_ERROR_MSGBOX(L"%f", factor);
//
//
//	//return 0;
//
	pApp->SetInitialMemory();
	pApp->Startup(hInstance, 800, 600, true);
	pApp->SetCallbackOnFrameRender(OnFrameRender);

	CDXUIManager* pUIMan = CDXUIManager::GetInstance();
	pUIMan->AddButton(0, L"Hello1", 50, 100, 150, 50);
	pUIMan->AddEditBox(2, 30, 300, 90, 250, 60);
	pUIMan->AddButton(1, L"Hello2", 110, 110, 100, 50);
	pUIMan->AddStatic(3, L"This is a static control!\nHello! again!", 400, 400, 24.0f);
	////pUIMan->AddCheckBox(4, 100, 200);
	////pUIMan->AddRadioButton(5, 0, 100, 250);
	////pUIMan->AddRadioButton(6, 0, 100, 300);
	////pUIMan->AddRadioButton(7, 0, 100, 350);

	////pUIMan->AddRadioButton(8, 1, 200, 250);
	////pUIMan->AddRadioButton(9, 1, 200, 300);
	////pUIMan->AddRadioButton(10, 1, 200, 350);

	////pUIMan->AddScrollBar(11, 300, 250, 200, 40);

	//IDXUIElement* pB1 = pUIMan->GetElement(0);
	//pB1->SetZOrder(DX_UIELEMENT_DEFAULT_ZORDER-1);

	//pB1 = pUIMan->GetElement(1);
	//pB1->SetZOrder(DX_UIELEMENT_DEFAULT_ZORDER+1);

	//pB1 = pUIMan->GetElement(2);
	//pB1->SetZOrder(DX_UIELEMENT_DEFAULT_ZORDER);

	//pB1 = pUIMan->GetElement(6);
	//((CDXUIRadioButton*)pB1)->SetChecked(true);
	pUIMan->SortElements();

	pApp->Run();

	pApp->Shutdown();

	return 0;
}