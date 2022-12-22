#pragma once

String DateTime2GameTime(const DateTime&);
double ItemAmount2Visual(ITEMTYPE, double);

ItemRate makeItemRate(ITEMTYPE, double, double);

void initialize_lib();

bool search_map(Point, Robot*);

RectF getOverlappingRectF(const RectF&, const RectF&);
