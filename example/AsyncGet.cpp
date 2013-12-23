#include <boost/asio/io_service.hpp>
#include <boost/program_options.hpp>
#include "MemcachedClient.h"
#include "ConsistentHashDistri.h"
#include <thread>

using namespace boost::asio;
using namespace boost::program_options;
int main(int argc,char** argv)
{
    options_description opts("dataFormatChecker options");
    opts.add_options()
	("help","print this help information.")
	("host,h",value<std::string>(),"host name")
	("port,p",value<int>(),"port.")
	("key,k",value<int>(),"key"); 

    variables_map vm;
    store(parse_command_line(argc,argv,opts),vm);

    if(!vm.count("key") || vm.count("help"))
    {
	std::cout<<opts<<std::endl;
	exit(-1);
    }

    std::string host = "localhost";
    if(vm.count("host"))
    {
	host = vm["host"].as<std::string>();
    }

    int port = 11211;
    if(vm.count("port"))
    {
	port = vm["port"].as<int>();
    }

    std::string key = vm["key"].as<std::string>();

    io_service io;
    MemcachedClient client(io);
    client.Servers.SetDistributeAlgorithm(new ConsistentHashDistri());
    client.Servers.Add(host,port,io);
    
    auto result = client.Get(key,[&](const MemResult::Ptr& result)
	    {
	        if(ERequest_SUCCESS == result->ErrorCode())
		{
		    std::cout<<result->Value().ToString()<<std::endl;
		}
		else
		{
		    std::cout<<"Failed to get: "<<key<<std::endl;
		}
	    });
    io.run();

    return 0;
}

