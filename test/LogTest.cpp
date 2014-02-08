#include "log/LogManager.h"
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/trivial.hpp>
int main(int argc,char** argv)
{
    logging::add_common_attributes();

#ifdef _WIN32
    ConfigLog("conf\\*_log.conf");
#else
    ConfigLog("conf/*_log.conf");
#endif
    auto lg1 = GetLogger("Feeder.FeedSource");
    INFO(lg1) << "FeedSource INFO";
    DEBUG(lg1) <<"FeedSource debug";
    WARN(lg1) <<" FeedSource warn";
    ERROR(lg1) <<"FeedSource error";
    FATAL(lg1) <<"FeedSource fatal";

    auto lg2 = LogManager::Instance().GetLogger("Manager");
    INFO(lg2) << "Manager INFO";
    DEBUG(lg2) <<"Manager debug";
    WARN(lg2) <<" Manager warn";
    ERROR(lg2) <<"Manager error";
    FATAL(lg2) <<"Manager fatal";


    auto lg3 = LogManager::Instance().GetLogger("Core.Indicator.EWMA");
    INFO(lg3) << "EWMA INFO";
    DEBUG(lg3) <<"EWMA debug";
    WARN(lg3) <<" EWMA warn";
    ERROR(lg3) <<"EWMA error";
    FATAL(lg3) <<"EWMA fatal";

    auto lg4 = LogManager::Instance().GetLogger("Manager");
    FATAL(lg4) <<"It's ok"<<std::endl;

    return 0;
}

