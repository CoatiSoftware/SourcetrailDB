import argparse
import os
import sourcetraildb as srctrl

def main():
	parser = argparse.ArgumentParser(description="Index a Python source file and store the indexed data to a Sourcetrail database file.")
	parser.add_argument("--database-file-path", help="path to the generated Sourcetrail database file", type=str, required=True)
	parser.add_argument("--source-file-path", help="path to the source file to index", type=str, required=True)

	args = parser.parse_args()
	databaseFilePath = args.database_file_path
	sourceFilePath = args.source_file_path

	if not srctrl.open(databaseFilePath):
		print("ERROR: " + srctrl.getLastError())

	if srctrl.isEmpty():
		print("Loaded database is empty.")
	else:
		print("Loaded database contains data.")

	print("start all")
	srctrl.beginTransaction()

	fileId = srctrl.recordFile(sourceFilePath)
	srctrl.recordFileLanguage(fileId, "python")

	if len(srctrl.getLastError()) > 0:
		print("ERROR: " + srctrl.getLastError())

	symbolId = srctrl.recordSymbol('{ "name_delimiter": ".", "name_elements": [ { "prefix": "", "name": "MyType", "postfix": "" } ] }')
	srctrl.recordSymbolDefinitionKind(symbolId, srctrl.DEFINITION_EXPLICIT)
	srctrl.recordSymbolKind(symbolId, srctrl.SYMBOL_CLASS)
	srctrl.recordSymbolLocation(symbolId, fileId, 2, 7, 2, 12)
	srctrl.recordSymbolScopeLocation(symbolId, fileId, 2, 1, 7, 1)

	if len(srctrl.getLastError()) > 0:
		print("ERROR: " + srctrl.getLastError())

	memberId = srctrl.recordSymbol('{ "name_delimiter": ".", "name_elements": [ { "prefix": "", "name": "MyType", "postfix": "" }, { "prefix": "", "name": "my_member", "postfix": "" } ] }')
	srctrl.recordSymbolDefinitionKind(memberId, srctrl.DEFINITION_EXPLICIT)
	srctrl.recordSymbolKind(memberId, srctrl.SYMBOL_FIELD)
	srctrl.recordSymbolLocation(memberId, fileId, 4, 2, 4, 10)

	if len(srctrl.getLastError()) > 0:
		print("ERROR: " + srctrl.getLastError())

	methodId = srctrl.recordSymbol('{ "name_delimiter": ".", "name_elements": [ { "prefix": "", "name": "MyType", "postfix": "" }, { "prefix": "", "name": "my_method", "postfix": "" } ] }')
	srctrl.recordSymbolDefinitionKind(methodId, srctrl.DEFINITION_EXPLICIT)
	srctrl.recordSymbolKind(methodId, srctrl.SYMBOL_METHOD)
	srctrl.recordSymbolLocation(methodId, fileId, 6, 6, 6, 14)
	srctrl.recordSymbolScopeLocation(methodId, fileId, 6, 1, 7, 1)

	if len(srctrl.getLastError()) > 0:
		print("ERROR: " + srctrl.getLastError())

	useageId = srctrl.recordReference(methodId, memberId, srctrl.REFERENCE_USAGE)
	srctrl.recordReferenceLocation(useageId, fileId, 7, 10, 7, 18)

	if len(srctrl.getLastError()) > 0:
		print("ERROR: " + srctrl.getLastError())

	srctrl.commitTransaction()
	print("end all")

	if not srctrl.close():
		print("ERROR: " + srctrl.getLastError())

	if len(srctrl.getLastError()) > 0:
		print("ERROR: " + srctrl.getLastError())
main()
