#include <gtest/gtest.h>
#include <iostream>
using namespace std;

TEST(SampleTest, HandlesIntegerInput) {
    EXPECT_EQ(1, 1);
    EXPECT_NE(1, 2);
    cout<<"Hola desde el test"<<endl;
}