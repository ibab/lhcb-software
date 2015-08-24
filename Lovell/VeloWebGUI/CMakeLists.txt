################################################################################
# Package: VeloWebGUI
################################################################################
gaudi_subdir(VeloWebGUI v1r2)

gaudi_depends_on_subdirs(VeloAnalysisFramework
			GaudiKernel)

gaudi_install_python_modules()

gaudi_add_test(QMTest QMTEST)

install (DIRECTORY "resources/templates"
                   "resources/static"
         DESTINATION resources)
