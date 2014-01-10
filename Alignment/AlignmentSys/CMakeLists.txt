################################################################################
# Package: AlignmentSys
################################################################################
gaudi_subdir(AlignmentSys v9r2)

gaudi_depends_on_subdirs(Alignment/AlignEvent
                         Alignment/AlignKernel
                         Alignment/AlignSolvTools
                         Alignment/AlignTrTools
                         Alignment/AlignmentDBVisualisationTool
                         Alignment/AlignmentInterfaces
                         Alignment/AlignmentTools
                         Alignment/Escher
                         Alignment/MisAligner
                         Alignment/TAlignment
                         Alignment/VeloAlignment
                         Calibration/OTCalibration
                         Phys/CommonParticles
                         Tr/TrackUtils)

gaudi_install_python_modules()

