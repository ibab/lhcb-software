################################################################################
# Package: HltServices
################################################################################
gaudi_subdir(HltServices v1r0)

gaudi_depends_on_subdirs(Det/DetCond
                         Det/DetDesc
                         Event/DAQEvent
                         Event/HltEvent
                         GaudiAlg
                         GaudiKernel
                         Hlt/HltDAQ
                         Kernel/HltInterfaces
                         Phys/LoKiHlt)

find_package(AIDA)
find_package(Boost COMPONENTS iostreams filesystem)

gaudi_add_module(HltServices
                 src/*.cpp
                 INCLUDE_DIRS Boost AIDA
                 LINK_LIBRARIES Boost DetCondLib DetDescLib DAQEventLib HltEvent GaudiAlgLib GaudiKernel HltInterfaces LoKiHltLib)

