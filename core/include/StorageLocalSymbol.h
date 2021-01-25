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

#ifndef SOURCETRAIL_STORAGE_LOCAL_SYMBOL_H
#define SOURCETRAIL_STORAGE_LOCAL_SYMBOL_H

#include <string>

namespace sourcetrail
{
struct StorageLocalSymbolData
{
	StorageLocalSymbolData(): name("") {}

	StorageLocalSymbolData(std::string name): name(std::move(name)) {}

	std::string name;
};

struct StorageLocalSymbol: public StorageLocalSymbolData
{
	StorageLocalSymbol(): StorageLocalSymbolData(), id(0) {}

	StorageLocalSymbol(int id, const StorageLocalSymbolData& data): StorageLocalSymbolData(data), id(id) {}

	StorageLocalSymbol(int id, std::string name): StorageLocalSymbolData(std::move(name)), id(id) {}

	int id;
};
}	 // namespace sourcetrail

#endif	  // SOURCETRAIL_STORAGE_LOCAL_SYMBOL_H
