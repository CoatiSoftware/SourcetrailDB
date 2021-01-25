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

#ifndef SOURCETRAIL_DATABASE_STORAGE_H
#define SOURCETRAIL_DATABASE_STORAGE_H

#include <memory>
#include <string>
#include <vector>

#include "CppSQLite3.h"

#include "StorageEdge.h"
#include "StorageElementComponent.h"
#include "StorageError.h"
#include "StorageFile.h"
#include "StorageLocalSymbol.h"
#include "StorageNode.h"
#include "StorageOccurrence.h"
#include "StorageSourceLocation.h"
#include "StorageSymbol.h"

namespace sourcetrail
{
/**
 * Class wrapping the write and update interface of the Sourcetrail database.
 *
 * The DatabaseStorage provides the interface for writing and updating the Sourcetrail database. This interface only
 * knows about basic data types, more elaborate types like enums, structs and classes need to be converted to basic
 * types before using this interface.
 */
class DatabaseStorage
{
public:
	static int getSupportedDatabaseVersion();
	static std::unique_ptr<DatabaseStorage> openDatabase(const std::string& dbFilePath);
	~DatabaseStorage();

	void setupDatabase();
	void clearDatabase();

	void setProjectSettingsText(const std::string& text);

	bool isEmpty() const;
	bool isCompatible() const;
	int getLoadedDatabaseVersion() const;
	void beginTransaction();
	void commitTransaction();
	void rollbackTransaction();
	void optimizeDatabaseMemory();

	int addElementComponent(const StorageElementComponentData& storageElementComponentData);
	int addNode(const StorageNodeData& storageNodeData);
	void addSymbol(const StorageSymbol& storageSymbol);
	void addFile(const StorageFile& storageFile);
	int addEdge(const StorageEdgeData& storageEdgeData);
	int addLocalSymbol(const StorageLocalSymbolData& storageLocalSymbolData);
	int addSourceLocation(const StorageSourceLocationData& storageSourceLocationData);
	void addOccurrence(const StorageOccurrence& storageOccurrence);
	int addError(const StorageErrorData& storageErrorData);

	void setNodeType(int nodeId, int nodeKind);
	void setFileLanguage(int fileId, const std::string& languageIdentifier);

	template <typename ResultType>
	std::vector<ResultType> getAll() const
	{
		return doGetAll<ResultType>("");
	}

private:
	DatabaseStorage() = default;

	void setupTables();
	void clearTables();
	void setupIndices();
	void setupPrecompiledStatements();
	void clearPrecompiledStatements();

	int insertElement();
	void insertOrUpdateMetaValue(const std::string& key, const std::string& value);
	CppSQLite3Statement compileStatement(const std::string& statement) const;
	void executeStatement(const std::string& statement) const;
	void executeStatement(CppSQLite3Statement& statement) const;
	CppSQLite3Query executeQuery(const std::string& query) const;
	CppSQLite3Query executeQuery(CppSQLite3Statement& statement) const;

	template <typename ResultType>
	std::vector<ResultType> doGetAll(const std::string& query) const;

	mutable CppSQLite3DB m_database;

	CppSQLite3Statement m_insertElementStatement;
	CppSQLite3Statement m_insertElementComponentStatement;
	CppSQLite3Statement m_findNodeStatement;
	CppSQLite3Statement m_insertNodeStatement;
	CppSQLite3Statement m_setNodeTypeStmt;
	CppSQLite3Statement m_insertSymbolStatement;
	CppSQLite3Statement m_findFileStatement;
	CppSQLite3Statement m_insertFileStatement;
	CppSQLite3Statement m_setFileLanguageStmt;
	CppSQLite3Statement m_insertFileContentStatement;
	CppSQLite3Statement m_findEdgeStatement;
	CppSQLite3Statement m_insertEdgeStatement;
	CppSQLite3Statement m_findLocalSymbolStmt;
	CppSQLite3Statement m_insertLocalSymbolStmt;
	CppSQLite3Statement m_findSourceLocationStmt;
	CppSQLite3Statement m_insertSourceLocationStmt;
	CppSQLite3Statement m_insertOccurenceStmt;
	CppSQLite3Statement m_findErrorStatement;
	CppSQLite3Statement m_insertErrorStatement;
	CppSQLite3Statement m_insertOrUpdateMetaValueStmt;
};

template <>
std::vector<StorageEdge> DatabaseStorage::doGetAll<StorageEdge>(const std::string& query) const;
template <>
std::vector<StorageNode> DatabaseStorage::doGetAll<StorageNode>(const std::string& query) const;
template <>
std::vector<StorageSymbol> DatabaseStorage::doGetAll<StorageSymbol>(const std::string& query) const;
template <>
std::vector<StorageFile> DatabaseStorage::doGetAll<StorageFile>(const std::string& query) const;
template <>
std::vector<StorageLocalSymbol> DatabaseStorage::doGetAll<StorageLocalSymbol>(const std::string& query) const;
template <>
std::vector<StorageSourceLocation> DatabaseStorage::doGetAll<StorageSourceLocation>(const std::string& query) const;
template <>
std::vector<StorageOccurrence> DatabaseStorage::doGetAll<StorageOccurrence>(const std::string& query) const;
template <>
std::vector<StorageError> DatabaseStorage::doGetAll<StorageError>(const std::string& query) const;
}	 // namespace sourcetrail

#endif	  // SOURCETRAIL_DATABASE_STORAGE_H
