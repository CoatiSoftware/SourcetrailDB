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
#include "ElementComponentKind.h"
#include "LocationKind.h"
#include "NameHierarchy.h"
#include "ReferenceKind.h"
#include "SourceRange.h"
#include "SymbolKind.h"

namespace sourcetrail
{
class DatabaseStorage;

/**
 * SourcetrailDBWriter
 *
 * This class is the main interface for writing data to a Sourcetrail project database.
 * It be used to manage a Sourcetrail database file and to write information to it.
 *
 * The following code snippet illustrates a very basic usage of the SourcetrailDBWriter class:
 *
 *   sourcetrail::SourcetrailDBWriter writer;
 *   writer.open("MyProject.srctrldb");
 *   writer.recordSymbol({ "::",{ { "void", "foo", "()" } } });
 *   writer.close();
 */
class SourcetrailDBWriter
{
public:
	SourcetrailDBWriter();
	~SourcetrailDBWriter();

	/**
	 * Provides the version of the SourcetrailDB Core as string with format "vXX.dbYY.pZZ"
	 *
	 *  - XX: interface version. This version increases on every change that breaks backwards
	 *        compatibility.
	 *  - YY: Sourcetrail database version. This version needs to match the database version
	 *        of the used Sourcetrail instance. You can find the database version of Sourcetrail
	 *        in its About dialog.
	 *  - ZZ: patch number of the build. It will increase with every release that publishes
	 *        bugfixes and features that don't break any compatibility.
	 *
	 *  return: version string
	 */
	std::string getVersionString() const;

	/**
	 * Provides the supported database version as integer
	 *
	 *  return: supported database version
	 *
	 *  see: getVersionString() for details
	 */
	int getSupportedDatabaseVersion() const;

	/**
	 * Provides the last error that occurred while using the SourcetrailDBWriter
	 *
	 * The last error is empty if no error occurred since instantiation of the class or since the
	 * error has last been cleared.
	 *
	 *  return: error message of last error that occured
	 *
	 *  see: clearLastError()
	 */
	const std::string& getLastError() const;

	/**
	 * Modifies the stored error message
	 *
	 *  param: error - the new error message
	 *
	 *  see: getLastError()
	 */
	void setLastError(const std::string& error) const;

	/**
	 * Clears the stored error message
	 *
	 *  see: getLastError()
	 */
	void clearLastError();

	/**
	 * Opens a Sourcetrail database
	 *
	 * Call this method to open a Sourcetrail database file. If the database does not have any
	 * related .srctrlprj project file, a minimal project file will be created that allows for
	 * opening the database with Sourcetrail.
	 *
	 *  param: databaseFilePath - absolute file path of the database file, including file extension
	 *
	 *  return: true if successful. false on failure. getLastError() provides the error message.
	 */
	bool open(const std::string& databaseFilePath);

	/**
	 * Closes the currently open Sourcetrail database
	 *
	 *  return: Returns true if the operation was successful. Otherwise false is returned and getLastError()
	 *    can be checked for more detailed information.
	 *
	 *  see: open(const std::string& databaseFilePath)
	 */
	bool close();

	/**
	 * Clears the currently open Sourcetrail database
	 *
	 *  return: true if successful. false on failure. getLastError() provides the error message.
	 *
	 *  see: open(const std::string& databaseFilePath)
	 */
	bool clear();

	/**
	 * Checks if the currently open database file contains any data
	 *
	 *  return: true after opening a non-existing database file or clearing the open database
	 */
	bool isEmpty() const;

	/**
	 * Checks if the currently open database is compatible with the SourcetrailDBWriter version.
	 *
	 *  return: true for an empty database or a database that has been created with the same
	 *   database version.
	 *
	 *  see: getSupportedDatabaseVersion()
	 *  see: getLoadedDatabaseVersion()
	 */
	bool isCompatible() const;

