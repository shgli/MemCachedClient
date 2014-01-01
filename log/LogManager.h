#ifndef _LOGMANAGER_H
#define _LOGMANAGER_H
#include <boost/log/core/core.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/xpressive/xpressive_dynamic.hpp>
#include <boost/unordered/unordered_map.hpp>
#include "Singleton.h"
namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace attrs = boost::log::attributes;
namespace keywords = boost::log::keywords;
namespace fs = boost::filesystem;
enum SeverityLevel
{
    Info,
    Debug,
    Warn,
    Error,
    Fatal
};

typedef Src::severity_logger_mt<SeverityLevel> Logger;

class LogManager:
    public Singleton<LogManager>
{
public:
    bool Initialize(const std::string& path); //support wildcard
    Logger& GetLogger(const std::string& name);
    Logger& GetLogger(const std::string& module,const std::string& name);

private:
    void EscapeRegex(std::string &regex);
    boost::xpressive::sregex Wildcard2Regex(const std::string& wildcardPattern, bool caseSensitive = true);
    void LoadConfig(const fs::path& path);

    boost::unordered_map<std::string,Logger> mLoggers;
};
#endif
 
