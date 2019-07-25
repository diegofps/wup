#ifndef TEST_GENERIC_SLICES_HPP
#define TEST_GENERIC_SLICES_HPP

#include <cxxtest/TestSuite.h>
#include <wup/wup.hpp>
#include <string>

using namespace wup;
using namespace std;

WUP_STATICS;

class TestGeneric : public CxxTest::TestSuite
{
    string tmp;
    string tmp2;

public:

    TestGeneric()
    {
        // This will be executed only once, when the test class is created
        tmp = "Diego Souza";
    }

    ~TestGeneric()
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

    void test_rotl_32bits()
    {
        const uint32_t first32 = 1;
        const uint32_t last32 = UINT32_MAX - (UINT32_MAX>>1);

        uint32_t a = first32;

        uint32_t b1 = rotl(a,1);
        uint32_t b2 = rotr(a,1);

        uint32_t c1 = rotl32(a,1);
        uint32_t c2 = rotr32(a,1);


        TS_ASSERT_EQUALS(b1, c1);
        TS_ASSERT_EQUALS(b2, c2);

        TS_ASSERT_EQUALS(b2, last32);
        TS_ASSERT_EQUALS(b1, 2);
    }

    void test_rotl_64bits()
    {
        const uint64_t first64 = 1;
        const uint64_t last64 = UINT64_MAX - (UINT64_MAX>>1);

        TS_ASSERT_EQUALS(rotl(first64,1), (first64<<1));
        TS_ASSERT_EQUALS(rotr(first64,1), last64);

        TS_ASSERT_EQUALS(rotl(last64,1), first64);
        TS_ASSERT_EQUALS(rotr(last64,1), (last64>>1));
    }

    void test_range()
    {
        vector<uint> goal = {0, 1, 2, 3, 4, 5};
        uint * r = range(6);
        TS_ASSERT_SAME_DATA(goal.data(), r, goal.size() * sizeof(uint));
    }

    void test_range2D()
    {
        vector<int> goal = {0, 1, 4, 5, 8, 9};
        uint * r = range2D(3, 2, 4);
        TS_ASSERT_SAME_DATA(goal.data(), r, goal.size() * sizeof(uint));
    }

    void test_range3DPlane()
    {
        vector<int> goal = {0, 1, 4, 5, 8, 9, 16, 17, 20, 21, 24, 25};
        uint * r = range3DPlane(3, 2, 2, 4, 16);
        TS_ASSERT_SAME_DATA(goal.data(), r, goal.size() * sizeof(uint));
    }

    void test_range3DCell()
    {
        vector<int> goal = {0, 1, 2, 3, 4, 5, 6, 7};
        uint * r = range3DCell(4, 2, 2, 2 * 2);
        TS_ASSERT_SAME_DATA(goal.data(), r, goal.size() * sizeof(uint));

        vector<int> goal2 = {0, 1, 4, 5};
        uint * r2 = range3DCell(4, 1, 2, 2 * 2);
        TS_ASSERT_SAME_DATA(goal2.data(), r2, goal2.size() * sizeof(uint));
    }

};

#endif // TEST_GENERIC_SLICES_HPP
