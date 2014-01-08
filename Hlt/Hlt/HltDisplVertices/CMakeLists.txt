################################################################################
# Package: HltDisplVertices
################################################################################
gaudi_subdir(HltDisplVertices v4r0p1)

gaudi_depends_on_subdirs(Det/CaloDet
                         Det/DetDesc
                         Det/VeloDet
                         Phys/DaVinciKernel
                         Phys/LoKi
                         Phys/LoKiHlt
                         Phys/LoKiPhys
                         Phys/LoKiTrigger)

gaudi_install_headers(Kernel)

gaudi_add_module(HltDisplVertices
                 src/*.cpp
                 INCLUDE_DIRS Phys/LoKi
                 LINK_LIBRARIES CaloDetLib DetDescLib VeloDetLib DaVinciKernelLib LoKiHltLib LoKiPhysLib LoKiTriggerLib)


gaudi_add_test(QMTest QMTEST)
