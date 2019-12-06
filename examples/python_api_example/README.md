
## Python API Example

### Requirements

* CMake
* C++ Compiler
* Python
* SWIG

### Running from Command Line

* Run cmake with `BUILD_BINDINGS_PYTHON=ON`
* Build target `_sourcetraildb`
* Copy two files from the `bindings_python` build directory into `SourcetrailDB/examples/python_api_example/src`:
	- `sourcetraildb.py`
	- Windows: `_sourcetraildb.pyd`
	- Linux: `_sourcetraildb.so`
	- macOS: `_sourcetraildb.dylib` and rename to `_sourcetraildb.so`
* Run example from the directory `SourcetrailDB/examples/python_api_example/src` with:

```
$ python example.py --database-file-path=absolute/path/to/SourcetrailDB/examples/python_api_example/src/example.srctrldb --source-file-path=absolute/path/to/SourcetrailDB/examples/python_api_example/data/file.py
```

### Running with Sourcetrail

* Run cmake with `BUILD_EXAMPLES=ON` and `BUILD_BINDINGS_PYTHON=ON`
* Build target `_sourcetraildb` and `python_api_example`.
* Open `python_api_example.srctrlprj` located in the build directory of `python_api_example` with Sourcetrail and index the project.