	/**
	 * Provides the database version of the loaded database file as an integer
	 *
	 *  return: database version of loaded database
	 */
	int getLoadedDatabaseVersion() const;

	/**
	 * Starts a database transaction on the currently open database
	 *
	 * Apart from allowing to restore the database to the state it was in before this method has
	 * been called, wrapping multiple calls that record data with one transaction significantly
	 * increases the performance of these database operations.
	 *
	 *  return: true if successful. false on failure. getLastError() provides the error message.
	 *
	 *  see: commitTransaction()
	 *  see: rollbackTransaction()
	 */
	bool beginTransaction();

	/**
	 * Ends the current transaction and writes all of its changes persistently to the database
	 *
	 *  return: true if successful. false on failure. getLastError() provides the error message.
	 */
	bool commitTransaction();

	/**
	 * Reverts the database to the state it was in before the current transaction was started
	 *
	 *  return: true if successful. false on failure. getLastError() provides the error message.
	 */
	bool rollbackTransaction();

	/**
	 * Reduces the on disk memory consumption of the open database to a minimum
	 *
	 *  return: true if successful. false on failure. getLastError() provides the error message.
	 */
	bool optimizeDatabaseMemory();

	/**
	 * Stores a symbol to the database
	 *
	 *  note: Calling this method multiple times with the same input on the same Sourcetrail
	 *    database will always return the same id.
	 *
	 *  param: nameHierarchy - the name of the symbol to store.
	 *
	 *  return: symbolId - integer id of the stored symbol. 0 on failure. getLastError()
	 *    provides the error message.
	 *
	 *  see: NameHierarchy
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
	 *  param: symbolId - the id of the symbol for which a DefinitionKind shall be recorded.
	 *  param: definitionKind - the DefinitionKind that shall be recorded for the respective symbol.
	 *
	 *  return: true if successful. false on failure. getLastError() provides the error message.
	 *
	 *  see: DefinitionKind
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
	 *  param: symbolId - the id of the symbol for which a SymbolKind shall be recorded.
	 *  param: symbolKind - the SymbolKind that shall be recorded for the respective symbol.
	 *
	 *  return: true if successful. false on failure. getLastError() provides the error message.
	 *
	 *  see: SymbolKind
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
	 *  param: symbolId - the id of the symbol for which a location shall be recorded.
	 *  param: location - the SourceRange that shall be recorded as location for the respective symbol.
	 *
	 *  return: true if successful. false on failure. getLastError() provides the error message.
	 *
	 *  see: SourceRange
	 */
	bool recordSymbolLocation(int symbolId, const SourceRange& location);

	/**
	 * Stores a scope location for a specific symbol to the database
	 *
	 * This method allows to store a scope location for a symbol referenced by id to the database.
	 * Calling this method with the same symbolId multiple times adds multiple scope locations for the
	 * respective symbol. The stored location will only be displayable and not clickable. When the
	 * symbol with the specified id is activated, this location will be fully displayed but not
	 * highlighted by Sourcetrail.
	 *
	 *  param: symbolId - the id of the symbol for which a scope location shall be recorded.
	 *  param: location - the SourceRange that shall be recorded as scope location for the respective
	 *    symbol.
	 *
	 *  return: true if successful. false on failure. getLastError() provides the error message.
	 *
	 *  see: SourceRange
	 */
	bool recordSymbolScopeLocation(int symbolId, const SourceRange& location);

	/**
	 * Stores a signature location for a specific symbol to the database
	 *
	 * This method allows to store a signature location for a symbol referenced by id to the
	 * database. If a signature location is recorded for a symbol, Sourcetrail will display the
	 * respective source code in a tooltip whenever the symbol with the referenced id or any of
	 * that symbol's locations gets hovered.
	 * If no signature location is recorded for a symbol, Sourcetrail will show its name hierarchy.
	 *
	 *  note: Calling this method with the same symbolId multiple times adds multiple signature
	 *    locations for the respective symbol. It is not guaranteed which one will be used, so it is
	 *    advised to call it only once per symbol.
	 *
	 *  param: symbolId - the id of the symbol for which a signature location shall be recorded.
	 *  param: location - the SourceRange that shall be recorded as signature location for the
	 *    respective symbol.
	 *
	 *  return: true if successful. false on failure. getLastError() provides the error message.
	 *
	 *  see: SourceRange
	 */
	bool recordSymbolSignatureLocation(int symbolId, const SourceRange& location);

