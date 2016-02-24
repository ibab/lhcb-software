################################################################################
# Package: HltTracking
################################################################################
gaudi_subdir(HltJets v1r0)

gaudi_depends_on_subdirs(Event/HltEvent
                         Hlt/HltCommon
                         Phys/JetAccessories
                         GaudiKernel)

set_property(DIRECTORY PROPERTY CONFIGURABLE_USER_MODULES HltJets.Configuration)
gaudi_install_python_modules()
