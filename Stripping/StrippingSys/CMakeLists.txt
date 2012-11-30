################################################################################
# Package: StrippingSys
################################################################################
gaudi_subdir(StrippingSys v6r3)

gaudi_depends_on_subdirs(Phys/CommonParticles
                         Phys/DisplVertices
                         Phys/MvaPreSelB2DH
                         Phys/StandardParticles
                         Phys/StrippingAlgs
                         Phys/StrippingArchive
                         Phys/StrippingConf
                         Phys/StrippingNeuroBayes
                         Phys/StrippingSelections
                         Phys/StrippingSettings
                         Phys/StrippingTCK
                         Phys/StrippingUtils
                         Phys/TMVASelections)


gaudi_add_test(QMTest QMTEST)
