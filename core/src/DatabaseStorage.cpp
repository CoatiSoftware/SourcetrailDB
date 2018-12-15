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

#include "DatabaseStorage.h"

#include <vector>

#include "SourcetrailException.h"
#include "StorageFile.h"
#include "StorageNode.h"
#include "StorageSymbol.h"
#include "NodeKind.h"
#include "utility.h"
#include "version.h"

namespace sourcetrail
{
	// --- Public Interface ---

	int DatabaseStorage::getSupportedDatabaseVersion()
	{
		return DATABASE_VERSION;
	}

	std::shared_ptr<DatabaseStorage> DatabaseStorage::openDatabase(const std::string& dbFilePath)
	{
		try
		{
			std::shared_ptr<DatabaseStorage> storage = std::shared_ptr<DatabaseStorage>(new DatabaseStorage());
			storage->m_database.open(dbFilePath.c_str());
			storage->executeStatement("PRAGMA foreign_keys=ON;");
			return storage;
		}
		catch (CppSQLite3Exception e)
		{
			throw SourcetrailException(e.errorMessage());
		}
	}

	DatabaseStorage::~DatabaseStorage()
	{
		m_insertElementStatement.finalize();
		m_findNodeStatement.finalize();
		m_insertNodeStatement.finalize();
		m_setNodeTypeStmt.finalize();
		m_insertSymbolStatement.finalize();
		m_findFileStatement.finalize();
		m_insertFileStatement.finalize();
		m_setFileLanguageStmt.finalize();
		m_insertFileContentStatement.finalize();
		m_findEdgeStatement.finalize();
		m_insertEdgeStatement.finalize();
		m_findLocalSymbolStmt.finalize();
		m_insertLocalSymbolStmt.finalize();
		m_findSourceLocationStmt.finalize();
		m_insertSourceLocationStmt.finalize();
		m_insertOccurenceStmt.finalize();
		m_findErrorStatement.finalize();
		m_insertErrorStatement.finalize();
		m_insertOrUpdateMetaValueStmt.finalize();

		m_database.close();
	}

	void DatabaseStorage::setupDatabase()
	{
		executeStatement("PRAGMA foreign_keys=ON;");

		if (!isCompatible())
		{
			throw SourcetrailException("Unable to setup database tables because database is not compatible.");
		}

		setupTables();

		setupIndices();

		setupPrecompiledStatements();

		insertOrUpdateMetaValue("storage_version", std::to_string(getSupportedDatabaseVersion()));
	}

	void DatabaseStorage::clearDatabase()
	{
		executeStatement("PRAGMA foreign_keys=OFF;");

		const std::vector<std::string> tableNames = {
			"meta",
			"error",
			"component_access",
			"occurrence",
			"source_location",
			"local_symbol",
			"filecontent",
			"file",
			"symbol",
			"node",
			"edge",
			"element"
		};

		for (const std::string& tableName : tableNames)
		{
			executeStatement("DROP TABLE IF EXISTS main." + tableName + ";");
		}

		setupDatabase();
	}

	bool DatabaseStorage::isEmpty() const
	{
		const std::string tableName = "meta";
		CppSQLite3Query q = executeQuery(
			"SELECT name FROM sqlite_master WHERE type='table' AND name='" + tableName + "';"
		);

		if (!q.eof())
		{
			return q.getStringField(0, "") == tableName;
		}

		return true;
	}

	bool DatabaseStorage::isCompatible() const
	{
		if (isEmpty())
		{
			return true;
		}

		return getLoadedDatabaseVersion() == getSupportedDatabaseVersion();
	}

	int DatabaseStorage::getLoadedDatabaseVersion() const
	{
		if (isEmpty())
		{
			throw SourcetrailException("Unable to determine version of an empty database.");
		}

		CppSQLite3Query q = executeQuery("SELECT value FROM meta WHERE key = 'storage_version';");
		if (!q.eof())
		{
			return std::stoi(q.getStringField(0, "0"));
		}
		return 0;
	}

	void DatabaseStorage::beginTransaction()
	{
		executeStatement("BEGIN TRANSACTION;");
	}

	void DatabaseStorage::commitTransaction()
	{
		executeStatement("COMMIT TRANSACTION;");
	}

	void DatabaseStorage::rollbackTransaction()
	{
		executeStatement("ROLLBACK TRANSACTION;");
	}

