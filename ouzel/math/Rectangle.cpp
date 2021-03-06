// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include <cmath>
#include <cassert>
#include "Rectangle.h"

namespace ouzel
{
    bool Rectangle::intersect(const Rectangle& r1, const Rectangle& r2, Rectangle* dst)
    {
        assert(dst);

        float xmin = std::max(r1.x, r2.x);
        float xmax = std::min(r1.right(), r2.right());
        if (xmax > xmin)
        {
            float ymin = std::max(r1.y, r2.y);
            float ymax = std::min(r1.bottom(), r2.bottom());
            if (ymax > ymin)
            {
                dst->set(xmin, ymin, xmax - xmin, ymax - ymin);
                return true;
            }
        }

        dst->set(0, 0, 0, 0);
        return false;
    }

    void Rectangle::combine(const Rectangle& r1, const Rectangle& r2, Rectangle* dst)
    {
        assert(dst);

        dst->x = std::min(r1.x, r2.x);
        dst->y = std::min(r1.y, r2.y);
        dst->width = std::max(r1.x + r1.width, r2.x + r2.width) - dst->x;
        dst->height = std::max(r1.y + r1.height, r2.y + r2.height) - dst->y;
    }
}
