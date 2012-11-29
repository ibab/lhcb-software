################################################################################
# Package: LoKi
################################################################################
gaudi_subdir(LoKi v9r0)

gaudi_depends_on_subdirs(Phys/LoKiAlgo
                         Phys/LoKiArrayFunctors
                         Phys/LoKiCore
                         Phys/LoKiHlt
                         Phys/LoKiNumbers
                         Phys/LoKiPhys)

gaudi_install_headers(LoKi)

