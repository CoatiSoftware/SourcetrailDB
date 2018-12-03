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

#include "ReferenceKind.h"

#include "EdgeKind.h"

namespace sourcetrail
{
	EdgeKind referenceKindToEdgeKind(ReferenceKind v)
	{
		switch (v)
		{
		case REFERENCE_TYPE_USAGE:
			return EDGE_TYPE_USAGE;
		case REFERENCE_USAGE:
			return EDGE_USAGE;
		case REFERENCE_CALL:
			return EDGE_CALL;
		case REFERENCE_INHERITANCE:
			return EDGE_INHERITANCE;
		case REFERENCE_OVERRIDE:
			return EDGE_OVERRIDE;
		case REFERENCE_TEMPLATE_ARGUMENT:
			return EDGE_TEMPLATE_ARGUMENT;
		case REFERENCE_TYPE_ARGUMENT:
			return EDGE_TYPE_ARGUMENT;
		case REFERENCE_TEMPLATE_DEFAULT_ARGUMENT:
			return EDGE_TEMPLATE_DEFAULT_ARGUMENT;
		case REFERENCE_TEMPLATE_SPECIALIZATION:
			return EDGE_TEMPLATE_SPECIALIZATION;
		case REFERENCE_TEMPLATE_MEMBER_SPECIALIZATION:
			return EDGE_TEMPLATE_MEMBER_SPECIALIZATION;
		case REFERENCE_INCLUDE:
			return EDGE_INCLUDE;
		case REFERENCE_IMPORT:
			return EDGE_IMPORT;
		case REFERENCE_MACRO_USAGE:
			return EDGE_MACRO_USAGE;
		case REFERENCE_ANNOTATION_USAGE:
			return EDGE_ANNOTATION_USAGE;
		}
		return EDGE_UNKNOWN;
	}
}
