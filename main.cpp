#include <iostream>
#include <Eigen/Dense>

#include "Direct.h"
#include "Shekel.h"



int main ()
{
    Shekel<10> f;
    Vec l = Vec::Constant(4, 0.0);
    Vec u = Vec::Constant(4, 10.0);

    Direct direct(1e-4);

    auto res = direct(f, l, u);

    std::cout << res.transpose() << "\n";



    return 0;
}
