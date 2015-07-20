################################################################################
# Package: OTDAQ
################################################################################
gaudi_subdir(OTDAQ v7r20)

gaudi_depends_on_subdirs(Det/OTDet
                         Event/DAQEvent
                         Event/DigiEvent
			 DAQ/DAQKernel
                         GaudiAlg)

find_package(GSL)

gaudi_add_module(OTDAQ
                 src/*.cpp
                 INCLUDE_DIRS GSL Event/DigiEvent 
                 LINK_LIBRARIES GSL OTDetLib DAQKernelLib DAQEventLib GaudiAlgLib)

gaudi_add_dictionary(OTDAQ
                     dict/OTDAQDict.h
                     dict/OTDAQDict.xml
                     INCLUDE_DIRS GSL Event/DigiEvent
                     LINK_LIBRARIES GSL OTDetLib DAQEventLib GaudiAlgLib
                     OPTIONS "-U__MINGW32__")

gaudi_install_headers(Event OTDAQ)

