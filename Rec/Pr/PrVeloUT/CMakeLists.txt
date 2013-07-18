################################################################################
# Package: PrVeloUT
################################################################################
gaudi_subdir(PrVeloUT v1r2)

gaudi_depends_on_subdirs(Det/STDet
                         Event/DigiEvent
                         Event/TrackEvent
                         Pr/PrKernel
			 Tf/TfKernel	
			 Tr/TrackInterfaces)

gaudi_add_module(PrVeloUT
                 src/*.cpp
                 INCLUDE_DIRS Event/DigiEvent Pr/PrKernel Tf/TfKernel Tr/TrackInterfaces
                 LINK_LIBRARIES STDetLib TrackEvent)

gaudi_install_python_modules()

