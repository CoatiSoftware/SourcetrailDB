import argparse
import os
import sourcetraildb as srctrl

def main():
	parser = argparse.ArgumentParser(description="SourcetrailDB Python API Example")
	parser.add_argument("--database-file-path", help="path to the generated Sourcetrail database file",
		type=str, required=True)
	parser.add_argument("--source-file-path", help="path to the source file to index",
		type=str, required=True)
	parser.add_argument("--database-version", help="database version of the invoking Sourcetrail binary",
		type=int, required=False, default=0)

	args = parser.parse_args()
	databaseFilePath = args.database_file_path
	sourceFilePath = args.source_file_path.replace("\\", "/")
	dbVersion = args.database_version

	print("SourcetrailDB Python API Example")
	print("Supported database version: " + str(srctrl.getSupportedDatabaseVersion()))

	if dbVersion > 0 and dbVersion != srctrl.getSupportedDatabaseVersion():
		print("ERROR: Only supports database version: " + str(srctrl.getSupportedDatabaseVersion()) +
			". Requested version: " + str(dbVersion))
		return 1

	if not srctrl.open(databaseFilePath):
		print("ERROR: " + srctrl.getLastError())
		return 1

	print("Clearing loaded database now...")
	srctrl.clear()

	print("start indexing")
	srctrl.beginTransaction()

	fileId = srctrl.recordFile(sourceFilePath)
	srctrl.recordFileLanguage(fileId, "python")

	if len(srctrl.getLastError()) > 0:
		print("ERROR: " + srctrl.getLastError())
		return 1

	symbolId = srctrl.recordSymbol(
		'{ "name_delimiter": ".", "name_elements": [ '
			'{ "prefix": "", "name": "MyType", "postfix": "" } '
		'] }')
	srctrl.recordSymbolDefinitionKind(symbolId, srctrl.DEFINITION_EXPLICIT)
	srctrl.recordSymbolKind(symbolId, srctrl.SYMBOL_CLASS)
	srctrl.recordSymbolLocation(symbolId, fileId, 2, 7, 2, 12)
	srctrl.recordSymbolScopeLocation(symbolId, fileId, 2, 1, 7, 1)

	memberId = srctrl.recordSymbol(
		'{ "name_delimiter": ".", "name_elements": [ '
			'{ "prefix": "", "name": "MyType", "postfix": "" }, '
			'{ "prefix": "", "name": "my_member", "postfix": "" } '
		'] }')
	srctrl.recordSymbolDefinitionKind(memberId, srctrl.DEFINITION_EXPLICIT)
	srctrl.recordSymbolKind(memberId, srctrl.SYMBOL_FIELD)
	srctrl.recordSymbolLocation(memberId, fileId, 4, 2, 4, 10)

	methodId = srctrl.recordSymbol(
		'{ "name_delimiter": ".", "name_elements": [ '
			'{ "prefix": "", "name": "MyType", "postfix": "" }, '
			'{ "prefix": "", "name": "my_method", "postfix": "" } '
		'] }')
	srctrl.recordSymbolDefinitionKind(methodId, srctrl.DEFINITION_EXPLICIT)
	srctrl.recordSymbolKind(methodId, srctrl.SYMBOL_METHOD)
	srctrl.recordSymbolLocation(methodId, fileId, 6, 6, 6, 14)
	srctrl.recordSymbolScopeLocation(methodId, fileId, 6, 1, 7, 1)

	useageId = srctrl.recordReference(methodId, memberId, srctrl.REFERENCE_USAGE)
	srctrl.recordReferenceLocation(useageId, fileId, 7, 10, 7, 18)

	srctrl.commitTransaction()

	if len(srctrl.getLastError()) > 0:
		print("ERROR: " + srctrl.getLastError())
		return 1

	if not srctrl.close():
		print("ERROR: " + srctrl.getLastError())
		return 1

	print("done")
	return 0
main()
