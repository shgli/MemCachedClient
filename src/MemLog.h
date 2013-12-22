#ifndef _MEMLOG_H
#define _MEMLOG_H
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/severity_logger.hpp>
namespace Logging = boost::log;
namespace Sinks = boost::log::sinks;
namespace Src = boost::log::sources;
namespace Attrs = boost::log::attributes;
namespace Keyworks = boost::log::keywords;

enum SeverityLevel
{
    Info,
    Debug,
    Warn,
    Error,
    Fatal
};

typedef Src::severity_logger_mt<SeverityLevel> MemLog;

#define MEMLOG(lg,level) BOOST_LOG_SEV(lg,level)

#endif

