#ifndef _SERVERMANAGER_INTERNAL_MACROS_
#define _SERVERMANAGER_INTERNAL_MACROS_

#ifdef _WIN32
#ifdef BUILDING_SERVERMGR
#define SERVERMGR_EXPORT __declspec(dllexport)
#else
#define SERVERMGR_EXPORT __declspec(dllimport)
#endif
#else
#define SERVERMGR_EXPORT 
#endif
#endif
