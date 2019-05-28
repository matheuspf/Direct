#pragma once

#include <Eigen/Dense>
#include <handy/Handy.h>
#include <map>
#include <queue>


using Vec = Eigen::VectorXd;
using Mat = Eigen::MatrixXd;


struct Interval
{
    double fx;
    double size;
    std::vector<int> k;
    Vec x;
};

bool operator< (const Interval& a, const Interval& b)
{
    return a.fx <= b.fx;
}


struct Direct
{
    struct IntervalComp
    {
        bool operator() (float a, float b)
        {
            return a < b + 1e-13;
        }
    };

    using IntervalMap = std::map<float, std::priority_queue<Interval>, IntervalComp>;

    Direct (double eps = 1e-4, int numIterations = 1e4) : eps(eps), numIterations(numIterations)
    {
    }

    template <class Func>
    Vec operator () (const Func& func, Vec lower, Vec upper)
    {
        Vec scale = upper - lower;

        auto scaleX = [&lower, &scale](const auto& x){
            return (lower.array() + x.array() * scale.array()).matrix();
        };

        auto scaledF = [&func, &scaleX](const auto& x){
            return func(scaleX(x));
        };

        int N = lower.size();

        Interval best = { func(Vec::Constant(N, 0.5)), 0.5*std::sqrt(N), std::vector<int>(N), Vec(Vec::Constant(N, 0.5)) };

        IntervalMap intervals = { { 0.5*std::sqrt(N), { best } } };

        for(int iter = 0; iter < numIterations; ++iter)
        {
            handy::print("Iter: ", iter);
            for(const auto& [k, ints] : intervals)
            {
                for(const auto& v : ints)
                    handy::print(v.size, "       ", v.x.transpose());
                handy::print("\n");
            }
            handy::print("potset:");

            auto potSet = potentialSet(intervals, best);
            
            for(const auto& v : potSet)
                handy::print(v.size, "       ", v.x.transpose());
            handy::print("\n\n");

            auto bestIter = createSplits(scaledF, potSet, intervals);

            best = std::min(best, bestIter);
        }

        return scaleX(best.x);
    }

    std::vector<Interval> potentialSet (IntervalMap& intervals, const Interval& best)
    {
        auto hull = convexHull(intervals);

        std::vector<Interval> potSet;
        potSet.reserve(hull.size());
        potSet.push_back(hull.front());

        if(hull.size() > 1)
            potSet.push_back(hull.back());

        for(int i = 1; i < hull.size() - 1; ++i)
        {
            double k1 = (hull[i].fx - hull[i-1].fx) / (hull[i].size - hull[i-1].size);
            double k2 = (hull[i].fx - hull[i+1].fx) / (hull[i].size - hull[i+1].size);
            double k = std::max(k1, k2);

            if(hull[i].fx - k * hull[i].size <= best.fx - eps * std::abs(best.fx))
                potSet.push_back(hull[i]);
        }

        return potSet;
    }

    template <class Func>
    Interval createSplits(const Func& func, const std::vector<Interval>& potSet, IntervalMap& intervals)
    {
        Interval best{1e8};

        for(auto& interval : potSet)
        {
            int smallestK = *std::min_element(interval.k.begin(), interval.k.end());
            std::vector<std::tuple<Interval, Interval, int>> newIntervals;

            for(int i = 0; i < interval.k.size(); ++i) if(interval.k[i] == smallestK)
            {
                Interval left = interval;
                Interval right = interval;

                left.x(i) -= pow(3, -(interval.k[i] + 1));
                right.x(i) += pow(3, -(interval.k[i] + 1));

                left.fx = func(left.x);
                right.fx = func(right.x);

                newIntervals.emplace_back(std::move(left), std::move(right), i);
            }

            std::sort(newIntervals.begin(), newIntervals.end(), [](auto& ta, auto& tb){
                return std::min(std::get<0>(ta).fx, std::get<1>(ta).fx) < std::min(std::get<0>(tb).fx, std::get<1>(tb).fx);
            });

            std::vector<int> prevDims;

            for(auto& [left, right, k] : newIntervals)
            {
                prevDims.push_back(k);

                for(int prev : prevDims)
                {
                    left.k[prev]++;
                    right.k[prev]++;
                }

                left.size = intervalSize(left);
                right.size = intervalSize(right);

                best = std::min(best, std::min(left, right));

                intervals[left.size].push(std::move(left));
                intervals[right.size].push(std::move(right));
            }
        }

        return best;
    }


    std::vector<Interval> convexHull (IntervalMap& intervals)
    {
        std::vector<IntervalMap::iterator> hull;
        hull.reserve(intervals.size());

        for(auto it = intervals.begin(); it != intervals.end(); ++it)
        {
            while(hull.size() >= 2 && crossProduct(hull[hull.size()-2]->second.top(), hull[hull.size()-1]->second.top(), it->second.top()))
                hull.pop_back();
            
            hull.push_back(it);
        }

        std::vector<Interval> hullValues;
        hullValues.reserve(hull.size());

        for(auto it : hull)
        {
            hullValues.emplace_back(it->second.top());
            it->second.pop();

            if(it->second.empty())
                it = intervals.erase(it);
            
            else
                it++;
        }

        return hullValues;
    }

    double crossProduct (const Interval& o, const Interval& a, const Interval& b) const
    {
        return (a.size - o.size) * (b.fx - o.fx) - (a.fx - o.fx) * (b.size - o.size);
    }

    float intervalSize (const Interval& interval) const
    {
        return 0.5 * std::sqrt(std::accumulate(interval.k.begin(), interval.k.end(), 0.0f, [](float sum, int k){
            return sum + std::pow(3, -2*k);
        }));
    }


    double eps;
    int numIterations;
};