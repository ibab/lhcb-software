################################################################################
# Package: Bender
################################################################################
gaudi_subdir(Bender v25r7p3)

gaudi_depends_on_subdirs(GaudiPython
                         Phys/AnalysisPython
                         Phys/DaVinci)

gaudi_install_python_modules()
gaudi_install_scripts()

gaudi_alias(bender_mc
            benderMc)
gaudi_alias(bender_MC
            benderMc)
gaudi_alias(get-grid-url
            get_grid_url)
