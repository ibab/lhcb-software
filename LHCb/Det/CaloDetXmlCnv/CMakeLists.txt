################################################################################
# Package: CaloDetXmlCnv
################################################################################
gaudi_subdir(CaloDetXmlCnv v3r3p1)

gaudi_depends_on_subdirs(Det/CaloDet
                         Det/DetDescCnv)

gaudi_add_module(CaloDetXmlCnv
                 src/*.cpp
                 LINK_LIBRARIES CaloDetLib DetDescCnvLib)

