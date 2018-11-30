#include "EdgeKind.h"

#include "ReferenceKind.h"

namespace sourcetrail
{
	int edgeKindToInt(EdgeKind edgeKind)
	{
		return edgeKind;
	}

	EdgeKind intToEdgeKind(int i)
	{
		switch (i)
		{
		case EDGE_MEMBER:
			return EDGE_MEMBER;
		case EDGE_TYPE_USAGE:
			return EDGE_TYPE_USAGE;
		case EDGE_USAGE:
			return EDGE_USAGE;
		case EDGE_CALL:
			return EDGE_CALL;
		case EDGE_INHERITANCE:
			return EDGE_INHERITANCE;
		case EDGE_OVERRIDE:
			return EDGE_OVERRIDE;
		case EDGE_TEMPLATE_ARGUMENT:
			return EDGE_TEMPLATE_ARGUMENT;
		case EDGE_TYPE_ARGUMENT:
			return EDGE_TYPE_ARGUMENT;
		case EDGE_TEMPLATE_DEFAULT_ARGUMENT:
			return EDGE_TEMPLATE_DEFAULT_ARGUMENT;
		case EDGE_TEMPLATE_SPECIALIZATION:
			return EDGE_TEMPLATE_SPECIALIZATION;
		case EDGE_TEMPLATE_MEMBER_SPECIALIZATION:
			return EDGE_TEMPLATE_MEMBER_SPECIALIZATION;
		case EDGE_INCLUDE:
			return EDGE_INCLUDE;
		case EDGE_IMPORT:
			return EDGE_IMPORT;
		case EDGE_AGGREGATION:
			return EDGE_AGGREGATION;
		case EDGE_MACRO_USAGE:
			return EDGE_MACRO_USAGE;
		}

		return EDGE_UNKNOWN;
	}
}
