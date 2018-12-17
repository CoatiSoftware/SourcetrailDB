#include "sourcetraildb.h"

#include "DefinitionKind.h"
#include "NameHierarchy.h"
#include "SymbolKind.h"
#include "SourceRange.h"
#include "SourcetrailDBWriter.h"
#include "ReferenceKind.h"

namespace
{
	sourcetrail::DefinitionKind convertDefinitionKind(::DefinitionKind v)
	{
		switch (v)
		{
		case DEFINITION_IMPLICIT:
			return sourcetrail::DefinitionKind::IMPLICIT;
		case DEFINITION_EXPLICIT:
			return sourcetrail::DefinitionKind::EXPLICIT;
		}
		return sourcetrail::DefinitionKind::EXPLICIT;
	}

	sourcetrail::SymbolKind convertSymbolKind(::SymbolKind v)
	{
		switch (v)
		{
		case SYMBOL_TYPE:
			return sourcetrail::SymbolKind::TYPE;
		case SYMBOL_BUILTIN_TYPE:
			return sourcetrail::SymbolKind::BUILTIN_TYPE;
		case SYMBOL_MODULE:
			return sourcetrail::SymbolKind::MODULE;
		case SYMBOL_NAMESPACE:
			return sourcetrail::SymbolKind::NAMESPACE;
		case SYMBOL_PACKAGE:
			return sourcetrail::SymbolKind::PACKAGE;
		case SYMBOL_STRUCT:
			return sourcetrail::SymbolKind::STRUCT;
		case SYMBOL_CLASS:
			return sourcetrail::SymbolKind::CLASS;
		case SYMBOL_INTERFACE:
			return sourcetrail::SymbolKind::INTERFACE;
		case SYMBOL_ANNOTATION:
			return sourcetrail::SymbolKind::ANNOTATION;
		case SYMBOL_GLOBAL_VARIABLE:
			return sourcetrail::SymbolKind::GLOBAL_VARIABLE;
		case SYMBOL_FIELD:
			return sourcetrail::SymbolKind::FIELD;
		case SYMBOL_FUNCTION:
			return sourcetrail::SymbolKind::FUNCTION;
		case SYMBOL_METHOD:
			return sourcetrail::SymbolKind::METHOD;
		case SYMBOL_ENUM:
			return sourcetrail::SymbolKind::ENUM;
		case SYMBOL_ENUM_CONSTANT:
			return sourcetrail::SymbolKind::ENUM_CONSTANT;
		case SYMBOL_TYPEDEF:
			return sourcetrail::SymbolKind::TYPEDEF;
		case SYMBOL_TEMPLATE_PARAMETER:
			return sourcetrail::SymbolKind::TEMPLATE_PARAMETER;
		case SYMBOL_TYPE_PARAMETER:
			return sourcetrail::SymbolKind::TYPE_PARAMETER;
		case SYMBOL_MACRO:
			return sourcetrail::SymbolKind::MACRO;
		case SYMBOL_UNION:
			return sourcetrail::SymbolKind::UNION;
		}
		return sourcetrail::SymbolKind::TYPE;
	}

	sourcetrail::ReferenceKind convertReferenceKind(::ReferenceKind v)
	{
		switch (v)
		{
		case REFERENCE_TYPE_USAGE:
			return sourcetrail::ReferenceKind::TYPE_USAGE;
		case REFERENCE_USAGE:
			return sourcetrail::ReferenceKind::USAGE;
		case REFERENCE_CALL:
			return sourcetrail::ReferenceKind::CALL;
		case REFERENCE_INHERITANCE:
			return sourcetrail::ReferenceKind::INHERITANCE;
		case REFERENCE_OVERRIDE:
			return sourcetrail::ReferenceKind::OVERRIDE;
		case REFERENCE_TEMPLATE_ARGUMENT:
			return sourcetrail::ReferenceKind::TEMPLATE_ARGUMENT;
		case REFERENCE_TYPE_ARGUMENT:
			return sourcetrail::ReferenceKind::TYPE_ARGUMENT;
		case REFERENCE_TEMPLATE_DEFAULT_ARGUMENT:
			return sourcetrail::ReferenceKind::TEMPLATE_DEFAULT_ARGUMENT;
		case REFERENCE_TEMPLATE_SPECIALIZATION:
			return sourcetrail::ReferenceKind::TEMPLATE_SPECIALIZATION;
		case REFERENCE_TEMPLATE_MEMBER_SPECIALIZATION:
			return sourcetrail::ReferenceKind::TEMPLATE_MEMBER_SPECIALIZATION;
		case REFERENCE_INCLUDE:
			return sourcetrail::ReferenceKind::INCLUDE;
		case REFERENCE_IMPORT:
			return sourcetrail::ReferenceKind::IMPORT;
		case REFERENCE_MACRO_USAGE:
			return sourcetrail::ReferenceKind::MACRO_USAGE;
		case REFERENCE_ANNOTATION_USAGE:
			return sourcetrail::ReferenceKind::ANNOTATION_USAGE;
		}
		return sourcetrail::ReferenceKind::TYPE_USAGE;
	}
}

