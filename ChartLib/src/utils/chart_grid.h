#pragma once

#include "headers/libs.h"

using namespace Gdiplus;

struct chart_grid
{
	long				segment_value;
	long				max_value = LONG_MAX;
	Gdiplus::Rect		grid_size;
};