	void DatabaseStorage::optimizeDatabaseMemory()
	{
		executeStatement("VACUUM;");
	}

	int DatabaseStorage::addNode(const StorageNodeData& storageNodeData)
	{
		int id = 0;

		{
			m_findNodeStatement.bind(1, storageNodeData.serializedName.c_str());
			CppSQLite3Query q = executeQuery(m_findNodeStatement);
			if (!q.eof())
			{
				id = q.getIntField(0, 0);
			}
			m_findNodeStatement.reset();
		}

		// FIXME: update node nodeKind here

		if (id == 0)
		{
			id = insertElement();

			m_insertNodeStatement.bind(1, id);
			m_insertNodeStatement.bind(2, storageNodeData.nodeKind);
			m_insertNodeStatement.bind(3, storageNodeData.serializedName.c_str());
			executeStatement(m_insertNodeStatement);
			m_insertNodeStatement.reset();
		}
		return id;
	}

	void DatabaseStorage::addSymbol(const StorageSymbol& storageSymbol)
	{
		m_insertSymbolStatement.bind(1, storageSymbol.id);
		m_insertSymbolStatement.bind(2, storageSymbol.definitionKind);
		executeStatement(m_insertSymbolStatement);
		m_insertSymbolStatement.reset();
	}

	void DatabaseStorage::addFile(const StorageFile& storageFile)
	{
		{
			m_findFileStatement.bind(1, storageFile.id);
			CppSQLite3Query q = executeQuery(m_findFileStatement);
			bool exists = !q.eof();
			m_findFileStatement.reset();
			if (exists)
			{
				return;
			}
		}

		std::string content = "";
		if (utility::getFileExists(storageFile.filePath))
		{
			content = utility::getFileContent(storageFile.filePath);
		}
		const int lineCount = utility::getLineCount(content);

		{
			m_insertFileStatement.bind(1, storageFile.id);
			m_insertFileStatement.bind(2, storageFile.filePath.c_str());
			m_insertFileStatement.bind(3, storageFile.languageIdentifier.c_str());
			m_insertFileStatement.bind(4, storageFile.modificationTime.c_str());
			m_insertFileStatement.bind(5, storageFile.indexed);
			m_insertFileStatement.bind(6, storageFile.complete);
			m_insertFileStatement.bind(7, lineCount);
			executeStatement(m_insertFileStatement);
			m_insertFileStatement.reset();
		}

		if (!content.empty())
		{
			m_insertFileContentStatement.bind(1, storageFile.id);
			m_insertFileContentStatement.bind(2, content.c_str());
			executeStatement(m_insertFileContentStatement);
			m_insertFileContentStatement.reset();
		}
	}

	int DatabaseStorage::addEdge(const StorageEdgeData& storageEdgeData)
	{
		int id = 0;

		{
			m_findEdgeStatement.bind(1, storageEdgeData.sourceNodeId);
			m_findEdgeStatement.bind(2, storageEdgeData.targetNodeId);
			m_findEdgeStatement.bind(3, storageEdgeData.edgeKind);
			CppSQLite3Query q = executeQuery(m_findEdgeStatement);
			if (!q.eof())
			{
				id = q.getIntField(0, 0);
			}
			m_findEdgeStatement.reset();
		}

		if (id == 0)
		{
			id = insertElement();

			m_insertEdgeStatement.bind(1, id);
			m_insertEdgeStatement.bind(2, storageEdgeData.edgeKind);
			m_insertEdgeStatement.bind(3, storageEdgeData.sourceNodeId);
			m_insertEdgeStatement.bind(4, storageEdgeData.targetNodeId);
			executeStatement(m_insertEdgeStatement);
			m_insertEdgeStatement.reset();
		}
		return id;
	}

	int DatabaseStorage::addLocalSymbol(const StorageLocalSymbolData& storageLocalSymbolData)
	{
		int id = 0;

		{
			m_findLocalSymbolStmt.bind(1, storageLocalSymbolData.name.c_str());
			CppSQLite3Query q = executeQuery(m_findLocalSymbolStmt);
			if (!q.eof())
			{
				id = q.getIntField(0, 0);
			}
			m_findLocalSymbolStmt.reset();
		}

		if (id == 0)
		{
			id = insertElement();

			m_insertLocalSymbolStmt.bind(1, id);
			m_insertLocalSymbolStmt.bind(2, storageLocalSymbolData.name.c_str());
			executeStatement(m_insertLocalSymbolStmt);
			m_insertLocalSymbolStmt.reset();
		}
		return id;
	}

