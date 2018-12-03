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

#ifndef SOURCETRAIL_LOCATION_KIND_H
#define SOURCETRAIL_LOCATION_KIND_H

namespace sourcetrail
{
	/**
	* Enum providing all possible values for kinds of locations that can be stored to the Sourcetrail database.
	*/
	enum LocationKind
	{
		LOCATION_TOKEN = 0,
		LOCATION_SCOPE = 1,
		LOCATION_QUALIFIER = 2,
		LOCATION_LOCAL_SYMBOL = 3,
		LOCATION_SIGNATURE = 4,
		LOCATION_COMMENT = 5,
		LOCATION_ERROR = 6,
		LOCATION_FULLTEXT_SEARCH = 7,
		LOCATION_SCREEN_SEARCH = 8
	};

	LocationKind intToLocationKind(int i);
	int locationKindToInt(LocationKind locationKind);
}

#endif // SOURCETRAIL_LOCATION_KIND_H
