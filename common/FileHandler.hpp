#include <boost/filesystem/fstream.hpp>


template<typename Visit>
void TraverseFile(const filesystem::path& filePath,Visit visit)
{
    std::string strTempFileName(filePath.string());
    strTempFileName += "temp~";
    filesystem::path pathTemp(strTempFileName);
    {
	filesystem::fstream fileIn(filePath);
	if(!fileIn.is_open())
	{
	    std::string strErr("can not open file ");
	    strErr += filePath.string();
	    throw std::runtime_error(strErr);
	}

	filesystem::ofstream fileOut(pathTemp);

	std::string strLine;
	while(getline(fileIn,strLine))
	{
	    visit(strLine);
	    fileOut<<strLine<<std::endl;
	}
    }

    remove(filePath);
    rename(pathTemp,filePath);

}

