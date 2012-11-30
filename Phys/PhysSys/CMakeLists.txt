################################################################################
# Package: PhysSys
################################################################################
gaudi_subdir(PhysSys v16r3)

gaudi_depends_on_subdirs(Kernel/SelectionLine
                         Phys/BBDecTreeTool
                         Phys/DaVinciInterfaces
                         Phys/DaVinciKernel
                         Phys/DaVinciMCKernel
                         Phys/DaVinciTools
                         Phys/DaVinciTransporter
                         Phys/DaVinciTypes
                         Phys/DaVinciUtils
                         Phys/DecayTreeFitter
                         Phys/HighPtJets
                         Phys/KalmanFilter
                         Phys/LoKi
                         Phys/LoKiAlgo
                         Phys/LoKiArrayFunctors
                         Phys/LoKiFitters
                         Phys/LoKiJets
                         Phys/LoKiPhys
                         Phys/LoKiProtoParticles
                         Phys/LoKiTracks
                         Phys/LoKiUtils
                         Phys/NeuroBayesTools
                         Phys/ParticleMaker
                         Phys/PhysConf
                         Phys/PhysDict
                         Phys/ProtoParticleFilter
                         Phys/SelPy
                         Phys/Swimming
                         Phys/TisTosTobbing
                         Phys/VertexFit
                         PhysSel/PhysSelPython)

