#ifndef _MEMCACHED_CLIENT_BUFFER_H
#define _MEMCACHED_CLIENT_BUFFER_H

struct Buffer
{
public:
    virtual ~Buffer(){}

    typedef const_buffer value_type;
    typedef const boost::asio::const_buffer* const_iterator;
    virtual const_iterator begin() = 0;
    virtual const_iterator end() = 0;
};
#endif

