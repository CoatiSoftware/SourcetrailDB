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

#ifndef SOURCETRAIL_SOURCE_LOCATION_H
#define SOURCETRAIL_SOURCE_LOCATION_H

#include <string>

namespace sourcetrail
{
	/**
	 * Struct that represents a single character location in a source file.
	 *
	 * Note: Line and column numbers start at 1 instead of 0!
	*/
	struct SourceLocation
	{
		std::string filePath;
		int line;
		int column;
	};
}

#endif // SOURCETRAIL_SOURCE_LOCATION_H
