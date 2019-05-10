#pragma once

#include "Helpers.h"


struct Direct
{
    Direct (double eps = 1e-4, int numIterations = 1e4) : eps(eps), numIterations(numIterations)
    {
    }

    template <class Func>
    Vec operator () (const Func& func, Vec lower, Vec upper)
    {
        Vec scale = upper - lower;

        auto scaledF = [&func, &scale](const auto& x){
            return f(x * scale);
        };

        int N = lower.size();

        Interval best {1e8};

        std::unordered_map<int, std::vector<Interval>> intervals({
            { 0, { func(Vec::Constant(N, 0.5)), 0, std::vector<int>(N), Vec::Constant(N, 0.5) } }
        });


        for(int iter = 0; iter < numIterations; ++iter)
        {
            for(const auto& [k, ints] : intervals)
            {
                for(const auto& v : ints)
                    handy::print(v.x.transpose());
                handy::print("\n");
            }
            handy::print("\n\n");

            auto potSet = potentialSet(intervals, best);

            auto bestIter = createSplits(scaledF, potSet, intervals);

            if(bestIter < best)
                best = std::move(bestIter);
        }

        return best.x;
    }

    std::vector<Interval> potentialSet (std::unordered_map<int, std::vector<Interval>>& intervals, const Interval& best)
    {
        std::vector<Interval> potSet;
        potSet.reserve(intervals.size());

        for(auto it = intervals.begin(); it != intervals.end();)
        {
            // if(it->second[0].fx -  <= best.fx - eps * std::abs(best.fx))
                potSet.emplace_back(std::move(pop_heap(it->second)));

            if(it->second.empty())
                intervals.erase(it++);

            else
                ++it;
        }

        return potSet;
    }
    
    template <class Func>
    Interval createSplits(const Func& func, const std::vector<Interval>& potSet, std::unordered_map<int, std::vector<Interval>>& intervals)
    {
        Interval best{1e8};

        for(auto& interval : potSet)
        {
            int smallestK = std::min_element(interval.k.begin(), interval.k.end()) - interval.k.begin();
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

                left.divisions = std::accumulate(left.k.begin(), left.k.end());
                right.divisions = std::accumulate(right.k.begin(), right.k.end());

                const auto& bestAxis = std::min(left, right);

                if(bestAxis < best)
                    best = bestAxis;

                push_heap(intervals[left.divisions], std::move(left));
                push_heap(intervals[right.divisions], std::move(right));
            }
        }

        return best;
    }


    double eps;
    int numIterations;
};