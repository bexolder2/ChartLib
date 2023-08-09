#pragma once

#include "headers/libs.h"

using namespace Gdiplus;

struct bar_item 
{
	Pen* pen;
	SolidBrush* brush;
	Gdiplus::Rect coord;
	long value;
	std::wstring label;
};

struct bar_collection
{
	std::vector<bar_item> items;
};