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

namespace sourcetrail
{
	enum NodeKind;

	/**
	* Enum providing all possible values for kinds of symbols that can be recorded using the SourcetrailDBWriter interface.
	*/
	enum SymbolKind
	{
		SYMBOL_TYPE,
		SYMBOL_BUILTIN_TYPE,
		SYMBOL_MODULE,
		SYMBOL_NAMESPACE,
		SYMBOL_PACKAGE,
		SYMBOL_STRUCT,
		SYMBOL_CLASS,
		SYMBOL_INTERFACE,
		SYMBOL_ANNOTATION,
		SYMBOL_GLOBAL_VARIABLE,
		SYMBOL_FIELD,
		SYMBOL_FUNCTION,
		SYMBOL_METHOD,
		SYMBOL_ENUM,
		SYMBOL_ENUM_CONSTANT,
		SYMBOL_TYPEDEF,
		SYMBOL_TEMPLATE_PARAMETER,
		SYMBOL_TYPE_PARAMETER,
		SYMBOL_MACRO,
		SYMBOL_UNION
	};

	NodeKind symbolKindToNodeKind(SymbolKind v);
}

#endif // SOURCETRAIL_SYMBOL_KIND_H
