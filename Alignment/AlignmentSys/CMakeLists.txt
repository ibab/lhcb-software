################################################################################
# Package: AlignmentSys
################################################################################
gaudi_subdir(AlignmentSys v10r4)

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
                         Phys/CommonParticles)
# removed v10r4		 Tr/TrackUtils # Temporary untile release of Rec v17r6 - MM, 2/4/2015
#			 Det/OTDet # Temporary until release of LHCb v38r6 - MM, 1/4/2015
#			 Kernel/LHCbMath) # Temporary until release of LHCb v38r6 - MM, 1/4/2015

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
