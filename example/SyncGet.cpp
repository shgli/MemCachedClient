#include <boost/asio/io_service.hpp>
#include <boost/make_shared.hpp>
#include "memcachedClient/MemcachedClient.h"
#include "ConsistentHashDistri.h"
#include <boost/thread.hpp>

using namespace boost::asio;

int main(int argc,char** argv)
{
    io_service io;
    MemcachedClient client(io);
    client.Servers.SetDistributeAlgorithm(boost::make_shared<ConsistentHashDistri>());
    client.Servers.Add(std::string("127.0.0.1"),11211,io);

    std::string value;
    value.resize(13);
    auto result = client.Get("Test",Buffer((void*)value.c_str(),13));

    boost::thread t([&io]{io.run();});
    if(ERequest_SUCCESS == result->Finish())
    {
        std::cout<<value<<std::endl;
    }
    else
    {
        std::cout<<"get failed"<<std::endl;
    }

    t.join();

    return 0;
}

