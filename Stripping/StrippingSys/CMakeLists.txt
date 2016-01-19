################################################################################
# Package: StrippingSys
################################################################################
gaudi_subdir(StrippingSys v9r12p1)

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
                         Phys/IsolationTools
                         ########################################
                         # Patched packages just for this version
                         Muon/MuonID
                    
)

gaudi_add_test(QMTest QMTEST)
