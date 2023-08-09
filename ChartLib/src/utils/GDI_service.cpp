#include "headers\GDI_service.h"

ULONG_PTR m_gdiplusToken;
std::unique_ptr<Graphics> graphics;

void GDI_service::initGDI()
{
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, nullptr);
}

void GDI_service::shutdownGDI()
{
	GdiplusShutdown(m_gdiplusToken);
}

void GDI_service::initDrawing(HDC hdc)
{
	graphics = std::make_unique<Graphics>(hdc);
}

void GDI_service::drawRectangle(Rect coord, Color rect_color)
{
	Pen blackPen(rect_color, 1);
	graphics->DrawRectangle(&blackPen, coord);
}

void GDI_service::drawCoordinateGrid(chart_grid chart_settings)
{
	//Draw border
	int segmentSize = 5;
	Pen blackPen(Color(255, 0, 0, 0), 1);
	drawRectangle(chart_settings.grid_size, Color(255, 0, 0, 0));

	long numberOfSegments = chart_settings.max_value / chart_settings.segment_value;
	Gdiplus::Point pStart = Gdiplus::Point(0, chart_settings.grid_size.Y + chart_settings.grid_size.Height);
	Gdiplus::Point pEnd = Gdiplus::Point(0, chart_settings.grid_size.Y + chart_settings.grid_size.Height - segmentSize);

	//DrawX
	for (int i = numberOfSegments; i > 0; --i)
	{		
		pStart.X = i * chart_settings.segment_value + chart_settings.grid_size.X;
		pEnd.X = pStart.X;
		graphics->DrawLine(&blackPen, pStart, pEnd);
	}

	//DrawY
	pStart.X = chart_settings.grid_size.X;
	pEnd.X = chart_settings.grid_size.X + segmentSize;
	for (int i = numberOfSegments; i > 0; --i)
	{
		pStart.Y = chart_settings.grid_size.Y + chart_settings.grid_size.Height - i * chart_settings.segment_value;
		pEnd.Y = pStart.Y;
		graphics->DrawLine(&blackPen, pStart, pEnd);
	}

	GDI_service::drawTextLabels(chart_settings);
}

void GDI_service::drawTextLabels(chart_grid label_settings)
{
	FontFamily font_family(L"Segoe UI");
	Gdiplus::Font font(&font_family, 10, FontStyleBold, UnitPoint);
	SolidBrush solid_brush(Color(255, 0, 0, 0));
	long numberOfSegments = label_settings.max_value / label_settings.segment_value;
	HDC hdc = graphics->GetHDC();
	graphics->ReleaseHDC(hdc);

	SIZE text_size;
	PointF label_position(0.0f, label_settings.grid_size.Y + label_settings.grid_size.Height);

	//Labels for X
	for (long i = 0; i <= numberOfSegments; ++i)
	{
		long current_number = i * label_settings.segment_value;
		std::wstring string_number = std::to_wstring(current_number);
		GetTextExtentPoint32W(hdc, string_number.data(), string_number.size(), &text_size);
		label_position.X = i * label_settings.segment_value + label_settings.grid_size.X - text_size.cx / 2;

		graphics->DrawString(string_number.data(), -1, &font, label_position, &solid_brush);
	}

	//Labels for Y	
	int x_offset = 5;
	int y_offset = 10;
	for (long i = 0; i <= numberOfSegments; ++i)
	{
		long current_number = i * label_settings.segment_value;
		std::wstring string_number = std::to_wstring(current_number);		
		GetTextExtentPoint32W(hdc, string_number.data(), string_number.size(), &text_size);
		label_position.X = label_settings.grid_size.X - text_size.cx - x_offset;
		label_position.Y = label_settings.grid_size.Y + label_settings.grid_size.Height - current_number - y_offset;

		graphics->DrawString(string_number.data(), -1, &font, label_position, &solid_brush);
	}
}