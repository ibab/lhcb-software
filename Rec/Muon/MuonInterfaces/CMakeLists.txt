################################################################################
# Package: MuonInterfaces
################################################################################
gaudi_subdir(MuonInterfaces v2r0)

gaudi_depends_on_subdirs(Det/MuonDet
                         Event/DAQEvent
                         Event/RecEvent
                         GaudiKernel
                         Kernel/LHCbKernel
                         Muon/MuonKernel)

gaudi_add_library(MuonInterfacesLib
                  src/Lib/*.cpp
                  PUBLIC_HEADERS MuonInterfaces
                  LINK_LIBRARIES MuonDetLib DAQEventLib RecEvent GaudiKernel LHCbKernel MuonKernelLib)

gaudi_add_dictionary(MuonInterfaces
                     dict/MuonInterfacesDict.h
                     dict/MuonInterfacesDict.xml
                     LINK_LIBRARIES MuonDetLib DAQEventLib RecEvent GaudiKernel LHCbKernel MuonKernelLib MuonInterfacesLib
                     OPTIONS "-U__MINGW32__")

