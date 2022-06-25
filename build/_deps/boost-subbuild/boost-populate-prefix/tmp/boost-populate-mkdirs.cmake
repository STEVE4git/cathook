# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/stephen/cathook/build/_deps/boost-src"
  "/home/stephen/cathook/build/_deps/boost-build"
  "/home/stephen/cathook/build/_deps/boost-subbuild/boost-populate-prefix"
  "/home/stephen/cathook/build/_deps/boost-subbuild/boost-populate-prefix/tmp"
  "/home/stephen/cathook/build/_deps/boost-subbuild/boost-populate-prefix/src/boost-populate-stamp"
  "/home/stephen/cathook/build/_deps/boost-subbuild/boost-populate-prefix/src"
  "/home/stephen/cathook/build/_deps/boost-subbuild/boost-populate-prefix/src/boost-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/stephen/cathook/build/_deps/boost-subbuild/boost-populate-prefix/src/boost-populate-stamp/${subDir}")
endforeach()
