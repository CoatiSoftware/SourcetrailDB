#!/bin/bash

cd $TRAVIS_BUILD_DIR

VERSION=$(head -1 build/version.txt)

if [[ "$PYTHON_BINDING" == "1" ]]; then
	PYTHON_VERSION=$(python3 -c "import sys; sys.stdout.write(str(sys.version_info.major)); sys.stdout.write(str(sys.version_info.minor));")
	BUNDLE_NAME=sourcetraildb_python${PYTHON_VERSION}_${VERSION}-${TRAVIS_OS_NAME}-64bit-${TRAVIS_COMPILER}

	mkdir -p $BUNDLE_NAME
	cp build/bindings_python/sourcetraildb.py $BUNDLE_NAME
	cp build/bindings_python/_sourcetraildb* $BUNDLE_NAME/_sourcetraildb.so
else
	BUNDLE_NAME=sourcetraildb_core_${VERSION}-${TRAVIS_OS_NAME}-64bit-${TRAVIS_COMPILER}

	mkdir -p $BUNDLE_NAME/include
	mkdir -p $BUNDLE_NAME/lib

	cp build/core/*.a $BUNDLE_NAME/lib
	cp core/include/* $BUNDLE_NAME/include
	cp build/core/include/* $BUNDLE_NAME/include
fi

mkdir -p $BUNDLE_NAME/license
cp LICENSE.txt $BUNDLE_NAME/license/license_sourcetraildb.txt
cp external/catch/license_catch.txt $BUNDLE_NAME/license/
cp external/cpp_sqlite/license_cpp_sqlite.txt $BUNDLE_NAME/license/
cp external/json/license_json.txt $BUNDLE_NAME/license/

zip -r $BUNDLE_NAME $BUNDLE_NAME
