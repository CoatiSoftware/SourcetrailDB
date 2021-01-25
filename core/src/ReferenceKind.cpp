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
EdgeKind referenceKindToEdgeKind(ReferenceKind kind)
{
	switch (kind)
	{
	case ReferenceKind::TYPE_USAGE:
		return EdgeKind::TYPE_USAGE;
	case ReferenceKind::USAGE:
		return EdgeKind::USAGE;
	case ReferenceKind::CALL:
		return EdgeKind::CALL;
	case ReferenceKind::INHERITANCE:
		return EdgeKind::INHERITANCE;
	case ReferenceKind::OVERRIDE:
		return EdgeKind::OVERRIDE;
	case ReferenceKind::TYPE_ARGUMENT:
		return EdgeKind::TYPE_ARGUMENT;
	case ReferenceKind::TEMPLATE_SPECIALIZATION:
		return EdgeKind::TEMPLATE_SPECIALIZATION;
	case ReferenceKind::INCLUDE:
		return EdgeKind::INCLUDE;
	case ReferenceKind::IMPORT:
		return EdgeKind::IMPORT;
	case ReferenceKind::MACRO_USAGE:
		return EdgeKind::MACRO_USAGE;
	case ReferenceKind::ANNOTATION_USAGE:
		return EdgeKind::ANNOTATION_USAGE;
	}
	return EdgeKind::UNKNOWN;
}
}	 // namespace sourcetrail
