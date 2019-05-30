#include <iostream>
#include <Eigen/Dense>

#include "Direct.h"
#include "Shekel.h"



int main ()
{
    Shekel<5> f;

    Vec l = Vec::Constant(4, 0.0);
    Vec u = Vec::Constant(4, 10.0);

    Direct direct(1e-4, 10000);

    auto res = direct(f, l, u);

    handy::print("\n\nFinished\n");
    handy::print("x: ", res.transpose(), "    fx: ", f(res));
    

    return 0;
}
