#pragma once

double ItemAmount2Visual(ITEMTYPE, double);

ItemRate makeItemRate(ITEMTYPE, double, double);

void initialize_lib();

bool search_map(Point, Robot*);
