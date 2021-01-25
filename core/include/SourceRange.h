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

#ifndef SOURCETRAIL_SOURCE_RANGE_H
#define SOURCETRAIL_SOURCE_RANGE_H

namespace sourcetrail
{
/**
 * Struct that represents the location of a range of characters in a source file.
 *
 *  note: Line and column numbers start at 1 instead of 0!
 *  note: The SourceRange includes both, the start and the end column number.
 */
struct SourceRange
{
	int fileId;
	int startLine;
	int startColumn;
	int endLine;
	int endColumn;
};
}	 // namespace sourcetrail

#endif	  // SOURCETRAIL_SOURCE_RANGE_H
