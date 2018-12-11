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

#ifndef SOURCETRAIL_NAME_HIERARCHY_H
#define SOURCETRAIL_NAME_HIERARCHY_H

#include <string>
#include <vector>

#include "NameElement.h"

namespace sourcetrail
{
	/**
	* Struct that represents an entire name of a symbol.
	*
	* TODO: explain prefix and postfix
	* TODO: alway provide prefix and postfix of all name elements to make them unique. Example from C++ (2 foo::bar (foo has different signature)
	*/
	struct NameHierarchy
	{
		std::string nameDelimiter;
		std::vector<NameElement> nameElements;
	};

	std::string serializeNameHierarchyToDatabaseString(const NameHierarchy& nameHierarchy);
	std::string serializeNameHierarchyToJson(const NameHierarchy& nameHierarchy);
	NameHierarchy deserializeNameHierarchyFromJson(const std::string& serializedNameHierarchy);
}

#endif // SOURCETRAIL_NAME_HIERARCHY_H
