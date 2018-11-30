#include "NameHierarchy.h"

#include "json/json.hpp"

namespace sourcetrail
{
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
