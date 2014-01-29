#ifndef _SINGLETON_H
#define _SINGLETON_H
#include <boost/noncopyable.hpp>
#include <boost/thread/once.hpp>

#ifdef _WIN32
    #ifdef  _EXPORT_SINGLETON
        #define SINGLETON_EXPORT __declspec(dllexport)
    #else
        #define SINGLETON_EXPORT __declspec(dllimport)
    #endif
#else
    #define SINGLETON_EXPORT
#endif

template<typename T>
class Singleton:boost::noncopyable
{
public:
    SINGLETON_EXPORT static T& Instance( void )
    {
        static boost::shared_ptr<T> gpInstance;
        static boost::once_flag init_instance = BOOST_ONCE_INIT;
        boost::call_once(init_instance,[](){gpInstance.reset(new T());});
        return *gpInstance;
    }
};

#endif
