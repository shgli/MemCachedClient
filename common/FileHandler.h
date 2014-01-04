/**
* @file FileHandler.h
* @brief 
* @author Li Shugan
* @date 2011-10-28 16:19:10
* @version 1.0
* @copyright: fenge.com
* @email: li_shugan@126.com 
* All rights reserved.
*/
#ifndef _FILEHANDLER_H
#define _FILEHANDLER_H

#include <iterator>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/function.hpp>
#include <boost/xpressive/xpressive_dynamic.hpp>
namespace fs = boost::filesystem;
namespace re = boost::xpressive;

typedef boost::function<bool (const fs::path& path,bool isDir)> Filter;
typedef std::vector<fs::path> PathVec;

sregex Wildcard2Regex(const std::string& wildcardPattern);
void FindFiles(const fs::path& dirPath,const Filter& filter,PathVec& out);
void FindFiles(const fs::path& dirPath,const std::string pattern,PathVec& out);
void FindFiles(const fs::path& dirPath,const sregex& pattern,PathVec& out);

template<typename Visit>
void TraverseFile(const fs::path& filePath,Visit visit);

void FindAndReplace(const fs::path& filePath,const std::string& strFrom,const std::string& strTo);

void FindAndReplace(const fs::path& filePath,const sregex& regFrom,const std::string& strTo);

#include "FileHandler.hpp"
#endif 
