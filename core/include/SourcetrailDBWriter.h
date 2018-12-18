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

#include "DefinitionKind.h"
#include "EdgeKind.h"
#include "LocationKind.h"
#include "NameHierarchy.h"
#include "ReferenceKind.h"
#include "SourceRange.h"
#include "SymbolKind.h"

namespace sourcetrail
{
	class DatabaseStorage;	// forward declaration prevents leakage of sqlite include dependency to users
							// of the SourcetrailDBWriter

	/**
	 * Class wrapping the main interface for writing data to a Sourcetrail project database.
	 *
	 * This class can be used to manage a Sourcetrail database file and to write information to such
	 * a file.
	 *
	 * The following code snippet illustrates a very basic usage of the SourcetrailDBWriter class:
	 *
	 * sourcetrail::SourcetrailDBWriter writer;
	 * writer.open("MyProject.srctrldb");
	 * writer.recordSymbol({ "::",{ { "void", "foo", "()" } } });
	 * writer.close();
	 */
	class SourcetrailDBWriter
	{
	public:
		SourcetrailDBWriter();

		/**
		 * Provides the version of the SourcetrailDB Core as string with format "vXX_dbYY_pZZ"
		 *
		 *  - "XX" marks the interface version of the API. This version is increases on every change
		 *    that breaks backwards compatibility of the API.
		 *  - "YY" marks the version of the database that will be generated when using the
		 *    SourcetrailDB API. This version needs to match the database version of the Sourcetrail
		 *    instance that is used to open the generated database file to be compatible.
		 *  - "ZZ" marks the patch number of the build. It will increase with every release that
		 *    publishes bugfixes and features that don't break any compatibility.
		 */
		std::string getVersionString() const;

		/**
		 * Provides the supported database version as integer
		 *
		 * See getVersionString() for details
		 */
		int getSupportedDatabaseVersion() const;

		/**
		 * Provides the last error that occurred while using the SourcetrailDBWriter
		 *
		 * The last error is empty if no error occurred since instantiation of the class or since the
		 * error has last been cleared.
		 *
		 * See: clearLastError()
		 */
		const std::string& getLastError() const;

		/**
		 * Modifies the stored error message
		 *
		 * See: getLastError()
		 */
		void setLastError(const std::string& error) const;

		/**
		 * Clears the stored error message
		 *
		 * See: getLastError()
		 */
		void clearLastError();

		/**
		 * Opens a Sourcetrail database
		 *
		 * Call this method to open a Sourcetrail database file. If the database does not have any
		 * related .srctrlprj project file, a minimal project file will be created that allows for
		 * opening the database with Sourcetrail.
		 * Param databaseFilePath - absolute file path of the database file, including file extension
		 * Returns true if the operation was successful. Otherwise false is returned and getLastError()
		 * can be checked for more detailed information.
		 */
		bool open(const std::string& databaseFilePath);

		/**
		 * Closes the currently open Sourcetrail database
		 *
		 * Returns true if the operation was successful. Otherwise false is returned and getLastError()
		 * can be checked for more detailed information.
		 *
		 * See open(const std::string& databaseFilePath)
		 */
		bool close();

		/**
		 * Clears the currently open Sourcetrail database
		 *
		 * Returns true if the operation was successful. Otherwise false is returned and getLastError()
		 * can be checked for more detailed information.
		 *
		 * See open(const std::string& databaseFilePath)
		 */
		bool clear();

		/**
		 * Checks if the currently open database file contains any data
		 *
		 * Returns true after opening a non-existing database file or clearing the open Sourcetrail
		 * database.
		 */
		bool isEmpty() const;

		/**
		 * Checks if the currently open database is compatible with the SourcetrailDBWriter version.
		 *
		 * Returns true for an empty database or a database that has been created with the same
		 * database version.
		 *
		 * See: getSupportedDatabaseVersion()
		 * See: getLoadedDatabaseVersion()
		 */
		bool isCompatible() const;

