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
std::string getDateTimeString(const time_t& time);
int getLineCount(const std::string s);
}	 // namespace utility
}	 // namespace sourcetrail

#endif	  // SOURCETRAIL_UTILITY_H
