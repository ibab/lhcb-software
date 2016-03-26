################################################################################
# Package: OstapTutor
################################################################################
gaudi_subdir(OstapTutor v2r0)

gaudi_depends_on_subdirs(Ostap)

find_package(Ostap)

gaudi_install_python_modules()

