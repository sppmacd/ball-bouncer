#!/bin/bash

path=$PWD

for builddir in build*; do
	cd $path/$builddir
	cmake --build . -j4
	cd root
	zip -r ../../ballbouncer-$builddir.zip *
done