	int DatabaseStorage::addSourceLocation(const StorageSourceLocationData& storageSourceLocationData)
	{
		int id = 0;

		{
			m_findSourceLocationStmt.bind(1, storageSourceLocationData.fileNodeId);
			m_findSourceLocationStmt.bind(2, storageSourceLocationData.startLineNumber);
			m_findSourceLocationStmt.bind(3, storageSourceLocationData.startColumnNumber);
			m_findSourceLocationStmt.bind(4, storageSourceLocationData.endLineNumber);
			m_findSourceLocationStmt.bind(5, storageSourceLocationData.endColumnNumber);
			m_findSourceLocationStmt.bind(6, storageSourceLocationData.locationKind);
			CppSQLite3Query q = executeQuery(m_findSourceLocationStmt);
			if (!q.eof())
			{
				id = q.getIntField(0, 0);
			}
			m_findSourceLocationStmt.reset();
		}

		if (id == 0)
		{
			m_insertSourceLocationStmt.bind(1, storageSourceLocationData.fileNodeId);
			m_insertSourceLocationStmt.bind(2, storageSourceLocationData.startLineNumber);
			m_insertSourceLocationStmt.bind(3, storageSourceLocationData.startColumnNumber);
			m_insertSourceLocationStmt.bind(4, storageSourceLocationData.endLineNumber);
			m_insertSourceLocationStmt.bind(5, storageSourceLocationData.endColumnNumber);
			m_insertSourceLocationStmt.bind(6, storageSourceLocationData.locationKind);
			executeStatement(m_insertSourceLocationStmt);
			id = m_database.lastRowId();
			m_insertSourceLocationStmt.reset();
		}
		return id;
	}

	void DatabaseStorage::addOccurrence(const StorageOccurrence& storageOccurrence)
	{
		m_insertOccurenceStmt.bind(1, storageOccurrence.elementId);
		m_insertOccurenceStmt.bind(2, storageOccurrence.sourceLocationId);
		executeStatement(m_insertOccurenceStmt);
		m_insertOccurenceStmt.reset();
	}

	int DatabaseStorage::addError(const StorageErrorData& storageErrorData)
	{
		int id = 0;
		{
			m_findErrorStatement.bind(1, storageErrorData.message.c_str());
			m_findErrorStatement.bind(2, storageErrorData.fatal);
			CppSQLite3Query q = executeQuery(m_findErrorStatement);
			if (!q.eof() && q.numFields() > 0)
			{
				id = q.getIntField(0, -1);
			}
			m_findErrorStatement.reset();
		}

		if (id == 0)
		{
			id = insertElement();

			m_insertErrorStatement.bind(1, id);
			m_insertErrorStatement.bind(2, storageErrorData.message.c_str());
			m_insertErrorStatement.bind(3, storageErrorData.fatal);
			m_insertErrorStatement.bind(4, storageErrorData.indexed);
			m_insertErrorStatement.bind(5, storageErrorData.translationUnit.c_str());
			executeStatement(m_insertErrorStatement);
			id = m_database.lastRowId();
			m_insertErrorStatement.reset();
		}
		return id;
	}

	void DatabaseStorage::setNodeType(int nodeId, int nodeType)
	{
		m_setNodeTypeStmt.bind(1, nodeType);
		m_setNodeTypeStmt.bind(2, nodeId);
		executeStatement(m_setNodeTypeStmt);
		m_setNodeTypeStmt.reset();
	}

	void DatabaseStorage::setFileLanguage(int fileId, const std::string& languageIdentifier)
	{
		m_setFileLanguageStmt.bind(1, languageIdentifier.c_str());
		m_setFileLanguageStmt.bind(2, fileId);
		executeStatement(m_setFileLanguageStmt);
		m_setFileLanguageStmt.reset();
	}

	// --- Private Interface ---

