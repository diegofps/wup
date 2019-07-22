#ifndef TEST_SBIO_INTREADER_AND_INTWRITER_HPP
#define TEST_SBIO_INTREADER_AND_INTWRITER_HPP

#include <cxxtest/TestSuite.h>
#include <wup/wup.hpp>
#include <string>

using namespace wup;
using namespace std;

//WUP_STATICS;

class TestSbioIntReaderAndIntWriter : public CxxTest::TestSuite
{
    const int numBytes = 991;
    const int numLongs = 123;

    int      a = 10;
    uint32_t b = 30;
    uint64_t c = 90;
    int32_t  d = 100;
    int64_t  e = 150;
    string   f = "Hello World!!!";
    bool     g = false;
    bool     h = true;

    uint8_t * dataBytes = new uint8_t[numBytes];
    int64_t * dataLongs = new int64_t[numLongs];

public:

    TestSbioIntReaderAndIntWriter()
    {
        // This will be executed only once, when the test class is created

        for (int i=0;i!=numBytes;++i)
            dataBytes[i] = i % 256;

        for (int i=0;i!=numLongs;++i)
        {
            int64_t tmp = 0;
            for (int k=i;k!=i+8;++k)
                tmp = (tmp << 8) | k;
            dataLongs[i] = tmp;
        }
    }

    ~TestSbioIntReaderAndIntWriter()
    {
        // This will be executed only once, when the test class is destroyed
        delete [] dataBytes;
        delete [] dataLongs;
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

public: // TESTS

    void test_memory()
    {
        vector<int32_t> buffer;

        {
            VectorSink<int32_t> snk(buffer);
            IntWriter writer(snk);
            writeAll(writer);
        }

        {
            MemSource<int32_t> src(buffer.data(), buffer.size());
            IntReader reader(src);
            readAndValidateAll(reader);
        }
    }

    void test_disk()
    {
        const char * filename = "./tmp";

        {
            FileSink<int32_t> snk(filename);
            IntWriter writer(snk);
            writeAll(writer);
        }

        {
            FileSource<int32_t> src(filename);
            IntReader reader(src);
            readAndValidateAll(reader);
        }
    }

private:

    void
    writeAll(IntWriter & writer)
    {
        writer.put(a);
        writer.putUInt(b);
        writer.putUInt64(c);
        writer.putInt32(d);
        writer.putInt64(e);
        writer.putString(f);
        writer.putBool(g);
        writer.putBool(h);

        writer.putBytes(dataBytes, numBytes);
        writer.putBytes(dataLongs, numLongs);
    }

    void
    readAndValidateAll(IntReader & reader)
    {
        int      a2 = reader.get();
        uint32_t b2 = reader.getUInt32();
        uint64_t c2 = reader.getUInt64();
        int32_t  d2 = reader.getInt32();
        int64_t  e2 = reader.getInt64();
        string   f2 = reader.getString();
        bool     g2 = reader.getBool();
        bool     h2 = reader.getBool();

        uint8_t * dataBytes2 = (uint8_t*) reader.getBytes();
        int64_t * dataLongs2 = (int64_t*) reader.getBytes();

        TS_ASSERT_EQUALS(a, a2);
        TS_ASSERT_EQUALS(b, b2);
        TS_ASSERT_EQUALS(c, c2);
        TS_ASSERT_EQUALS(d, d2);
        TS_ASSERT_EQUALS(e, e2);
        TS_ASSERT_EQUALS(f, f2);
        TS_ASSERT_EQUALS(g, g2);
        TS_ASSERT_EQUALS(h, h2);

        TS_ASSERT_SAME_DATA(dataBytes, dataBytes2, numBytes);
        TS_ASSERT_SAME_DATA(dataLongs, dataLongs2, numLongs);
    }

};

#endif // TEST_SBIO_INTREADER_AND_INTWRITER_HPP
