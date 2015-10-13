################################################################################
# Package: RichRecTemplateRings
################################################################################
gaudi_subdir(RichRecTemplateRings v1r7p1)

gaudi_depends_on_subdirs(Event/MCEvent
                         Kernel/MCInterfaces
                         Rich/RichRecBase)

find_package(AIDA)
find_package(FFTW)

gaudi_add_module(RichRecTemplateRings
                 src/*.cpp
                 src/base/*.cpp
                 src/TargetData/*.cpp
                 src/TemplateData/*.cpp
                 src/Utility/*.cpp
                 src/FFPlan/*.cpp
                 src/HistoMonitor/*.cpp
                 src/RingFit/*.cpp
                 src/MCTruth/*.cpp
                 INCLUDE_DIRS AIDA FFTW Kernel/MCInterfaces src
                 LINK_LIBRARIES FFTW MCEvent RichRecBase)

gaudi_install_python_modules()

