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

    //ClassProject::M anager manager;
};

TEST(createVarTest, IDTeset)
{
    ClassProject::Manager table = ClassProject::Manager();
    const std::string test =  "a";
    ClassProject::BDD_ID test_ID = table.createVar(test);
    EXPECT_EQ(test_ID, 2);
    const std::string test2 = "b";
    ClassProject::BDD_ID test_ID2 = table.createVar(test2);
    EXPECT_EQ(test_ID2, 3);

}


TEST(BDDTests, ExampleTest)
{
    EXPECT_EQ(1, 1);
}




TEST_F(TestFixture, ExampleTestFixture)
{

}

#endif
