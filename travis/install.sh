#!/bin/bash

if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
    brew install swig
    brew link --force swig
elif [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
	sudo apt-get update
    sudo apt-get install -qq python-dev swig
    sudo apt-get update -qq
fi
