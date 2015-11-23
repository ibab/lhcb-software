################################################################################
# Package: HltServices
################################################################################
gaudi_subdir(HltServices v2r11)

gaudi_depends_on_subdirs(Det/DetCond
                         Det/DetDesc
                         Event/DAQEvent
                         Event/HltEvent
                         GaudiAlg
                         GaudiKernel
                         Kernel/HltInterfaces
                         Phys/LoKiHlt)

find_package(AIDA)
find_package(Boost COMPONENTS iostreams filesystem program_options)

gaudi_add_module(HltServices
                 src/*.cpp
                 INCLUDE_DIRS Boost AIDA
                 LINK_LIBRARIES Boost DetCondLib DetDescLib DAQEventLib HltEvent GaudiAlgLib GaudiKernel HltInterfaces LoKiHltLib)

gaudi_add_executable(hlttck_cdb_listkeys  utilities/main.cpp src/cdb.cpp src/tar.cpp
                     INCLUDE_DIRS Boost
                     LINK_LIBRARIES GaudiKernel Boost HltInterfaces)