		/**
		 * Provides the database version of the loaded database file as an integer
		 */
		int getLoadedDatabaseVersion() const;

		/**
		 * Starts a database transaction on the currently open database
		 *
		 * Apart from allowing to restore the database to the state it was in before this method has
		 * been called, wrapping multiple calls that record data with one transaction significantly
		 * increases the performance of these database operations.
		 *
		 * Returns true if the operation was successful. Otherwise false is returned and getLastError()
		 * can be checked for more detailed information.
		 *
		 * See: commitTransaction()
		 * See: rollbackTransaction()
		 */
		bool beginTransaction();

		/**
		 * Ends the current transaction and writes all of its changes persistently to the database
		 *
		 * Returns true if the operation was successful. Otherwise false is returned and getLastError()
		 * can be checked for more detailed information.
		 */
		bool commitTransaction();

		/**
		 * Reverts the database to the state it was in before the current transaction was started
		 *
		 * Returns true if the operation was successful. Otherwise false is returned and getLastError()
		 * can be checked for more detailed information.
		 */
		bool rollbackTransaction();

		/**
		 * Reduces the on disk memory consumption of the open database to a minimum
		 *
		 * Returns true if the operation was successful. Otherwise false is returned and getLastError()
		 * can be checked for more detailed information.
		 */
		bool optimizeDatabaseMemory();

		/**
		 * Stores a symbol to the database
		 *
		 * Param nameHierarchy - the name of the symbol to store.
		 * Returns an integer id of the stored symbol. Calling this method multiple times with the same
		 * input on the same Sourcetrail database will always return the same id. If this operation fails
		 * the invalid id 0 is returned and getLastError() can be checked for more detailed information.
		 *
		 * See: NameHierarchy
		 */
		int recordSymbol(const NameHierarchy& nameHierarchy);

		/**
		 * Stores a definition kind for a specific symbol to the database
		 *
		 * Calling this method allows to store a DefinitionKind (e.g. explicitly defined, implicitly
		 * defined) for a symbol referenced by id to the database. Calling this method with the same
		 * symbolId multiple times overwrites the symbol's previously recorded DefinitionKind. If no
		 * DefinitionKind is recorded for a symbol, Sourcetrail treats this symbol as "non-indexed".
		 * This may be desired while recording a reference to a symbol with a definition that is located
		 * outside of the indexed source files.
		 *
		 * Param symbolId - the id of the symbol for which a DefinitionKind shall be recorded.
		 * Param definitionKind - the DefinitionKind that shall be recorded for the respective symbol.
		 * Returns true if the operation was successful. Otherwise false is returned and getLastError()
		 * can be checked for more detailed information.
		 *
		 * See: DefinitionKind
		 */
		bool recordSymbolDefinitionKind(int symbolId, DefinitionKind definitionKind);

		/**
		 * Stores a symbol kind for a specific symbol to the database
		 *
		 * Calling this method allows to store a SymbolKind (e.g. "class", "function", etc.) for a
		 * symbol referenced by id to the database. Calling this method with the same symbolId multiple
		 * times overwrites the symbol's previously recorded SymbolKind. If no SymbolKind is recorded
		 * for a symbol, Sourcetrail displays the type of this symbol as "symbol".
		 *
		 * Param symbolId - the id of the symbol for which a SymbolKind shall be recorded.
		 * Param symbolKind - the SymbolKind that shall be recorded for the respective symbol.
		 * Returns true if the operation was successful. Otherwise false is returned and getLastError()
		 * can be checked for more detailed information.
		 *
		 * See: SymbolKind
		 */
		bool recordSymbolKind(int symbolId, SymbolKind symbolKind);

