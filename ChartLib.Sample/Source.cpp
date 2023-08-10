#include <Windows.h>
#include <cstdlib>
#include <ChartLib/src/utils/headers/GDI_service.h>

HINSTANCE	 hInst;
LPCTSTR		 szWindowClass = "MyClass";
LPCTSTR		 szTitle = "Title";
INT			 wWidth = 850;
INT			 wHeight = 820;
GDI_service  gdi;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	gdi.initGDI();
	MSG msg;

	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	gdi.shutdownGDI();
	return msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_SIZE);
	wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = (LPCSTR)szWindowClass;
	wcex.hIconSm = NULL;

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;

	HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_MINIMIZEBOX | WS_SYSMENU | WS_CAPTION | WS_MAXIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, wWidth, wHeight, nullptr, nullptr, hInstance, nullptr);
	
	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	RECT rt;

	switch (message)
	{
	case WM_CREATE:
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		GetClientRect(hWnd, &rt);
		hdc = BeginPaint(hWnd, &ps);

		gdi.initDrawing(hdc);
		chart_grid chart_settings = { 500, 25000, Rect(60, 10, 740, 740) };
		gdi.drawCoordinateGrid(chart_settings);

		time_t current_time = time(NULL);
		srand((unsigned)time(NULL));

		bar_collection bars;	
		for (int i = 0; i < 10; ++i)
		{
			Color c(0 + (rand() % 255), 0 + (rand() % 255), 0 + (rand() % 255));
			bar_item item;
			item.value = 500 + (rand() % 25000);
			item.label = L"val_" + std::to_wstring(item.value);
			item.pen = new Pen{ c };
			item.brush = new SolidBrush{ c };
			bars.items.push_back(item);
		}
		gdi.drawBars(bars);

		for (int i = 0; i < 10; ++i)
		{
			delete bars.items[i].pen;
			delete bars.items[i].brush;
		}

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}