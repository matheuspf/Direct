#pragma once

#include <Eigen/Dense>
#include <handy/Handy.h>


using Vec = Eigen::VectorXd;
using Mat = Eigen::MatrixXd;


struct Interval
{
    std::vector<int> k;
    Vec x;
    double fx;
};


namespace std
{

template <>
struct hash<Interval>
{
    size_t operator() (const Interval& interval) const
    {
        return accumulate(interval.k.begin(), interval.k.end(), 0);
    }
};

} // namespace std


template <typename T>
void make_heap (std::vector<T>& v)
{
    std::make_heap(v.begin(), v.end(), std::greater<T>{});
}

template <typename T>
void push_heap (std::vector<T>& v, T&& t)
{
    v.emplace_back(std::forward<T>(t));
    std::push_heap(v.begin(), v.end(), std::greater<T>{});
}

template <typename T>
T pop_heap (std::vector<T>& v)
{
    std::pop_heap(v.begin(), v.end(), std::greater<T>{});
    T back = v.back();
    v.pop_back();

    return back;
}

