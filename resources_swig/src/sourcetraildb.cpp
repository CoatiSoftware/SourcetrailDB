#include "sourcetraildb.h"

#include "DefinitionKind.h"
#include "NameHierarchy.h"
#include "SymbolKind.h"
#include "SourceLocation.h"
#include "SourceRange.h"
#include "ReferenceKind.h"

#include "SrctrldbWriter.h"

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

sourcetrail::SrctrldbWriter srctrldbWriter;

int getSupportedDatabaseVersion()
{
	return srctrldbWriter.getSupportedDatabaseVersion();
}

std::string getLastError()
{
	return srctrldbWriter.getLastError();
}

void clearLastError()
{
	srctrldbWriter.clearLastError();
}

bool openProject(std::string projectDirectory, std::string projectName)
{
	return srctrldbWriter.openProject(projectDirectory, projectName);
}

bool closeProject()
{
	return srctrldbWriter.closeProject();
}

bool clearProject()
{
	return srctrldbWriter.clearProject();
}

bool isEmpty()
{
	return srctrldbWriter.isEmpty();
}

bool isCompatible()
{
	return srctrldbWriter.isCompatible();
}

int getLoadedDatabaseVersion()
{
	return srctrldbWriter.getLoadedDatabaseVersion();
}

bool beginTransaction()
{
	return srctrldbWriter.beginTransaction();
}

bool commitTransaction()
{
	return srctrldbWriter.commitTransaction();
}

bool rollbackTransaction()
{
	return srctrldbWriter.rollbackTransaction();
}

bool optimizeDatabaseMemory()
{
	return srctrldbWriter.optimizeDatabaseMemory();
}

int recordSymbol(std::string serializedNameHierarchy)
{
	const sourcetrail::NameHierarchy nameHierarchy = sourcetrail::deserializeNameHierarchyFromJson(serializedNameHierarchy);
	if (nameHierarchy.nameElements.empty())
	{
		// TODO: handle this case!
		//srctrldbWriter.setLastError("Unable to deserialize name hierarchy \"" + serializedNameHierarchy + "\".");
		return 0;
	}
	return srctrldbWriter.recordSymbol(nameHierarchy);
}

bool recordSymbolDefinitionKind(int symbolId, DefinitionKind symbolDefinitionKind)
{
	return srctrldbWriter.recordSymbolDefinitionKind(symbolId, convertDefinitionKind(symbolDefinitionKind));
}

bool recordSymbolKind(int symbolId, SymbolKind symbolKind)
{
	return srctrldbWriter.recordSymbolKind(symbolId, convertSymbolKind(symbolKind));
}

bool recordSymbolLocation(int symbolId, std::string filePath, int startLine, int startColumn, int endLine, int endColumn)
{
	return srctrldbWriter.recordSymbolLocation(symbolId, sourcetrail::SourceRange({ filePath, startLine, startColumn, endLine, endColumn }));
}

bool recordSymbolScopeLocation(int symbolId, std::string filePath, int startLine, int startColumn, int endLine, int endColumn)
{
	return srctrldbWriter.recordSymbolScopeLocation(symbolId, sourcetrail::SourceRange({ filePath, startLine, startColumn, endLine, endColumn }));
}

bool recordSymbolSignatureLocation(int symbolId, std::string filePath, int startLine, int startColumn, int endLine, int endColumn)
{
	return srctrldbWriter.recordSymbolSignatureLocation(symbolId, sourcetrail::SourceRange({ filePath, startLine, startColumn, endLine, endColumn }));
}

int recordReference(int contextSymbolId, int referencedSymbolId, ReferenceKind referenceKind)
{
	return srctrldbWriter.recordReference(contextSymbolId, referencedSymbolId, convertReferenceKind(referenceKind));
}

bool recordReferenceLocation(int referenceId, std::string filePath, int startLine, int startColumn, int endLine, int endColumn)
{
	return srctrldbWriter.recordReferenceLocation(referenceId, sourcetrail::SourceRange({ filePath, startLine, startColumn, endLine, endColumn }));
}

int recordFile(std::string filePath)
{
	return srctrldbWriter.recordFile(filePath);
}

int recordLocalSymbol(std::string name)
{
	return srctrldbWriter.recordLocalSymbol(name);
}

bool recordLocalSymbolLocation(int localSymbolId, std::string filePath, int startLine, int startColumn, int endLine, int endColumn)
{
	return srctrldbWriter.recordLocalSymbolLocation(localSymbolId, sourcetrail::SourceRange({ filePath, startLine, startColumn, endLine, endColumn }));
}

bool recordCommentLocation(std::string filePath, int startLine, int startColumn, int endLine, int endColumn)
{
	return srctrldbWriter.recordCommentLocation(sourcetrail::SourceRange({ filePath, startLine, startColumn, endLine, endColumn }));
}

bool recordError(std::string message, bool fatal, std::string filePath, int startLine, int startColumn, int endLine, int endColumn)
{
	return srctrldbWriter.recordError(message, fatal, sourcetrail::SourceRange({ filePath, startLine, startColumn, endLine, endColumn }));
}
