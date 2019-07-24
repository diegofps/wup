#ifndef TEST_ZIP_HPP
#define TEST_ZIP_HPP

#include <cxxtest/TestSuite.h>
#include <wup/wup.hpp>
#include <string>

using namespace wup;
using namespace std;

//WUP_STATICS;

class TestZip : public CxxTest::TestSuite
{
    string tmp;
    string tmp2;

public:

    TestZip()
    {
        // This will be executed only once, when the test class is created
        tmp = "Diego Souza";
    }

    ~TestZip()
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

    void test_zip()
    {
        const char * filename = "./tmp.bin";

        uint64_t LENGTH = 102400;
        uint8_t * buffer = createBuffer(LENGTH);

        {
            uint64_t compressedSize = 0;
            uint8_t * compressedBuffer = nullptr;

            zip(buffer, LENGTH, compressedBuffer, compressedSize);
            saveBytes(filename, compressedBuffer, compressedSize);

            //print("RealSize: ", LENGTH);
            //print("CompressedSize: ", compressedSize);

            delete [] compressedBuffer;
        }

        {
            uint64_t compressedSize = 0;
            uint8_t * compressedBuffer = nullptr;
            loadBytes(filename, compressedBuffer, compressedSize);

            uint64_t uncompressedSize = 0;
            uint8_t * uncompressedBuffer = nullptr;
            unzip(compressedBuffer, compressedSize, uncompressedBuffer, uncompressedSize);

            //print("UncompressedSize: ", uncompressedSize);
            TS_ASSERT_SAME_DATA(buffer, uncompressedBuffer, LENGTH);

            delete [] uncompressedBuffer;
        }

        delete [] buffer;
    }

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

#endif // TEST_ZIP
