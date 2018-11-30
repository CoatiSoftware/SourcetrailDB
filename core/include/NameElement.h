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
