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

#include "SymbolKind.h"

#include "NodeKind.h"

namespace sourcetrail
{
	NodeKind symbolKindToNodeKind(SymbolKind kind)
	{
		switch (kind)
		{
		case SymbolKind::TYPE:
			return NodeKind::TYPE;
		case SymbolKind::BUILTIN_TYPE:
			return NodeKind::BUILTIN_TYPE;
		case SymbolKind::MODULE:
			return NodeKind::MODULE;
		case SymbolKind::NAMESPACE:
			return NodeKind::NAMESPACE;
		case SymbolKind::PACKAGE:
			return NodeKind::PACKAGE;
		case SymbolKind::STRUCT:
			return NodeKind::STRUCT;
		case SymbolKind::CLASS:
			return NodeKind::CLASS;
		case SymbolKind::INTERFACE:
			return NodeKind::INTERFACE;
		case SymbolKind::ANNOTATION:
			return NodeKind::ANNOTATION;
		case SymbolKind::GLOBAL_VARIABLE:
			return NodeKind::GLOBAL_VARIABLE;
		case SymbolKind::FIELD:
			return NodeKind::FIELD;
		case SymbolKind::FUNCTION:
			return NodeKind::FUNCTION;
		case SymbolKind::METHOD:
			return NodeKind::METHOD;
		case SymbolKind::ENUM:
			return NodeKind::ENUM;
		case SymbolKind::ENUM_CONSTANT:
			return NodeKind::ENUM_CONSTANT;
		case SymbolKind::TYPEDEF:
			return NodeKind::TYPEDEF;
		case SymbolKind::TYPE_PARAMETER:
			return NodeKind::TYPE_PARAMETER;
		case SymbolKind::MACRO:
			return NodeKind::MACRO;
		case SymbolKind::UNION:
			return NodeKind::UNION;
		}
		return NodeKind::UNKNOWN;
	}
}
