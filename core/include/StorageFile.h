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

#ifndef SOURCETRAIL_STORAGE_FILE_H
#define SOURCETRAIL_STORAGE_FILE_H

#include <string>

namespace sourcetrail
{
struct StorageFile
{
	StorageFile(): id(0), filePath(""), languageIdentifier(""), modificationTime(""), indexed(true), complete(true) {}

	StorageFile(int id, std::string filePath, std::string languageIdentifier, std::string modificationTime, bool indexed, bool complete)
		: id(id)
		, filePath(std::move(filePath))
		, languageIdentifier(std::move(languageIdentifier))
		, modificationTime(std::move(modificationTime))
		, indexed(indexed)
		, complete(complete)
	{
	}

	int id;
	std::string filePath;
	std::string languageIdentifier;
	std::string modificationTime;
	bool indexed;
	bool complete;
};
}	 // namespace sourcetrail

#endif	  // SOURCETRAIL_STORAGE_FILE_H
