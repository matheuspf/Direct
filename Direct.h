#pragma once

#include <Eigen/Dense>
#include <handy/Handy.h>
#include <map>
#include <queue>
#include "convexHull.hpp"


using Vec = Eigen::VectorXd;
using Mat = Eigen::MatrixXd;


struct Interval
{
    double fx;
    double size;
    int divisions;
    std::vector<int> k;
    Vec x;
};


std::ostream& operator << (std::ostream& out, const Interval& itv)
{
    out << "fx: " << itv.fx << "\t size: " << itv.size << " \t div:" << itv.divisions << "\t ks: [";
    
    for(int i = 0; i < itv.k.size(); ++i)
    {
        if(i)
            out << ",";
        out << itv.k[i];
    }

    out << "]\t xs: [";

    for(int i = 0; i < itv.x.size(); ++i)
    {
        if(i)
            out << ",";
        out << itv.x[i];
    }

    return out << "]\n";
}

bool operator< (const Interval& a, const Interval& b)
{
    return a.fx < b.fx;
}

bool operator> (const Interval& a, const Interval& b)
{
    return a.fx > b.fx;
}


struct Direct
{
    using IntervalMap = std::map<int, std::priority_queue<Interval, std::vector<Interval>, std::greater<Interval>>, std::greater<int>>;

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

        Interval best = { func(Vec::Constant(N, 0.5)), 0.5, 0, std::vector<int>(N), Vec(Vec::Constant(N, 0.5)) };

        IntervalMap intervals;
        intervals[best.divisions].push(best);

        for(int iter = 0; iter < numIterations; ++iter)
        {
            handy::print("Intervals:");
            for(auto p : intervals)
                handy::print(p.second.top());
            handy::print("");

            auto potSet = potentialSet(intervals, best);
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
        bool done = false;
        double eps_ = eps;

        // gamb:
        for(int i = 0; i < hull.size() - 1; ++i)
        {
            double k1 = i > 0 ? (hull[i].fx - hull[i-1].fx) / (hull[i].size - hull[i-1].size) : -1e8;
            double k2 = i < hull.size() - 1 ? (hull[i].fx - hull[i+1].fx) / (hull[i].size - hull[i+1].size) : -1e8;
            double k = std::max(0.0, std::max(k1, k2));

            if(hull[i].fx - k * hull[i].size <= best.fx - eps_ * std::abs(best.fx))
            {
                potSet.push_back(hull[i]);
                done = true;
                // handy::print(hull[i].divisions);
            }
        }

        // if(!done && eps_ != 0.0)
        // {
        //     eps_ = 0.0;
        //     goto gamb;
        // }

        potSet.push_back(hull.back());
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

                    left.divisions++;
                    right.divisions++;
                }

                left.size = intervalSize(left);
                right.size = intervalSize(right);

                best = std::min(best, std::min(left, right));

                intervals[left.divisions].push(std::move(left));
                intervals[right.divisions].push(std::move(right));
            }
        }

        return best;
    }

    std::vector<Interval> convexHull (IntervalMap& intervals)
    {
        auto hullIters = algs::convexHull2D(intervals.begin(), intervals.end(), [](auto& p){
            return std::make_pair(p.second.top().size, p.second.top().fx);
        });

        std::vector<Interval> hull;
        hull.reserve(hullIters.size());

        for(auto it : hullIters)
        {
            hull.emplace_back(it->second.top());
            it->second.pop();

            if(it->second.empty())
                it = intervals.erase(it);
            
            else
                it++;
        }

        return hull;
    }

    double intervalSize (const Interval& interval) const
    {
        int N = interval.k.size();
        int level = interval.divisions / interval.k.size();
        int stage = interval.divisions % interval.k.size();

        return 0.5 * std::pow(3.0, -level) * std::sqrt(N - (8.0 / 9) * stage);
        // return 0.5 * std::pow(3.0, -level);

        // return 0.5 * std::sqrt(std::accumulate(interval.k.begin(), interval.k.end(), 0.0, [](double sum, int k){
        //     return sum + std::pow(3, -2*k);
        // }));
    }


    double eps;
    int numIterations;
};