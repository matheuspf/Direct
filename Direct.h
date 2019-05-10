#pragma once

#include "Helpers.h"


struct Direct
{
    Direct (double eps = 1e-4) : eps(eps)
    {
    }

    template <class F>
    Vec operator () (F f, Vec l, Vec u)
    {
        return Vec{};
    }



    double eps;


};