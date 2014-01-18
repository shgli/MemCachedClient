#include "LogCommon.h"
// The operator puts a human-friendly representation of the severity level to the stream
const char* to_string(SeverityLevel level)
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
	return strings[level];
    else
	return "";
}

std::ostream& operator<< (std::ostream& strm, SeverityLevel level)
{
    const char* str = to_string(level);
    if (str)
        strm << str;
    else
        strm << static_cast< int >(level);
    return strm;
}
