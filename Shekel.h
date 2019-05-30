#pragma once


#include <Eigen/Dense>


template <int M = 5>
struct Shekel
{
    Shekel() : C(4, 10), b(10)
    {
        C << 4, 1, 8, 6, 3, 2, 5, 8, 6, 7,
             4, 1, 8, 6, 7, 9, 3, 1, 2, 3.6,
             4, 1, 8, 6, 3, 2, 5, 8, 6, 7,
             4, 1, 8, 6, 7, 9, 3, 1, 2, 3.6;
        b << 1, 2, 2, 4, 4, 6, 3, 7, 5, 5;
        b /= 10;
    }

    double operator() (const Eigen::VectorXd& x) const
    {
        double res = 0.0;

        for(int i = 0; i < M; ++i)
        {
            double r = 0.0;

            for(int j = 0; j < 4; ++j)
                r += std::pow(x(j) - C(j, i), 2);

            res += -1 / (r + b(i));
        }

        return res;
    }


    static constexpr double minimum ()
    {
        if constexpr(M == 5)
            return -10.1532;

        if constexpr(M == 7)
            return -10.4029;

        if constexpr(M == 10)
            return -10.5364;

        return 0.0;
    }


    Mat C;
    Vec b;
};
