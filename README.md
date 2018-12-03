# SourcetrailDB

Windows Builds: [![Build status](https://ci.appveyor.com/api/projects/status/gbblx1l790vm952c/branch/master?svg=true)](https://ci.appveyor.com/project/mlangkabel/sourcetraildb/branch/master)


TODO: write short description here with code example and sourcetrail screenshorts of resulting graph


## Used By
* TODO: reference Python project here


## Requirements Core
* None. This package is self contained.


## Requirements Python Bindings:
* __SourcetrailDB Core__. Is contained within this repository.
* __Python__. Usually CMake will auto-detect your Python installation. If you want to use a specific version of Python, please define the `PYTHON_INCLUDE_DIRS` and `PYTHON_LIBRARIES` variables accordingly when running CMake. Make sure to link to a 32 bit version of Python when building this target for a 32 bit architecture and use a 64 bit Python when bulding this target for a 64 bit architecture.
* __SWIG 3.0.12__ is used to automatically generate python binding code. Make sure that SWIG is added to your path environment variable.


## TODO
* add documentation to code
* add "How to Build" section to readme file
* add tests and add section "Running the Tests" here
* improve name hierarchy and name element types
* versioning in package name (e.g. SourcetrailDB_v1_db21_c684)
* add 3rd party license references
* add cmake packaging and use find_package
