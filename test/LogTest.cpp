#include "log/LogManager.h"

int main(int argc,char** argv)
{
    LogManager::Instance().Initialize("conf/*_log.conf");

    auto lg1 = LogManager::Instance().GetLogger("Feeder.FeedSource");
    INFO(lg1) << "lg1 INFO";
    DEBUG(lg1) << "lg1 debug";
    WARN(lg1) <<" lg1 warn";
    ERROR(lg1) <<"lg1 error";
    FATAL(lg1) <<"lg1 fatal";

    auto lg2 = LogManager::Instance().GetLogger("Feeder.Manager");
    INFO(lg2) << "lg2 INFO";
    DEBUG(lg2) << "lg2 debug";
    WARN(lg2) <<" lg2 warn";
    ERROR(lg2) <<"lg2 error";
    FATAL(lg2) <<"lg2 fatal";


    auto lg3 = LogManager::Instance().GetLogger("Indicator.EWMA");
    INFO(lg3) << "lg3 INFO";
    DEBUG(lg3) << "lg3 debug";
    WARN(lg3) <<" lg3 warn";
    ERROR(lg3) <<"lg3 error";
    FATAL(lg3) <<"lg3 fatal";

    return 0;
}

