#include "utility.h"

#include <fstream>
#include <filesystem>

#include "SourcetrailException.h"

namespace sourcetrail
{
	namespace utility
	{
		bool getFileExists(const std::string& filePath)
		{
			std::ifstream file(filePath);
			return file.good();
		}

		std::string getFileContent(const std::string& filePath)
		{
			std::string content;

			std::ifstream file;
			file.open(filePath);

			if (file.fail())
			{
				throw SourcetrailException("Could not open file " + filePath);
			}

			for (std::string line; std::getline(file, line); )
			{
				content += line + "\n";
			}

			file.close();

			return content;
		}

		time_t getFileModificationTime(const std::string& filePath)
		{
			std::experimental::filesystem::path p(filePath);
			auto ftime = std::experimental::filesystem::last_write_time(p);

			// assuming system_clock for this demo
			// note: not true on MSVC; C++20 will allow portable output
			return decltype(ftime)::clock::to_time_t(ftime);
		}

		std::string getDateTimeString(const time_t& time)
		{
			std::tm* ptm = std::localtime(&time);
			char buffer[32];
			std::strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", ptm);
			return buffer;
		}

		int getLineCount(const std::string s)
		{
			return std::count(s.begin(), s.end(), '\n');
		}
	}
}
