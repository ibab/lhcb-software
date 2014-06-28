################################################################################
# Package: PyPAW
################################################################################
gaudi_subdir(PyPAW v2r0)

gaudi_depends_on_subdirs(Analysis/Ostap)

find_package(ROOT)

gaudi_install_python_modules()

