################################################################################
# Package: GaudiSerialize
################################################################################
gaudi_subdir(GaudiSerialize v0r11)

gaudi_depends_on_subdirs(DAQ/MDF
                         Event/DAQEvent
                         Online/RootCnv)

find_package(ROOT COMPONENTS Reflex)
find_package(Boost)

include_directories(${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
reflex_generate_dictionary(GaudiSerialize
                           dict/Serialize_dict.h dict/Serialize_dict.xml)

gaudi_add_module(GaudiSerialize src/*.cpp GaudiSerialize_dict.cpp
                 LINK_LIBRARIES ROOT DAQEventLib MDFLib RootCnvLib
                 GENCONF_PRELOAD MDF)
