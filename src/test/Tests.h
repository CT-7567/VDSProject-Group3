//
// Created by tobias on 21.11.16.
//

#ifndef VDSPROJECT_TESTS_H
#define VDSPROJECT_TESTS_H

#include <gtest/gtest.h>
#include "../Manager.h"

class TestFixture : public ::testing::Test {
protected:
    TestFixture() {

    }

    ClassProject::Manager manager;
};




TEST(BDDTests, ExampleTest)
{
    EXPECT_EQ(1, 1);
}




TEST_F(TestFixture, ExampleTestFixture)
{
    
}

#endif
