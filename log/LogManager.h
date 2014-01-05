#ifndef _LOGMANAGER_H
#define _LOGMANAGER_H
#include <boost/property_tree/ptree.hpp>
#include <boost/pool/object_pool.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/log/utility/setup/settings_parser.hpp>
#include "common/Singleton.h"
#include "LogCommon.h"
#include "SinkInfo.h"
#include "LoggerInfo.h"

namespace pt = boost::property_tree;
namespace fs = boost::filesystem;
class LogManager:
    public Singleton<LogManager>
{
public:
    typedef logging::basic_settings_section< char> section;
    bool Initialize(const std::string& path); //support wildcard
    Logger& GetLogger(const std::string& name);
private:
    void LoadConfig(const fs::path& path);
    void LoadLogger(section& sec,int nLevel);
    LoggerInfo* GetLoggerInfo(const std::string& name);
    LoggerInfo* RootInfo( void );

    typedef pt::basic_ptree<std::string,LoggerInfo*> LoggerInfoTree;

    LoggerInfoTree mLoggerInfos;
    boost::unordered_map<std::string,SinkInfo*> mSinks;

    boost::object_pool<LoggerInfo> mLoggerInfoPool;
    boost::object_pool<SinkInfo> mSinkInfoPool;
    boost::object_pool<Logger> mLoggerPool;
};

#endif
 
