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

#include "catch.hpp"

#include "DatabaseStorage.h"
#include "NodeKind.h"
#include "SourcetrailDBWriter.h"

namespace sourcetrail
{
	TEST_CASE("Testing SourcetrailDBWriter performs general operations")
	{
		const std::string databasePath = "testing.db";

		std::shared_ptr<DatabaseStorage> storage = DatabaseStorage::openDatabase(databasePath);

		SourcetrailDBWriter writer;
		REQUIRE(writer.getLastError() == "");

		writer.open(databasePath);
		REQUIRE(writer.getLastError() == "");

		writer.clear();
		REQUIRE(writer.getLastError() == "");

		SECTION("writer is compatible after clearing")
		{
			REQUIRE(writer.isCompatible());
		}

		SECTION("writer is empty after clearing")
		{
			REQUIRE(writer.isEmpty());
		}

		writer.close();
		REQUIRE(writer.getLastError() == "");
	}

	TEST_CASE("Testing SourcetrailDBWriter records nodes")
	{
		const std::string databasePath = "testing.db";

		std::shared_ptr<DatabaseStorage> storage = DatabaseStorage::openDatabase(databasePath);

		SourcetrailDBWriter writer;
		REQUIRE(writer.getLastError() == "");

		writer.open(databasePath);
		REQUIRE(writer.getLastError() == "");

		writer.clear();
		REQUIRE(writer.getLastError() == "");

		const NameHierarchy nameSymbol1({ "." ,{ { "void", "foo", "()" } } });
		const int idSymbol1 = writer.recordSymbol(nameSymbol1);
		REQUIRE(idSymbol1 != 0);
		REQUIRE(writer.getLastError() == "");

		SECTION("database contains node after recording simple symbol")
		{
			const std::vector<StorageNode> nodes = storage->getAll<StorageNode>();
			REQUIRE(nodes.size() == 1);
			REQUIRE(nodes.front().id == idSymbol1);
			REQUIRE(nodes.front().serializedName == serializeNameHierarchyToDatabaseString(nameSymbol1));
			REQUIRE(nodes.front().nodeKind == nodeKindToInt(NodeKind::UNKNOWN));
		}

		SECTION("writer does not record node for symbol twice")
		{
			const int secondIdSymbol1 = writer.recordSymbol(nameSymbol1);
			REQUIRE(secondIdSymbol1 == idSymbol1);
			REQUIRE(writer.getLastError() == "");

			const std::vector<StorageNode> nodes = storage->getAll<StorageNode>();
			REQUIRE(nodes.size() == 1);
		}

		SECTION("writer records \"explicit\" symbol definition kind")
		{
			const bool success = writer.recordSymbolDefinitionKind(idSymbol1, DefinitionKind::EXPLICIT);
			REQUIRE(success);
			REQUIRE(writer.getLastError() == "");

			const std::vector<StorageSymbol> symbols = storage->getAll<StorageSymbol>();
			REQUIRE(symbols.size() == 1);
			REQUIRE(symbols.front().definitionKind == definitionKindToInt(DefinitionKind::EXPLICIT));
		}

		SECTION("writer records \"class\" symbol kind")
		{
			const bool success = writer.recordSymbolKind(idSymbol1, SymbolKind::CLASS);
			REQUIRE(success);
			REQUIRE(writer.getLastError() == "");

			const std::vector<StorageNode> nodes = storage->getAll<StorageNode>();
			REQUIRE(nodes.size() == 1);
			REQUIRE(nodes.front().nodeKind == nodeKindToInt(NodeKind::CLASS));
		}

		SECTION("writer records symbol locations")
		{
			const std::string filePath = "path/to/non_existing_file.cpp";
			const int fileId = writer.recordFile(filePath);
			const int startLine = 1;
			const int startCol = 2;
			const int endLine = 3;
			const int endCol = 4;

			std::vector<StorageSourceLocation> sourceLocations;

			SECTION("writer records symbol token location")
			{
				const bool success = writer.recordSymbolLocation(
					idSymbol1,
					{ fileId, startLine, startCol, endLine, endCol }
				);
				REQUIRE(success);
				REQUIRE(writer.getLastError() == "");

				sourceLocations = storage->getAll<StorageSourceLocation>();
				REQUIRE(sourceLocations.size() == 1);
				REQUIRE(sourceLocations.front().locationKind == locationKindToInt(LocationKind::TOKEN));
			}

			SECTION("writer records symbol scope location")
			{
				const bool success = writer.recordSymbolScopeLocation(
					idSymbol1,
					{ fileId, startLine, startCol, endLine, endCol }
				);
				REQUIRE(success);
				REQUIRE(writer.getLastError() == "");

				sourceLocations = storage->getAll<StorageSourceLocation>();
				REQUIRE(sourceLocations.size() == 1);
				REQUIRE(sourceLocations.front().locationKind == locationKindToInt(LocationKind::SCOPE));
			}

			SECTION("writer records symbol signature location")
			{
				const bool success = writer.recordSymbolSignatureLocation(
					idSymbol1,
					{ fileId, startLine, startCol, endLine, endCol }
				);
				REQUIRE(success);
				REQUIRE(writer.getLastError() == "");

				sourceLocations = storage->getAll<StorageSourceLocation>();
				REQUIRE(sourceLocations.size() == 1);
				REQUIRE(sourceLocations.front().locationKind == locationKindToInt(LocationKind::SIGNATURE));
			}

			REQUIRE(sourceLocations.front().startLineNumber == startLine);
			REQUIRE(sourceLocations.front().startColumnNumber == startCol);
			REQUIRE(sourceLocations.front().endLineNumber == endLine);
			REQUIRE(sourceLocations.front().endColumnNumber == endCol);

			const std::vector<StorageOccurrence> occurrences = storage->getAll<StorageOccurrence>();
			REQUIRE(occurrences.size() == 1);
			REQUIRE(occurrences.front().elementId == idSymbol1);
			REQUIRE(occurrences.front().sourceLocationId == sourceLocations.front().id);

			const std::vector<StorageFile> files = storage->getAll<StorageFile>();
			REQUIRE(files.size() == 1);
			REQUIRE(files.front().filePath == filePath);
			REQUIRE(sourceLocations.front().fileNodeId == files.front().id);
		}

		writer.close();
		REQUIRE(writer.getLastError() == "");
	}

