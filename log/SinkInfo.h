#ifndef _SINKINFO_H
#define _SINKINFO_H
#include <boost/log/sinks/sink.hpp>
#include <boost/log/sinks/basic_sink_frontend.hpp>
typedef boost::shared_ptr<sinks::basic_sink_frontend> SinkPtr;
struct SinkInfo
{
    logging::filter Filter;
    SinkPtr Sink;

    SinkInfo(SinkPtr sin)
	:Sink(sin)
    {}

    void AddFilter(logging::filter filt)
    {
	//Filter = []() { return filt };
	Sink->set_filter(filt);
    }
};
#endif

