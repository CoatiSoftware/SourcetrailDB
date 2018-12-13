#!/bin/bash

cd $TRAVIS_BUILD_DIR

VERSION=$(head -1 build/version.txt)

if [[ "$PYTHON_BINDING" == "1" ]]; then
	PYTHON_VERSION=$(python3 -c "import sys; sys.stdout.write(str(sys.version_info.major)); sys.stdout.write(str(sys.version_info.minor));")
	BUNDLE_NAME=sourcetraildb_python${PYTHON_VERSION}_${VERSION}_${TRAVIS_OS_NAME}_64bit

	mkdir -p $BUNDLE_NAME
	cp build/bindings_python/sourcetraildb.py $BUNDLE_NAME
	cp build/bindings_python/_sourcetraildb* $BUNDLE_NAME/_sourcetraildb.so

	zip -r $BUNDLE_NAME $BUNDLE_NAME
else
	BUNDLE_NAME=sourcetraildb_core_${VERSION}_${TRAVIS_OS_NAME}_64bit

	mkdir -p $BUNDLE_NAME/include
	cp build/core/*.a $BUNDLE_NAME
	cp core/include/* $BUNDLE_NAME/include
	cp build/core/include/* $BUNDLE_NAME/include

	zip -r $BUNDLE_NAME $BUNDLE_NAME
fi
