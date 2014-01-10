################################################################################
# Package: AlignSolvTools
################################################################################
gaudi_subdir(AlignSolvTools v3r3)

gaudi_depends_on_subdirs(Alignment/AlignKernel
                         Alignment/AlignmentInterfaces
                         GaudiAlg
                         GaudiGSL
                         GaudiKernel)

find_package(ROOT COMPONENTS Matrix)

gaudi_add_module(AlignSolvTools
                 src/*.cpp
                 INCLUDE_DIRS ROOT Alignment/AlignmentInterfaces
                 LINK_LIBRARIES ROOT AlignKernel GaudiAlgLib GaudiGSLLib GaudiKernel)

