#pragma once

#include <vector>
#include <algorithm>

namespace algs
{

struct Identity
{
    template <class T>
    decltype(auto) operator () (T&& x){ return std::forward<T>(x); }
};

template <class T>
T cprod (const std::pair<T, T>& o, const std::pair<T, T>& a, const std::pair<T, T>& b)
{
    return (a.first - o.first) * (b.second - o.second) - (a.second - o.second) * (b.first - o.first);
}

template <class Iter, class Getter = Identity>
std::vector<Iter> convexHull2D (Iter first, Iter last, Getter getter = Getter{})
{
    std::vector<Iter> hull;

    for(auto it = first; it != last; ++it)
    {
        while(hull.size() >= 2 && cprod(getter(*hull[hull.size()-2]), getter(*hull[hull.size()-1]), getter(*it)) <= 0)
            hull.pop_back();

        hull.push_back(it);
    }

    // int lowerSize = hull.size();

    // if(lowerSize <= 2)
    //     return hull;

    // for(auto it = std::prev(last); it != first; --it)
    // {
    //     while(hull.size() > lowerSize && cprod(getter(*hull[hull.size()-2]), getter(*hull[hull.size()-1]), getter(*it)) <= 0)
    //         hull.pop_back();

    //     hull.push_back(it);
    // }

    // hull.pop_back();

    return hull;
}

} // namespace algs