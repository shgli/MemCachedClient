#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE MemcachedTest
#include <boost/test/unit_test.hpp>
#include "SharedBuffer.h"

BOOST_AUTO_TEST_SUITE(SharedBufferTest)
    BOOST_AUTO_TEST_CASE(Test1)
    {
        SharedBuffer t(new char[12],12,[](const void* p)
                       {
                           std::cout<<"OK"<<std::endl;
                           delete [](const char*)p;
                       });
    }
BOOST_AUTO_TEST_SUITE_END()


