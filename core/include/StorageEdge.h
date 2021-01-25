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

#ifndef SOURCETRAIL_STORAGE_EDGE_H
#define SOURCETRAIL_STORAGE_EDGE_H

namespace sourcetrail
{
struct StorageEdgeData
{
	StorageEdgeData(): sourceNodeId(0), targetNodeId(0), edgeKind(0) {}

	StorageEdgeData(int sourceNodeId, int targetNodeId, int edgeKind)
		: sourceNodeId(sourceNodeId), targetNodeId(targetNodeId), edgeKind(edgeKind)
	{
	}

	int sourceNodeId;
	int targetNodeId;
	int edgeKind;
};

struct StorageEdge: public StorageEdgeData
{
	StorageEdge(): StorageEdgeData(), id(0) {}

	StorageEdge(int id, const StorageEdgeData& data): StorageEdgeData(data), id(id) {}

	StorageEdge(int id, int sourceNodeId, int targetNodeId, int edgeKind): StorageEdgeData(sourceNodeId, targetNodeId, edgeKind), id(id) {}

	int id;
};
}	 // namespace sourcetrail

#endif	  // SOURCETRAIL_STORAGE_EDGE_H
