################################################################################
# Package: DSTWriters
################################################################################
gaudi_subdir(DSTWriters v4r7)

gaudi_depends_on_subdirs(GaudiAlg
                         GaudiConf)

gaudi_add_module(DSTWriters
                 src/*.cpp
                 LINK_LIBRARIES GaudiAlgLib)

gaudi_install_python_modules()

