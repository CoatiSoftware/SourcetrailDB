#include <fstream>
#include <iostream>
#include <regex>

#include "SourcetrailDBWriter.h"
#include "NameHierarchy.h"
#include "SourceRange.h"

sourcetrail::NameHierarchy toNameHierarchy(const std::vector<std::string>& strs)
{
	sourcetrail::NameHierarchy name = { " - ", { } };
	for (auto str : strs)
	{
		if (str.size())
		{
			name.nameElements.emplace_back( sourcetrail::NameElement({ "", str, "" }) );
		}
	}
	return name;
}

int main(int argc, const char *argv[])
{
	sourcetrail::SourcetrailDBWriter dbWriter;

	std::cout << "SourcetrailDB Poetry Indexer" << std::endl;
	std::cout << std::endl;
	std::cout << "SourcetrailDB version: " << dbWriter.getVersionString() << std::endl;
	std::cout << "Supported database version: " << dbWriter.getSupportedDatabaseVersion() << std::endl;
	std::cout << std::endl;

	if (argc != 4)
	{
		std::cout << "usage: poetry_indexer <database_path> <database_version> <source_path>";
	}

	std::string dbPath = argv[1];
	int dbVersion = std::stoi(argv[2]);
	std::string sourcePath = argv[3];

	if (dbVersion != dbWriter.getSupportedDatabaseVersion())
	{
		std::cout << "error: binary only supports database version: " << dbWriter.getSupportedDatabaseVersion()
			<< ". Requested version: " << dbVersion << std::endl;
		return 1;
	}

	if (!dbWriter.open(dbPath))
	{
		std::cout << "error: " << dbWriter.getLastError() << std::endl;
		return 1;
	}

	if (!dbWriter.beginTransaction())
	{
		std::cout << "error: " << dbWriter.getLastError() << std::endl;
		return 1;
	}

	int fileId = dbWriter.recordFile(sourcePath);
	if (fileId == 0)
	{
		std::cout << "error: " << dbWriter.getLastError() << std::endl;
		return 1;
	}

	std::ifstream fileStream(sourcePath.c_str());
	if (!fileStream.good())
	{
		std::cout << "error: opening file failed: " << sourcePath << std::endl;
		return 1;
	}

	std::string author;

	struct {
		int id = 0;
		std::string name;
	} poem;

	struct {
		int id = 0;
		int number = 0;
		sourcetrail::SourceRange range;
	} paragraph;

	struct {
		int number = 0;
		std::string content;
	} line;

	int symbolId = 0;

	// foreach poem line
	while (getline(fileStream, line.content, '\n'))
	{
		line.number++;

		// first line: author
		// second line: title
		if (line.number <= 2)
		{
			// store the author as namespace, the title as type
			symbolId = dbWriter.recordSymbol(toNameHierarchy({ author, line.content }));
			dbWriter.recordSymbolDefinitionKind(symbolId, sourcetrail::DefinitionKind::EXPLICIT);
			dbWriter.recordSymbolKind(symbolId, line.number == 1 ? sourcetrail::SymbolKind::NAMESPACE : sourcetrail::SymbolKind::TYPE);

			sourcetrail::SourceRange range = { fileId, line.number, 1, line.number, static_cast<int>(line.content.size()) };
			dbWriter.recordSymbolLocation(symbolId, range);

			if (line.number == 1)
			{
				author = line.content;
			}
			else
			{
				poem.id = symbolId;
				poem.name = line.content;
			}

			continue;
		}
		else if (line.content.size() <= 1) // empty line
		{
			if (paragraph.id) // if paragraph exists then close it
			{
				dbWriter.recordSymbolScopeLocation(paragraph.id, paragraph.range);
				dbWriter.recordSymbolSignatureLocation(paragraph.id, paragraph.range);
				paragraph.id = 0;
			}
		}
		else if (!paragraph.id) // start new paragraph
		{
			paragraph.number++;

			// store each paragraph as method
			paragraph.id = dbWriter.recordSymbol(toNameHierarchy({ author, poem.name, "paragraph " + std::to_string(paragraph.number) }));
			dbWriter.recordSymbolDefinitionKind(paragraph.id, sourcetrail::DefinitionKind::EXPLICIT);
			dbWriter.recordSymbolKind(paragraph.id, sourcetrail::SymbolKind::METHOD);
			paragraph.range = { fileId, line.number, 1, 0, 0 };
			symbolId = paragraph.id;
		}

		if (line.content.size() > 1)
		{
			// match every word in the line
			std::string str = line.content;
			std::smatch match;
			std::regex regExp("[\\w']+");
			size_t pos = 0;

			while (std::regex_search(str, match, regExp))
			{
				std::string m = match[0];
				std::string word;
				std::transform(m.begin(), m.end(), std::back_inserter(word), tolower);

				int wordPos = match.position(0) + 1;
				int wordLen = match.length(0);

				// store each word as global variable
				int wordId = dbWriter.recordSymbol(toNameHierarchy({ word }));
				dbWriter.recordSymbolDefinitionKind(wordId, sourcetrail::DefinitionKind::EXPLICIT);
				dbWriter.recordSymbolKind(wordId, sourcetrail::SymbolKind::GLOBAL_VARIABLE);

				// create a reference between word and paragraph
				int referenceId = dbWriter.recordReference(symbolId, wordId, sourcetrail::ReferenceKind::USAGE);
				sourcetrail::SourceRange range = { fileId, line.number, static_cast<int>(pos + wordPos), line.number, static_cast<int>(pos + wordPos + wordLen) - 1 };
				dbWriter.recordReferenceLocation(referenceId, range);

				str = match.suffix();
				pos += wordPos + wordLen - 1;
			}
		}

		paragraph.range.endLine = line.number;
		paragraph.range.endColumn = line.content.size();
	}

	if (paragraph.id)
	{
		dbWriter.recordSymbolScopeLocation(paragraph.id, paragraph.range);
		dbWriter.recordSymbolSignatureLocation(paragraph.id, paragraph.range);
		paragraph.id = 0;
	}

	sourcetrail::SourceRange range = { fileId, 2, 1, line.number, int(line.content.size()) };
	dbWriter.recordSymbolScopeLocation(poem.id, range);

	fileStream.close();

	if (!dbWriter.commitTransaction())
	{
		std::cout << "error: " << dbWriter.getLastError() << std::endl;
		return 1;
	}

	if (dbWriter.getLastError().size())
	{
		std::cout << "error: " << dbWriter.getLastError() << std::endl;
		return 1;
	}

	return 0;
}
