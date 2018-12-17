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

#include "NodeKind.h"

namespace sourcetrail
{
	int nodeKindToInt(NodeKind kind)
	{
		return static_cast<int>(kind);
	}

	NodeKind intToNodeKind(int i)
	{
		const NodeKind kinds[] = {
			NodeKind::UNKNOWN,
			NodeKind::TYPE,
			NodeKind::BUILTIN_TYPE,
			NodeKind::MODULE,
			NodeKind::NAMESPACE,
			NodeKind::PACKAGE,
			NodeKind::STRUCT,
			NodeKind::CLASS,
			NodeKind::INTERFACE,
			NodeKind::ANNOTATION,
			NodeKind::GLOBAL_VARIABLE,
			NodeKind::FIELD,
			NodeKind::FUNCTION,
			NodeKind::METHOD,
			NodeKind::ENUM,
			NodeKind::ENUM_CONSTANT,
			NodeKind::TYPEDEF,
			NodeKind::TEMPLATE_PARAMETER,
			NodeKind::TYPE_PARAMETER,
			NodeKind::FILE,
			NodeKind::MACRO,
			NodeKind::UNION
		};

		for (NodeKind kind : kinds)
		{
			if (i == nodeKindToInt(kind))
			{
				return kind;
			}
		}

		return NodeKind::UNKNOWN;
	}
}
