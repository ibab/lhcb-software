################################################################################
# Package: HltDAQ
################################################################################
gaudi_subdir(HltDAQ v4r24)

gaudi_depends_on_subdirs(Det/DetCond
                         Det/DetDesc
                         Event/DAQEvent
                         Event/HltEvent
                         Event/LumiEvent
                         Event/EventPacker
                         GaudiAlg
                         GaudiKernel
                         GaudiObjDesc
                         Hlt/HltServices
                         Kernel/HltInterfaces
                         Phys/LoKiHlt
			 DAQ/DAQKernel)

find_package(AIDA)
find_package(Boost COMPONENTS iostreams filesystem)

include(GaudiObjDesc)

god_build_headers(xml/*.xml
                  DESTINATION HltDAQ)

gaudi_install_headers(HltDAQ)

gaudi_add_module(HltDAQ
                 src/*.cpp
                 INCLUDE_DIRS Boost AIDA HltDAQ
                 LINK_LIBRARIES Boost DetCondLib DetDescLib DAQEventLib DAQKernelLib HltEvent GaudiAlgLib GaudiKernel HltInterfaces LoKiHltLib)

gaudi_add_unit_test(utestTrackingCoder
                    src/utest/utestTrackingCoder.cpp src/HltTrackingCoder.cpp
                    LINK_LIBRARIES GaudiKernel HltEvent
                    TYPE Boost)

gaudi_add_test(QMTest QMTEST)
