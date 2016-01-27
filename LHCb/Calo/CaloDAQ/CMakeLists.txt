################################################################################
# Package: CaloDAQ
################################################################################
gaudi_subdir(CaloDAQ v4r39)

gaudi_depends_on_subdirs(Calo/CaloUtils
                         DAQ/DAQUtils
                         DAQ/DAQKernel
                         Det/CaloDet
                         Event/DAQEvent
                         Event/DigiEvent
                         Event/L0Event
                         Event/RecEvent
                         GaudiAlg)

gaudi_add_module(CaloDAQ
                 src/*.cpp
                 INCLUDE_DIRS DAQ/DAQUtils Event/DigiEvent
                 LINK_LIBRARIES CaloUtils CaloDetLib DAQEventLib DAQKernelLib L0Event RecEvent GaudiAlgLib)

gaudi_add_dictionary(CaloDAQ
                     dict/CaloDAQDict.h
                     dict/CaloDAQDict.xml
                     INCLUDE_DIRS DAQ/DAQUtils Event/DigiEvent
                     LINK_LIBRARIES CaloUtils CaloDetLib DAQEventLib L0Event RecEvent GaudiAlgLib
                     OPTIONS "-U__MINGW32__ -DBOOST_DISABLE_ASSERTS")

gaudi_install_headers(CaloDAQ)
gaudi_install_python_modules()

