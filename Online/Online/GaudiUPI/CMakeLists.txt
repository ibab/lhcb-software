################################################################################
# Package: GaudiUPI
################################################################################
gaudi_subdir(GaudiUPI v1r16)

gaudi_depends_on_subdirs(GaudiKernel
                         Online/UPI
                         Online/DIM)

gaudi_install_headers(GaudiUPI)

gaudi_add_module(GaudiUPI src/*.cpp
                 LINK_LIBRARIES GaudiKernel dim UPI)
