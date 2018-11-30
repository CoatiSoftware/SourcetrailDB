#ifndef SOURCETRAIL_NAME_HIERARCHY_H
#define SOURCETRAIL_NAME_HIERARCHY_H

#include <string>
#include <vector>

#include "NameElement.h"

namespace sourcetrail
{
	/**
	* Struct that represents an entire name of a symbol.
	*/
	struct NameHierarchy
	{
		std::string nameDelimiter;
		std::vector<NameElement> nameElements;
	};

	std::string serializeNameHierarchyToJson(const NameHierarchy& nameHierarchy);
	NameHierarchy deserializeNameHierarchyFromJson(const std::string& serializedNameHierarchy);
}

#endif // SOURCETRAIL_NAME_HIERARCHY_H
