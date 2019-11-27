
## Python API Example

### Requirements

* CMake
* C++ Compiler
* Perl
* SWIG

### Running from Command Line

* Run cmake with `BUILD_BINDINGS_PERL=ON`
* Build target `sourcetraildb`
* Copy two files from the `bindings_perl` build directory into `SourcetrailDB/examples/perl_api_example/src`:
	- `sourcetraildb.pm`
	- Windows: `sourcetraildb.dll`
	- Linux: `sourcetraildb.so`
	- macOS: `sourcetraildb.dylib` and rename to `sourcetraildb.so`
* Run example from the directory `SourcetrailDB/examples/perl_api_example/src` with:

```
$ perl example.pl --database-file-path=absolute/path/to/SourcetrailDB/examples/perl_api_example/src/example.srctrldb --source-file-path=absolute/path/to/SourcetrailDB/examples/perl_api_example/data/file.pm
```

### Running with Sourcetrail

* Run cmake with `BUILD_EXAMPLES=ON` and `BUILD_BINDINGS_PERL=ON`
* Build target `sourcetraildb` and `perl_api_example`.
* Open `perl_api_exampl.srctrlprj` located in the build directory of `perl_api_example` with Sourcetrail and index the project.
