
## Java API Example

### Requirements

* CMake
* C++ Compiler
* Java JDK
* SWIG

### Running from Command Line

* Run cmake with `BUILD_BINDINGS_JAVA=ON`
* Build `make`
* Copy shared library from the `bindings_java` build directory into `SourcetrailDB/examples/java_api_example/src`:
	- Windows: `lib_sourcetraildb.dll`
	- Linux: `lib_sourcetraildb.so`
	- macOS: `lib_sourcetraildb.dylib` and rename to `lib_sourcetraildb.so`
* Run example from the directory `SourcetrailDB/examples/java_api_example/src` with:

```
$ java -Djava.library.path=. -jar java_api_example.jar --database-file-path=absolute/path/to/SourcetrailDB/examples/java_api_example/src/example.srctrldb --source-file-path=absolute/path/to/SourcetrailDB/examples/java_api_example/data/file.py
```

### Running with Sourcetrail

* Run cmake with `BUILD_EXAMPLES=ON` and `BUILD_BINDINGS_JAVA=ON`
* Build `make`.
* Open `java_api_example.srctrlprj` located in the build directory of `java_api_example` with Sourcetrail and index the project.
