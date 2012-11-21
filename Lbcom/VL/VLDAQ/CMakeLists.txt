################################################################################
# Package: VLDAQ
################################################################################
gaudi_subdir(VLDAQ v1r1)

gaudi_depends_on_subdirs(Det/VLDet
                         Event/DAQEvent
                         Event/DigiEvent
                         Event/RecEvent)

gaudi_add_module(VLDAQ
                 src/*.cpp
                 INCLUDE_DIRS Event/DigiEvent
                 LINK_LIBRARIES VLDetLib DAQEventLib RecEvent)

gaudi_install_python_modules()

