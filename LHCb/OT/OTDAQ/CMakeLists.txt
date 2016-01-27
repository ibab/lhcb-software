################################################################################
# Package: OTDAQ
################################################################################
gaudi_subdir(OTDAQ v8r0)

gaudi_depends_on_subdirs(Det/OTDet
                         Event/DAQEvent
                         Event/DigiEvent
			 DAQ/DAQKernel
                         GaudiAlg)


gaudi_add_module(OTDAQ
                 src/*.cpp
                 INCLUDE_DIRS Event/DigiEvent
                 LINK_LIBRARIES OTDetLib DAQKernelLib DAQEventLib GaudiAlgLib)

gaudi_add_dictionary(OTDAQ
                     dict/OTDAQDict.h
                     dict/OTDAQDict.xml
                     INCLUDE_DIRS Event/DigiEvent
                     LINK_LIBRARIES OTDetLib DAQEventLib GaudiAlgLib
                     OPTIONS "-U__MINGW32__")

gaudi_install_headers(Event OTDAQ)

