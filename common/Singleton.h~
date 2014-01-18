#ifndef _SINGLETON_H
#define _SINGLETON_H
#include <boost/noncopyable.hpp>

template<typename T>
class Singleton:boost::noncopyable
{
public:
    static T& Instance( void )
    {
        static T instance;
        return instance;
    }
};
#endif
