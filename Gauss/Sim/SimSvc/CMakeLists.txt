################################################################################
# Package: SimSvc
################################################################################
gaudi_subdir(SimSvc v5r2)

gaudi_depends_on_subdirs(Det/DetDescCnv)

gaudi_add_module(SimSvc
                 src/*.cpp
                 LINK_LIBRARIES DetDescCnvLib)

gaudi_install_headers(SimSvc)

