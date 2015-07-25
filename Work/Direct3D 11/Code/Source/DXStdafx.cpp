#include <DXStdafx.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "dxerr.lib")
#pragma comment (lib, "dxguid.lib")

#ifdef DX_DEBUG
#pragma comment (lib, "d3dx11d.lib")
#pragma comment (lib, "d3d9.lib")
#else
#pragma comment (lib, "d3dx11.lib")
#endif