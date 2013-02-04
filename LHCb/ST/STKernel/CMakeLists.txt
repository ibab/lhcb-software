################################################################################
# Package: STKernel
################################################################################
gaudi_subdir(STKernel v2r11)

gaudi_depends_on_subdirs(Det/STDet
                         Event/DAQEvent
                         Event/DigiEvent
                         Event/RecEvent
                         GaudiAlg
                         GaudiKernel
                         Kernel/LHCbKernel
                         Si/SiDAQ)

find_package(AIDA)
find_package(Boost)

gaudi_add_library(STKernelLib
                  src/*.cpp
                  PUBLIC_HEADERS Kernel
                  INCLUDE_DIRS Boost AIDA Event/DigiEvent Si/SiDAQ
                  LINK_LIBRARIES Boost STDetLib DAQEventLib RecEvent GaudiAlgLib GaudiKernel LHCbKernel)

gaudi_add_dictionary(STKernel
                     dict/STKernelDict.h
                     dict/STKernelDict.xml
                     INCLUDE_DIRS Boost Event/DigiEvent Si/SiDAQ
                     LINK_LIBRARIES Boost STDetLib DAQEventLib RecEvent GaudiAlgLib GaudiKernel LHCbKernel STKernelLib
                     OPTIONS "-U__MINGW32__ -DBOOST_DISABLE_ASSERTS")