	TEST_CASE("Testing SourcetrailDBWriter records edges")
	{
		const std::string databasePath = "testing.db";

		std::shared_ptr<DatabaseStorage> storage = DatabaseStorage::openDatabase(databasePath);

		SourcetrailDBWriter writer;
		REQUIRE(writer.getLastError() == "");

		writer.open(databasePath);
		REQUIRE(writer.getLastError() == "");

		writer.clear();
		REQUIRE(writer.getLastError() == "");

		const NameHierarchy nameSymbol1({ "." ,{ { "void", "foo", "()" } } });
		const int idSymbol1 = writer.recordSymbol(nameSymbol1);
		REQUIRE(idSymbol1 != 0);
		REQUIRE(writer.getLastError() == "");

		const NameHierarchy nameSymbol2({ "." ,{ { "void", "bar", "()" } } });
		const int idSymbol2 = writer.recordSymbol(nameSymbol2);
		REQUIRE(idSymbol2 != 0);
		REQUIRE(writer.getLastError() == "");

		const ReferenceKind kindReference1 = ReferenceKind::CALL;
		const int idReference1 = writer.recordReference(idSymbol1, idSymbol2, kindReference1);
		REQUIRE(idReference1 != 0);
		REQUIRE(writer.getLastError() == "");

		SECTION("database contains edge after recording simple reference")
		{
			const std::vector<StorageEdge> edges = storage->getAll<StorageEdge>();
			REQUIRE(edges.size() == 1);
			REQUIRE(edges.front().id == idReference1);
			REQUIRE(edges.front().sourceNodeId == idSymbol1);
			REQUIRE(edges.front().targetNodeId == idSymbol2);
			REQUIRE(edges.front().edgeKind == edgeKindToInt(referenceKindToEdgeKind(kindReference1)));
		}

		SECTION("writer does not record edge for reference twice")
		{
			const int secondIdReference1 = writer.recordReference(idSymbol1, idSymbol2, ReferenceKind::CALL);
			REQUIRE(secondIdReference1 == idReference1);
			REQUIRE(writer.getLastError() == "");

			const std::vector<StorageEdge> edges = storage->getAll<StorageEdge>();
			REQUIRE(edges.size() == 1);
		}

		SECTION("writer records reference location")
		{
			const std::string filePath = "path/to/non_existing_file.cpp";
			const int fileId = writer.recordFile(filePath);
			const int startLine = 1;
			const int startCol = 2;
			const int endLine = 3;
			const int endCol = 4;

			const bool success = writer.recordReferenceLocation(
				idReference1,
				{ fileId, startLine, startCol, endLine, endCol }
			);
			REQUIRE(success);
			REQUIRE(writer.getLastError() == "");

			const std::vector<StorageSourceLocation> sourceLocations = storage->getAll<StorageSourceLocation>();
			REQUIRE(sourceLocations.size() == 1);
			REQUIRE(sourceLocations.front().locationKind == locationKindToInt(LocationKind::TOKEN));
			REQUIRE(sourceLocations.front().startLineNumber == startLine);
			REQUIRE(sourceLocations.front().startColumnNumber == startCol);
			REQUIRE(sourceLocations.front().endLineNumber == endLine);
			REQUIRE(sourceLocations.front().endColumnNumber == endCol);

			const std::vector<StorageOccurrence> occurrences = storage->getAll<StorageOccurrence>();
			REQUIRE(occurrences.size() == 1);
			REQUIRE(occurrences.front().elementId == idReference1);
			REQUIRE(occurrences.front().sourceLocationId == sourceLocations.front().id);

			const std::vector<StorageFile> files = storage->getAll<StorageFile>();
			REQUIRE(files.size() == 1);
			REQUIRE(files.front().filePath == filePath);
			REQUIRE(sourceLocations.front().fileNodeId == files.front().id);
		}

		writer.close();
		REQUIRE(writer.getLastError() == "");
	}

