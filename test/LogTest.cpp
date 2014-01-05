#include "log/LogManager.h"

int main(int argc,char** argv)
{
    LogManager::Instance().Initialize("conf/*_log.conf");

    auto lg1 = LogManager::Instance().GetLogger("Feeder.FeedSource");

    DEBUG(lg1) << "Connected ";

    auto lg2 = LogManager::Instance().GetLogger("Feeder.Manager");
    WARN(lg2) << "IF1302 Added";

    auto lg3 = LogManager::Instance().GetLogger("Indicator.EWMA");
    ERROR(lg3) << "Update value 2 2.0";

    return 0;
}

