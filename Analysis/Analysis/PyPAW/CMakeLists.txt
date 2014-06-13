################################################################################
# Package: PyPAW
################################################################################
gaudi_subdir(PyPAW v1r4)

gaudi_depends_on_subdirs(Phys/AnalysisPython
                         Analysis/PyAna)

find_package(ROOT)

gaudi_install_python_modules()
gaudi_install_scripts()

