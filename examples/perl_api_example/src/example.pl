#!/usr/bin/perl

use v5.10;

use strict;
use warnings;

use Getopt::Long;

use FindBin;
use lib $FindBin::Bin;
use sourcetraildb;

my ( $database_file_path, $source_file_path );
my $db_version = 0;

GetOptions(
	'database-file-path=s' => \$database_file_path,
	'source-file-path=s'   => \$source_file_path,
	'database-version=i'   => \$db_version,
);

say "SourcetrailDB Perl API Example";
my $supported_db_version = sourcetraildb::getSupportedDatabaseVersion;
say "Supported database version: $supported_db_version";

die "ERROR: Only supports database version: $supported_db_version. Requested version: $db_version\n"
	if $db_version > 0 and $db_version != $supported_db_version;

die "ERROR: " . sourcetraildb::getLastError unless sourcetraildb::open($database_file_path);

say "Clearing loaded database now...";
sourcetraildb::clear;

say "start indexing";
sourcetraildb::beginTransaction;

my $file_id = sourcetraildb::recordFile($source_file_path);
sourcetraildb::recordFileLanguage( $file_id, 'perl' );
die "ERROR: " . sourcetraildb::getLastError if sourcetraildb::getLastError;

my $symbol_id = sourcetraildb::recordSymbol(
	'{ "name_delimiter": "::", "name_elements": [ { "prefix": "", "name": "MyClass", "postfix": "" } ] }');
sourcetraildb::recordSymbolDefinitionKind( $symbol_id, $sourcetraildb::DEFINITION_EXPLICIT );
sourcetraildb::recordSymbolKind( $symbol_id, $sourcetraildb::SYMBOL_PACKAGE );
sourcetraildb::recordSymbolLocation( $symbol_id, $file_id, 1, 9, 1, 15 );
sourcetraildb::recordSymbolScopeLocation( $symbol_id, $file_id, 1, 1, 7, 2 );

my $member_id
	= sourcetraildb::recordSymbol( '{ "name_delimiter": "::", "name_elements": [ '
		. '{ "prefix": "", "name": "MyClass", "postfix": "" }, '
		. '{ "prefix": "$", "name": "my_member", "postfix": "" } '
		. '] }' );
sourcetraildb::recordSymbolDefinitionKind( $member_id, $sourcetraildb::DEFINITION_EXPLICIT );
sourcetraildb::recordSymbolKind( $member_id, $sourcetraildb::SYMBOL_FIELD );
sourcetraildb::recordSymbolLocation( $member_id, $file_id, 3, 4, 3, 13 );

my $method_id
	= sourcetraildb::recordSymbol( '{ "name_delimiter": "::", "name_elements": [ '
		. '{ "prefix": "", "name": "MyClass", "postfix": "" }, '
		. '{ "prefix": "&", "name": "my_method", "postfix": "" } '
		. '] }' );
sourcetraildb::recordSymbolDefinitionKind( $method_id, $sourcetraildb::DEFINITION_EXPLICIT );
sourcetraildb::recordSymbolKind( $method_id, $sourcetraildb::SYMBOL_METHOD );
sourcetraildb::recordSymbolLocation( $method_id, $file_id, 5, 5, 5, 13 );
sourcetraildb::recordSymbolScopeLocation( $method_id, $file_id, 6, 1, 7, 1 );

my $usage_id = sourcetraildb::recordReference( $method_id, $member_id, $sourcetraildb::REFERENCE_USAGE );
sourcetraildb::recordReferenceLocation( $usage_id, $file_id, 6, 9, 6, 18 );

sourcetraildb::commitTransaction;
die "ERROR: " . sourcetraildb::getLastError if sourcetraildb::getLastError;
die "ERROR: " . sourcetraildb::getLastError unless sourcetraildb::close;

say "done";
