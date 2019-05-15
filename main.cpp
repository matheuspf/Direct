#include <iostream>
#include <Eigen/Dense>

#include "Direct.h"
#include "Shekel.h"



int main ()
{
    Shekel<10> f;

    Vec l = Vec::Constant(4, 0.0);
    Vec u = Vec::Constant(4, 10.0);

    Vec x(4); x << 1.00037, 1.00026, 1.00036, 1.00024;
    handy::print(f(x));
    // Direct direct(1e-4, 100);

    // auto res = direct(f, l, u);

    // handy::print("Finished");
    // handy::print("x: ", res.transpose(), "    fx: ", f(res));
    

    return 0;
}
