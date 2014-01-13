################################################################################
# Package: GaudiOnlineTests
################################################################################
gaudi_subdir(GaudiOnlineTests v0r1)

gaudi_depends_on_subdirs(GaudiAud
			 GaudiSvc
			 GaudiAlg
			 DAQ/MDF
                         Event/DAQEvent
                         Online/DIM
			 Online/RootCnv
                         Online/OnlineKernel
                         Online/RawBankSizes
			 Online/GaudiOnline
                         Online/GaudiCheckpointing 
			 GaudiExamples)

find_package(PythonLibs)
find_package(AIDA)

gaudi_add_test(QMTest QMTEST)
