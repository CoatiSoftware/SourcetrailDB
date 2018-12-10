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

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() function

#include "catch/catch.hpp"

#include "SourcetrailDBWriter.h"
#include "DatabaseStorage.h"

namespace sourcetrail
{
	TEST_CASE("Testing SourcetrailDBWriter")
	{
		const std::string databasePath = "testing.db";

		std::shared_ptr<DatabaseStorage> storage = DatabaseStorage::openDatabase(databasePath);

		SourcetrailDBWriter writer;
		REQUIRE(writer.getLastError().empty());

		writer.open(databasePath);
		REQUIRE(writer.getLastError().empty());

		writer.clear();
		REQUIRE(writer.getLastError().empty());

		SECTION("writer is compatible after clearing")
		{
			REQUIRE(writer.isCompatible());
		}

		SECTION("writer is empty after clearing")
		{
			REQUIRE(writer.isEmpty());
		}

		SECTION("writer can record symbol")
		{
			NameHierarchy nameSymbol1({ "." , {{ "void", "foo", "()" } } });
			int idSymbol1 = writer.recordSymbol(nameSymbol1);
			REQUIRE(idSymbol1 != 0);
			REQUIRE(writer.getLastError().empty());

			SECTION("database contains node after recording plane symbol")
			{
				std::vector<StorageNode> nodes = storage->getAll<StorageNode>();
				REQUIRE(nodes.size() == 1);
			}

			SECTION("writer does not record symbol twice")
			{
				int secondIdSymbol1 = writer.recordSymbol(nameSymbol1);
				REQUIRE(secondIdSymbol1 == idSymbol1);
				REQUIRE(writer.getLastError().empty());

				std::vector<StorageNode> nodes = storage->getAll<StorageNode>();
				REQUIRE(nodes.size() == 1);
			}

			SECTION("writer records \"explicit\" symbol definition kind")
			{
				bool success = writer.recordSymbolDefinitionKind(idSymbol1, DEFINITION_EXPLICIT);
				REQUIRE(success);
				REQUIRE(writer.getLastError().empty());

				std::vector<StorageSymbol> symbols = storage->getAll<StorageSymbol>();
				REQUIRE(symbols.size() == 1);
				REQUIRE(symbols.front().definitionKind == definitionKindToInt(DEFINITION_EXPLICIT));
			}

			SECTION("writer records \"class\" symbol kind")
			{
				bool success = writer.recordSymbolKind(idSymbol1, SYMBOL_CLASS);
				REQUIRE(success);
				REQUIRE(writer.getLastError().empty());

				std::vector<StorageNode> nodes = storage->getAll<StorageNode>();
				REQUIRE(nodes.size() == 1);
				REQUIRE(nodes.front().nodeKind == symbolKindToNodeKind(SYMBOL_CLASS));
			}
		}

		writer.close();
		REQUIRE(writer.getLastError().empty());
	}
}
