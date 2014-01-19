#include "log/LogManager.h"
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/trivial.hpp>
int main(int argc,char** argv)
{
    logging::add_common_attributes();

    ConfigLog("conf/*_log.conf");

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


    auto lg3 = LogManager::Instance().GetLogger("Indicator.EWMA");
    INFO(lg3) << "EWMA INFO";
    DEBUG(lg3) <<"EWMA debug";
    WARN(lg3) <<" EWMA warn";
    ERROR(lg3) <<"EWMA error";
    FATAL(lg3) <<"EWMA fatal";

    return 0;
}