	/**
	 * Stores a reference between two symbols to the database
	 *
	 * This method allows to store the information of symbols referencing one another to the database.
	 * For each recorded reference Sourcetrail's graph view will display an edge that originates at
	 * the reference's recorded context symbol and points to the recorded referenced symbol. The
	 * recorded ReferenceKind is used to determine the type of the displayed edge and to generate a
	 * description in the hover tooltip of the edge.
	 *
	 *  note: Calling this method multiple times with the same input on the same Sourcetrail database
	 *    will always return the same id.
	 *
	 *  param: contextSymbolId - the id of the source of the recorded reference edge
	 *  param: referencedSymbolId - the id of the target of the recorded reference edge
	 *  param: referenceKind - kind of the recorded reference edge
	 *
	 *  return: referenceId - integer id of the stored reference. 0 on failure. getLastError()
	 *    provides the error message.
	 *
	 *  see: ReferenceKind
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
	 *  param: referenceId - the id of the reference for which a location shall be recorded.
	 *  param: location - the SourceRange that shall be recorded as location for the respective
	 *    reference.
	 *
	 *  return: true if successful. false on failure. getLastError() provides the error message.
	 *
	 *  see: SourceRange
	 */
	bool recordReferenceLocation(int referenceId, const SourceRange& location);

	/**
	 * Marks a reference that is stored in the database as "ambiguous"
	 *
	 * This method allows to additional information for a reference to the database. Sourcetrail will
	 * display an "ambiguous" reference with a special style to emphasize that the existance of the
	 * reference is questionable. This method is intended to be called in situations when an indexed
	 * token may have meanings, all of which shall be recorded.
	 *
	 *  param: referenceId - the id of the reference that shall be marked as ambiguous.
	 *
	 *  return: true if successful. false on failure. getLastError() provides the error message.
	 */
	bool recordReferenceIsAmbiguous(int referenceId);

	/**
	 * Stores a location between a specific context and an "unsolved" symbol to the database
	 *
	 * This method allows to store all available information to the database in the case that a symbol
	 * is referenced in a certain context but the referenced symbol could not be resolved to a concrete
	 * name. For each reference recorded by this method, Sourcetrail's graph view will display an edge
	 * that originates at the recorded context symbol and points to a node called "unsolved symbol".
	 * Furthermore Sourcetrail's code view will use a different highlight when the provided source range
	 * gets hovered.
	 *
	 *  param: contextSymbolId - the id of the source of the recorded reference edge
	 *  param: referenceKind - kind of the recorded reference edge
	 *  param: location - the SourceRange that shall be recorded as location for the respective
	 *    reference.
	 *
	 *  return: referenceId - integer id of the stored reference. 0 on failure. getLastError()
	 *    provides the error message.
	 *
	 *  see: SourceRange
	 */
	int recordReferenceToUnsolvedSymhol(int contextSymbolId, ReferenceKind referenceKind, const SourceRange& location);

	/**
	 * Stores a location for the usage of a symbol's name as qualifier to the database
	 *
	 * This method allows to store a location where a specific symbol is used as a qualifier to the
	 * database. Calling this method with the same referencedSymbolId multiple times adds multiple locations
	 * for the respective reference. The stored location will be clickable but not displayable: When the
	 * reference location is clicked Sourcetrail will activate the symbol referenced by the respective
	 * reference. When the symbol with the specified id is activated, this location will NOT be displayed and
	 * highlighted by Sourcetrail.
	 *
	 *  param: referencedSymbolId - the id of the symbol that is used as qualifier at the current location.
	 *  param: location - the SourceRange that shall be recorded as location for the symbol's occurrence as
	 *    qualifier
	 *
	 *  return: true if successful. false on failure. getLastError() provides the error message.
	 *
	 *  see: SourceRange
	 */
	bool recordQualifierLocation(int referencedSymbolId, const SourceRange& location);

