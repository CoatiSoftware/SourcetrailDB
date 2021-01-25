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

#ifndef SOURCETRAIL_STORAGE_NODE_H
#define SOURCETRAIL_STORAGE_NODE_H

#include <string>

namespace sourcetrail
{
struct StorageNodeData
{
	StorageNodeData(): nodeKind(0), serializedName("") {}

	StorageNodeData(int nodeKind, std::string serializedName): nodeKind(nodeKind), serializedName(std::move(serializedName)) {}

	int nodeKind;
	std::string serializedName;
};

struct StorageNode: public StorageNodeData
{
	StorageNode(): StorageNodeData(), id(0) {}

	StorageNode(int id, int nodeKind, std::string serializedName): StorageNodeData(nodeKind, std::move(serializedName)), id(id) {}

	StorageNode(int id, const StorageNodeData& data): StorageNodeData(data), id(id) {}

	int id;
};
}	 // namespace sourcetrail

#endif	  // SOURCETRAIL_STORAGE_NODE_H
