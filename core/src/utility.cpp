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

	for (std::string line; std::getline(file, line);)
	{
		content += line + "\n";
	}

	file.close();

	return content;
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
}	 // namespace utility
}	 // namespace sourcetrail
