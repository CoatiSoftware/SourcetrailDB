#include "sourcetraildb.h"

#include "DefinitionKind.h"
#include "NameHierarchy.h"
#include "SymbolKind.h"
#include "SourceLocation.h"
#include "SourceRange.h"
#include "SourcetrailDBWriter.h"
#include "ReferenceKind.h"


namespace
{
	sourcetrail::DefinitionKind convertDefinitionKind(::DefinitionKind v)
	{
		switch (v)
		{
		case DEFINITION_IMPLICIT: return sourcetrail::DEFINITION_IMPLICIT;
		case DEFINITION_EXPLICIT: return sourcetrail::DEFINITION_EXPLICIT;
		}
		return sourcetrail::DEFINITION_EXPLICIT;
	}

	sourcetrail::SymbolKind convertSymbolKind(::SymbolKind v)
	{
		switch (v)
		{
		case SYMBOL_TYPE: return sourcetrail::SYMBOL_TYPE;
		case SYMBOL_BUILTIN_TYPE: return sourcetrail::SYMBOL_BUILTIN_TYPE;
		case SYMBOL_NAMESPACE: return sourcetrail::SYMBOL_NAMESPACE;
		case SYMBOL_PACKAGE: return sourcetrail::SYMBOL_PACKAGE;
		case SYMBOL_STRUCT: return sourcetrail::SYMBOL_STRUCT;
		case SYMBOL_CLASS: return sourcetrail::SYMBOL_CLASS;
		case SYMBOL_INTERFACE: return sourcetrail::SYMBOL_INTERFACE;
		case SYMBOL_ANNOTATION: return sourcetrail::SYMBOL_ANNOTATION;
		case SYMBOL_GLOBAL_VARIABLE: return sourcetrail::SYMBOL_GLOBAL_VARIABLE;
		case SYMBOL_FIELD: return sourcetrail::SYMBOL_FIELD;
		case SYMBOL_FUNCTION: return sourcetrail::SYMBOL_FUNCTION;
		case SYMBOL_METHOD: return sourcetrail::SYMBOL_METHOD;
		case SYMBOL_ENUM: return sourcetrail::SYMBOL_ENUM;
		case SYMBOL_ENUM_CONSTANT: return sourcetrail::SYMBOL_ENUM_CONSTANT;
		case SYMBOL_TYPEDEF: return sourcetrail::SYMBOL_TYPEDEF;
		case SYMBOL_TEMPLATE_PARAMETER: return sourcetrail::SYMBOL_TEMPLATE_PARAMETER;
		case SYMBOL_TYPE_PARAMETER: return sourcetrail::SYMBOL_TYPE_PARAMETER;
		case SYMBOL_MACRO: return sourcetrail::SYMBOL_MACRO;
		case SYMBOL_UNION: return sourcetrail::SYMBOL_UNION;
		}
		return sourcetrail::SYMBOL_TYPE;
	}

