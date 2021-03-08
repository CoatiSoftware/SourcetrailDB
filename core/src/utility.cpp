/*
 * Copyright 2018 Coati Software KG
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "utility.h"

#include <algorithm>
#include <ctime>
#include <fstream>
#include <vector>

#include "SourcetrailException.h"

namespace
{
std::istream& safeGetline(std::istream& is, std::string& t)
{
	t.clear();

	// The characters in the stream are read one-by-one using a std::streambuf.
	// That is faster than reading them one-by-one using the std::istream.
	// Code that uses streambuf this way must be guarded by a sentry object.
	// The sentry object performs various tasks,
	// such as thread synchronization and updating the stream state.

	std::istream::sentry se(is, true);
	std::streambuf* sb = is.rdbuf();

	while (true)
	{
		int c = sb->sbumpc();
		switch (c)
		{
		case '\n':
			return is;
		case '\r':
			if (sb->sgetc() == '\n')
			{
				sb->sbumpc();
			}
			return is;
		case std::streambuf::traits_type::eof():
			// Also handle the case when the last line has no line ending
			if (t.empty())
			{
				is.setstate(std::ios::eofbit);
			}
			return is;
		default:
			t += (char)c;
		}
	}
}
}	 // namespace

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
	std::vector<std::string> lines;

	try
	{
		std::ifstream srcFile;
		srcFile.open(filePath, std::ios::binary | std::ios::in);

		if (srcFile.fail())
		{
			throw SourcetrailException("Could not open file " + filePath);
		}

		while (!srcFile.eof())
		{
			std::string line;
			safeGetline(srcFile, line);
			lines.push_back(line + '\n');
		}

		srcFile.close();
	}
	catch (std::exception& e)
	{
		throw SourcetrailException("Exception thrown while reading file \"" + filePath + "\": " + e.what());
	}
	catch (...)
	{
		throw SourcetrailException("Unknown exception thrown while reading file \"" + filePath + "\"");
	}

	if (!lines.empty())
	{
		std::string last = lines.back().substr(0, lines.back().size() - 1);
		lines.pop_back();
		if (!last.empty())
		{
			lines.push_back(last);
		}
	}

	std::string content;

	for (const std::string& line: lines)
	{
		content += line;
	}

	return content;
}

std::string getDateTimeString(const time_t& time)
{
#pragma warning(push)
#pragma warning(disable : 4996)
	std::tm* ptm = std::localtime(&time);
#pragma warning(pop)
	char buffer[32];
	std::strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", ptm);
	return buffer;
}

int getLineCount(const std::string s)
{
	return static_cast<int>(std::count(s.begin(), s.end(), '\n'));
}
}	 // namespace utility
}	 // namespace sourcetrail
