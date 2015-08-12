################################################################################
# Package: StrippingSys
################################################################################
gaudi_subdir(StrippingSys v9r9p1)

gaudi_depends_on_subdirs(Phys/CommonParticles
                         Phys/CommonParticlesArchive
                         Phys/DisplVertices
                         Phys/IncTopoVert
                         Phys/StandardParticles
                         Phys/StrippingAlgs
                         Phys/StrippingArchive
                         Phys/StrippingConf
                         Phys/StrippingNeuroBayes
                         Phys/StrippingSelections
                         Phys/StrippingSettings
                         Phys/StrippingTCK
                         Phys/StrippingUtils
                         Phys/TMVASelections
                         Phys/IsolationTools)
#                         Phys/StrippingCache

gaudi_add_test(QMTest QMTEST)
