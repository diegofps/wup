#ifndef TEST_GENERIC_SLICES_HPP
#define TEST_GENERIC_SLICES_HPP

#include <cxxtest/TestSuite.h>
#include <wup/wup.hpp>
#include <string>

using namespace wup;
using namespace std;

WUP_STATICS;

class TestGenericSlices : public CxxTest::TestSuite
{
    string tmp;
    string tmp2;

public:

    TestGenericSlices()
    {
        // This will be executed only once, when the test class is created
        tmp = "Diego Souza";
    }

    ~TestGenericSlices()
    {
        // This will be executed only once, when the test class is destroyed
    }

    void
    setUp()
    {
        // This will be executed for each test in this test class, before the test
        tmp2 = "Diego Souza";
    }

    void
    tearDown()
    {
        // This will be executed for each test in this test class, after the test
    }

    void test_slice()
    {
        TS_ASSERT_EQUALS("Diego", slice(tmp, 0, 5));
        TS_ASSERT_EQUALS("Diego", slice(tmp, 0, -6));
        TS_ASSERT_EQUALS("Souza", slice(tmp, 6, 11));
        TS_ASSERT_EQUALS("Souza", slice(tmp, 6, 20));
    }

    void test_slice_from()
    {
        TS_ASSERT_EQUALS("Souza", slice_from(tmp, 6));
        TS_ASSERT_EQUALS("Souza", slice_from(tmp, -5));
    }

    void test_slice_to()
    {
        TS_ASSERT_EQUALS("Diego", slice_to(tmp, 5));
        TS_ASSERT_EQUALS("Diego", slice_to(tmp, -6));
    }

};

#endif // TEST_GENERIC_SLICES_HPP
