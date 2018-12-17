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

#include "EdgeKind.h"

#include "ReferenceKind.h"

namespace sourcetrail
{
	int edgeKindToInt(EdgeKind kind)
	{
		return static_cast<int>(kind);
	}

	EdgeKind intToEdgeKind(int i)
	{
		const EdgeKind kinds[] = {
			EdgeKind::MEMBER,
			EdgeKind::TYPE_USAGE,
			EdgeKind::USAGE,
			EdgeKind::CALL,
			EdgeKind::INHERITANCE,
			EdgeKind::OVERRIDE,
			EdgeKind::TEMPLATE_ARGUMENT,
			EdgeKind::TYPE_ARGUMENT,
			EdgeKind::TEMPLATE_DEFAULT_ARGUMENT,
			EdgeKind::TEMPLATE_SPECIALIZATION,
			EdgeKind::TEMPLATE_MEMBER_SPECIALIZATION,
			EdgeKind::INCLUDE,
			EdgeKind::IMPORT,
			EdgeKind::AGGREGATION,
			EdgeKind::MACRO_USAGE
		};

		for (EdgeKind kind : kinds)
		{
			if (i == edgeKindToInt(kind))
			{
				return kind;
			}
		}

		return EdgeKind::UNKNOWN;
	}
}
