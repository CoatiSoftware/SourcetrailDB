
## Java API Example

### Requirements

* CMake
* C++ Compiler
* Java JDK
* SWIG

### Running from Command Line

* Run cmake with `BUILD_BINDINGS_JAVA=ON`
* Build `make`
* Copy two files from the `bindings_java` build directory into `SourcetrailDB/examples/java_api_example`:
	- SourcetrailDB.jar
	- Windows: `_sourcetraildb.dll`
	- Linux: `lib_sourcetraildb.so`
	- macOS: `lib_sourcetraildb.jnilib`
* Run example from the directory `SourcetrailDB/examples/java_api_example` with:

## Windows
```
$ javac -cp SourcetrailDB.jar src/za/co/spazzymoto/example.java
$ java -cp "SourcetrailDB.jar;src" -Djava.library.path=. za.co.spazzymoto.example --database-file-path=absolute/path/to/SourcetrailDB/examples/java_api_example/src/example.srctrldb --source-file-path=absolute/path/to/SourcetrailDB/examples/java_api_example/data/file.py
```

## Unix/macOS
```
$ javac -cp SourcetrailDB.jar src/za/co/spazzymoto/example.java
$ java -cp "SourcetrailDB.jar:src" -Djava.library.path=. za.co.spazzymoto.example --database-file-path=absolute/path/to/SourcetrailDB/examples/java_api_example/src/example.srctrldb --source-file-path=absolute/path/to/SourcetrailDB/examples/java_api_example/data/file.py
```

### Running with Sourcetrail

* Run cmake with `BUILD_EXAMPLES=ON` and `BUILD_BINDINGS_JAVA=ON`
* Build `make`.
* Open `java_api_example.srctrlprj` located in the build directory of `java_api_example` with Sourcetrail and index the project.
