#include "common/FileHandler.h"
#include <boost/algorithm/string.hpp>

COMMON_EXPORT void FindFiles(const fs::path& dirPath,const Filter& filter,PathVec& out)
{
    fs::directory_iterator end;
    for(fs::directory_iterator pos(dirPath); pos != end; ++pos)
    {
	bool isDir = is_directory(*pos);
	if(filter(*pos,isDir))
	{
	    out.push_back(*pos);
	    if (isDir)
	    {
		FindFiles(*pos,filter,out);
	    }
	}
    }
}

COMMON_EXPORT re::sregex Wildcard2Regex(std::string wildcardPattern)
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
    wildcardPattern += '$';
    return re::sregex::compile(wildcardPattern);
}

COMMON_EXPORT void FindFiles(const fs::path& dirPath,const re::sregex& pattern,PathVec& out)
{
    Filter filter = [&pattern](const fs::path& path,bool isDir)
	    {
	        return isDir || re::regex_search(path.string(),pattern);
	    };

    return FindFiles(dirPath
	    ,filter
	    ,out
	    );
}


COMMON_EXPORT void FindFiles(const fs::path& dirPath,const std::string pattern,PathVec& out)
{
    return FindFiles(dirPath,re::sregex::compile(pattern),out);
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

COMMON_EXPORT void FindAndReplace(const fs::path& filePath,const std::string& strFrom,const std::string& strTo)
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

COMMON_EXPORT void FindAndReplace(const fs::path& filePath,const sregex& regFrom,const std::string& strTo)
{
    TraverseFile<ReplaceVisit>(filePath,ReplaceVisit(regFrom,strTo));
}

