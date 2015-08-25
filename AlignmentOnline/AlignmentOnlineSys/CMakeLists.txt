################################################################################
# Package: AlignmentOnlineSys
################################################################################
gaudi_subdir(AlignmentOnlineSys v10r5)

gaudi_depends_on_subdirs(AlignmentOnline/AlignOnline
			 AlignmentOnline/PyGeneticOnline
			 Calibration/OTOnlineCalibration)

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
