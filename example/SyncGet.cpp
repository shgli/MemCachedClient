#include <boost/asio/io_service.hpp>
#include "MemcachedClient.h"
#include "ConsistentHashDistri.h"
#include <thread>

using namespace boost::asio;

int main(int argc,char** argv)
{
    io_service io;
    MemcachedClient client(io);
    client.Servers.SetDistributeAlgorithm(new ConsistentHashDistri());
    client.Servers.Add(std::string("127.0.0.1"),11211,io);
    
    std::string value;
    value.resize(13);
    auto result = client.Get("Test",Buffer((void*)value.c_str(),13));

    std::thread t([&io]{io.run();});
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

