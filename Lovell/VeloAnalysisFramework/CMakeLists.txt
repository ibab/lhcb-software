################################################################################
# Package: VeloAnalysisFramework
################################################################################
gaudi_subdir(VeloAnalysisFramework v1r0)

#Modify as necessary
find_package(ROOT COMPONENTS Hist Matrix Graf)

gaudi_depends_on_subdirs(GaudiKernel)

gaudi_install_python_modules()

gaudi_install_scripts()
