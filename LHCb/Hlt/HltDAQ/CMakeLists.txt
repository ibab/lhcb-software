################################################################################
# Package: HltDAQ
################################################################################
gaudi_subdir(HltDAQ v2r18)

gaudi_depends_on_subdirs(Det/DetCond
                         Det/DetDesc
                         Event/DAQEvent
                         Event/HltEvent
                         GaudiAlg
                         GaudiKernel
                         GaudiObjDesc
                         Hlt/HltServices
                         Kernel/HltInterfaces
                         Phys/LoKiHlt)

find_package(AIDA)
find_package(Boost COMPONENTS iostreams filesystem)

include(GaudiObjDesc)

god_build_headers(xml/*.xml
                  PRIVATE)

gaudi_add_module(HltDAQ
                 src/*.cpp
                 INCLUDE_DIRS Boost AIDA
                 LINK_LIBRARIES Boost DetCondLib DetDescLib DAQEventLib HltEvent GaudiAlgLib GaudiKernel HltInterfaces LoKiHltLib)

