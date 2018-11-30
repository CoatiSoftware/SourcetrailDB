#include "SourcetrailDBWriter.h"

#include <fstream>
#include <vector>

#include "sqlite/CppSQLite3.h"

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
#include "version.h"

namespace sourcetrail
{
	// --- Public Interface ---

	SourcetrailDBWriter::SourcetrailDBWriter()
		: m_lastError("")
	{
	}

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

	void SourcetrailDBWriter::clearLastError()
	{
		m_lastError.clear();
	}

	bool SourcetrailDBWriter::openProject(const std::string& projectDirectory, const std::string& projectName)
	{
		m_projectDirectory = projectDirectory;
		m_projectName = projectName;

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
		}
		catch (const SourcetrailException e)
		{
			m_lastError = e.getMessage();
			return false;
		}

		{
			bool projectFileExists = false;
			{
				std::ifstream f(getProjectFilePath().c_str());
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

		return true;
	}

	bool SourcetrailDBWriter::closeProject()
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

	bool SourcetrailDBWriter::clearProject()
	{
		try
		{
			clearDatabaseTables();
		}
		catch (const SourcetrailException e)
		{
			m_lastError = e.getMessage();
			return false;
		}

		try
		{
			createOrResetProjectFile();
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
			m_storage->addSymbol(symbolId, definitionKindToInt(definitionKind));
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
			addSourceLocation(symbolId, location, LOCATION_TOKEN);
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
			addSourceLocation(symbolId, location, LOCATION_SCOPE);
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
			addSourceLocation(symbolId, location, LOCATION_SIGNATURE);
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
			return false;
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
			m_lastError = "Unable to record symbol signature location, because no database is currently open.";
			return false;
		}

		try
		{
			addSourceLocation(referenceId, location, LOCATION_TOKEN);
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

	int SourcetrailDBWriter::recordLocalSymbol(const std::string& name)
	{
		if (!m_storage)
		{
			m_lastError = "Unable to record local symbol, because no database is currently open.";
			return false;
		}

		try
		{
			return m_storage->addLocalSymbol(name);
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
			addSourceLocation(localSymbolId, location, LOCATION_LOCAL_SYMBOL);
			return true;
		}
		catch (const SourcetrailException e)
		{
			m_lastError = e.getMessage();
			return false;
		}
	}

	bool SourcetrailDBWriter::recordCommentLocation(const SourceRange& location)
	{
		if (!m_storage)
		{
			m_lastError = "Unable to record comment location, because no database is currently open.";
			return false;
		}

		try
		{
			const int fileId = addFile(location.filePath);
			const int sourceLocationId = m_storage->addSourceLocation(
				fileId,
				location.startLine,
				location.startColumn,
				location.endLine,
				location.endColumn,
				LOCATION_COMMENT
			);

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
			const int errorId = m_storage->addError(message, fatal);
			addSourceLocation(errorId, location, LOCATION_ERROR);
			return true;
		}
		catch (const SourcetrailException e)
		{
			m_lastError = e.getMessage();
			return false;
		}
	}

	// --- Private Interface ---

	std::string SourcetrailDBWriter::serializeNameHierarchy(const NameHierarchy& nameHierarchy)
	{
		static std::string META_DELIMITER = "\tm";
		static std::string NAME_DELIMITER = "\tn";
		static std::string PARTS_DELIMITER = "\ts";
		static std::string SIGNATURE_DELIMITER = "\tp";

		std::string serialized = nameHierarchy.nameDelimiter + META_DELIMITER;
		for (size_t i = 0; i < nameHierarchy.nameElements.size(); i++)
		{
			if (i != 0)
			{
				serialized += NAME_DELIMITER;
			}
			const NameElement& nameElement = nameHierarchy.nameElements[i];
			serialized += nameElement.name + PARTS_DELIMITER + nameElement.prefix + SIGNATURE_DELIMITER + nameElement.postfix;
		}
		return serialized;
	}

	std::string SourcetrailDBWriter::getProjectFilePath() const
	{
		return m_projectDirectory + "/" + m_projectName + ".srctrlprj";
	}
	std::string SourcetrailDBWriter::getDatabaseFilePath() const
	{
		return m_projectDirectory + "/" + m_projectName + ".srctrldb";
	}

	void SourcetrailDBWriter::openDatabase()
	{
		if (m_storage)
		{
			closeDatabase();
		}

		try
		{
			m_storage = DatabaseStorage::openDatabase(getDatabaseFilePath());
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
		m_storage->setupTables();
	}

	void SourcetrailDBWriter::clearDatabaseTables()
	{
		if (!m_storage)
		{
			throw SourcetrailException("Unable to setup database tables, because no database is currently open.");
		}
		m_storage->clearTables();
	}

	void SourcetrailDBWriter::createOrResetProjectFile()
	{
		try
		{
			std::ofstream fileStream;
			fileStream.open(getProjectFilePath(), std::ios::out);
			fileStream << std::string(
				"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
				"<config>\n"
				"    <version>0</version>\n"
				"</config>\n"
			);
			fileStream.close();
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

			int nodeId = m_storage->addNode(serializeNameHierarchy(currentNameHierarchy));

			if (parentNodeId != 0)
			{
				addEdge(parentNodeId, nodeId, EDGE_MEMBER);
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
		m_storage->setNodeType(nodeId, nodeKindToInt(NODE_FILE));
		m_storage->addFile(nodeId, filePath);

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

		return m_storage->addEdge(sourceId, targetId, edgeKindToInt(edgeKind));
	}

	void SourcetrailDBWriter::addSourceLocation(int elementId, const SourceRange& location, LocationKind kind)
	{
		const int fileId = addFile(location.filePath);

		const int sourceLocationId = m_storage->addSourceLocation(
			fileId,
			location.startLine,
			location.startColumn,
			location.endLine,
			location.endColumn,
			locationKindToInt(kind)
		);

		m_storage->addOccurrence(
			elementId,
			sourceLocationId
		);
	}
}
