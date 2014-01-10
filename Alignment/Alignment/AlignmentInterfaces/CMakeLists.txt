################################################################################
# Package: AlignmentInterfaces
################################################################################
gaudi_subdir(AlignmentInterfaces v3r3)

gaudi_depends_on_subdirs(Alignment/AlignKernel
                         GaudiKernel)

find_package(Boost COMPONENTS system)

gaudi_add_dictionary(AlignmentInterfaces
                     dict/AlignmentInterfacesDict.h
                     dict/AlignmentInterfacesDict.xml
                     INCLUDE_DIRS Boost
                     LINK_LIBRARIES Boost AlignKernel GaudiKernel
                     OPTIONS "-U__MINGW32__")

gaudi_install_headers(AlignmentInterfaces)

