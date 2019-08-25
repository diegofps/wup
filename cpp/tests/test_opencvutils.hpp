#ifndef TEST_OPENCVUTILS_HPP
#define TEST_OPENCVUTILS_HPP

#include <cxxtest/TestSuite.h>
#include <wup/wup.hpp>
#include <string>

using namespace wup;
using namespace std;

//WUP_STATICS;

class TestOpencvUtils : public CxxTest::TestSuite
{
public:

    TestOpencvUtils()
    {
        // This will be executed only once, when the test class is created

//        const uint8_t original2[] =
//        {
//            20,20,20,20,
//            20,20,20,20,
//            20,20,20,20,
//            20,20,20,20
//        };

//        const uint8_t original3[] =
//        {
//            30,30,30,30,
//            30,30,30,30,
//            30,30,30,30,
//            30,30,30,30
//        };

    }

    ~TestOpencvUtils()
    {
        // This will be executed only once, when the test class is destroyed
    }

    void
    setUp()
    {
        // This will be executed for each test in this test class, before the test
    }

    void
    tearDown()
    {
        // This will be executed for each test in this test class, after the test
    }

    void test_calculateImageIntegral()
    {
        uint8_t original1[] =
        {
            10,10,10,10,
            10,10,10,10,
            10,10,10,10,
            10,10,10,10
        };

        uint32_t expected[] =
        {
             10, 20, 30, 40,
             20, 40, 60, 80,
             30, 60, 90,120,
             40, 80,120,160
        };

        cv::Mat tmp(4, 4, CV_8UC1, original1);

        Bundle<uint32_t> ii;

        calculateImageIntegral(tmp, ii);

        TS_ASSERT_SAME_DATA(expected, ii.data(), ii.size() * sizeof(uint32_t));
    }

    void test_calculateImageIntegral3D()
    {
        uint8_t original1[] =
        {
            10,20,30, 10,20,30, 10,20,30, 10,20,30,
            10,20,30, 10,20,30, 10,20,30, 10,20,30,
            10,20,30, 10,20,30, 10,20,30, 10,20,30,
            10,20,30, 10,20,30, 10,20,30, 10,20,30
        };

        uint32_t target[] =
        {
            10,20, 30, 20, 40, 60,  30, 60, 90,  40, 80,120,
            20,40, 60, 40, 80,120,  60,120,180,  80,160,240,
            30,60, 90, 60,120,180,  90,180,270, 120,240,360,
            40,80,120, 80,160,240, 120,240,360, 160,320,480
        };

        cv::Mat tmp(4, 4, CV_8UC3, original1);
        Bundle3D<uint> ii;

        calculateImageIntegral3D(tmp, ii);

//        print();
//        for (int c=0;c!=3;++c)
//        {
//            print("CHANNEL", c);
//            uint * data = ii.data() + c;

//            for (int i=0;i!=4;++i)
//            {
//                for (int j=0;j!=4;++j)
//                {
//                    printn(*data, " ");
//                    data += 3;
//                }
//                print();
//            }
//            print();
//        }

        TS_ASSERT_SAME_DATA(target, ii.data(), ii.size() * sizeof(uint32_t));    }

//    void test_equality()
//    {
//        uint8_t original1[16];

//        for (int i=0;i!=16;++i)
//            original1[i] = rand() % 256;

//        cv::Mat tmp(4, 4, CV_8UC1, original1);
//        Bundle<uint32_t> ii1;
//        Bundle<uint32_t> ii2;
//        Bundle<uint32_t> ii3;
//        Bundle<uint32_t> ii4;
//        Bundle<int32_t> ii5;
//        Bundle<uint32_t> ii6;

//        print();
//        Clock c;

//        calculateImageIntegral(tmp, ii1);
//        calculateImageIntegral2(tmp, ii2);
//        calculateImageIntegral3(tmp, ii3);
//        calculateImageIntegral4(tmp, ii4);
//        calculateImageIntegral5(tmp, ii5);
////        calculateImageIntegral6(tmp, ii6);


//        calculateImageIntegral3(tmp, ii3); c.start();
//        for (int i=0;i!=1000;++i)
//            calculateImageIntegral3(tmp, ii3);
//        c.lap_micro("Time for image integral 3");

//        calculateImageIntegral3(tmp, ii3); c.start();
//        for (int i=0;i!=1000;++i)
//            calculateImageIntegral(tmp, ii1);
//        c.lap_micro("Time for image integral 1");

//        calculateImageIntegral3(tmp, ii3); c.start();
//        for (int i=0;i!=1000;++i)
//            calculateImageIntegral2(tmp, ii2);
//        c.lap_micro("Time for image integral 2");

//        calculateImageIntegral3(tmp, ii3); c.start();
//        for (int i=0;i!=1000;++i)
//            calculateImageIntegral4(tmp, ii4);
//        c.lap_micro("Time for image integral 4");

//        calculateImageIntegral3(tmp, ii3); c.start();
//        for (int i=0;i!=1000;++i)
//            calculateImageIntegral5(tmp, ii5);
//        c.lap_micro("Time for image integral 5");

//        calculateImageIntegral3(tmp, ii3); c.start();
//        for (int i=0;i!=1000;++i)
//            calculateImageIntegral6(tmp, ii6);
//        c.lap_micro("Time for image integral 6");

//        print();

////        calculateImageIntegral3(tmp, ii3);
////        c.lap_micro("Time for image integral 3");
//    }

private:

    uint8_t *
    createBuffer(uint64_t size)
    {
        uint8_t * tmp = new uint8_t[size];
        for (uint64_t i=0;i!=size;++i)
            tmp[i] = i % 256;
        return tmp;
    }

    uint8_t *
    createRandomBuffer(uint64_t size)
    {
        uint8_t * tmp = new uint8_t[size];
        for (uint64_t i=0;i!=size;++i)
            tmp[i] = rand() % 256;
        return tmp;
    }

};

#endif // TEST_OPENCVUTILS_HPP
