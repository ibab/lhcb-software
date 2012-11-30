################################################################################
# Package: RichPIDQC
################################################################################
gaudi_subdir(RichPIDQC v1r7p1)

gaudi_depends_on_subdirs(Phys/DSTWriters
                         Rich/RichKernel)

set_property(DIRECTORY PROPERTY CONFIGURABLE_USER_MODULES RichPIDQC.Configuration RichPIDQC.KshortPiPi RichPIDQC.LambdaToProtonPion RichPIDQC.JPsiMuMu RichPIDQC.DsToPhiPi RichPIDQC.DstarToDzeroPi)
gaudi_install_python_modules()

