################################################################################
# Package: InsertDB
################################################################################
gaudi_subdir(InsertDB v1r11)

gaudi_depends_on_subdirs(GaudiAlg
                         Online/DIM
                         Online/OnlineHistDB
                         Online/OnlineKernel)

# find_package(XYZ)

gaudi_add_module(InsertDB src/*.cpp
                 #INCLUDE_DIRS ...
                 LINK_LIBRARIES OnlineHistDB OnlineKernel dim GaudiAlgLib)
