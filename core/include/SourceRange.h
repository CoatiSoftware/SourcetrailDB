#ifndef SOURCETRAIL_SOURCE_RANGE_H
#define SOURCETRAIL_SOURCE_RANGE_H

#include <string>

namespace sourcetrail
{
	/**
	* Struct that represents the location of a range of characters in a source file.
	*
	* Note: Line and column numbers start at 1 instead of 0!
	*/
	struct SourceRange
	{
		std::string filePath;
		int startLine;
		int startColumn;
		int endLine;
		int endColumn;
	};
}

#endif // SOURCETRAIL_SOURCE_RANGE_H
