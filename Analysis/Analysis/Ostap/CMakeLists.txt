################################################################################
# Package:   Ostap
################################################################################
gaudi_subdir(Ostap v1r11)

gaudi_depends_on_subdirs(Phys/AnalysisPython
                         Analysis/PyAna)

find_package(ROOT)

gaudi_install_python_modules()
gaudi_install_scripts()

gaudi_add_test(QMTest QMTEST)