	TEST_CASE("Testing SourcetrailDBWriter records file")
	{
		const std::string databasePath = "testing.db";

		std::shared_ptr<DatabaseStorage> storage = DatabaseStorage::openDatabase(databasePath);

		SourcetrailDBWriter writer;
		REQUIRE(writer.getLastError() == "");

		writer.open(databasePath);
		REQUIRE(writer.getLastError() == "");

		writer.clear();
		REQUIRE(writer.getLastError() == "");

		const std::string filePath = "path/to/non_existing_file.cpp";
		const int idFile1 = writer.recordFile(filePath);
		REQUIRE(idFile1 != 0);
		REQUIRE(writer.getLastError() == "");

		SECTION("database contains file after recording file")
		{
			const std::vector<StorageFile> files = storage->getAll<StorageFile>();
			REQUIRE(files.size() == 1);
			REQUIRE(files.front().id == idFile1);
			REQUIRE(files.front().filePath == filePath);
			REQUIRE(files.front().languageIdentifier == "");
		}

		SECTION("writer does not record file twice")
		{
			const int secondIdFile1 = writer.recordFile(filePath);
			REQUIRE(secondIdFile1 == idFile1);
			REQUIRE(writer.getLastError() == "");

			const std::vector<StorageFile> files = storage->getAll<StorageFile>();
			REQUIRE(files.size() == 1);
		}

		SECTION("writer records file language")
		{
			const std::string languageIdentifier = "testlanguage";

			const bool success = writer.recordFileLanguage(
				idFile1,
				languageIdentifier
			);
			REQUIRE(success);
			REQUIRE(writer.getLastError() == "");

			const std::vector<StorageFile> files = storage->getAll<StorageFile>();
			REQUIRE(files.size() == 1);
			REQUIRE(files.front().id == idFile1);
			REQUIRE(files.front().languageIdentifier == languageIdentifier);
		}

		writer.close();
		REQUIRE(writer.getLastError() == "");
	}

