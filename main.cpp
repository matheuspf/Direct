#include <iostream>
#include <Eigen/Dense>

#include "Direct.h"
#include "Functions.h"



int main ()
{
    constexpr int N = 5;

    Shekel<N> f;

    // Vec x(4); x << 4, 4, 4, 4; handy::print(f(x));

    Vec l = Vec::Constant(N, 0.0);
    Vec u = Vec::Constant(N, 10.0);

    Direct direct(1e-4, 20);

    auto res = direct(f, l, u);

    handy::print("\n\nFinished\n");
    handy::print("x: ", res.transpose(), "    fx: ", f(res));
    

    return 0;
}
