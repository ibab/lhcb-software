################################################################################
# Package: Bender
################################################################################
gaudi_subdir(Bender v26r2p1)

gaudi_depends_on_subdirs(GaudiPython
                         Phys/AnalysisPython
                         Phys/DaVinci)

gaudi_install_python_modules()
gaudi_install_scripts()