	sourcetrail::ReferenceKind convertReferenceKind(::ReferenceKind v)
	{
		switch (v)
		{
		case REFERENCE_TYPE_USAGE: return sourcetrail::REFERENCE_TYPE_USAGE;
		case REFERENCE_USAGE: return sourcetrail::REFERENCE_USAGE;
		case REFERENCE_CALL: return sourcetrail::REFERENCE_CALL;
		case REFERENCE_INHERITANCE: return sourcetrail::REFERENCE_INHERITANCE;
		case REFERENCE_OVERRIDE: return sourcetrail::REFERENCE_OVERRIDE;
		case REFERENCE_TEMPLATE_ARGUMENT: return sourcetrail::REFERENCE_TEMPLATE_ARGUMENT;
		case REFERENCE_TYPE_ARGUMENT: return sourcetrail::REFERENCE_TYPE_ARGUMENT;
		case REFERENCE_TEMPLATE_DEFAULT_ARGUMENT: return sourcetrail::REFERENCE_TEMPLATE_DEFAULT_ARGUMENT;
		case REFERENCE_TEMPLATE_SPECIALIZATION: return sourcetrail::REFERENCE_TEMPLATE_SPECIALIZATION;
		case REFERENCE_TEMPLATE_MEMBER_SPECIALIZATION: return sourcetrail::REFERENCE_TEMPLATE_MEMBER_SPECIALIZATION;
		case REFERENCE_INCLUDE: return sourcetrail::REFERENCE_INCLUDE;
		case REFERENCE_IMPORT: return sourcetrail::REFERENCE_IMPORT;
		case REFERENCE_MACRO_USAGE: return sourcetrail::REFERENCE_MACRO_USAGE;
		case REFERENCE_ANNOTATION_USAGE: return sourcetrail::REFERENCE_ANNOTATION_USAGE;
		}
		return sourcetrail::REFERENCE_TYPE_USAGE;
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

bool openProject(std::string projectDirectory, std::string projectName)
{
	return dbWriter.openProject(projectDirectory, projectName);
}

bool closeProject()
{
	return dbWriter.closeProject();
}

bool clearProject()
{
	return dbWriter.clearProject();
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
	const sourcetrail::NameHierarchy nameHierarchy = sourcetrail::deserializeNameHierarchyFromJson(serializedNameHierarchy);
	if (nameHierarchy.nameElements.empty())
	{
		// TODO: handle this case!
		//dbWriter.setLastError("Unable to deserialize name hierarchy \"" + serializedNameHierarchy + "\".");
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

bool recordSymbolLocation(int symbolId, std::string filePath, int startLine, int startColumn, int endLine, int endColumn)
{
	return dbWriter.recordSymbolLocation(symbolId, sourcetrail::SourceRange({ filePath, startLine, startColumn, endLine, endColumn }));
}

bool recordSymbolScopeLocation(int symbolId, std::string filePath, int startLine, int startColumn, int endLine, int endColumn)
{
	return dbWriter.recordSymbolScopeLocation(symbolId, sourcetrail::SourceRange({ filePath, startLine, startColumn, endLine, endColumn }));
}

bool recordSymbolSignatureLocation(int symbolId, std::string filePath, int startLine, int startColumn, int endLine, int endColumn)
{
	return dbWriter.recordSymbolSignatureLocation(symbolId, sourcetrail::SourceRange({ filePath, startLine, startColumn, endLine, endColumn }));
}

int recordReference(int contextSymbolId, int referencedSymbolId, ReferenceKind referenceKind)
{
	return dbWriter.recordReference(contextSymbolId, referencedSymbolId, convertReferenceKind(referenceKind));
}

bool recordReferenceLocation(int referenceId, std::string filePath, int startLine, int startColumn, int endLine, int endColumn)
{
	return dbWriter.recordReferenceLocation(referenceId, sourcetrail::SourceRange({ filePath, startLine, startColumn, endLine, endColumn }));
}

int recordFile(std::string filePath)
{
	return dbWriter.recordFile(filePath);
}

int recordLocalSymbol(std::string name)
{
	return dbWriter.recordLocalSymbol(name);
}

bool recordLocalSymbolLocation(int localSymbolId, std::string filePath, int startLine, int startColumn, int endLine, int endColumn)
{
	return dbWriter.recordLocalSymbolLocation(localSymbolId, sourcetrail::SourceRange({ filePath, startLine, startColumn, endLine, endColumn }));
}

bool recordCommentLocation(std::string filePath, int startLine, int startColumn, int endLine, int endColumn)
{
	return dbWriter.recordCommentLocation(sourcetrail::SourceRange({ filePath, startLine, startColumn, endLine, endColumn }));
}

bool recordError(std::string message, bool fatal, std::string filePath, int startLine, int startColumn, int endLine, int endColumn)
{
	return dbWriter.recordError(message, fatal, sourcetrail::SourceRange({ filePath, startLine, startColumn, endLine, endColumn }));
}
