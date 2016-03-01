################################################################################
# Package: AlignmentSys
################################################################################
gaudi_subdir(AlignmentSys v10r6)

gaudi_depends_on_subdirs(Alignment/AlignEvent
                         Alignment/AlignKernel
                         Alignment/AlignSolvTools
                         Alignment/AlignTrTools
                         Alignment/AlignmentDBVisualisationTool
                         Alignment/AlignmentInterfaces
                         Alignment/AlignmentMonitoring
                         Alignment/AlignmentTools
                         Alignment/Escher
                         Alignment/MisAligner
                         Alignment/TAlignment
                         Alignment/VeloAlignment
                         Calibration/OTCalibration
                         Phys/CommonParticles
			 Tr/TrackTools # to be removed after LHCb v40r1
                         Tf/TfKernel ) # to be removed after LHCb v40r1

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