		/**
		 * Stores a location for a specific symbol to the database
		 *
		 * This method allows to store a location for a symbol referenced by id to the database.
		 * Calling this method with the same symbolId multiple times adds multiple locations for the
		 * respective symbol. The stored location will be clickable and displayable. When clicked
		 * Sourcetrail will activate the symbol with the specified id. When the symbol with the specified
		 * id is activated, this location will be displayed and highlighted by Sourcetrail.
		 *
		 * Param symbolId - the id of the symbol for which a location shall be recorded.
		 * Param location - the SourceRange that shall be recorded as location for the respective symbol.
		 * Returns true if the operation was successful. Otherwise false is returned and getLastError()
		 * can be checked for more detailed information.
		 *
		 * See: SourceRange
		 */
		bool recordSymbolLocation(int symbolId, const SourceRange& location);

		/**
		 * Stores a scope location for a specific symbol to the database
		 *
		 * This method allows to store a scope location for a symbol referenced by id to the database.
		 * Calling this method with the same symbolId multiple times adds multiple scope locations for the
		 * respective symbol. The stored location will only be displayable and not clickable. When the
		 * symbol with the specified id is activated, this location will be displayed but not highlighted
		 * by Sourcetrail.
		 *
		 * Param symbolId - the id of the symbol for which a scope location shall be recorded.
		 * Param location - the SourceRange that shall be recorded as scope location for the respective
		 * symbol.
		 * Returns true if the operation was successful. Otherwise false is returned and getLastError()
		 * can be checked for more detailed information.
		 *
		 * See: SourceRange
		 */
		bool recordSymbolScopeLocation(int symbolId, const SourceRange& location);

		/**
		 * Stores a signature location for a specific symbol to the database
		 *
		 * This method allows to store a signature location for a symbol referenced by id to the
		 * database. Calling this method with the same symbolId multiple times adds multiple signature
		 * locations for the respective symbol. Sourcetrail will only make use of one of the recorded
		 * signature locations, so please try to call this method only once.
		 * If a signature location is recorded for a symbol, Sourcetrail will display the respective
		 * source code in a tooltip whenever the symbol with the referenced id or any of that symbol's
		 * locations gets hovered.
		 * If no signature location is recorded for a symbol, Sourcetrail will try to derive the text
		 * of the shown tooltip from the recorded name hierarchy.
		 *
		 * Param symbolId - the id of the symbol for which a signature location shall be recorded.
		 * Param location - the SourceRange that shall be recorded as signature location for the
		 * respective symbol.
		 * Returns true if the operation was successful. Otherwise false is returned and getLastError()
		 * can be checked for more detailed information.
		 *
		 * See: SourceRange
		 */
		bool recordSymbolSignatureLocation(int symbolId, const SourceRange& location);

		/**
		 * Stores a reference between two symbols to the database
		 *
		 * This method allows to store the information of symbols referencing one another to the database.
		 * For each recorded reference Sourcetrail's graph view will display an edge that originates at
		 * the reference's recorded context symbol and points to the recorded referenced symbol. The
		 * recorded ReferenceKind is used to determine the color of the displayed edge and to generate a
		 * description in the hover tooltip of the edge.
		 *
		 * Param contextSymbolId - the id of the source of the recorded reference edge
		 * Param referencedSymbolId - the id of the target of the recorded reference edge
		 * Param referenceKind - kind of the recorded reference edge
		 * Returns an integer id of the stored reference. Calling this method multiple times with the same
		 * input on the same Sourcetrail database will always return the same id. If this operation fails
		 * the invalid id 0 is returned and getLastError() can be checked for more detailed information.
		 *
		 * See: ReferenceKind
		 */
		int recordReference(int contextSymbolId, int referencedSymbolId, ReferenceKind referenceKind);

		/**
		 * Stores a location for a specific reference to the database
		 *
		 * This method allows to store a location for a reference to the database.
		 * Calling this method with the same referenceId multiple times adds multiple locations for the
		 * respective reference. The stored location will be clickable and displayable. When the reference
		 * location is clicked Sourcetrail will activate the symbol referenced by the respective reference.
		 * When the reference with the specified id is activated, this location will be displayed and
		 * highlighted by Sourcetrail.
		 *
		 * Param referenceId - the id of the reference for which a location shall be recorded.
		 * Param location - the SourceRange that shall be recorded as location for the respective reference.
		 * Returns true if the operation was successful. Otherwise false is returned and getLastError()
		 * can be checked for more detailed information.
		 *
		 * See: SourceRange
		 */
		bool recordReferenceLocation(int referenceId, const SourceRange& location);

