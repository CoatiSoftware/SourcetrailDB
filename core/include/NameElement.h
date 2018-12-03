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

#ifndef SOURCETRAIL_NAME_ELEMENT_H
#define SOURCETRAIL_NAME_ELEMENT_H

#include <string>

namespace sourcetrail
{
	/**
	* Struct that represents a single hierarchical element that is part of a symbol's name.
	*/
	struct NameElement
	{
		std::string prefix;
		std::string name;
		std::string postfix;
	};
}

#endif // SOURCETRAIL_NAME_ELEMENT_H
