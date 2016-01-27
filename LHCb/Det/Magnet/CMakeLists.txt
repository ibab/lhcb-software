################################################################################
# Package: Magnet
################################################################################
gaudi_subdir(Magnet v8r0)

gaudi_depends_on_subdirs(Det/DDDB
                         Det/DetDesc
                         GaudiAlg
                         GaudiConf
                         GaudiKernel
                         Kernel/LHCbKernel)

gaudi_add_module(Magnet
                 src/*.cpp
                 LINK_LIBRARIES DetDescLib GaudiKernel LHCbKernel GaudiAlgLib)

