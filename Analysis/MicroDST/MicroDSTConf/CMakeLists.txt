################################################################################
# Package: MicroDSTConf
################################################################################
gaudi_subdir(MicroDSTConf v2r10p1)

gaudi_depends_on_subdirs(Event/EventAssoc
                         MicroDST/MicroDSTAlgorithm
                         Phys/AnalysisConf)

gaudi_install_python_modules()

gaudi_env(SET MICRODSTOPTS \${MICRODSTCONFROOT}/options)

