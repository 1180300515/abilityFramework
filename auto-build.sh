#!/usr/bin/env bash
rm -rf bin/*
rm -rf build/
echo "创建编译目录:build"
mkdir build
echo "in编译目录:build"
cd build/
echo "do:cmake"
cmake ../
echo "do:make"
make -j 8