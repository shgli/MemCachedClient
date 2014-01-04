#include "FileHandler.h"
#include <boost/algorithm/string.hpp>

void FindFiles(const filesystem::path& dirPath,const Filter& filter,PathVec& out)
{
    filesystem::directory_iterator end;
    for(filesystem::directory_iterator pos(dirPath); pos != end; ++pos)
    {
	bool isDir = is_directory(*pos);
	if(filter(*pos,isDir))
	{
	    out.push_back(*pos);
	    if (isDir)
	    {
		FindFile(*pos,filter,out);
	    }
	}
    }
}

sregex Wildcard2Regex(const std::string& wildcardPattern)
{
    boost::replace_all(wildcardPattern, "\\", "\\\\");
    boost::replace_all(wildcardPattern, "^", "\\^");
    boost::replace_all(wildcardPattern, ".", "\\.");
    boost::replace_all(wildcardPattern, "$", "\\$");
    boost::replace_all(wildcardPattern, "|", "\\|");
    boost::replace_all(wildcardPattern, "(", "\\(");
    boost::replace_all(wildcardPattern, ")", "\\)");
    boost::replace_all(wildcardPattern, "[", "\\[");
    boost::replace_all(wildcardPattern, "]", "\\]");
    boost::replace_all(wildcardPattern, "*", "\\*");
    boost::replace_all(wildcardPattern, "+", "\\+");
    boost::replace_all(wildcardPattern, "?", "\\?");
    boost::replace_all(wildcardPattern, "/", "\\/");

    boost::replace_all(wildcardPattern, "\\?", ".");
    boost::replace_all(wildcardPattern, "\\*", ".*");

    return sregex::compile(wildcardPattern);
}

void FindFiles(const fs::path& dirPath,const sregex& pattern,PathVec& out)
{
    return FildFiles(dirPath
	    ,[&pattern](const filesystem::path& path,bool isDir)
	    {
	        return isdir || regex_match(path.string(),pattern);
	    }
	    ,out
	    );
}


void FindFiles(const filesystem::path& dirPath,const std::string pattern,PathVec& out)
{
    return FindFiles(dirPath,sregex::compile(pattern),out);
}

using namespace boost::xpressive;
class SReplaceVisit
{
public:
    SReplaceVisit(const std::string& strFrom,const std::string& strTo):
        m_strFrom(strFrom)
        ,m_strTo(strTo){}

    void operator()(std::string& strLine)const
    {
        std::string::size_type nIndex = strLine.find(m_strFrom,0);
        while(nIndex != strLine.npos)
        {
	    strLine.replace(nIndex,m_strFrom.length(),m_strTo);
	    nIndex = strLine.find(m_strFrom,nIndex + m_strTo.length());
        }
    }

private:
    const std::string& m_strFrom;
    const std::string& m_strTo;
};

void FindAndReplace(const filesystem::path& filePath,const std::string& strFrom,const std::string& strTo)
{
    TraverseFile<SReplaceVisit>(filePath,SReplaceVisit(strFrom,strTo));
}

class ReplaceVisit
{
public:
    ReplaceVisit(const sregex& reg,const std::string& strTo):
        m_regFrom(reg)
        ,m_strTo(strTo){}

    void operator()(std::string& strLine)const
    {
        strLine = regex_replace(strLine,m_regFrom,m_strTo);
    }
private:
    sregex m_regFrom;
    const std::string& m_strTo;
};

void FindAndReplace(const filesystem::path& filePath,const sregex& regFrom,const std::string& strTo)
{
    TraverseFile<ReplaceVisit>(filePath,ReplaceVisit(regFrom,strTo));
}

