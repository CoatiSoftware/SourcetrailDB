#include "DefinitionKind.h"

namespace sourcetrail
{
	int definitionKindToInt(DefinitionKind v)
	{
		return v;
	}

	DefinitionKind intToDefinitionKind(int v)
	{
		if (v == definitionKindToInt(DEFINITION_IMPLICIT))
			return DEFINITION_IMPLICIT;
		if (v == definitionKindToInt(DEFINITION_EXPLICIT))
			return DEFINITION_EXPLICIT;

		return DEFINITION_EXPLICIT;
	}
}
