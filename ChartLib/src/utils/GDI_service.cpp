#include "headers\GDI_service.h"

ULONG_PTR m_gdiplusToken;
const wstring GDI_service::font_name = L"Segoe UI";
std::unique_ptr<Gdiplus::Graphics> graphics;
std::unique_ptr<Gdiplus::FontFamily> font_family;
std::unique_ptr<Gdiplus::Font> font;

long x_axis_segment_pixels_value;
long x_axis_segment_phisical_value;
long y_axis_segment_pixels_value;
long y_axis_segment_phisical_value;

long x_chart_shift;
long y_chart_shift;
long chart_width;
long chart_height;

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
	font_family = std::make_unique<Gdiplus::FontFamily>(font_name.data());
	font = std::make_unique<Gdiplus::Font>(font_family.get(), 10, FontStyleBold, UnitPoint);
}

void GDI_service::drawRectangle(Rect coord, Color rect_color)
{
	Pen blackPen(rect_color, 1);
	graphics->DrawRectangle(&blackPen, coord);
}

void GDI_service::drawCoordinateGrid(chart_grid chart_settings, bool disable_x_labels)
{
	//Draw border
	int segmentSize = 5;
	Pen blackPen(Color(255, 0, 0, 0), 1);
	drawRectangle(chart_settings.grid_size, Color(255, 0, 0, 0));

	long numberOfSegments = chart_settings.max_value / chart_settings.segment_value;
	Gdiplus::Point pStart = Gdiplus::Point(0, chart_settings.grid_size.Y + chart_settings.grid_size.Height);
	Gdiplus::Point pEnd = Gdiplus::Point(0, chart_settings.grid_size.Y + chart_settings.grid_size.Height - segmentSize);

	x_chart_shift = chart_settings.grid_size.X;
	y_chart_shift = chart_settings.grid_size.Y;
	chart_width = chart_settings.grid_size.Width;
	chart_height = chart_settings.grid_size.Height;
	y_axis_segment_phisical_value = chart_settings.segment_value;
	y_axis_segment_pixels_value = chart_settings.grid_size.Height / numberOfSegments;
	//For bars chart
	x_axis_segment_phisical_value = y_axis_segment_phisical_value;
	x_axis_segment_pixels_value = y_axis_segment_pixels_value;

	//DrawX
	for (int i = numberOfSegments; i > 0; --i)
	{		
		pStart.X = i * x_axis_segment_pixels_value + chart_settings.grid_size.X;
		pEnd.X = pStart.X;
		graphics->DrawLine(&blackPen, pStart, pEnd);
	}

	//DrawY
	pStart.X = chart_settings.grid_size.X;
	pEnd.X = chart_settings.grid_size.X + segmentSize;
	for (int i = numberOfSegments; i > 0; --i)
	{
		pStart.Y = chart_settings.grid_size.Y + chart_settings.grid_size.Height - i * y_axis_segment_pixels_value;
		pEnd.Y = pStart.Y;
		graphics->DrawLine(&blackPen, pStart, pEnd);
	}

	GDI_service::drawTextLabels(chart_settings, disable_x_labels);
}

void GDI_service::drawTextLabels(chart_grid label_settings, bool disable_x_labels)
{
	SIZE text_size;
	long current_number;
	std::wstring string_number;
	HDC hdc = GDI_service::GetDC();
	SolidBrush solid_brush(Color(255, 0, 0, 0));
	long numberOfSegments = label_settings.max_value / label_settings.segment_value;
	PointF label_position(0.0f, label_settings.grid_size.Y + label_settings.grid_size.Height);

	if (!disable_x_labels)
	{
		//Labels for X
		for (long i = 0; i <= numberOfSegments; ++i)
		{
			current_number = i * x_axis_segment_pixels_value;
			string_number = std::to_wstring(i * x_axis_segment_phisical_value);
			text_size = GetTextScreenSize(hdc, string_number);
			label_position.X = i * x_axis_segment_pixels_value + label_settings.grid_size.X - text_size.cx / 2;

			graphics->DrawString(string_number.data(), -1, font.get(), label_position, &solid_brush);
		}
	}

	//Labels for Y	
	int x_offset = 5;
	int y_offset = 10;
	for (long i = 0; i <= numberOfSegments; ++i)
	{
		current_number = i * y_axis_segment_pixels_value;
		string_number = std::to_wstring(i * y_axis_segment_phisical_value);
		text_size = GetTextScreenSize(hdc, string_number);
		label_position.X = label_settings.grid_size.X - text_size.cx - x_offset;
		label_position.Y = label_settings.grid_size.Y + label_settings.grid_size.Height - current_number - y_offset;

		graphics->DrawString(string_number.data(), -1, font.get(), label_position, &solid_brush);
	}
}

void GDI_service::drawRectangle(bar_item item)
{
	HDC hdc = GDI_service::GetDC();
	SolidBrush solid_brush(Color(255, 255, 255, 255));
	std::wstring string_number = std::to_wstring(item.value);
	PointF text_position(item.coord.X + ((item.coord.Width - GetTextScreenSize(hdc, string_number).cx) / 2), item.coord.Y + 20);

	graphics->DrawRectangle(item.pen, item.coord);
	graphics->FillRectangle(item.brush, item.coord);
	graphics->DrawString(string_number.data(), -1, font.get(), text_position, &solid_brush);

	text_position.X = item.coord.X + ((item.coord.Width - GetTextScreenSize(hdc, item.label).cx) / 2) + 22;
	text_position.Y = item.coord.Y + item.coord.Height - 20 - GetTextScreenSize(hdc, item.label).cx;
	StringFormat stringFormat;
	stringFormat.SetFormatFlags(Gdiplus::StringFormatFlagsDirectionVertical);
	graphics->DrawString(item.label.data(), -1, font.get(), text_position, &stringFormat, &solid_brush);
}

void GDI_service::drawBars(bar_collection bars)
{
	for (int i = 0; i < bars.items.size(); ++i)
	{
		bars.items[i].coord.Width = 50; //TODO: Move 50 to constants
		bars.items[i].coord.Height = bars.items[i].value / y_axis_segment_phisical_value * y_axis_segment_pixels_value;
		bars.items[i].coord.X = x_chart_shift + i * 60 + 10; //TODO: Move 10 to constants
		bars.items[i].coord.Y = y_chart_shift + chart_height - bars.items[i].coord.Height;
		drawRectangle(bars.items[i]);
	}
}

//Utils
HDC GDI_service::GetDC() 
{
	HDC hdc = graphics->GetHDC();
	graphics->ReleaseHDC(hdc);
	return hdc;
}

SIZE& GDI_service::GetTextScreenSize(HDC hdc, wstring text) 
{
	SIZE text_size;
	GetTextExtentPoint32W(hdc, text.data(), text.size(), &text_size);
	return text_size;
}