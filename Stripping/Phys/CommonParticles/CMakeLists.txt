################################################################################
# Package: CommonParticles
################################################################################
gaudi_subdir(CommonParticles v8r22)

gaudi_depends_on_subdirs(Phys/DaVinciKernel
                         Tr/TrackTools)

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