	void DatabaseStorage::setupTables()
	{
		executeStatement(
			"CREATE TABLE IF NOT EXISTS meta("
			"	id INTEGER, "
			"	key TEXT, "
			"	value TEXT, "
			"	PRIMARY KEY(id)"
			");"
		);

		executeStatement(
			"CREATE TABLE IF NOT EXISTS element("
			"	id INTEGER, "
			"	PRIMARY KEY(id)"
			");"
		);

		executeStatement(
			"CREATE TABLE IF NOT EXISTS edge("
			"	id INTEGER NOT NULL, "
			"	type INTEGER NOT NULL, "
			"	source_node_id INTEGER NOT NULL, "
			"	target_node_id INTEGER NOT NULL, "
			"	PRIMARY KEY(id), "
			"	FOREIGN KEY(id) REFERENCES element(id) ON DELETE CASCADE, "
			"	FOREIGN KEY(source_node_id) REFERENCES node(id) ON DELETE CASCADE, "
			"	FOREIGN KEY(target_node_id) REFERENCES node(id) ON DELETE CASCADE"
			");"
		);

		executeStatement(
			"CREATE TABLE IF NOT EXISTS node("
			"	id INTEGER NOT NULL, "
			"	type INTEGER NOT NULL, "
			"	serialized_name TEXT, "
			"	PRIMARY KEY(id), "
			"	FOREIGN KEY(id) REFERENCES element(id) ON DELETE CASCADE"
			");"
		);

		executeStatement(
			"CREATE TABLE IF NOT EXISTS symbol("
			"	id INTEGER NOT NULL, "
			"	definition_kind INTEGER NOT NULL, "
			"	PRIMARY KEY(id), "
			"	FOREIGN KEY(id) REFERENCES node(id) ON DELETE CASCADE"
			");"
		);

		executeStatement(
			"CREATE TABLE IF NOT EXISTS file("
			"	id INTEGER NOT NULL, "
			"	path TEXT, "
			"	language TEXT, "
			"	modification_time TEXT, "
			"	indexed INTEGER, "
			"	complete INTEGER, "
			"	line_count INTEGER, "
			"	PRIMARY KEY(id), "
			"	FOREIGN KEY(id) REFERENCES node(id) ON DELETE CASCADE"
			");"
		);

		executeStatement(
			"CREATE TABLE IF NOT EXISTS filecontent("
			"	id INTERGER, "
			"	content TEXT, "
			"	FOREIGN KEY(id) REFERENCES file(id)"
			"	ON DELETE CASCADE "
			"	ON UPDATE CASCADE"
			");"
		);

		executeStatement(
			"CREATE TABLE IF NOT EXISTS local_symbol("
			"	id INTEGER NOT NULL, "
			"	name TEXT, "
			"	PRIMARY KEY(id), "
			"	FOREIGN KEY(id) REFERENCES element(id) ON DELETE CASCADE"
			");"
		);

		executeStatement(
			"CREATE TABLE IF NOT EXISTS source_location("
			"	id INTEGER NOT NULL, "
			"	file_node_id INTEGER, "
			"	start_line INTEGER, "
			"	start_column INTEGER, "
			"	end_line INTEGER, "
			"	end_column INTEGER, "
			"	type INTEGER, "
			"	PRIMARY KEY(id), "
			"	FOREIGN KEY(file_node_id) REFERENCES node(id) ON DELETE CASCADE"
			");"
		);

		executeStatement(
			"CREATE TABLE IF NOT EXISTS occurrence("
			"	element_id INTEGER NOT NULL, "
			"	source_location_id INTEGER NOT NULL, "
			"	PRIMARY KEY(element_id, source_location_id), "
			"	FOREIGN KEY(element_id) REFERENCES element(id) ON DELETE CASCADE, "
			"	FOREIGN KEY(source_location_id) REFERENCES source_location(id) ON DELETE CASCADE"
			");"
		);

		executeStatement(
			"CREATE TABLE IF NOT EXISTS component_access("
			"	node_id INTEGER NOT NULL, "
			"	type INTEGER NOT NULL, "
			"	PRIMARY KEY(node_id), "
			"	FOREIGN KEY(node_id) REFERENCES node(id) ON DELETE CASCADE"
			");"
		);

		executeStatement(
			"CREATE TABLE IF NOT EXISTS error("
			"	id INTEGER NOT NULL, "
			"	message TEXT, "
			"	fatal INTEGER NOT NULL, "
			"	indexed INTEGER NOT NULL, "
			"	translation_unit TEXT, "
			"	PRIMARY KEY(id), "
			"	FOREIGN KEY(id) REFERENCES element(id) ON DELETE CASCADE"
			");"
		);
	}

