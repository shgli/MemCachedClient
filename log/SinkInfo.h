#ifndef _SINKINFO_H
#define _SINKINFO_H
#include <boost/log/sinks/sink.hpp>
#include <boost/log/sinks/basic_sink_frontend.hpp>
typedef boost::shared_ptr<sinks::basic_sink_frontend> SinkPtr;
struct SinkInfo
{
    logging::filter ReDirectFilter;
    logging::filter InitFilter;
    SinkPtr Sink;
    std::string Name;

    SinkInfo(const std::string& name,SinkPtr sin)
        :ReDirectFilter([](logging::attribute_value_set const& values){return false;})
        ,Sink(sin)
        ,Name(name)
    {}

    void AddFilter(logging::filter filt1,logging::filter filt2)
    {
        auto oldFilter = ReDirectFilter;
        ReDirectFilter = [oldFilter,filt1,filt2](logging::attribute_value_set const& values)
        {
            return (filt1(values) && filt2(values)) || oldFilter(values);
        };
    }

    bool FiltFun(logging::attribute_value_set const& values)
    {
        return InitFilter(values) && ReDirectFilter(values);
    }
};
#endif

