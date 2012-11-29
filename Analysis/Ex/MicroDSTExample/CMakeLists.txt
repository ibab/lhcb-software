################################################################################
# Package: MicroDSTExample
################################################################################
gaudi_subdir(MicroDSTExample v2r14)

gaudi_depends_on_subdirs(MicroDST/MicroDSTAlgorithm
                         MicroDST/MicroDSTConf
                         Phys/AnalysisPython
                         PhysSel/PhysSelPython)

gaudi_install_python_modules()

