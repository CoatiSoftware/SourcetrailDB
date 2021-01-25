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
enum class EdgeKind : int
{
	UNKNOWN = 0,
	MEMBER = 1 << 0,
	TYPE_USAGE = 1 << 1,
	USAGE = 1 << 2,
	CALL = 1 << 3,
	INHERITANCE = 1 << 4,
	OVERRIDE = 1 << 5,
	TYPE_ARGUMENT = 1 << 6,
	TEMPLATE_SPECIALIZATION = 1 << 7,
	INCLUDE = 1 << 8,
	IMPORT = 1 << 9,
	MACRO_USAGE = 1 << 11,	  // needs new db version if decremented
	ANNOTATION_USAGE = 1 << 12
};

int edgeKindToInt(EdgeKind kind);
EdgeKind intToEdgeKind(int i);
}	 // namespace sourcetrail

#endif	  // SOURCETRAIL_EDGE_KIND_H
