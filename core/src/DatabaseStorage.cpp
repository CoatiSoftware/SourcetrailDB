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
		m_database.close();
	}

	void DatabaseStorage::setupTables()
	{
		executeStatement("PRAGMA foreign_keys=ON;");

		if (!isCompatible())
		{
			throw SourcetrailException("Unable to setup database tables because database is not compatible.");
		}

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

		insertOrUpdateMetaValue("storage_version", std::to_string(getSupportedDatabaseVersion()));
	}

	void DatabaseStorage::clearTables()
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

		setupTables();
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
			CppSQLite3Statement stmt = m_database.compileStatement(
				"SELECT id FROM node WHERE serialized_name == ? LIMIT 1;"
			);
			stmt.bind(1, storageNodeData.serializedName.c_str());
			CppSQLite3Query q = executeQuery(stmt);
			if (!q.eof())
			{
				id = q.getIntField(0, 0);
			}
		}

		// FIXME: update node nodeKind here

		if (id == 0)
		{
			{
				CppSQLite3Statement stmt = m_database.compileStatement(
					"INSERT INTO element(id) VALUES(NULL);"
				);
				executeStatement(stmt);
				id = m_database.lastRowId();
			}
			{
				CppSQLite3Statement stmt = m_database.compileStatement(
					"INSERT INTO node(id, type, serialized_name) VALUES(?, ?, ?);"
				);
				stmt.bind(1, id);
				stmt.bind(2, storageNodeData.nodeKind);
				stmt.bind(3, storageNodeData.serializedName.c_str());
				executeStatement(stmt);
			}
		}
		return id;
	}

	void DatabaseStorage::addSymbol(const StorageSymbol& storageSymbol)
	{
		CppSQLite3Statement stmt = m_database.compileStatement(
			"INSERT OR IGNORE INTO symbol(id, definition_kind) VALUES(?, ?);"
		);
		stmt.bind(1, storageSymbol.id);
		stmt.bind(2, storageSymbol.definitionKind);
		executeStatement(stmt);
	}

	void DatabaseStorage::addFile(const StorageFile& storageFile)
	{
		{
			CppSQLite3Statement stmt = m_database.compileStatement(
				"SELECT id FROM file WHERE id == ?;"
			);
			stmt.bind(1, storageFile.id);
			CppSQLite3Query q = executeQuery(stmt);
			if (!q.eof())
			{
				return; // early exit if the file already exists
			}
		}

		std::string content = "";
		if (utility::getFileExists(storageFile.filePath))
		{
			content = utility::getFileContent(storageFile.filePath);
		}
		const int lineCount = utility::getLineCount(content);

		{
			CppSQLite3Statement stmt = m_database.compileStatement(
				"INSERT OR IGNORE INTO file(id, path, language, modification_time, indexed, complete, line_count) VALUES(?, ?, ?, ?, ?, ?, ?);"
			);
			stmt.bind(1, storageFile.id);
			stmt.bind(2, storageFile.filePath.c_str());
			stmt.bind(3, storageFile.languageIdentifier.c_str());
			stmt.bind(4, storageFile.modificationTime.c_str());
			stmt.bind(5, storageFile.indexed);
			stmt.bind(6, storageFile.complete);
			stmt.bind(7, lineCount);
			executeStatement(stmt);
		}

		if (!content.empty())
		{
			CppSQLite3Statement stmt = m_database.compileStatement(
				"INSERT INTO filecontent(id, content) VALUES(?, ?);"
			);
			stmt.bind(1, storageFile.id);
			stmt.bind(2, content.c_str());
			executeStatement(stmt);
		}
	}

	int DatabaseStorage::addEdge(const StorageEdgeData& storageEdgeData)
	{
		int id = 0;

		{
			CppSQLite3Statement stmt = m_database.compileStatement(
				"SELECT id FROM edge WHERE source_node_id == ? AND target_node_id == ? AND type == ? LIMIT 1;"
			);
			stmt.bind(1, storageEdgeData.sourceNodeId);
			stmt.bind(2, storageEdgeData.targetNodeId);
			stmt.bind(3, storageEdgeData.edgeKind);
			CppSQLite3Query q = executeQuery(stmt);
			if (!q.eof())
			{
				id = q.getIntField(0, 0);
			}
		}

		if (id == 0)
		{
			{
				CppSQLite3Statement stmt = m_database.compileStatement(
					"INSERT INTO element(id) VALUES(NULL);"
				);
				executeStatement(stmt);
				id = m_database.lastRowId();
			}
			{
				CppSQLite3Statement stmt = m_database.compileStatement(
					"INSERT INTO edge(id, type, source_node_id, target_node_id) VALUES(?, ?, ?, ?);"
				);
				stmt.bind(1, id);
				stmt.bind(2, storageEdgeData.edgeKind);
				stmt.bind(3, storageEdgeData.sourceNodeId);
				stmt.bind(4, storageEdgeData.targetNodeId);
				executeStatement(stmt);
			}
		}
		return id;
	}

	int DatabaseStorage::addLocalSymbol(const StorageLocalSymbolData& storageLocalSymbolData)
	{
		int id = 0;

		{
			CppSQLite3Statement stmt = m_database.compileStatement(
				"SELECT id FROM local_symbol WHERE name == ? LIMIT 1;"
			);
			stmt.bind(1, storageLocalSymbolData.name.c_str());
			CppSQLite3Query q = executeQuery(stmt);
			if (!q.eof())
			{
				id = q.getIntField(0, 0);
			}
		}

		if (id == 0)
		{
			{
				CppSQLite3Statement stmt = m_database.compileStatement(
					"INSERT INTO element(id) VALUES(NULL);"
				);
				executeStatement(stmt);
				id = m_database.lastRowId();
			}
			{
				CppSQLite3Statement stmt = m_database.compileStatement(
					"INSERT INTO local_symbol(id, name) VALUES(?, ?);"
				);
				stmt.bind(1, id);
				stmt.bind(2, storageLocalSymbolData.name.c_str());
				executeStatement(stmt);
			}
		}
		return id;
	}

	int DatabaseStorage::addSourceLocation(const StorageSourceLocationData& storageSourceLocationData)
	{
		int id = 0;

		{
			CppSQLite3Statement stmt = m_database.compileStatement(
				"SELECT id FROM source_location WHERE "
				"file_node_id = ? AND "
				"start_line = ? AND "
				"start_column = ? AND "
				"end_line = ? AND "
				"end_column = ? AND "
				"type = ? "
				"LIMIT 1;"
			);
			stmt.bind(1, storageSourceLocationData.fileNodeId);
			stmt.bind(2, storageSourceLocationData.startLineNumber);
			stmt.bind(3, storageSourceLocationData.startColumnNumber);
			stmt.bind(4, storageSourceLocationData.endLineNumber);
			stmt.bind(5, storageSourceLocationData.endColumnNumber);
			stmt.bind(6, storageSourceLocationData.locationKind);
			CppSQLite3Query q = executeQuery(stmt);
			if (!q.eof())
			{
				id = q.getIntField(0, 0);
			}
		}

		if (id == 0)
		{
			CppSQLite3Statement stmt = m_database.compileStatement(
				"INSERT INTO source_location(id, file_node_id, start_line, start_column, end_line, end_column, type) VALUES(NULL, ?, ?, ?, ?, ?, ?);"
			);
			stmt.bind(1, storageSourceLocationData.fileNodeId);
			stmt.bind(2, storageSourceLocationData.startLineNumber);
			stmt.bind(3, storageSourceLocationData.startColumnNumber);
			stmt.bind(4, storageSourceLocationData.endLineNumber);
			stmt.bind(5, storageSourceLocationData.endColumnNumber);
			stmt.bind(6, storageSourceLocationData.locationKind);
			executeStatement(stmt);
			id = m_database.lastRowId();
		}
		return id;
	}

	void DatabaseStorage::addOccurrence(const StorageOccurrence& storageOccurrence)
	{
		CppSQLite3Statement stmt = m_database.compileStatement(
			"INSERT OR IGNORE INTO occurrence(element_id, source_location_id) VALUES(?, ?);"
		);
		stmt.bind(1, storageOccurrence.elementId);
		stmt.bind(2, storageOccurrence.sourceLocationId);
		executeStatement(stmt);
	}

	int DatabaseStorage::addError(const StorageErrorData& storageErrorData)
	{
		int id = 0;
		{
			CppSQLite3Statement stmt = m_database.compileStatement(
				"SELECT id FROM error WHERE "
				"message = ? AND "
				"fatal == ? "
				"LIMIT 1;"
			);
			stmt.bind(1, storageErrorData.message.c_str());
			stmt.bind(2, storageErrorData.fatal);
			CppSQLite3Query q = executeQuery(stmt);
			if (!q.eof() && q.numFields() > 0)
			{
				id = q.getIntField(0, -1);
			}
		}

		if (id == 0)
		{
			{
				CppSQLite3Statement stmt = m_database.compileStatement(
					"INSERT INTO element(id) VALUES(NULL);"
				);
				executeStatement(stmt);
				id = m_database.lastRowId();
			}

			CppSQLite3Statement stmt = m_database.compileStatement(
				"INSERT INTO error(id, message, fatal, indexed, translation_unit) "
				"VALUES(?, ?, ?, ?, ?);"
			);
			stmt.bind(1, id);
			stmt.bind(2, storageErrorData.message.c_str());
			stmt.bind(3, storageErrorData.fatal);
			stmt.bind(4, storageErrorData.indexed);
			stmt.bind(5, storageErrorData.translationUnit.c_str());
			executeStatement(stmt);
			id = m_database.lastRowId();
		}
		return id;
	}

	void DatabaseStorage::setNodeType(int nodeId, int nodeType)
	{
		CppSQLite3Statement stmt = m_database.compileStatement(
			"UPDATE node SET type = ? WHERE id == ?;"
		);
		stmt.bind(1, nodeType);
		stmt.bind(2, nodeId);
		executeStatement(stmt);
	}

	void DatabaseStorage::setFileLanguage(int fileId, const std::string& languageIdentifier)
	{
		CppSQLite3Statement stmt = m_database.compileStatement(
			"UPDATE file SET language = ? WHERE id == ?;"
		);
		stmt.bind(1, languageIdentifier.c_str());
		stmt.bind(2, fileId);
		executeStatement(stmt);
	}

	// --- Private Interface ---

	void DatabaseStorage::insertOrUpdateMetaValue(const std::string& key, const std::string& value)
	{
		CppSQLite3Statement stmt = m_database.compileStatement(std::string(
			"INSERT OR REPLACE INTO meta(id, key, value) VALUES("
				"(SELECT id FROM meta WHERE key = ?), ?, ?"
			");"
		).c_str());

		stmt.bind(1, key.c_str());
		stmt.bind(2, key.c_str());
		stmt.bind(3, value.c_str());
		executeStatement(stmt);
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
