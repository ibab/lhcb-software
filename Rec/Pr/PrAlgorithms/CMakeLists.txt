################################################################################
# Package: PrAlgorithms
################################################################################
gaudi_subdir(PrAlgorithms v1r17)

gaudi_depends_on_subdirs(Det/FTDet
                         Event/DAQEvent
                         Event/RecEvent
			 Tr/TrackInterfaces
                         Pr/PrKernel)

gaudi_add_module(PrAlgorithms
                 src/*.cpp
                 INCLUDE_DIRS Tr/TrackInterfaces
                 LINK_LIBRARIES FTDetLib DAQEventLib RecEvent PrKernel)

