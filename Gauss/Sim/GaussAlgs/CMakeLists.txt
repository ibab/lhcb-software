################################################################################
# Package: GaussAlgs
################################################################################
gaudi_subdir(GaussAlgs v8r0p1)

gaudi_depends_on_subdirs(Kernel/LHCbKernel
                         Sim/GaussTools)

find_package(Boost)

gaudi_add_module(GaussAlgs
                 src/*.cpp
                 INCLUDE_DIRS Boost
                 LINK_LIBRARIES Boost LHCbKernel GaussToolsLib)

