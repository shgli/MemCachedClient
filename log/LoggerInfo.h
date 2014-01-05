#ifndef _LOGGERINFO_H
#define _LOGGERINFO_H
#include <cstdint>
typedef std::vector<SinkInfo*> SinkInfoList;
struct LoggerInfo
{
    static const uint8_t LEVEL_BITS = 8;
    static const uint8_t MAX_LEVEL = 8;

    uint64_t Id;
    uint8_t  Level;
    logging::filter   Filter;
    SinkInfoList SinkInfos; 
    Logger*  Log;

    uint64_t NextId() const
    {
	uint64_t shift = LEVEL_BITS * (MAX_LEVEL - Level);
	uint64_t delta = 0;
	if(shift > 32)
	{
	    delta = 1UL << 32;
	    delta = delta << (shift - 32);
	}
	else
	{
	    delta = 1 << shift;
	}
        return Id + delta;
    }
};
#endif

