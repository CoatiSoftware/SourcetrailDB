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

#include "LocationKind.h"

#include "SourcetrailException.h"

namespace sourcetrail
{
	LocationKind intToLocationKind(int i)
	{
		switch (i)
		{
		case LOCATION_TOKEN:
			return LOCATION_TOKEN;
		case LOCATION_SCOPE:
			return LOCATION_SCOPE;
		case LOCATION_QUALIFIER:
			return LOCATION_QUALIFIER;
		case LOCATION_LOCAL_SYMBOL:
			return LOCATION_LOCAL_SYMBOL;
		case LOCATION_SIGNATURE:
			return LOCATION_SIGNATURE;
		case LOCATION_COMMENT:
			return LOCATION_COMMENT;
		case LOCATION_ERROR:
			return LOCATION_ERROR;
		case LOCATION_FULLTEXT_SEARCH:
			return LOCATION_FULLTEXT_SEARCH;
		case LOCATION_SCREEN_SEARCH:
			return LOCATION_SCREEN_SEARCH;
		}
		throw SourcetrailException("Unable to convert integer \"" + std::to_string(i) + "\" to location kind.");
	}

	int locationKindToInt(LocationKind locationKind)
	{
		return locationKind;
	}
}
