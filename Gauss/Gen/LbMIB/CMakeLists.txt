################################################################################
# Package: LbMIB
################################################################################
gaudi_subdir(LbMIB v7r0)

gaudi_depends_on_subdirs(Event/GenEvent
                         GaudiAlg
                         Kernel/PartProp)

gaudi_add_module(LbMIB
                 src/*.cpp
                 INCLUDE_DIRS AIDA
                 LINK_LIBRARIES GenEvent GaudiAlgLib PartPropLib)

gaudi_install_python_modules()

