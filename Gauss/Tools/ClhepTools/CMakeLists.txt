################################################################################
# Package: ClhepTools
################################################################################
gaudi_subdir(ClhepTools v2r1p1)

gaudi_depends_on_subdirs(GaudiKernel)

find_package(CLHEP)

gaudi_install_headers(ClhepTools)

