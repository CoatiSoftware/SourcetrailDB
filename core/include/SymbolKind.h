#ifndef SOURCETRAIL_SYMBOL_KIND_H
#define SOURCETRAIL_SYMBOL_KIND_H

namespace sourcetrail
{
	enum NodeKind;

	/**
	* Enum providing all possible values for kinds of symbols that can be recorded using the SourcetrailDBWriter interface.
	*/
	enum SymbolKind
	{
		SYMBOL_TYPE,
		SYMBOL_BUILTIN_TYPE,
		SYMBOL_NAMESPACE,
		SYMBOL_PACKAGE,
		SYMBOL_STRUCT,
		SYMBOL_CLASS,
		SYMBOL_INTERFACE,
		SYMBOL_ANNOTATION,
		SYMBOL_GLOBAL_VARIABLE,
		SYMBOL_FIELD,
		SYMBOL_FUNCTION,
		SYMBOL_METHOD,
		SYMBOL_ENUM,
		SYMBOL_ENUM_CONSTANT,
		SYMBOL_TYPEDEF,
		SYMBOL_TEMPLATE_PARAMETER,
		SYMBOL_TYPE_PARAMETER,
		SYMBOL_MACRO,
		SYMBOL_UNION,
	};

	NodeKind symbolKindToNodeKind(SymbolKind v);
}

#endif // SOURCETRAIL_SYMBOL_KIND_H
