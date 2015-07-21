################################################################################
# Package: BooleSys
################################################################################
gaudi_subdir(BooleSys v29r8)

gaudi_depends_on_subdirs(Digi/Boole
				Det/DDDB
				Det/DetCond
				Det/OTDet
)

gaudi_add_test(QMTest QMTEST)
