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
int locationKindToInt(LocationKind kind)
{
	return static_cast<int>(kind);
}

LocationKind intToLocationKind(int i)
{
	const LocationKind kinds[] = {
		LocationKind::TOKEN,
		LocationKind::SCOPE,
		LocationKind::QUALIFIER,
		LocationKind::LOCAL_SYMBOL,
		LocationKind::SIGNATURE,
		LocationKind::ATOMIC_RANGE,
		LocationKind::INDEXER_ERROR,
		LocationKind::FULLTEXT_SEARCH,
		LocationKind::SCREEN_SEARCH,
		LocationKind::UNSOLVED};

	for (LocationKind kind: kinds)
	{
		if (i == locationKindToInt(kind))
		{
			return kind;
		}
	}

	throw SourcetrailException("Unable to convert integer \"" + std::to_string(i) + "\" to location kind.");
}
}	 // namespace sourcetrail
