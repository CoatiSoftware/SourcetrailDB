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
