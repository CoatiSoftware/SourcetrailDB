#ifndef SOURCETRAIL_NODE_KIND_H
#define SOURCETRAIL_NODE_KIND_H

namespace sourcetrail
{
	/**
	* Enum providing all possible values for kinds of nodes that can be stored to the Sourcetrail database.
	*/
	enum NodeKind
	{
		NODE_UNKNOWN = 1 << 0,
		NODE_TYPE = 1 << 1,
		NODE_BUILTIN_TYPE = 1 << 2,
		NODE_NAMESPACE = 1 << 3,
		NODE_PACKAGE = 1 << 4,
		NODE_STRUCT = 1 << 5,
		NODE_CLASS = 1 << 6,
		NODE_INTERFACE = 1 << 7,
		NODE_ANNOTATION = 1 << 8,
		NODE_GLOBAL_VARIABLE = 1 << 9,
		NODE_FIELD = 1 << 10,
		NODE_FUNCTION = 1 << 11,
		NODE_METHOD = 1 << 12,
		NODE_ENUM = 1 << 13,
		NODE_ENUM_CONSTANT = 1 << 14,
		NODE_TYPEDEF = 1 << 15,
		NODE_TEMPLATE_PARAMETER = 1 << 16,
		NODE_TYPE_PARAMETER = 1 << 17,
		NODE_FILE = 1 << 18,
		NODE_MACRO = 1 << 19,
		NODE_UNION = 1 << 20,
	};

	int nodeKindToInt(NodeKind v);
	NodeKind intToNodeKind(int v);
}

#endif // SOURCETRAIL_NODE_KIND_H
