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

#ifndef SOURCETRAIL_STORAGE_SOURCE_LOCATION_H
#define SOURCETRAIL_STORAGE_SOURCE_LOCATION_H

namespace sourcetrail
{
struct StorageSourceLocationData
{
	StorageSourceLocationData()
		: fileNodeId(0), startLineNumber(-1), startColumnNumber(-1), endLineNumber(-1), endColumnNumber(-1), locationKind(0)
	{
	}

	StorageSourceLocationData(int fileNodeId, int startLineNumber, int startColumnNumber, int endLineNumber, int endColumnNumber, int locationKind)
		: fileNodeId(fileNodeId)
		, startLineNumber(startLineNumber)
		, startColumnNumber(startColumnNumber)
		, endLineNumber(endLineNumber)
		, endColumnNumber(endColumnNumber)
		, locationKind(locationKind)
	{
	}

	int fileNodeId;
	int startLineNumber;
	int startColumnNumber;
	int endLineNumber;
	int endColumnNumber;
	int locationKind;
};

struct StorageSourceLocation: public StorageSourceLocationData
{
	StorageSourceLocation(): StorageSourceLocationData(), id(0) {}

	StorageSourceLocation(int id, const StorageSourceLocationData& data): StorageSourceLocationData(data), id(id) {}

	StorageSourceLocation(
		int id, int fileNodeId, int startLineNumber, int startColumnNumber, int endLineNumber, int endColumnNumber, int locationKind)
		: StorageSourceLocationData(fileNodeId, startLineNumber, startColumnNumber, endLineNumber, endColumnNumber, locationKind), id(id)
	{
	}

	int id;
};
}	 // namespace sourcetrail

#endif	  // SOURCETRAIL_STORAGE_SOURCE_LOCATION_H
