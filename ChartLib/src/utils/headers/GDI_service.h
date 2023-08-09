#pragma once

#include "libs.h"
#include "../chart_grid.h"

using namespace Gdiplus;
using namespace std;

class GDI_service
{
private:
	ULONG_PTR m_gdiplusToken;
	std::unique_ptr<Graphics> graphics;
	void drawTextLabels(chart_grid label_settings);

public:
	void initGDI();
	void shutdownGDI();
	void initDrawing(HDC hdc);

	void drawRectangle(Gdiplus::Rect coord, Gdiplus::Color rect_color);
	void drawCoordinateGrid(chart_grid chart_settings);
};