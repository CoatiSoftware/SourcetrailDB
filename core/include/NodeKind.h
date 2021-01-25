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

#ifndef SOURCETRAIL_NODE_KIND_H
#define SOURCETRAIL_NODE_KIND_H

namespace sourcetrail
{
/**
 * Enum providing all possible values for kinds of nodes that can be stored to the Sourcetrail database.
 */
enum class NodeKind : int
{
	UNKNOWN = 1 << 0,
	TYPE = 1 << 1,
	BUILTIN_TYPE = 1 << 2,
	MODULE = 1 << 3,
	NAMESPACE = 1 << 4,
	PACKAGE = 1 << 5,
	STRUCT = 1 << 6,
	CLASS = 1 << 7,
	INTERFACE = 1 << 8,
	ANNOTATION = 1 << 9,
	GLOBAL_VARIABLE = 1 << 10,
	FIELD = 1 << 11,
	FUNCTION = 1 << 12,
	METHOD = 1 << 13,
	ENUM = 1 << 14,
	ENUM_CONSTANT = 1 << 15,
	TYPEDEF = 1 << 16,
	TYPE_PARAMETER = 1 << 17,
	FILE = 1 << 18,
	MACRO = 1 << 19,
	UNION = 1 << 20
};

int nodeKindToInt(NodeKind kind);
NodeKind intToNodeKind(int i);
}	 // namespace sourcetrail

#endif	  // SOURCETRAIL_NODE_KIND_H
