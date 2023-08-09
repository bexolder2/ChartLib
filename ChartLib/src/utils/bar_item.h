#pragma once

#include "headers/libs.h"

using namespace Gdiplus;

struct bar_item 
{
	Pen* pen;
	SolidBrush*	brush;
	std::wstring label;
	Gdiplus::Rect coord;

	long value;
};

struct bar_collection
{
	std::vector<bar_item> items;
};