	/**
	 * Stores a file to the database
	 *
	 *  note: Calling this method multiple times with the same input on the same Sourcetrail database
	 *    will always return the same id.
	 *
	 *  param: filePath - the absolute path to the file to store.
	 *
	 *  return: fileId - integer id of the stored file. 0 on failure. getLastError() provides the
	 *    error message.
	 */
	int recordFile(const std::string& filePath);

	/**
	 * Stores language information for a specific file to the database
	 *
	 * This method allows to store language information for a file to the database. Language information
	 * is passed and stored as string (e.g. "cpp", "java", etc.) and allows Sourcetrail to pick the
	 * correct syntax highlighting rules when displaying the respective source file.
	 *
	 *  param: fileId - the id of the file for which language information shall be recorded.
	 *  param: languageIdentifier - a string that denotes the programming language the respective file
	 *    is written in.
	 *
	 *  return: true if successful. false on failure. getLastError() provides the error message.
	 */
	bool recordFileLanguage(int fileId, const std::string& languageIdentifier);

	/**
	 * Stores a local symbol to the database
	 *
	 *  note: Calling this method multiple times with the same input on the same Sourcetrail database
	 *    will always return the same id.
	 *
	 *  param: name - a name that is unique for this local symbol (e.g. the string encoded location
	 *    of the local symbol's definition).
	 *
	 *  return: localSymbolId - integer id of the stored local symbol. 0 on failure. getLastError()
	 *    provides the error message.
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
	 *  param: localSymbolId - the id of the local symbol for which a location shall be recorded.
	 *  param: location - the SourceRange that shall be recorded as location for the respective
	 *    local symbol.
	 *
	 *  return: true if successful. false on failure. getLastError() provides the error message.
	 *
	 *  see: SourceRange
	 */
	bool recordLocalSymbolLocation(int localSymbolId, const SourceRange& location);

	/**
	 * Stores an atomic SourceRange to the database
	 *
	 * This method allows to store an atomic SourceRange to the database. These ranges will
	 * be used by Sourcetrail to prevent the code view from displaying only a part of the range. Thus,
	 * if any line that lies within one of the project's atomic ranges is dispayed, the remaining
	 * lines will be displayed as well. This may be useful for dealing with multi-line comments or
	 * multi-line strings.
	 *
	 *  param: sourceRange - the SourceRange to record.
	 *
	 *  return: true if successful. false on failure. getLastError() provides the error message.
	 *
	 *  see: SourceRange
	 */
	bool recordAtomicSourceRange(const SourceRange& sourceRange);

	/**
	 * Stores an indexing error to the database
	 *
	 * This method allows to store an indexing error to the database. Errors will be shown by
	 * Sourcetrail's error view. When clicking the error in Sourcetrail's error list, the code view
	 * will display this location.
	 *
	 *  param: message - an error message that will be displayed in Sourcetrail's error list.
	 *  param: fatal - boolean fatal indicates whether parsing/indexing was aborted at this point.
	 *  param: location - the SourceRange of the error encountered.
	 *
	 *  return: true if successful. false on failure. getLastError() provides the error message.
	 *
	 *  see: SourceRange
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
	void addElementComponent(int elementId, ElementComponentKind kind, const std::string& data);

	std::string m_projectFilePath;
	std::string m_databaseFilePath;
	std::unique_ptr<DatabaseStorage> m_storage;
	mutable std::string m_lastError;
};
}	 // namespace sourcetrail

#endif	  // SOURCETRAIL_SRCTRLDB_WRITER_H
