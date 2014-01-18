#include "log/LogCommon.h"
// The operator puts a human-friendly representation of the severity level to the stream
static const char* strings[] =
{
    "INFO",
    "DEBUG",
    "WARN",
    "ERROR",
    "FATAL"
};

const char* to_string(SeverityLevel level)
{
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

std::istream& operator>> (std::istream& strm,SeverityLevel& level)
{
    std::string strLevel;
    strm >> strLevel;
    level = Undefined;
    for(int i = 0; i < sizeof(strings) / sizeof(*strings); ++i)
    {
	if(0 == strcmp(strLevel.c_str(),strings[i]))
	{
	    level = static_cast<SeverityLevel>(i);
	    break;
	}
    }

    return strm;
}

