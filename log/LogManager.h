#ifndef _LOGMANAGER_H
#define _LOGMANAGER_H
#include <boost/property_tree/ptree.hpp>
#include <boost/pool/object_pool.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/log/utility/setup/settings_parser.hpp>
#ifdef BUILDING_LOG
#define _EXPORT_SINGLETON
#endif
#include "common/Singleton.h"
#undef _EXPORT_SINGLETON
#include "log/LogCommon.h"
#include "log/SinkInfo.h"
#include "log/LoggerInfo.h"

namespace pt = boost::property_tree;
namespace fs = boost::filesystem;

class LogManager:
    public Singleton<LogManager>
{
public:
    typedef logging::basic_settings_section< char> section;
    LOG_EXPORT bool Initialize(const std::string& path); //support wildcard
    LOG_EXPORT Logger& GetLogger(const std::string& name);

    LOG_EXPORT ~LogManager();
private:
    friend class Singleton<LogManager>;
    LOG_EXPORT LogManager();

    void LoadConfig(const fs::path& path,std::vector<LoggerInfo*>& loggers);
    void LoadSink(section::reference& rSection,const std::string& strName);
    void LoadLogger(section::reference& sec,uint8_t nLevel,std::vector<LoggerInfo*>& loggers,const std::string& name,uint64_t parentId);
    uint64_t GetId(const std::string& name,uint8_t level,uint64_t parentId);
    void LinkLoggerWithSink(std::vector<LoggerInfo*>& loggers);

    LoggerInfo* GetLoggerInfo(const std::string& name);
    LoggerInfo* RootInfo( void );


    boost::unordered_map<std::string,LoggerInfo*> mLoggerInfos;
    boost::unordered_map<std::string,SinkInfo*> mSinks;
    boost::unordered_map<std::string,uint64_t> mIdMap;

    boost::object_pool<LoggerInfo> mLoggerInfoPool;
    boost::object_pool<SinkInfo> mSinkInfoPool;
    boost::object_pool<Logger> mLoggerPool;
    uint32_t mIds[LoggerInfo::MAX_LEVEL];
    uint64_t mIdMultiple[LoggerInfo::MAX_LEVEL];
    uint32_t mFileId;

    LoggerInfo* mRootInfo;
    boost::once_flag mRootOnceFlag;

    boost::mutex mMutex;
};

LOG_EXPORT void ConfigLog(const std::string& path);
LOG_EXPORT Logger& GetLogger(const std::string& name);

#endif
 
