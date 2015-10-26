################################################################################
# Package: Bender
################################################################################
gaudi_subdir(Bender v27r0)

gaudi_depends_on_subdirs(GaudiPython
                         Phys/AnalysisPython
                         Phys/DaVinci)

gaudi_install_python_modules()
gaudi_install_scripts()

