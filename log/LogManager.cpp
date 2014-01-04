#include <algorithm>
#include <boost/xpressive/xpressive_dynamic.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "FileHandler.h"
#include  "LogManager.h"
using namespace boost::xpressive;

bool LogManager::Initialize(const std::string& strPath,bool ignoreCase)
{
    PathVec configs;
    auto itLast = std::find_if(strPath.begin(),strPath.end(),[](char c){ return '*' == c || '?' == c;});
    fs::path fullPath = fs::system_complete(fs::path(std::string(strPath.begin(),itLast)));

    auto printError = [](){std::cerr <<__FILE__<<":"<<__LINE__<<":"<<strPath<<"is not a valid log configure file"<<std::endl;}
    if(!fs::exists(fullPath))
    {
       printError(); 
    }

    if(fs::is_regular_file(fullPath))
    {
	configs.push_back(path);
    }
    else if(fs::is_directory(fullPath))
    {
	const auto& pattern = Wildcard2Regex(path);
	FindFiles(fullPath,pattern,configs);

	if(0 == configs.size())
	{
	    printError();
	}
	else
	{
	    for(auto config : configs)
	    {
		LoadConfig(config);
	    }
	}
    }
    else
    {
	printError();
    }
}

void LogManager::LoadConfig(const fs::path& path)
{
    using boost::property_tree::ptree;
    ptree pt;

    read_xml(path,pt);
}

Logger& LogManager::GetLogger(const std::string& name)
{
    return GetLogger("",name);
}

Logger& LogManager::GetLogger(const std::string& module,const std::string& name)
{
}

