################################################################################
# Package: BBDecTreeTool
################################################################################
gaudi_subdir(BBDecTreeTool v1r8)

gaudi_depends_on_subdirs(GaudiAlg
                         GaudiKernel
                         Phys/DaVinciKernel
                         Phys/LoKi
                         Phys/LoKiArrayFunctors)

find_package(Boost COMPONENTS system filesystem)

gaudi_add_module(BBDecTreeTool
                 src/*.cpp
                 INCLUDE_DIRS Boost Phys/LoKi
                 LINK_LIBRARIES Boost GaudiAlgLib GaudiKernel DaVinciKernelLib LoKiArrayFunctorsLib)

gaudi_install_python_modules()

