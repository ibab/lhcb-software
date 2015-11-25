################################################################################
# Package: VertexFit
################################################################################
gaudi_subdir(VertexFit v6r23)

gaudi_depends_on_subdirs(GaudiAlg
                         Kernel/LHCbMath
                         Phys/DaVinciInterfaces
                         Phys/DaVinciKernel
                         Phys/LoKiPhys
                         Tr/TrackInterfaces)

gaudi_add_module(VertexFit
                 src/*.cpp
                 INCLUDE_DIRS Tr/TrackInterfaces
                 LINK_LIBRARIES GaudiAlgLib LHCbMathLib DaVinciInterfacesLib DaVinciKernelLib LoKiPhysLib)


gaudi_add_test(QMTest QMTEST)
