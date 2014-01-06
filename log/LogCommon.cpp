#include "LogCommon.h"
// The operator puts a human-friendly representation of the severity level to the stream
std::ostream& operator<< (std::ostream& strm, SeverityLevel level)
{
    static const char* strings[] =
    {
	"INFO",
	"DEBUG",
	"WARN",
	"ERROR",
	"FATAL"
    };

    if (static_cast< std::size_t >(level) < sizeof(strings) / sizeof(*strings))
	strm << strings[level];
    else
	strm << static_cast< int >(level);

    return strm;
}
