#ifndef _LOGCOMMON_H
#define _LOGCOMMON_H
#include <ostream>
#include <boost/log/core/core.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/xpressive/xpressive_dynamic.hpp>
#include <boost/unordered/unordered_map.hpp>
namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace attrs = boost::log::attributes;
namespace keywords = boost::log::keywords;

enum SeverityLevel
{
    Info,
    Debug,
    Warn,
    Error,
    Fatal
};

typedef src::severity_logger_mt<SeverityLevel> Logger;
std::ostream& operator<< (std::ostream& strm, SeverityLevel level);

#define INFO(lg) BOOST_LOG_SEV(lg,Info)
#define DEBUG(lg) BOOST_LOG_SEV(lg,Debug)
#define WARN(lg) BOOST_LOG_SEV(lg,Warn)
#define ERROR(lg) BOOST_LOG_SEV(lg,Error)
#define FATAL(lg) BOOST_LOG_SEV(lg,Fatal)

#endif

