################################################################################
# Package: PatPixel
################################################################################
gaudi_subdir(PatPixel v1r1)

gaudi_depends_on_subdirs(Det/VPDet
                         Event/DigiEvent
                         Event/TrackEvent
                         Tf/PatKernel
                         GaudiAlg
                         GaudiKernel)


gaudi_add_module(PatPixel
                 src/*.cpp
                 INCLUDE_DIRS AIDA GSL Tf/PatKernel
		 LINK_LIBRARIES VPDetLib TrackEvent GaudiAlgLib GaudiKernel )



