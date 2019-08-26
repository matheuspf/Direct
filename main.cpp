#include <iostream>
#include <Eigen/Dense>

#include "Direct.h"
#include "Functions.h"



int main ()
{
    constexpr int N = 6;

    Hartmann<N> f;

    Vec l = Vec::Constant(N, 0.0);
    Vec u = Vec::Constant(N, 1.0);

    Direct direct(1e-4, 1000);

    auto res = direct(f, l, u);

    handy::print("\n\nFinished\n");
    handy::print("x: ", res.transpose(), "    fx: ", f(res));
    

    return 0;
}
