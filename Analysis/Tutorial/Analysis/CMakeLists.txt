################################################################################
# Package: Analysis
################################################################################
gaudi_subdir(Analysis v10r4p1)

gaudi_depends_on_subdirs(Phys/DaVinciKernel
                         Phys/DaVinciMCKernel)

gaudi_add_module(AnalysisTutorial
                 src/*.cpp
                 LINK_LIBRARIES DaVinciKernelLib DaVinciMCKernelLib)

gaudi_install_python_modules()

