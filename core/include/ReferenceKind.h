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

#ifndef SOURCETRAIL_REFERENCE_KIND_H
#define SOURCETRAIL_REFERENCE_KIND_H

#include "EdgeKind.h"

namespace sourcetrail
{
/**
 * Enum providing all possible values for kinds of references that can be recorded using the SourcetrailDBWriter interface.
 */
enum class ReferenceKind
{
	TYPE_USAGE,
	USAGE,
	CALL,
	INHERITANCE,
	OVERRIDE,
	TYPE_ARGUMENT,
	TEMPLATE_SPECIALIZATION,
	INCLUDE,
	IMPORT,
	MACRO_USAGE,
	ANNOTATION_USAGE
};

EdgeKind referenceKindToEdgeKind(ReferenceKind kind);
}	 // namespace sourcetrail

#endif	  // SOURCETRAIL_REFERENCE_KIND_H
