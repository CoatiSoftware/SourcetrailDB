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

#ifndef SOURCETRAIL_SYMBOL_KIND_H
#define SOURCETRAIL_SYMBOL_KIND_H

#include "NodeKind.h"

namespace sourcetrail
{
/**
 * Enum providing all possible values for kinds of symbols that can be recorded using the SourcetrailDBWriter interface.
 */
enum class SymbolKind
{
	TYPE,
	BUILTIN_TYPE,
	MODULE,
	NAMESPACE,
	PACKAGE,
	STRUCT,
	CLASS,
	INTERFACE,
	ANNOTATION,
	GLOBAL_VARIABLE,
	FIELD,
	FUNCTION,
	METHOD,
	ENUM,
	ENUM_CONSTANT,
	TYPEDEF,
	TYPE_PARAMETER,
	MACRO,
	UNION
};

NodeKind symbolKindToNodeKind(SymbolKind kind);
}	 // namespace sourcetrail

#endif	  // SOURCETRAIL_SYMBOL_KIND_H
