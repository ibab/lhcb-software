################################################################################
# Package: DDDB
################################################################################
gaudi_subdir(DDDB v1r78)

gaudi_depends_on_subdirs(Det/DetCond
                         Tools/CondDBEntityResolver)

find_package(Oracle)
find_package(SQLite)

gaudi_install_python_modules()

