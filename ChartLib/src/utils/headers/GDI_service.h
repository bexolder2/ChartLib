#pragma once

#include "libs.h"
#include "../chart_grid.h"
#include "../bar_item.h"

using namespace Gdiplus;
using namespace std;

class GDI_service
{
private:
	ULONG_PTR m_gdiplusToken;
	std::unique_ptr<Gdiplus::Graphics> graphics;
	std::unique_ptr<Gdiplus::FontFamily> font_family;
	std::unique_ptr<Gdiplus::Font> font;
	static const wstring font_name;

	HDC GetDC();
	SIZE& GetTextScreenSize(HDC hdc, wstring text);
	void drawTextLabels(chart_grid label_settings);
	void drawRectangle(bar_item item);
	void drawRectangle(Gdiplus::Rect coord, Gdiplus::Color rect_color);

public:
	void initGDI();
	void shutdownGDI();
	void initDrawing(HDC hdc);

	void drawCoordinateGrid(chart_grid chart_settings);
	void drawBars(bar_collection bars);
};