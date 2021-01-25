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

#include "SourcetrailDBWriter.h"

#include <algorithm>
#include <fstream>
#include <vector>

#include "CppSQLite3.h"

#include "DatabaseStorage.h"
#include "DefinitionKind.h"
#include "EdgeKind.h"
#include "LocationKind.h"
#include "NameHierarchy.h"
#include "NodeKind.h"
#include "ReferenceKind.h"
#include "SourceRange.h"
#include "SourcetrailException.h"
#include "SymbolKind.h"
#include "utility.h"
#include "version.h"

namespace sourcetrail
{
// --- Public Interface ---

SourcetrailDBWriter::SourcetrailDBWriter(): m_lastError("") {}

SourcetrailDBWriter::~SourcetrailDBWriter() {}

std::string SourcetrailDBWriter::getVersionString() const
{
	return VERSION_STRING;
}

int SourcetrailDBWriter::getSupportedDatabaseVersion() const
{
	return DatabaseStorage::getSupportedDatabaseVersion();
}

const std::string& SourcetrailDBWriter::getLastError() const
{
	return m_lastError;
}

void SourcetrailDBWriter::setLastError(const std::string& error) const
{
	m_lastError = error;
}

void SourcetrailDBWriter::clearLastError()
{
	m_lastError.clear();
}

bool SourcetrailDBWriter::open(const std::string& databaseFilePath)
{
	m_databaseFilePath = databaseFilePath;

	m_projectFilePath = databaseFilePath;	 // FIXME: only find dot after last slash/backslash! otherwise it may be part of the file path
	const size_t pos = m_projectFilePath.rfind('.');
	if (pos != std::string::npos)
	{
		m_projectFilePath = m_projectFilePath.substr(0, pos);
	}
	m_projectFilePath += ".srctrlprj";

	{
		bool projectFileExists = false;
		{
			std::ifstream f(m_projectFilePath.c_str());
			projectFileExists = f.good();
			f.close();
		}
		if (!projectFileExists)
		{
			try
			{
				createOrResetProjectFile();
			}
			catch (const SourcetrailException e)
			{
				m_lastError = e.getMessage();
				return false;
			}
		}
	}

	try
	{
		openDatabase();
	}
	catch (const SourcetrailException e)
	{
		m_lastError = e.getMessage();
		return false;
	}

	try
	{
		setupDatabaseTables();
		updateProjectSettingsText();
	}
	catch (const SourcetrailException e)
	{
		m_lastError = e.getMessage();
		return false;
	}

	return true;
}

bool SourcetrailDBWriter::close()
{
	try
	{
		closeDatabase();
	}
	catch (const SourcetrailException e)
	{
		m_lastError = e.getMessage();
		return false;
	}

	return true;
}

bool SourcetrailDBWriter::clear()
{
	try
	{
		clearDatabaseTables();
		updateProjectSettingsText();
	}
	catch (const SourcetrailException e)
	{
		m_lastError = e.getMessage();
		return false;
	}

	return true;
}

bool SourcetrailDBWriter::isEmpty() const
{
	if (!m_storage)
	{
		m_lastError = "Unable to check if database is empty, because no database is currently open.";
		return true;
	}

	try
	{
		return m_storage->isEmpty();
	}
	catch (const SourcetrailException e)
	{
		m_lastError = e.getMessage();
		return true;
	}
}

bool SourcetrailDBWriter::isCompatible() const
{
	if (!m_storage)
	{
		m_lastError = "Unable to check if database is compatible, because no database is currently open.";
		return false;
	}

	try
	{
		return m_storage->isCompatible();
	}
	catch (const SourcetrailException e)
	{
		m_lastError = e.getMessage();
		return false;
	}
}

int SourcetrailDBWriter::getLoadedDatabaseVersion() const
{
	if (!m_storage)
	{
		m_lastError = "Unable to fetch database version, because no database is currently open.";
		return false;
	}

	try
	{
		return m_storage->getLoadedDatabaseVersion();
	}
	catch (const SourcetrailException e)
	{
		m_lastError = e.getMessage();
		return 0;
	}
}

bool SourcetrailDBWriter::beginTransaction()
{
	if (!m_storage)
	{
		m_lastError = "Unable to begin transaction, because no database is currently open.";
		return false;
	}

	try
	{
		m_storage->beginTransaction();
	}
	catch (const SourcetrailException e)
	{
		m_lastError = e.getMessage();
		return false;
	}

	return true;
}

bool SourcetrailDBWriter::commitTransaction()
{
	if (!m_storage)
	{
		m_lastError = "Unable to commit transaction, because no database is currently open.";
		return false;
	}

	try
	{
		m_storage->commitTransaction();
	}
	catch (const SourcetrailException e)
	{
		m_lastError = e.getMessage();
		return false;
	}

	return true;
}

bool SourcetrailDBWriter::rollbackTransaction()
{
	if (!m_storage)
	{
		m_lastError = "Unable to rollback transaction, because no database is currently open.";
		return false;
	}

	try
	{
		m_storage->rollbackTransaction();
	}
	catch (const SourcetrailException e)
	{
		m_lastError = e.getMessage();
		return false;
	}

	return true;
}

bool SourcetrailDBWriter::optimizeDatabaseMemory()
{
	if (!m_storage)
	{
		m_lastError = "Unable to optimize database memory, because no database is currently open.";
		return false;
	}

	try
	{
		m_storage->optimizeDatabaseMemory();
	}
	catch (const SourcetrailException e)
	{
		m_lastError = e.getMessage();
		return false;
	}

	return true;
}

int SourcetrailDBWriter::recordSymbol(const NameHierarchy& nameHierarchy)
{
	if (!m_storage)
	{
		m_lastError = "Unable to record symbol, because no database is currently open.";
		return false;
	}

	try
	{
		return addNodeHierarchy(nameHierarchy);
	}
	catch (const SourcetrailException e)
	{
		m_lastError = e.getMessage();
		return 0;
	}
}

bool SourcetrailDBWriter::recordSymbolDefinitionKind(int symbolId, DefinitionKind definitionKind)
{
	if (!m_storage)
	{
		m_lastError = "Unable to record symbol kind, because no database is currently open.";
		return false;
	}

	try
	{
		m_storage->addSymbol(StorageSymbol(symbolId, definitionKindToInt(definitionKind)));
	}
	catch (const SourcetrailException e)
	{
		m_lastError = e.getMessage();
		return false;
	}

	return true;
}

bool SourcetrailDBWriter::recordSymbolKind(int symbolId, SymbolKind symbolKind)
{
	if (!m_storage)
	{
		m_lastError = "Unable to record symbol kind, because no database is currently open.";
		return false;
	}

	try
	{
		m_storage->setNodeType(symbolId, nodeKindToInt(symbolKindToNodeKind(symbolKind)));
	}
	catch (const SourcetrailException e)
	{
		m_lastError = e.getMessage();
		return false;
	}

	return true;
}

bool SourcetrailDBWriter::recordSymbolLocation(int symbolId, const SourceRange& location)
{
	if (!m_storage)
	{
		m_lastError = "Unable to record symbol location, because no database is currently open.";
		return false;
	}

	try
	{
		addSourceLocation(symbolId, location, LocationKind::TOKEN);
		return true;
	}
	catch (const SourcetrailException e)
	{
		m_lastError = e.getMessage();
		return false;
	}
}

bool SourcetrailDBWriter::recordSymbolScopeLocation(int symbolId, const SourceRange& location)
{
	if (!m_storage)
	{
		m_lastError = "Unable to record symbol scope location, because no database is currently open.";
		return false;
	}

	try
	{
		addSourceLocation(symbolId, location, LocationKind::SCOPE);
		return true;
	}
	catch (const SourcetrailException e)
	{
		m_lastError = e.getMessage();
		return false;
	}
}

bool SourcetrailDBWriter::recordSymbolSignatureLocation(int symbolId, const SourceRange& location)
{
	if (!m_storage)
	{
		m_lastError = "Unable to record symbol signature location, because no database is currently open.";
		return false;
	}

	try
	{
		addSourceLocation(symbolId, location, LocationKind::SIGNATURE);
		return true;
	}
	catch (const SourcetrailException e)
	{
		m_lastError = e.getMessage();
		return false;
	}
}

int SourcetrailDBWriter::recordReference(int contextSymbolId, int referencedSymbolId, ReferenceKind referenceKind)
{
	if (!m_storage)
	{
		m_lastError = "Unable to record reference, because no database is currently open.";
		return 0;
	}

	try
	{
		return addEdge(contextSymbolId, referencedSymbolId, referenceKindToEdgeKind(referenceKind));
	}
	catch (const SourcetrailException e)
	{
		m_lastError = e.getMessage();
		return 0;
	}
}

bool SourcetrailDBWriter::recordReferenceLocation(int referenceId, const SourceRange& location)
{
	if (!m_storage)
	{
		m_lastError = "Unable to record symbol reference location, because no database is currently open.";
		return false;
	}

	try
	{
		addSourceLocation(referenceId, location, LocationKind::TOKEN);
		return true;
	}
	catch (const SourcetrailException e)
	{
		m_lastError = e.getMessage();
		return false;
	}
}

bool SourcetrailDBWriter::recordReferenceIsAmbiguous(int referenceId)
{
	if (!m_storage)
	{
		m_lastError = "Unable to record ambiguity of reference, because no database is currently open.";
		return false;
	}

	try
	{
		addElementComponent(referenceId, ElementComponentKind::IS_AMBIGUOUS, "");
		return false;
	}
	catch (const SourcetrailException e)
	{
		m_lastError = e.getMessage();
		return false;
	}
}

int SourcetrailDBWriter::recordReferenceToUnsolvedSymhol(int contextSymbolId, ReferenceKind referenceKind, const SourceRange& location)
{
	if (!m_storage)
	{
		m_lastError = "Unable to record symbol reference, because no database is currently open.";
		return 0;
	}

	try
	{
		NameHierarchy unsolvedSymbolName;
		NameElement unsolvedSymbolNameElement;
		unsolvedSymbolNameElement.name = "unsolved symbol";
		unsolvedSymbolName.nameElements.push_back(unsolvedSymbolNameElement);
		int unsolvedSymbolId = addNodeHierarchy(unsolvedSymbolName);
		int referenceId = addEdge(contextSymbolId, unsolvedSymbolId, referenceKindToEdgeKind(referenceKind));
		addSourceLocation(referenceId, location, LocationKind::UNSOLVED);
		return referenceId;
	}
	catch (const SourcetrailException e)
	{
		m_lastError = e.getMessage();
		return 0;
	}
}

bool SourcetrailDBWriter::recordQualifierLocation(int referencedSymbolId, const SourceRange& location)
{
	if (!m_storage)
	{
		m_lastError = "Unable to record symbol qualifier location, because no database is currently open.";
		return false;
	}

	try
	{
		addSourceLocation(referencedSymbolId, location, LocationKind::QUALIFIER);
		return true;
	}
	catch (const SourcetrailException e)
	{
		m_lastError = e.getMessage();
		return false;
	}
}

int SourcetrailDBWriter::recordFile(const std::string& filePath)
{
	if (!m_storage)
	{
		m_lastError = "Unable to record file, because no database is currently open.";
		return false;
	}

	try
	{
		return addFile(filePath);
	}
	catch (const SourcetrailException e)
	{
		m_lastError = e.getMessage();
		return 0;
	}
}

bool SourcetrailDBWriter::recordFileLanguage(int fileId, const std::string& languageIdentifier)
{
	if (!m_storage)
	{
		m_lastError = "Unable to record file language, because no database is currently open.";
		return false;
	}

	try
	{
		m_storage->setFileLanguage(fileId, languageIdentifier);
	}
	catch (const SourcetrailException e)
	{
		m_lastError = e.getMessage();
		return false;
	}

	return true;
}

int SourcetrailDBWriter::recordLocalSymbol(const std::string& name)
{
	if (!m_storage)
	{
		m_lastError = "Unable to record local symbol, because no database is currently open.";
		return false;
	}

	try
	{
		return m_storage->addLocalSymbol(StorageLocalSymbolData(name));
	}
	catch (const SourcetrailException e)
	{
		m_lastError = e.getMessage();
		return 0;
	}
}

bool SourcetrailDBWriter::recordLocalSymbolLocation(int localSymbolId, const SourceRange& location)
{
	if (!m_storage)
	{
		m_lastError = "Unable to record local symbol location, because no database is currently open.";
		return false;
	}

	try
	{
		addSourceLocation(localSymbolId, location, LocationKind::LOCAL_SYMBOL);
		return true;
	}
	catch (const SourcetrailException e)
	{
		m_lastError = e.getMessage();
		return false;
	}
}

bool SourcetrailDBWriter::recordAtomicSourceRange(const SourceRange& sourceRange)
{
	if (!m_storage)
	{
		m_lastError = "Unable to record atomic source range, because no database is currently open.";
		return false;
	}

	try
	{
		const int sourceLocationId = m_storage->addSourceLocation(StorageSourceLocationData(
			sourceRange.fileId,
			sourceRange.startLine,
			sourceRange.startColumn,
			sourceRange.endLine,
			sourceRange.endColumn,
			locationKindToInt(LocationKind::ATOMIC_RANGE)));

		return true;
	}
	catch (const SourcetrailException e)
	{
		m_lastError = e.getMessage();
		return false;
	}
}

bool SourcetrailDBWriter::recordError(const std::string& message, bool fatal, const SourceRange& location)
{
	if (!m_storage)
	{
		m_lastError = "Unable to record error, because no database is currently open.";
		return false;
	}

	try
	{
		const int errorId = m_storage->addError(StorageErrorData(message, "", fatal, true));
		addSourceLocation(errorId, location, LocationKind::INDEXER_ERROR);
		return true;
	}
	catch (const SourcetrailException e)
	{
		m_lastError = e.getMessage();
		return false;
	}
}

// --- Private Interface ---

void SourcetrailDBWriter::openDatabase()
{
	if (m_storage)
	{
		closeDatabase();
	}

	try
	{
		m_storage = DatabaseStorage::openDatabase(m_databaseFilePath);
	}
	catch (CppSQLite3Exception e)
	{
		m_storage.reset();
		throw e;
	}
}

void SourcetrailDBWriter::closeDatabase()
{
	if (!m_storage)
	{
		throw SourcetrailException("Unable to close database, because no database is currently open.");
	}
	m_storage.reset();
}

void SourcetrailDBWriter::setupDatabaseTables()
{
	if (!m_storage)
	{
		throw SourcetrailException("Unable to setup database tables, because no database is currently open.");
	}
	m_storage->setupDatabase();
}

void SourcetrailDBWriter::clearDatabaseTables()
{
	if (!m_storage)
	{
		throw SourcetrailException("Unable to setup database tables, because no database is currently open.");
	}
	m_storage->clearDatabase();
}

void SourcetrailDBWriter::createOrResetProjectFile()
{
	try
	{
		std::ofstream fileStream;
		fileStream.open(m_projectFilePath, std::ios::out);
		fileStream << std::string(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <version>0</version>\n"
			"</config>\n");
		fileStream.close();
	}
	catch (...)
	{
		throw SourcetrailException("Exception occurred while creating project file.");
	}
}

void SourcetrailDBWriter::updateProjectSettingsText()
{
	try
	{
		std::ifstream f(m_projectFilePath.c_str());
		if (f.is_open())
		{
			std::string line;
			std::string text;
			while (getline(f, line))
			{
				text += line + '\n';
			}
			f.close();
			m_storage->setProjectSettingsText(text);
		}
		else
		{
			throw SourcetrailException("Unable to open project file.");
		}
	}
	catch (...)
	{
		throw SourcetrailException("Exception occurred while creating project file.");
	}
}

int SourcetrailDBWriter::addNodeHierarchy(const NameHierarchy& nameHierarchy)
{
	if (nameHierarchy.nameElements.size() == 0)
	{
		throw SourcetrailException("Unable to add nodes for an empty name hierarchy.");
	}

	int parentNodeId = 0;

	NameHierarchy currentNameHierarchy;
	currentNameHierarchy.nameDelimiter = nameHierarchy.nameDelimiter;

	for (size_t i = 0; i < nameHierarchy.nameElements.size(); i++)
	{
		currentNameHierarchy.nameElements.push_back(nameHierarchy.nameElements[i]);

		int nodeId = m_storage->addNode(
			StorageNodeData(nodeKindToInt(NodeKind::UNKNOWN), serializeNameHierarchyToDatabaseString(currentNameHierarchy)));

		if (parentNodeId != 0)
		{
			addEdge(parentNodeId, nodeId, EdgeKind::MEMBER);
		}

		parentNodeId = nodeId;
	}
	return parentNodeId;
}

int SourcetrailDBWriter::addFile(const std::string& filePath)
{
	NameElement nameElement;
	nameElement.name = filePath;
	NameHierarchy nameHierarchy;
	nameHierarchy.nameDelimiter = "/";
	nameHierarchy.nameElements.push_back(nameElement);

	const int nodeId = addNodeHierarchy(nameHierarchy);
	m_storage->setNodeType(nodeId, nodeKindToInt(NodeKind::FILE));

	m_storage->addFile(StorageFile(nodeId, filePath, "", utility::getDateTimeString(time(0)), true, true));

	return nodeId;
}

int SourcetrailDBWriter::addEdge(int sourceId, int targetId, EdgeKind edgeKind)
{
	if (!m_storage)
	{
		throw SourcetrailException("Unable to add edge, because no database is currently open.");
	}
	if (!sourceId)
	{
		throw SourcetrailException("Unable to add edge, because source id is invalid.");
	}
	if (!targetId)
	{
		throw SourcetrailException("Unable to add edge, because target id is invalid.");
	}

	return m_storage->addEdge(StorageEdgeData(sourceId, targetId, edgeKindToInt(edgeKind)));
}

void SourcetrailDBWriter::addSourceLocation(int elementId, const SourceRange& location, LocationKind kind)
{
	const int sourceLocationId = m_storage->addSourceLocation(StorageSourceLocationData(
		location.fileId, location.startLine, location.startColumn, location.endLine, location.endColumn, locationKindToInt(kind)));

	m_storage->addOccurrence(StorageOccurrence(elementId, sourceLocationId));
}

void SourcetrailDBWriter::addElementComponent(int elementId, ElementComponentKind kind, const std::string& data)
{
	const int sourceLocationId = m_storage->addElementComponent(StorageElementComponentData(elementId, elementComponentKindToInt(kind), data));
}
}	 // namespace sourcetrail
