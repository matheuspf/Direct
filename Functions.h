#pragma once
#include <Eigen/Dense>


using Vec = Eigen::VectorXd;
using Mat = Eigen::MatrixXd;


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

    double operator() (const Vec& x) const
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


template <int N = 3>
struct Hartmann
{
    Hartmann () : a(4), A(4, N), P(4, N)
    {
        a << 1, 1.2, 3, 3.2;
        init();
    }

    double operator () (const Vec& x) const
    {
        double res = 0.0;

        for(int i = 0; i < 4; ++i)
        {
            double ri = 0;

            for(int j = 0; j < N; ++j)
                ri += A(i, j) * std::pow(x(j) - P(i, j), 2);

            res += a(i) * std::exp(-ri);
        }
        
        return -res;
    }

    template <int N_ = N, std::enable_if_t<N_ == 3, int> = 0>
    void init ()
    {
        A << 3, 10, 30,
             0.1, 10, 35,
             3, 10, 30,
             0.1, 10, 35;

        P << 3689, 1170, 2673,
             4699, 4387, 7470,
             1091, 8732, 5547,
             381, 5743, 8828;

        P *= 1e-4;
    }

    template <int N_ = N, std::enable_if_t<N_ == 6, int> = 0>
    void init ()
    {
        A << 10, 3, 17, 3.5, 1.7, 8,
             0.05, 10, 17, 0.1, 8, 14,
             3, 3.5, 1.7, 10, 17, 8,
             17, 8, 0.05, 10, 0.1, 14;

        P << 1312, 1696, 5569, 124, 8283, 5886,
             2329, 4135, 8307, 3736, 1004, 9991,
             2348, 1451, 3522, 2883, 3047, 6650,
             4047, 8828, 8732, 5743, 1091, 381;

        P *= 1e-4;
    }


    Vec a;
    Mat A, P;
};

