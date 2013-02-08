################################################################################
# Package: PatPixel
################################################################################
gaudi_subdir(PatPixel v1r0)

gaudi_depends_on_subdirs(Det/VPDet
                         Event/DigiEvent
                         Event/TrackEvent
                         GaudiAlg
                         GaudiKernel)


gaudi_add_module(PatPixel
                 src/*.cpp
		 LINK_LIBRARIES VPDetLib TrackEvent GaudiAlgLib GaudiKernel )



