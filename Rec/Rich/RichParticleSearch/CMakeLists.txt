################################################################################
# Package: RichParticleSearch
################################################################################
gaudi_subdir(RichParticleSearch v1r12p1)

gaudi_depends_on_subdirs(Event/MCEvent
                         Event/PhysEvent
                         Kernel/MCInterfaces
                         Rich/RichRecBase)

gaudi_add_module(RichParticleSearch
                 src/*.cpp
                 INCLUDE_DIRS AIDA Kernel/MCInterfaces
                 LINK_LIBRARIES MCEvent PhysEvent RichRecBase)

gaudi_install_python_modules()

