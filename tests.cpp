#include <iostream>
#include <Eigen/Dense>

#include "Direct.h"
#include "Functions.h"

#include "gtest/gtest.h"


namespace
{

struct DirectTest : public ::testing::Test
{
    virtual ~DirectTest() {}

    virtual void SetUp ()
    {
        direct = Direct(eps, numIterations);
    }
    
    Direct direct;
    double eps = 1e-4;
    int numIterations = 1000;
};


TEST_F(DirectTest, ConvexHullTest)
{
}

} // namespace