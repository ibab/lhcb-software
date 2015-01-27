CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)
find_package(GaudiProject)
gaudi_project(B HEAD
              USE A HEAD
              DATA PkgA)
