#include "utils.h"

int rect_contains(int w, int h, int x, int y, int px, int py)
{
    return ((y < py && py < y + h) && (x < px && px < x + w));
}

