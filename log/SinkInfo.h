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

    void AddFilter(logging::filter filt1,logging::filter filt2)
    {
	auto oldFilter = Filter;
	auto newFilter = [oldFilter,filt1,filt2](logging::attribute_value_set const& values)
	{
	    return oldFilter(values) || (filt1(values) && filt2(values));
	};

	Filter = newFilter;
	Sink->set_filter(Filter);
    }
};
#endif

