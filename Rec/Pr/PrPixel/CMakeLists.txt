################################################################################
# Package: PrPixel
################################################################################
gaudi_subdir(PrPixel v1r5)

gaudi_depends_on_subdirs(Det/VPDet
                         Event/DAQEvent
                         Event/DigiEvent
                         Event/TrackEvent
                         Tf/PatKernel
                         GaudiAlg
                         GaudiKernel)


gaudi_add_module(PrPixel
                 src/*.cpp
                 INCLUDE_DIRS AIDA GSL Tf/PatKernel
		 LINK_LIBRARIES VPDetLib DAQEventLib TrackEvent GaudiAlgLib GaudiKernel )



