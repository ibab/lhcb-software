################################################################################
# Package: AnalysisConf
################################################################################
gaudi_subdir(AnalysisConf v2r5)

gaudi_depends_on_subdirs(GaudiConf)

set_property(DIRECTORY PROPERTY CONFIGURABLE_USER_MODULES AnalysisConf.Configuration)
gaudi_install_python_modules()

