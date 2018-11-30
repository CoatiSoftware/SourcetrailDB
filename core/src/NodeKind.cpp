#include "NodeKind.h"

namespace sourcetrail
{
	int nodeKindToInt(NodeKind v)
	{
		return v;
	}

	NodeKind intToNodeKind(int v)
	{
		if (v == nodeKindToInt(NODE_UNKNOWN)) { return NODE_UNKNOWN; }
		if (v == nodeKindToInt(NODE_TYPE)) { return NODE_TYPE; }
		if (v == nodeKindToInt(NODE_BUILTIN_TYPE)) { return NODE_BUILTIN_TYPE; }
		if (v == nodeKindToInt(NODE_NAMESPACE)) { return NODE_NAMESPACE; }
		if (v == nodeKindToInt(NODE_PACKAGE)) { return NODE_PACKAGE; }
		if (v == nodeKindToInt(NODE_STRUCT)) { return NODE_STRUCT; }
		if (v == nodeKindToInt(NODE_CLASS)) { return NODE_CLASS; }
		if (v == nodeKindToInt(NODE_INTERFACE)) { return NODE_INTERFACE; }
		if (v == nodeKindToInt(NODE_ANNOTATION)) { return NODE_ANNOTATION; }
		if (v == nodeKindToInt(NODE_GLOBAL_VARIABLE)) { return NODE_GLOBAL_VARIABLE; }
		if (v == nodeKindToInt(NODE_FIELD)) { return NODE_FIELD; }
		if (v == nodeKindToInt(NODE_FUNCTION)) { return NODE_FUNCTION; }
		if (v == nodeKindToInt(NODE_METHOD)) { return NODE_METHOD; }
		if (v == nodeKindToInt(NODE_ENUM)) { return NODE_ENUM; }
		if (v == nodeKindToInt(NODE_ENUM_CONSTANT)) { return NODE_ENUM_CONSTANT; }
		if (v == nodeKindToInt(NODE_TYPEDEF)) { return NODE_TYPEDEF; }
		if (v == nodeKindToInt(NODE_TEMPLATE_PARAMETER)) { return NODE_TEMPLATE_PARAMETER; }
		if (v == nodeKindToInt(NODE_TYPE_PARAMETER)) { return NODE_TYPE_PARAMETER; }
		if (v == nodeKindToInt(NODE_FILE)) { return NODE_FILE; }
		if (v == nodeKindToInt(NODE_MACRO)) { return NODE_MACRO; }
		if (v == nodeKindToInt(NODE_UNION)) { return NODE_UNION; }
		return NODE_UNKNOWN;
	}
}
