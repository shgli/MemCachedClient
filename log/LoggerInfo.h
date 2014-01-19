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
    uint32_t FileId;
    logging::filter   Filter;
    std::string SinkNames;
    SinkInfoList SinkInfos; 
    Logger*  Log;

    LoggerInfo()
        :Id(0)
        ,Level(0)
        ,FileId(0)
        ,Log(nullptr)
    {

    }

    uint64_t NextId() const
    {
        uint64_t shift = LEVEL_BITS * (MAX_LEVEL - Level - 1);
        uint64_t delta = ((uint64_t)1) << shift;

        return Id + delta;
    }
};
#endif

