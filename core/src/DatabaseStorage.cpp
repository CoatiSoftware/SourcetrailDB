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
		std::shared_ptr<DatabaseStorage> storage = std::shared_ptr<DatabaseStorage>(new DatabaseStorage());
		storage->m_database.open(dbFilePath.c_str());
		storage->executeStatement("PRAGMA foreign_keys=ON;");
		return storage;
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
			"error"
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

	int DatabaseStorage::addNode(const std::string& serializedNameHierarchy)
	{
		int id = 0;

		{
			CppSQLite3Statement stmt = m_database.compileStatement(
				"SELECT id FROM node WHERE serialized_name == ? LIMIT 1;"
			);
			stmt.bind(1, serializedNameHierarchy.c_str());
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
					"INSERT INTO node(id, type, serialized_name) VALUES(?, ?, ?);"
				);
				stmt.bind(1, id);
				stmt.bind(2, nodeKindToInt(NODE_UNKNOWN));
				stmt.bind(3, serializedNameHierarchy.c_str());
				executeStatement(stmt);
			}
		}
		return id;
	}

	void DatabaseStorage::addSymbol(int nodeId, int definitionKind)
	{
		CppSQLite3Statement stmt = m_database.compileStatement(
			"INSERT OR IGNORE INTO symbol(id, definition_kind) VALUES(?, ?);"
		);
		stmt.bind(1, nodeId);
		stmt.bind(2, definitionKind);
		executeStatement(stmt);
	}


	void DatabaseStorage::addFile(int nodeId, const std::string& filePath)
	{
		std::string modificationTime = utility::getDateTimeString(0);
		const bool indexed = true;
		const bool complete = true;
		std::string content = "";
		if (utility::getFileExists(filePath))
		{
			modificationTime = utility::getDateTimeString(utility::getFileModificationTime(filePath));
			content = utility::getFileContent(filePath);
		}
		const int lineCount = utility::getLineCount(content);

		{
			CppSQLite3Statement stmt = m_database.compileStatement(
				"INSERT OR IGNORE INTO file(id, path, modification_time, indexed, complete, line_count) VALUES(?, ?, ?, ?, ?, ?);"
			);
			stmt.bind(1, nodeId);
			stmt.bind(2, filePath.c_str());
			stmt.bind(3, modificationTime.c_str());
			stmt.bind(4, indexed);
			stmt.bind(5, complete);
			stmt.bind(6, lineCount);
			executeStatement(stmt);
		}

		if (!content.empty())
		{
			CppSQLite3Statement stmt = m_database.compileStatement(
				"INSERT INTO filecontent(id, content) VALUES(?, ?);"
			);
			stmt.bind(1, nodeId);
			stmt.bind(2, content.c_str());
			executeStatement(stmt);
		}
	}

	int DatabaseStorage::addEdge(int sourceNodeId, int targetNodeId, int edgeKind)
	{
		int id = 0;

		{
			CppSQLite3Statement stmt = m_database.compileStatement(
				"SELECT id FROM edge WHERE source_node_id == ? AND target_node_id == ? AND type == ? LIMIT 1;"
			);
			stmt.bind(1, sourceNodeId);
			stmt.bind(2, targetNodeId);
			stmt.bind(3, edgeKind);
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
				stmt.bind(2, edgeKind);
				stmt.bind(3, sourceNodeId);
				stmt.bind(4, targetNodeId);
				executeStatement(stmt);
			}
		}
		return id;
	}

	int DatabaseStorage::addLocalSymbol(const std::string& name)
	{
		int id = 0;

		{
			CppSQLite3Statement stmt = m_database.compileStatement(
				"SELECT id FROM local_symbol WHERE name == ? LIMIT 1;"
			);
			stmt.bind(1, name.c_str());
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
				stmt.bind(2, name.c_str());
				executeStatement(stmt);
			}
		}
		return id;
	}

	int DatabaseStorage::addSourceLocation(
		int fileId,
		int startLineNumber,
		int startColumnNumber,
		int endLineNumber,
		int endColumnNumber,
		int locationKind)
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
			stmt.bind(1, fileId);
			stmt.bind(2, startLineNumber);
			stmt.bind(3, startColumnNumber);
			stmt.bind(4, endLineNumber);
			stmt.bind(5, endColumnNumber);
			stmt.bind(6, locationKind);
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
			stmt.bind(1, fileId);
			stmt.bind(2, startLineNumber);
			stmt.bind(3, startColumnNumber);
			stmt.bind(4, endLineNumber);
			stmt.bind(5, endColumnNumber);
			stmt.bind(6, locationKind);
			executeStatement(stmt);
			id = m_database.lastRowId();
		}
		return id;
	}

	void DatabaseStorage::addOccurrence(int elementId, int sourceLocationId)
	{
		CppSQLite3Statement stmt = m_database.compileStatement(
			"INSERT OR IGNORE INTO occurrence(element_id, source_location_id) VALUES(?, ?);"
		);
		stmt.bind(1, elementId);
		stmt.bind(2, sourceLocationId);
		executeStatement(stmt);
	}

	int DatabaseStorage::addError(
		const std::string& message,
		bool fatal)
	{
		int id = 0;
		{
			CppSQLite3Statement stmt = m_database.compileStatement(
				"SELECT id FROM error WHERE "
				"message = ? AND "
				"fatal == ? "
				"LIMIT 1;"
			);
			stmt.bind(1, message.c_str());
			stmt.bind(2, fatal);
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
			stmt.bind(2, message.c_str());
			stmt.bind(3, fatal);
			stmt.bind(4, true);
			stmt.bind(5, "");
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
}
