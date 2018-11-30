#ifndef SOURCETRAIL_DATABASE_STORAGE_H
#define SOURCETRAIL_DATABASE_STORAGE_H

#include <memory>
#include <string>

#include "sqlite/CppSQLite3.h"

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
		static std::shared_ptr<DatabaseStorage> openDatabase(const std::string& dbFilePath);
		~DatabaseStorage();

		void setupTables();
		void clearTables();

		bool isEmpty() const;
		bool isCompatible() const;
		int getLoadedDatabaseVersion() const;
		void beginTransaction();
		void commitTransaction();
		void rollbackTransaction();
		void optimizeDatabaseMemory();

		int addNode(const std::string& serializedNameHierarchy);
		void addSymbol(int symbolId, int definitionKind);
		void addFile(int nodeId, const std::string& filePath);
		int addEdge(int sourceId, int targetId, int edgeKind);
		int addLocalSymbol(const std::string& name);
		int addSourceLocation(
			int fileId,
			int startLineNumber,
			int startColumnNumber,
			int endLineNumber,
			int endColumnNumber,
			int locationKind);
		void addOccurrence(int elementId, int sourceLocationId);
		int addError(
			const std::string& message,
			bool fatal);

		void setNodeType(int nodeId, int nodeKind);

	private:
		DatabaseStorage() = default;

		void insertOrUpdateMetaValue(const std::string& key, const std::string& value);
		void executeStatement(const std::string& statement) const;
		void executeStatement(CppSQLite3Statement& statement) const;
		CppSQLite3Query executeQuery(const std::string& query) const;
		CppSQLite3Query executeQuery(CppSQLite3Statement& statement) const;

		mutable CppSQLite3DB m_database;
	};

}

#endif // SOURCETRAIL_DATABASE_STORAGE_H