	TEST_CASE("Testing SourcetrailDBWriter records local symbols")
	{
		const std::string databasePath = "testing.db";

		std::shared_ptr<DatabaseStorage> storage = DatabaseStorage::openDatabase(databasePath);

		SourcetrailDBWriter writer;
		REQUIRE(writer.getLastError() == "");

		writer.open(databasePath);
		REQUIRE(writer.getLastError() == "");

		writer.clear();
		REQUIRE(writer.getLastError() == "");

		const std::string localSymbolName1 = "foo";
		const int idLocalSymbol1 = writer.recordLocalSymbol(localSymbolName1);
		REQUIRE(idLocalSymbol1 != 0);
		REQUIRE(writer.getLastError() == "");

		SECTION("database contains local symbol after recording local symbol")
		{
			const std::vector<StorageLocalSymbol> localSymbols = storage->getAll<StorageLocalSymbol>();
			REQUIRE(localSymbols.size() == 1);
			REQUIRE(localSymbols.front().id == idLocalSymbol1);
			REQUIRE(localSymbols.front().name == localSymbolName1);
		}

		SECTION("writer does not record local symbol twice")
		{
			const int secondIdLocalSymbol1 = writer.recordLocalSymbol(localSymbolName1);
			REQUIRE(secondIdLocalSymbol1 == idLocalSymbol1);
			REQUIRE(writer.getLastError() == "");

			const std::vector<StorageLocalSymbol> localSymbols = storage->getAll<StorageLocalSymbol>();
			REQUIRE(localSymbols.size() == 1);
		}

		SECTION("writer records local symbol location")
		{
			const std::string filePath = "path/to/non_existing_file.cpp";
			const int fileId = writer.recordFile(filePath);
			const int startLine = 1;
			const int startCol = 2;
			const int endLine = 3;
			const int endCol = 4;

			const bool success = writer.recordLocalSymbolLocation(
				idLocalSymbol1,
				{ fileId, startLine, startCol, endLine, endCol }
			);
			REQUIRE(success);
			REQUIRE(writer.getLastError() == "");

			const std::vector<StorageSourceLocation> sourceLocations = storage->getAll<StorageSourceLocation>();
			REQUIRE(sourceLocations.size() == 1);
			REQUIRE(sourceLocations.front().locationKind == locationKindToInt(LocationKind::LOCAL_SYMBOL));
			REQUIRE(sourceLocations.front().startLineNumber == startLine);
			REQUIRE(sourceLocations.front().startColumnNumber == startCol);
			REQUIRE(sourceLocations.front().endLineNumber == endLine);
			REQUIRE(sourceLocations.front().endColumnNumber == endCol);

			const std::vector<StorageOccurrence> occurrences = storage->getAll<StorageOccurrence>();
			REQUIRE(occurrences.size() == 1);
			REQUIRE(occurrences.front().elementId == idLocalSymbol1);
			REQUIRE(occurrences.front().sourceLocationId == sourceLocations.front().id);

			const std::vector<StorageFile> files = storage->getAll<StorageFile>();
			REQUIRE(files.size() == 1);
			REQUIRE(files.front().filePath == filePath);
			REQUIRE(sourceLocations.front().fileNodeId == files.front().id);
		}

		writer.close();
		REQUIRE(writer.getLastError() == "");
	}