	void DatabaseStorage::setupIndices()
	{
		executeStatement(
			"CREATE INDEX IF NOT EXISTS node_serialized_name_index ON node(serialized_name);"
		);

		executeStatement(
			"CREATE INDEX IF NOT EXISTS edge_source_target_type_index ON edge(source_node_id, target_node_id, type);"
		);

		executeStatement(
			"CREATE INDEX IF NOT EXISTS local_symbol_name_index ON local_symbol(name);"
		);

		executeStatement(
			"CREATE INDEX IF NOT EXISTS source_location_all_data_index "
			"ON source_location(file_node_id, start_line, start_column, end_line, end_column, type);"
		);

		executeStatement(
			"CREATE INDEX IF NOT EXISTS error_all_data_index ON error(message, fatal);"
		);
	}

	void DatabaseStorage::setupPrecompiledStatements()
	{
		m_insertElementStatement = compileStatement(
			"INSERT INTO element(id) VALUES(NULL);"
		);

		m_findNodeStatement = compileStatement(
			"SELECT id FROM node WHERE serialized_name == ? LIMIT 1;"
		);

		m_insertNodeStatement = compileStatement(
			"INSERT INTO node(id, type, serialized_name) VALUES(?, ?, ?);"
		);

		m_setNodeTypeStmt = compileStatement(
			"UPDATE node SET type = ? WHERE id == ?;"
		);

		m_insertSymbolStatement = compileStatement(
			"INSERT OR IGNORE INTO symbol(id, definition_kind) VALUES(?, ?);"
		);

		m_findFileStatement = compileStatement(
			"SELECT id FROM file WHERE id == ?;"
		);

		m_insertFileStatement = compileStatement(
			"INSERT OR IGNORE INTO file(id, path, language, modification_time, indexed, complete, line_count) VALUES(?, ?, ?, ?, ?, ?, ?);"
		);

		m_setFileLanguageStmt = compileStatement(
			"UPDATE file SET language = ? WHERE id == ?;"
		);

		m_insertFileContentStatement = compileStatement(
			"INSERT INTO filecontent(id, content) VALUES(?, ?);"
		);

		m_findEdgeStatement = compileStatement(
			"SELECT id FROM edge WHERE source_node_id == ? AND target_node_id == ? AND type == ? LIMIT 1;"
		);

		m_insertEdgeStatement = compileStatement(
			"INSERT INTO edge(id, type, source_node_id, target_node_id) VALUES(?, ?, ?, ?);"
		);

		m_findLocalSymbolStmt = compileStatement(
			"SELECT id FROM local_symbol WHERE name == ? LIMIT 1;"
		);

		m_insertLocalSymbolStmt = compileStatement(
			"INSERT INTO local_symbol(id, name) VALUES(?, ?);"
		);

		m_findSourceLocationStmt = compileStatement(
			"SELECT id FROM source_location WHERE "
			"file_node_id = ? AND "
			"start_line = ? AND "
			"start_column = ? AND "
			"end_line = ? AND "
			"end_column = ? AND "
			"type = ? "
			"LIMIT 1;"
		);

		m_insertSourceLocationStmt = compileStatement(
			"INSERT INTO source_location("
				"id, file_node_id, start_line, start_column, end_line, end_column, type) "
			"VALUES(NULL, ?, ?, ?, ?, ?, ?);"
		);

		m_insertOccurenceStmt = compileStatement(
			"INSERT OR IGNORE INTO occurrence(element_id, source_location_id) VALUES(?, ?);"
		);

		m_findErrorStatement = compileStatement(
			"SELECT id FROM error WHERE "
			"message = ? AND "
			"fatal == ? "
			"LIMIT 1;"
		);

		m_insertErrorStatement = compileStatement(
			"INSERT INTO error(id, message, fatal, indexed, translation_unit) "
			"VALUES(?, ?, ?, ?, ?);"
		);

		m_insertOrUpdateMetaValueStmt = compileStatement(
			"INSERT OR REPLACE INTO meta(id, key, value) VALUES("
				"(SELECT id FROM meta WHERE key = ?), ?, ?"
			");"
		);
	}

