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

using namespace boost;

typedef boost::function<bool (const filesystem::path& path,bool isDir)> Filter;
typedef std::vector<filesystem::path> PathVec;

void FindFiles(const filesystem::path& dirPath,const Filter& filter,PathVec& out);

template<typename Visit>
void TraverseFile(const filesystem::path& filePath,Visit visit);

void FindAndReplace(const filesystem::path& filePath,const std::string& strFrom,const std::string& strTo);

void FindAndReplace(const filesystem::path& filePath,const sregex& regFrom,const std::string& strTo);

#include "FileHandler.hpp"
#endif 