	TEST_CASE("Testing SourcetrailDBWriter records comment locations")
	{
		const std::string databasePath = "testing.db";

		std::shared_ptr<DatabaseStorage> storage = DatabaseStorage::openDatabase(databasePath);

		SourcetrailDBWriter writer;
		REQUIRE(writer.getLastError() == "");

		writer.open(databasePath);
		REQUIRE(writer.getLastError() == "");

		writer.clear();
		REQUIRE(writer.getLastError() == "");

		const std::string filePath = "path/to/non_existing_file.cpp";
		const int fileId = writer.recordFile(filePath);
		const int startLine = 1;
		const int startCol = 2;
		const int endLine = 3;
		const int endCol = 4;

		const bool success1 = writer.recordCommentLocation(
			{ fileId, startLine, startCol, endLine, endCol }
		);
		REQUIRE(success1);
		REQUIRE(writer.getLastError() == "");

		SECTION("database contains comment location after recording local symbol")
		{
			const std::vector<StorageSourceLocation> sourceLocations = storage->getAll<StorageSourceLocation>();
			REQUIRE(sourceLocations.size() == 1);
			REQUIRE(sourceLocations.front().locationKind == locationKindToInt(LocationKind::COMMENT));
			REQUIRE(sourceLocations.front().startLineNumber == startLine);
			REQUIRE(sourceLocations.front().startColumnNumber == startCol);
			REQUIRE(sourceLocations.front().endLineNumber == endLine);
			REQUIRE(sourceLocations.front().endColumnNumber == endCol);

			const std::vector<StorageFile> files = storage->getAll<StorageFile>();
			REQUIRE(files.size() == 1);
			REQUIRE(files.front().filePath == filePath);
			REQUIRE(sourceLocations.front().fileNodeId == files.front().id);
		}

		SECTION("writer does not record comment location twice")
		{
			const bool success2 = writer.recordCommentLocation(
				{ fileId, startLine, startCol, endLine, endCol }
			);
			REQUIRE(success2);
			REQUIRE(writer.getLastError() == "");

			const std::vector<StorageSourceLocation> sourceLocations = storage->getAll<StorageSourceLocation>();
			REQUIRE(sourceLocations.size() == 1);
		}

		writer.close();
		REQUIRE(writer.getLastError() == "");
	}

	TEST_CASE("Testing SourcetrailDBWriter records errors")
	{
		const std::string databasePath = "testing.db";

		std::shared_ptr<DatabaseStorage> storage = DatabaseStorage::openDatabase(databasePath);

		SourcetrailDBWriter writer;
		REQUIRE(writer.getLastError() == "");

		writer.open(databasePath);
		REQUIRE(writer.getLastError() == "");

		writer.clear();
		REQUIRE(writer.getLastError() == "");

		const std::string message = "This is a very serious test error message.";
		const bool fatal = false;
		const std::string filePath = "path/to/non_existing_file.cpp";
		const int fileId = writer.recordFile(filePath);
		const int startLine = 1;
		const int startCol = 2;
		const int endLine = 3;
		const int endCol = 4;

		const bool success1 = writer.recordError(
			message,
			fatal,
			{ fileId, startLine, startCol, endLine, endCol }
		);
		REQUIRE(success1);
		REQUIRE(writer.getLastError() == "");

		SECTION("database contains error after recording error")
		{
			const std::vector<StorageError> errors = storage->getAll<StorageError>();
			REQUIRE(errors.size() == 1);
			REQUIRE(errors.front().message == message);
			REQUIRE(errors.front().fatal == fatal);

			const std::vector<StorageSourceLocation> sourceLocations = storage->getAll<StorageSourceLocation>();
			REQUIRE(sourceLocations.size() == 1);
			REQUIRE(sourceLocations.front().locationKind == locationKindToInt(LocationKind::INDEXER_ERROR));
			REQUIRE(sourceLocations.front().startLineNumber == startLine);
			REQUIRE(sourceLocations.front().startColumnNumber == startCol);
			REQUIRE(sourceLocations.front().endLineNumber == endLine);
			REQUIRE(sourceLocations.front().endColumnNumber == endCol);

			const std::vector<StorageOccurrence> occurrences = storage->getAll<StorageOccurrence>();
			REQUIRE(occurrences.size() == 1);
			REQUIRE(occurrences.front().elementId == errors.front().id);
			REQUIRE(occurrences.front().sourceLocationId == sourceLocations.front().id);

			const std::vector<StorageFile> files = storage->getAll<StorageFile>();
			REQUIRE(files.size() == 1);
			REQUIRE(files.front().filePath == filePath);
			REQUIRE(sourceLocations.front().fileNodeId == files.front().id);
		}

		SECTION("writer does not record error twice")
		{
			const bool success2 = writer.recordError(
				message,
				fatal,
				{ fileId, startLine, startCol, endLine, endCol }
			);
			REQUIRE(success2);
			REQUIRE(writer.getLastError() == "");

			const std::vector<StorageError> errors = storage->getAll<StorageError>();
			REQUIRE(errors.size() == 1);
		}

		writer.close();
		REQUIRE(writer.getLastError() == "");
	}
}