	int DatabaseStorage::insertElement()
	{
		executeStatement(m_insertElementStatement);
		int id = m_database.lastRowId();
		m_insertElementStatement.reset();
		return id;
	}

	void DatabaseStorage::insertOrUpdateMetaValue(const std::string& key, const std::string& value)
	{
		m_insertOrUpdateMetaValueStmt.bind(1, key.c_str());
		m_insertOrUpdateMetaValueStmt.bind(2, key.c_str());
		m_insertOrUpdateMetaValueStmt.bind(3, value.c_str());
		executeStatement(m_insertOrUpdateMetaValueStmt);
		m_insertOrUpdateMetaValueStmt.reset();
	}

	CppSQLite3Statement DatabaseStorage::compileStatement(const std::string& statement) const
	{
		try
		{
			return m_database.compileStatement(statement.c_str());
		}
		catch (CppSQLite3Exception e)
		{
			throw SourcetrailException("Failed to compile statement \"" + statement + "\" with message \"" + e.errorMessage() + "\".");
		}

		return CppSQLite3Statement();
	}

	void DatabaseStorage::executeStatement(const std::string& statement) const
	{
		try
		{
			m_database.execDML(statement.c_str());
		}
		catch (CppSQLite3Exception e)
		{
			throw SourcetrailException("Failed to execute statement \"" + statement + "\" with message \"" + e.errorMessage() + "\".");
		}
	}

	void DatabaseStorage::executeStatement(CppSQLite3Statement& statement) const
	{
		try
		{
			statement.execDML();
		}
		catch (CppSQLite3Exception e)
		{
			throw SourcetrailException("Failed to execute statement with message \"" + std::string(e.errorMessage()) + "\".");
		}
	}

	CppSQLite3Query DatabaseStorage::executeQuery(const std::string& query) const
	{
		try
		{
			return m_database.execQuery(query.c_str());
		}
		catch (CppSQLite3Exception e)
		{
			throw SourcetrailException("Failed to execute query \"" + query + "\" with message \"" + e.errorMessage() + "\".");
		}
	}

	CppSQLite3Query DatabaseStorage::executeQuery(CppSQLite3Statement& statement) const
	{
		try
		{
			return statement.execQuery();
		}
		catch (CppSQLite3Exception e)
		{
			throw SourcetrailException("Failed to execute query with message \"" + std::string(e.errorMessage()) + "\".");
		}
	}

	template <>
	std::vector<StorageEdge> DatabaseStorage::doGetAll<StorageEdge>(const std::string& query) const
	{
		CppSQLite3Query q = executeQuery(
			"SELECT id, type, source_node_id, target_node_id FROM edge " + query + ";"
		);

		std::vector<StorageEdge> edges;
		while (!q.eof())
		{
			const int id = q.getIntField(0, 0);
			const int edgeKind = q.getIntField(1, -1);
			const int sourceId = q.getIntField(2, 0);
			const int targetId = q.getIntField(3, 0);

			if (id != 0 && edgeKind != -1)
			{
				edges.emplace_back(StorageEdge(id, sourceId, targetId, edgeKind));
			}

			q.nextRow();
		}
		return edges;
	}

	template <>
	std::vector<StorageNode> DatabaseStorage::doGetAll<StorageNode>(const std::string& query) const
	{
		CppSQLite3Query q = executeQuery(
			"SELECT id, type, serialized_name FROM node " + query + ";"
		);

		std::vector<StorageNode> nodes;
		while (!q.eof())
		{
			const int id = q.getIntField(0, 0);
			const int type = q.getIntField(1, -1);
			const std::string serializedName = q.getStringField(2, "");

			if (id != 0 && type != -1)
			{
				nodes.emplace_back(StorageNode(id, type, serializedName));
			}

			q.nextRow();
		}
		return nodes;
	}

	template <>
	std::vector<StorageSymbol> DatabaseStorage::doGetAll<StorageSymbol>(const std::string& query) const
	{
		CppSQLite3Query q = executeQuery(
			"SELECT id, definition_kind FROM symbol " + query + ";"
		);

		std::vector<StorageSymbol> symbols;
		while (!q.eof())
		{
			const int id = q.getIntField(0, 0);
			const int definitionKind = q.getIntField(1, 0);

			if (id != 0)
			{
				symbols.emplace_back(StorageSymbol(id, definitionKind));
			}

			q.nextRow();
		}
		return symbols;
	}

