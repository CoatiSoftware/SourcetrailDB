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

#ifndef SOURCETRAIL_SRCTRLDB_WRITER_H
#define SOURCETRAIL_SRCTRLDB_WRITER_H

#include <memory>
#include <string>

namespace sourcetrail
{
	class DatabaseStorage;

	struct SourceRange;
	struct NameHierarchy;

	enum DefinitionKind;
	enum EdgeKind;
	enum LocationKind;
	enum ReferenceKind;
	enum SymbolKind;

	/**
	 * Class wrapping the main interface for writing data to a Sourcetrail project database.
	 *
	 * TODO: write a detailed description here.
	 * TODO: document all public interface methods
	 * TODO: add small example for usage here
	 */
	class SourcetrailDBWriter
	{
	public:
		SourcetrailDBWriter();

		std::string getVersionString() const;

		int getSupportedDatabaseVersion() const;
		const std::string& getLastError() const;
		void clearLastError();

		bool open(const std::string& databaseFilePath);
		bool close();
		bool clear();

		bool isEmpty() const;
		bool isCompatible() const;
		int getLoadedDatabaseVersion() const;

		bool beginTransaction();
		bool commitTransaction();
		bool rollbackTransaction();
		bool optimizeDatabaseMemory();

		/// will return the same id for the same name hierarchy. will return 0 on failure. 0 is not a valid symbol id
		int recordSymbol(const NameHierarchy& nameHierarchy);
		bool recordSymbolDefinitionKind(int symbolId, DefinitionKind definitionKind);
		bool recordSymbolKind(int symbolId, SymbolKind symbolKind);
		/// The provided "location" will be clickable and displayable. When clicked it will cause the symbol with the specified "symbolId" to be activated. When
		/// the symbol with the specified "symbolId" is activated, this location will be displayed.
		bool recordSymbolLocation(int symbolId, const SourceRange& location);
		/// The provided "location" will be displayable. When the symbol with the specified "symbolId" is activated, this location will be displayed.
		bool recordSymbolScopeLocation(int symbolId, const SourceRange& location);
		bool recordSymbolSignatureLocation(int symbolId, const SourceRange& location);

		int recordReference(int contextSymbolId, int referencedSymbolId, ReferenceKind referenceKind);
		bool recordReferenceLocation(int referenceId, const SourceRange& location);

		int recordFile(const std::string& filePath);

		int recordLocalSymbol(const std::string& name);
		bool recordLocalSymbolLocation(int localSymbolId, const SourceRange& location);

		bool recordCommentLocation(const SourceRange& location);

		bool recordError(const std::string& message, bool fatal, const SourceRange& location);

	private:
		static std::string serializeNameHierarchy(const NameHierarchy& nameHierarchy);
		std::string getProjectFilePath() const;
		std::string getDatabaseFilePath() const;
		void openDatabase();
		void closeDatabase();
		void setupDatabaseTables();
		void clearDatabaseTables();
		void createOrResetProjectFile();
		int addNodeHierarchy(const NameHierarchy& nameHierarchy);
		int addFile(const std::string& filePath);
		int addEdge(int sourceId, int targetId, EdgeKind edgeKind);
		void addSourceLocation(int elementId, const SourceRange& location, LocationKind kind);

		std::string m_projectFilePath;
		std::string m_databaseFilePath;
		std::shared_ptr<DatabaseStorage> m_storage;
		mutable std::string m_lastError;
	};
}

#endif // SOURCETRAIL_SRCTRLDB_WRITER_H
