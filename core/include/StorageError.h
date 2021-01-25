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

#ifndef SOURCETRAIL_STORAGE_ERROR_H
#define SOURCETRAIL_STORAGE_ERROR_H

#include <string>

namespace sourcetrail
{
struct StorageErrorData
{
	StorageErrorData(): message(""), translationUnit(""), fatal(0), indexed(0) {}

	StorageErrorData(std::string message, std::string translationUnit, bool fatal, bool indexed)
		: message(std::move(message)), translationUnit(std::move(translationUnit)), fatal(fatal), indexed(indexed)
	{
	}

	std::string message;
	std::string translationUnit;
	bool fatal;
	bool indexed;
};

struct StorageError: public StorageErrorData
{
	StorageError(): StorageErrorData(), id(0) {}

	StorageError(int id, const StorageErrorData& data): StorageErrorData(data), id(id) {}

	StorageError(int id, std::string message, std::string translationUnit, bool fatal, bool indexed)
		: StorageErrorData(std::move(message), std::move(translationUnit), fatal, indexed), id(id)
	{
	}

	int id;
};
}	 // namespace sourcetrail

#endif	  // SOURCETRAIL_STORAGE_ERROR_H
