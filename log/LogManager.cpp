#include <algorithm>
#include <fstream>
#include <boost/xpressive/xpressive_dynamic.hpp>
#include <boost/log/expressions/predicates/is_in_range.hpp>
#include <boost/log/utility/setup/from_settings.hpp>
#include <boost/log/utility/setup/filter_parser.hpp>
#include <boost/algorithm/string/split.hpp>
#include "common/FileHandler.h"
#include  "LogManager.h"
using namespace boost::xpressive;

BOOST_LOG_ATTRIBUTE_KEYWORD(logger_id,"LoggerId",uint64_t)
LogManager::LogManager()
{
    memset(mIds,0,sizeof(mIds));
}

bool LogManager::Initialize(const std::string& strPath)
{
    PathVec configs;
    auto itLast = std::find_if(strPath.begin(),strPath.end(),[](char c){ return '*' == c || '?' == c;});
    fs::path fullPath = fs::system_complete(fs::path(std::string(strPath.begin(),itLast)));

    auto printError = [&strPath](){std::cerr <<__FILE__<<":"<<__LINE__<<":"<<strPath<<" is not a valid log configure file"<<std::endl;};
    if(!fs::exists(fullPath))
    {
       printError(); 
    }

    if(fs::is_regular_file(fullPath))
    {
	configs.push_back(fullPath);
    }
    else if(fs::is_directory(fullPath))
    {
	const auto& pattern = Wildcard2Regex(strPath);
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
    std::ifstream settingFile(path.string().c_str());
    if(settingFile.is_open())
    {
        auto setts = logging::parse_settings(settingFile);

	// Apply core settings
	if (section core_params = setts["Core"])
	    _apply_core_settings(core_params);

	// Construct and initialize sinks
	if (section sink_params = setts["Sinks"])
	{
	    for (typename section::const_iterator it = sink_params.begin(), end = sink_params.end(); it != end; ++it)
	    {
		section sink_params = *it;

		// Ignore empty sections as they are most likely individual parameters (which should not be here anyway)
		if (!sink_params.empty())
		{
		    auto pSinkInfo = mSinkInfoPool.construct(boost::static_pointer_cast<sinks::basic_sink_frontend>(construct_sink_from_settings(sink_params)));
		    auto filter = sink_params["Filter"].get();
		    if(filter)
		    {
		         pSinkInfo->Filter = logging::parse_filter(filter.get());
		    }
		    mSinks.insert(std::make_pair(it.get_name(),pSinkInfo));
		}
	    }

	    //std::for_each(new_sinks.begin(), new_sinks.end(), boost::bind(&core::add_sink, core::get(), _1));
	}

	if(section logger_parms = setts["Loggers"])
	{
	    LoadLogger(logger_parms,0);
	}
    }
    else
    {
	std::cerr << __FILE__ <<":"<<__LINE__<<":"<<"open log configure file failed."<<std::endl;
    }
}

void LogManager::LoadLogger(section& sec,int nLevel)
{
    for (typename section::const_iterator it = sec.begin(), end = sec.end(); it != end; ++it)
    {
	section logger_params = *it;

	if(!logger_params.empty())
	{
	    auto name = it.get_name();
	    LoggerInfo* pInfo = new LoggerInfo();
	    pInfo->Id = GetId(name,pInfo->Level);

	    auto opSinkNames = logger_params["Sinks"].get();
	    if(opSinkNames)
	    {
		std::vector<boost::iterator_range<std::string::iterator> > sinkNames;
		auto strSinks = opSinkNames.get();
		boost::split(sinkNames,strSinks,[](char c){ return ',' == c;},boost::token_compress_on);
		for(auto itSinkName : sinkNames)
		{
		    auto itSink = mSinks.find(std::string(itSinkName.begin(),itSinkName.end()));
		    if(itSink != mSinks.end())
		    {
			pInfo->SinkInfos.push_back(itSink->second);
		    }
		}
	    }

	    auto filter = logger_params["Filter"].get();
	    if(filter)
	    {
	        pInfo->Filter = logging::parse_filter(filter.get());
	    }
            mLoggerInfos.put<LoggerInfo*>(name,pInfo);
	}
    }
}

uint64_t LogManager::GetId(const std::string& name,uint8_t& level)
{
    using namespace std;
    uint64_t mulitple = (1UL << 32) << 24;
    uint64_t id;
    size_t nextDotPos = name.find_first_of('.');
    level = -1;
    do
    {
	++level;
	LoggerInfo* pInfo = mLoggerInfos.get<LoggerInfo*>(name.substr(0,nextDotPos));
	if(nullptr != pInfo)
	{
	    id = pInfo->Id;
	}
	else
	{
	    id += mIds[level] * mulitple;
	    ++mIds[level];
	}
        nextDotPos = name.find_first_of('.',nextDotPos + 1);
    }while(string::npos != nextDotPos);
   
    return id;
}

Logger& LogManager::GetLogger(const std::string& name)
{
    LoggerInfo* pInfo = GetLoggerInfo(name);
    if(nullptr != pInfo)
    {
        if(nullptr == pInfo->Log)
	{
	    pInfo->Log = mLoggerPool.construct();

	    for(auto pSink : pInfo->SinkInfos)
	    {
		pSink->AddFilter(expr::is_in_range(logger_id,pInfo->Id,pInfo->NextId()),pInfo->Filter);
	    }
	}

	return *pInfo->Log;
    }

    std::cerr << __FILE__ <<":"<<__LINE__<<":"<<"get logger "<<name << " failed."<<std::endl;
    throw std::runtime_error("get logger " + name + " failed");
}

LoggerInfo* LogManager::GetLoggerInfo(const std::string& name)
{
    using namespace std;
    size_t lastPos = name.size();
    while(string::npos != lastPos)
    {
	LoggerInfo* pInfo = mLoggerInfos.get<LoggerInfo*>(name.substr(0,lastPos));
	if(nullptr != pInfo)
	{
	    return pInfo;
	}
        lastPos = name.find_last_of('.');
    }

    return RootInfo();
}

LoggerInfo* LogManager::RootInfo( void )
{
    static LoggerInfo* rootInfo;
    if(nullptr == rootInfo)
    {
	rootInfo->Id = 0;
	rootInfo->Level = 0;
	rootInfo->Log = mLoggerPool.construct();
    }

    return rootInfo;
}