	template <>
	std::vector<StorageFile> DatabaseStorage::doGetAll<StorageFile>(const std::string& query) const
	{
		CppSQLite3Query q = executeQuery(
			"SELECT id, path, language, modification_time, indexed, complete FROM file " + query + ";"
		);

		std::vector<StorageFile> files;
		while (!q.eof())
		{
			const int id = q.getIntField(0, 0);
			const std::string filePath = q.getStringField(1, "");
			const std::string languageIdentifier = q.getStringField(2, "");
			const std::string modificationTime = q.getStringField(3, "");
			const bool indexed = q.getIntField(4, 0);
			const bool complete = q.getIntField(5, 0);

			if (id != 0)
			{
				files.emplace_back(StorageFile(id, filePath, languageIdentifier, modificationTime, indexed, complete));
			}
			q.nextRow();
		}

		return files;
	}

	template <>
	std::vector<StorageLocalSymbol> DatabaseStorage::doGetAll<StorageLocalSymbol>(const std::string& query) const
	{
		CppSQLite3Query q = executeQuery(
			"SELECT id, name FROM local_symbol " + query + ";"
		);

		std::vector<StorageLocalSymbol> localSymbols;

		while (!q.eof())
		{
			const int id = q.getIntField(0, 0);
			const std::string name = q.getStringField(1, "");

			if (id != 0)
			{
				localSymbols.emplace_back(StorageLocalSymbol(id, name));
			}

			q.nextRow();
		}
		return localSymbols;
	}

	template <>
	std::vector<StorageSourceLocation> DatabaseStorage::doGetAll<StorageSourceLocation>(const std::string& query) const
	{
		CppSQLite3Query q = executeQuery(
			"SELECT id, file_node_id, start_line, start_column, end_line, end_column, type FROM source_location " + query + ";"
		);

		std::vector<StorageSourceLocation> sourceLocations;

		while (!q.eof())
		{
			const int id = q.getIntField(0, 0);
			const int fileNodeId = q.getIntField(1, 0);
			const int startLineNumber = q.getIntField(2, -1);
			const int startColNumber = q.getIntField(3, -1);
			const int endLineNumber = q.getIntField(4, -1);
			const int endColNumber = q.getIntField(5, -1);
			const int locationKind = q.getIntField(6, -1);

			if (id != 0 && fileNodeId != 0 && startLineNumber != -1 && startColNumber != -1 && endLineNumber != -1 &&
				endColNumber != -1 && locationKind != -1)
			{
				sourceLocations.emplace_back(StorageSourceLocation(id, fileNodeId, startLineNumber, startColNumber, endLineNumber, endColNumber, locationKind));
			}

			q.nextRow();
		}
		return sourceLocations;
	}

	template <>
	std::vector<StorageOccurrence> DatabaseStorage::doGetAll<StorageOccurrence>(const std::string& query) const
	{
		CppSQLite3Query q = executeQuery(
			"SELECT element_id, source_location_id FROM occurrence " + query + ";"
		);

		std::vector<StorageOccurrence> occurrences;

		while (!q.eof())
		{
			const int elementId = q.getIntField(0, 0);
			const int sourceLocationId = q.getIntField(1, 0);

			if (elementId != 0 && sourceLocationId != 0)
			{
				occurrences.emplace_back(StorageOccurrence(elementId, sourceLocationId));
			}

			q.nextRow();
		}
		return occurrences;
	}

	template <>
	std::vector<StorageError> DatabaseStorage::doGetAll<StorageError>(const std::string& query) const
	{
		CppSQLite3Query q = executeQuery(
			"SELECT id, message, fatal, indexed, translation_unit FROM error " + query + ";"
		);

		std::vector<StorageError> errors;
		while (!q.eof())
		{
			const int id = q.getIntField(0, 0);
			const std::string message = q.getStringField(1, "");
			const bool fatal = q.getIntField(2, 0);
			const bool indexed = q.getIntField(3, 0);
			const std::string translationUnit = q.getStringField(4, "");

			if (id != 0)
			{
				errors.emplace_back(StorageError(id, message, translationUnit, fatal, indexed));
			}

			q.nextRow();
		}

		return errors;
	}

}
