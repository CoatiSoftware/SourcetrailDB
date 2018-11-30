#ifndef SOURCETRAIL_EDGE_KIND_H
#define SOURCETRAIL_EDGE_KIND_H

namespace sourcetrail
{
	/**
	 * Enum providing all possible values for kinds of edges that can be stored to the Sourcetrail database.
	 *
	 * The DatabaseStorage provides the interface for writing and updating the Sourcetrail database. This interface only
	 * The EdgeKind enum should only be used internally and contains a complete list of values that may be written to
	 * the Sourcetrail database file. This enum contains kinds for edges that can be recorded explicitly, but also kinds
	 * for edges that will be created implicitly. For example edges of type "EDGE_MEMBER" will be created while storing parent
	 * child node relations when recording symbols with hierarchical names (see NameHierarchy).
	 */
	enum EdgeKind
	{
		EDGE_UNKNOWN = 0,
		EDGE_MEMBER = 1 << 0,
		EDGE_TYPE_USAGE = 1 << 1,
		EDGE_USAGE = 1 << 2,
		EDGE_CALL = 1 << 3,
		EDGE_INHERITANCE = 1 << 4,
		EDGE_OVERRIDE = 1 << 5,
		EDGE_TEMPLATE_ARGUMENT = 1 << 6,
		EDGE_TYPE_ARGUMENT = 1 << 7,
		EDGE_TEMPLATE_DEFAULT_ARGUMENT = 1 << 8,
		EDGE_TEMPLATE_SPECIALIZATION = 1 << 9,
		EDGE_TEMPLATE_MEMBER_SPECIALIZATION = 1 << 10,
		EDGE_INCLUDE = 1 << 11,
		EDGE_IMPORT = 1 << 12,
		EDGE_AGGREGATION = 1 << 13,
		EDGE_MACRO_USAGE = 1 << 14,
		EDGE_ANNOTATION_USAGE = 1 << 15
	};

	int edgeKindToInt(EdgeKind edgeKind);
	EdgeKind intToEdgeKind(int i);
}

#endif // SOURCETRAIL_EDGE_KIND_H