sourcetrail::SourcetrailDBWriter dbWriter;

int getSupportedDatabaseVersion()
{
	return dbWriter.getSupportedDatabaseVersion();
}

std::string getLastError()
{
	return dbWriter.getLastError();
}

void clearLastError()
{
	dbWriter.clearLastError();
}

bool open(std::string databaseFilePath)
{
	return dbWriter.open(databaseFilePath);
}

bool close()
{
	return dbWriter.close();
}

bool clear()
{
	return dbWriter.clear();
}

bool isEmpty()
{
	return dbWriter.isEmpty();
}

bool isCompatible()
{
	return dbWriter.isCompatible();
}

int getLoadedDatabaseVersion()
{
	return dbWriter.getLoadedDatabaseVersion();
}

bool beginTransaction()
{
	return dbWriter.beginTransaction();
}

bool commitTransaction()
{
	return dbWriter.commitTransaction();
}

bool rollbackTransaction()
{
	return dbWriter.rollbackTransaction();
}

bool optimizeDatabaseMemory()
{
	return dbWriter.optimizeDatabaseMemory();
}

int recordSymbol(std::string serializedNameHierarchy)
{
	std::string error;
	const sourcetrail::NameHierarchy nameHierarchy = sourcetrail::deserializeNameHierarchyFromJson(serializedNameHierarchy, &error);
	if (error.size() || nameHierarchy.nameElements.empty())
	{
		dbWriter.setLastError("Unable to deserialize name hierarchy \"" + serializedNameHierarchy + "\": " + error);
		return 0;
	}
	return dbWriter.recordSymbol(nameHierarchy);
}

bool recordSymbolDefinitionKind(int symbolId, DefinitionKind symbolDefinitionKind)
{
	return dbWriter.recordSymbolDefinitionKind(symbolId, convertDefinitionKind(symbolDefinitionKind));
}

bool recordSymbolKind(int symbolId, SymbolKind symbolKind)
{
	return dbWriter.recordSymbolKind(symbolId, convertSymbolKind(symbolKind));
}

bool recordSymbolLocation(int symbolId, int fileId, int startLine, int startColumn, int endLine, int endColumn)
{
	return dbWriter.recordSymbolLocation(symbolId, { fileId, startLine, startColumn, endLine, endColumn });
}

bool recordSymbolScopeLocation(int symbolId, int fileId, int startLine, int startColumn, int endLine, int endColumn)
{
	return dbWriter.recordSymbolScopeLocation(symbolId, { fileId, startLine, startColumn, endLine, endColumn });
}

bool recordSymbolSignatureLocation(int symbolId, int fileId, int startLine, int startColumn, int endLine, int endColumn)
{
	return dbWriter.recordSymbolSignatureLocation(symbolId, { fileId, startLine, startColumn, endLine, endColumn });
}

int recordReference(int contextSymbolId, int referencedSymbolId, ReferenceKind referenceKind)
{
	return dbWriter.recordReference(contextSymbolId, referencedSymbolId, convertReferenceKind(referenceKind));
}

bool recordReferenceLocation(int referenceId, int fileId, int startLine, int startColumn, int endLine, int endColumn)
{
	return dbWriter.recordReferenceLocation(referenceId, { fileId, startLine, startColumn, endLine, endColumn });
}

int recordFile(std::string filePath)
{
	return dbWriter.recordFile(filePath);
}

bool recordFileLanguage(int fileId, std::string languageIdentifier)
{
	return dbWriter.recordFileLanguage(fileId, languageIdentifier);
}

int recordLocalSymbol(std::string name)
{
	return dbWriter.recordLocalSymbol(name);
}

bool recordLocalSymbolLocation(int localSymbolId, int fileId, int startLine, int startColumn, int endLine, int endColumn)
{
	return dbWriter.recordLocalSymbolLocation(localSymbolId, { fileId, startLine, startColumn, endLine, endColumn });
}

bool recordCommentLocation(int fileId, int startLine, int startColumn, int endLine, int endColumn)
{
	return dbWriter.recordCommentLocation({ fileId, startLine, startColumn, endLine, endColumn });
}

bool recordError(std::string message, bool fatal, int fileId, int startLine, int startColumn, int endLine, int endColumn)
{
	return dbWriter.recordError(message, fatal, { fileId, startLine, startColumn, endLine, endColumn });
}
