#include "SymbolKind.h"

#include "NodeKind.h"

namespace sourcetrail
{
	NodeKind symbolKindToNodeKind(SymbolKind v)
	{
		switch (v)
		{
		case SYMBOL_TYPE:
			return NODE_TYPE;
		case SYMBOL_BUILTIN_TYPE:
			return NODE_BUILTIN_TYPE;
		case SYMBOL_NAMESPACE:
			return NODE_NAMESPACE;
		case SYMBOL_PACKAGE:
			return NODE_PACKAGE;
		case SYMBOL_STRUCT:
			return NODE_STRUCT;
		case SYMBOL_CLASS:
			return NODE_CLASS;
		case SYMBOL_INTERFACE:
			return NODE_INTERFACE;
		case SYMBOL_ANNOTATION:
			return NODE_ANNOTATION;
		case SYMBOL_GLOBAL_VARIABLE:
			return NODE_GLOBAL_VARIABLE;
		case SYMBOL_FIELD:
			return NODE_FIELD;
		case SYMBOL_FUNCTION:
			return NODE_FUNCTION;
		case SYMBOL_METHOD:
			return NODE_METHOD;
		case SYMBOL_ENUM:
			return NODE_ENUM;
		case SYMBOL_ENUM_CONSTANT:
			return NODE_ENUM_CONSTANT;
		case SYMBOL_TYPEDEF:
			return NODE_TYPEDEF;
		case SYMBOL_TEMPLATE_PARAMETER:
			return NODE_TEMPLATE_PARAMETER;
		case SYMBOL_TYPE_PARAMETER:
			return NODE_TYPE_PARAMETER;
		case SYMBOL_MACRO:
			return NODE_MACRO;
		case SYMBOL_UNION:
			return NODE_UNION;
		}
		return NODE_UNKNOWN;
	}
}
