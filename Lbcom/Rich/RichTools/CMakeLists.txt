################################################################################
# Package: RichTools
################################################################################
gaudi_subdir(RichTools v2r50)

find_package(Eigen)

gaudi_depends_on_subdirs(Det/RichDet
                         Kernel/PartProp
                         Rich/RichKernel)

gaudi_add_module(RichTools
                 src/*.cpp
                 INCLUDE_DIRS Eigen
                 LINK_LIBRARIES RichDetLib PartPropLib RichKernelLib)

#set(CMAKE_CXX_FLAGS_RELEASE "-O3 -msse4")