		/**
		 * Stores a file to the database
		 *
		 * Param filePath - the absolute path to the file to store.
		 * Returns an integer id of the stored file. Calling this method multiple times with the same
		 * input on the same Sourcetrail database will always return the same id. If this operation fails
		 * the invalid id 0 is returned and getLastError() can be checked for more detailed information.
		 */
		int recordFile(const std::string& filePath);

		/**
		 * Stores language information for a specific file to the database
		 *
		 * This method allows to store language information for a file to the database. Language information
		 * is passed and stored as string (e.g. "cpp", "java", etc.) and allows Sourcetrail to pick the
		 * correct syntax highlighting rules when displaying the respective source file.
		 *
		 * Param fileId - the id of the file for which language information shall be recorded.
		 * Param languageIdentifier - a string that denotes the programming language the respective file
		 * is written in.
		 * Returns true if the operation was successful. Otherwise false is returned and getLastError()
		 * can be checked for more detailed information.
		 */
		bool recordFileLanguage(int fileId, const std::string& languageIdentifier);

		/**
		 * Stores a local symbol to the database
		 *
		 * Param name - a name that is unique for this local symbol (e.g. the string encoded location
		 * of the local symbol's definition).
		 * Returns an integer id of the stored local symbol. Calling this method multiple times with the same
		 * input on the same Sourcetrail database will always return the same id. If this operation fails
		 * the invalid id 0 is returned and getLastError() can be checked for more detailed information.
		 */
		int recordLocalSymbol(const std::string& name);

		/**
		 * Stores a location for a specific local symbol to the database
		 *
		 * This method allows to store a location for a local symbol symbol referenced by id to the database.
		 * Calling this method with the same symbolId multiple times adds multiple locations for the
		 * respective local symbol. The stored location will be clickable and displayable. When clicked
		 * Sourcetrail will activate this and all other local symbol locations that share the same local
		 * symbol id.
		 *
		 * Param localSymbolId - the id of the local symbol for which a location shall be recorded.
		 * Param location - the SourceRange that shall be recorded as location for the respective
		 * local symbol.
		 * Returns true if the operation was successful. Otherwise false is returned and getLastError()
		 * can be checked for more detailed information.
		 *
		 * See: SourceRange
		 */
		bool recordLocalSymbolLocation(int localSymbolId, const SourceRange& location);

		/**
		 * Stores a comment location to the database
		 *
		 * This method allows to store a comment location to the database. These comment locations will
		 * be used by Sourcetrail to prevent the code view from displaying comments imcomplete.
		 *
		 * param location - the SourceRange of the comment to record.
		 * Returns true if the operation was successful. Otherwise false is returned and getLastError()
		 * can be checked for more detailed information.
		 *
		 * See: SourceRange
		 */
		bool recordCommentLocation(const SourceRange& location);

		/**
		 * Stores an indexing error to the database
		 *
		 * This method allows to store an indexing error to the database. Errors will be shown by
		 * Sourcetrail's error view. When clicking the error in Sourcetrail's error list, the code view
		 * will display this location.
		 *
		 * param message - an error message that will be displayed in Sourcetrail's error list.
		 * param fatal - boolean that tells Sourcetrail if this is a fatal error.
		 * param location - the SourceRange of the error encountered.
		 * Returns true if the operation was successful. Otherwise false is returned and getLastError()
		 * can be checked for more detailed information.
		 *
		 * See: SourceRange
		 */
		bool recordError(const std::string& message, bool fatal, const SourceRange& location);

	private:
		void openDatabase();
		void closeDatabase();
		void setupDatabaseTables();
		void clearDatabaseTables();
		void createOrResetProjectFile();
		void updateProjectSettingsText();
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
