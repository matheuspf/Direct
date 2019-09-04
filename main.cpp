#include <iostream>
#include <Eigen/Dense>

#include "Direct.h"
#include "Functions.h"



int main ()
{
    constexpr int N = 4;
    constexpr int M = 5;

    Shekel<M> f;

    // Vec x(4); x << 4, 4, 4, 4; handy::print(f(x));

    Vec l = Vec::Constant(N, 0.0);
    Vec u = Vec::Constant(N, 10.0);

    Direct direct(1e-4, 5);

    auto res = direct(f, l, u);

    handy::print("\n\nFinished\n");
    handy::print("x: ", res.transpose(), "    fx: ", f(res));
    

    return 0;
}
