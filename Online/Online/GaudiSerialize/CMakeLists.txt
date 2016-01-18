################################################################################
# Package: GaudiSerialize
################################################################################
gaudi_subdir(GaudiSerialize v0r16)

gaudi_depends_on_subdirs(DAQ/MDF
                         Event/DAQEvent
                         Online/RootCnv)

find_package(ROOT)
if(ROOT_VERSION_MAJOR LESS 6)
  find_package(ROOT COMPONENTS Reflex)
else()
  find_package(ROOT COMPONENTS Cling)
endif()
find_package(Boost)

gaudi_install_headers(GaudiSerialize)

# override the genreflex call to wrap it in the right environment
set(ROOT_genreflex_CMD ${env_cmd} --xml ${env_xml} ${ROOT_genreflex_CMD})

include_directories(${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
reflex_generate_dictionary(GaudiSerialize
                           dict/Serialize_dict.h dict/Serialize_dict.xml)

gaudi_add_module(GaudiSerialize src/*.cpp GaudiSerializeDict.cpp
                 LINK_LIBRARIES ROOT DAQEventLib MDFLib RootCnvLib
                 GENCONF_PRELOAD MDF)
