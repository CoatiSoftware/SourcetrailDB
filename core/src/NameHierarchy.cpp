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

#include "NameHierarchy.h"

#include "json/json.hpp"

namespace sourcetrail
{
	std::string serializeNameHierarchyToDatabaseString(const NameHierarchy& nameHierarchy)
	{
		static std::string META_DELIMITER = "\tm";
		static std::string NAME_DELIMITER = "\tn";
		static std::string PARTS_DELIMITER = "\ts";
		static std::string SIGNATURE_DELIMITER = "\tp";

		std::string serialized = nameHierarchy.nameDelimiter + META_DELIMITER;
		for (size_t i = 0; i < nameHierarchy.nameElements.size(); i++)
		{
			if (i != 0)
			{
				serialized += NAME_DELIMITER;
			}
			const NameElement& nameElement = nameHierarchy.nameElements[i];
			serialized += nameElement.name + PARTS_DELIMITER + nameElement.prefix + SIGNATURE_DELIMITER + nameElement.postfix;
		}
		return serialized;
	}

	std::string serializeNameHierarchyToJson(const NameHierarchy& nameHierarchy)
	{
		typedef nlohmann::json json;

		nlohmann::json j;

		j["name_delimiter"] = nameHierarchy.nameDelimiter;

		for (const NameElement& nameElement : nameHierarchy.nameElements)
		{
			j["name_elements"].push_back(
			{
				{ "prefix", nameElement.prefix } ,
				{ "name", nameElement.name } ,
				{ "postfix", nameElement.postfix }
			});
		}

		return j.dump(4);
	}

	NameHierarchy deserializeNameHierarchyFromJson(const std::string& serializedNameHierarchy)
	{
		typedef nlohmann::json json;

		NameHierarchy nameHierarchy;
		try
		{
			json j = nlohmann::json::parse(serializedNameHierarchy);
			{
				json jDelimiter = j["name_delimiter"];
				if (jDelimiter.is_string())
				{
					nameHierarchy.nameDelimiter = jDelimiter.get<std::string>();
				}
			}

			{
				json jNameElements = j["name_elements"];
				if (jNameElements.is_array())
				{
					for (json::iterator it = jNameElements.begin(); it != jNameElements.end(); ++it)
					{
						NameElement nameElement;
						{
							json jPrefix = it.value()["prefix"];
							if (jPrefix.is_string())
							{
								nameElement.prefix = jPrefix.get<std::string>();
							}
						}
						{
							json jName = it.value()["name"];
							if (jName.is_string())
							{
								nameElement.name = jName.get<std::string>();
							}
						}
						{
							json jPostfix = it.value()["postfix"];
							if (jPostfix.is_string())
							{
								nameElement.postfix = jPostfix.get<std::string>();
							}
						}
						nameHierarchy.nameElements.push_back(nameElement);
					}
				}
			}
		}
		catch (...)
		{
			// do nothing
		}

		return nameHierarchy;
	}
}
