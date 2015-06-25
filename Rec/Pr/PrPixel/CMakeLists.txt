################################################################################
# Package: PrPixel
################################################################################
gaudi_subdir(PrPixel v1r11)

gaudi_depends_on_subdirs(Det/VPDet
                         DAQ/DAQKernel
                         Event/DAQEvent
                         Event/DigiEvent
                         Event/TrackEvent
                         Pr/PrKernel
                         GaudiAlg
                         GaudiKernel)


gaudi_add_module(PrPixel
                 src/*.cpp
                 INCLUDE_DIRS AIDA GSL Pr/PrKernel
		 LINK_LIBRARIES VPDetLib DAQEventLib DAQKernelLib TrackEvent GaudiAlgLib GaudiKernel)



