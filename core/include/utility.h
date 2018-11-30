#ifndef SOURCETRAIL_UTILITY_H
#define SOURCETRAIL_UTILITY_H

#include <string>
#include <time.h>

namespace sourcetrail
{
	namespace utility
	{
		bool getFileExists(const std::string& filePath);
		std::string getFileContent(const std::string& filePath);
		time_t getFileModificationTime(const std::string& filePath);
		std::string getDateTimeString(const time_t& time);
		int getLineCount(const std::string s);
	}
}

#endif // SOURCETRAIL_UTILITY_H
