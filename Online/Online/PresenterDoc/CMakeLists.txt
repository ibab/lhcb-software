################################################################################
# Package: PresenterDoc
################################################################################
gaudi_subdir(PresenterDoc v1r1)

gaudi_depends_on_subdirs(GaudiAlg)

gaudi_install_headers(PresenterDoc)

gaudi_add_module(PresenterDoc src/*.cpp
                 LINK_LIBRARIES GaudiAlgLib)
