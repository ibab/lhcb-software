################################################################################
# Package: MuonDAQ
################################################################################
gaudi_subdir(MuonDAQ v4r21)

gaudi_depends_on_subdirs(DAQ/DAQUtils
                         Det/MuonDet
                         Event/DAQEvent
                         Event/DigiEvent
                         Event/RecEvent
                         GaudiAlg
			 DAQ/DAQKernel
                         Muon/MuonKernel)

gaudi_add_module(MuonDAQ
                 src/*.cpp
                 INCLUDE_DIRS DAQ/DAQUtils Event/DigiEvent
                 LINK_LIBRARIES MuonDetLib DAQEventLib DAQKernelLib RecEvent GaudiAlgLib MuonKernelLib)

gaudi_add_dictionary(MuonDAQ
                     dict/MuonDAQDict.h
                     dict/MuonDAQDict.xml
                     INCLUDE_DIRS DAQ/DAQUtils Event/DigiEvent
                     LINK_LIBRARIES MuonDetLib DAQEventLib RecEvent GaudiAlgLib MuonKernelLib
                     OPTIONS "-U__MINGW32__ -DBOOST_DISABLE_ASSERTS")

gaudi_install_headers(Event MuonDAQ)

