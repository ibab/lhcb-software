
## @package RichRecQC
#  RICH PID Calibration and Monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   10/02/2009

__version__ = "$Id: JPsiMuMu.py,v 1.10 2009-05-22 10:54:55 nmangiaf Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from LHCbKernel.Configuration import *
                
## @class JPsiMuMuConf
#  Configurable for RICH J/psi -> mu mu PID monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @author Nicola Mangiafave
#  @date   15/08/2008
class JPsiMuMuConf(LHCbConfigurableUser) :

    ## Possible used Configurables
    __used_configurables__ = [ ]
    
    ## Steering options
    __slots__ = {
        "Sequencer"   : None    # The sequencer to add the calibration algorithms too
        ,"MCChecks"   : False
        ,"MakeNTuple" : False
        }

    ## Configure Jpsi -> Mu Mu selection
    def __apply_configuration__(self) :

        from Configurables import ( GaudiSequencer, PhysDesktop,
                                    CombineParticles )
        
        if not self.isPropertySet("Sequencer") :
            raise RuntimeError("ERROR : Sequence not set")
        seq = self.getProp("Sequencer")
   
        ## J/psi -> mu mu
        JPsiMuMuName                        = "RichJPsiMuMu"
        JPsiMuMu                            = CombineParticles(JPsiMuMuName)
        JPsiMuMu.DecayDescriptor            = "J/psi(1S) -> mu+ mu- "
        JPsiMuMu.addTool( PhysDesktop )
        JPsiMuMu.PhysDesktop.InputLocations = ["Phys/StdLooseMuons"]
        JPsiMuMu.CombinationCut             = "(ADAMASS('J/psi(1S)') < 150*MeV)"
        JPsiMuMu.MotherCut                  = "(ADMASS('J/psi(1S)') < 100*MeV) & (VFASPF(VCHI2/VDOF)<6)"
        JPsiMuMu.DaughtersCuts              = {"mu+"     :    "(PT>1000*MeV) & (P>5*GeV) & (TRCHI2DOF<2.0) & (PPINFO(LHCb.ProtoParticle.MuonMuLL,-10000)>-10) & (TINFO( LHCb.Track.GhostProbability,-1000,-1000)<0.6)",
                                               "mu-"     :    "(PT>1000*MeV) & (P>5*GeV) & (TRCHI2DOF<2.0) & (PPINFO(LHCb.ProtoParticle.MuonMuLL,-10000)>-10) & (TINFO( LHCb.Track.GhostProbability,-1000,-1000)<0.6)"}

        seq.Members += [JPsiMuMu]

        # Particle Monitoring plots
        from Configurables import ( ParticleMonitor )
        plotter =  ParticleMonitor(JPsiMuMuName+"Plots")
        plotter.addTool(PhysDesktop)
        plotter.PhysDesktop.InputLocations = [ "Phys/"+JPsiMuMuName ]
        plotter.PeakCut     = "(ADMASS('J/psi(1S)')<30*MeV)" # CRJ : Guess
        plotter.SideBandCut = "(ADMASS('J/psi(1S)')>30*MeV)" # CRJ : Guess
        plotter.PlotTools = [ "MassPlotTool","MomentumPlotTool",
                              "CombinedPidPlotTool",
                              "RichPlotTool","CaloPlotTool","MuonPlotTool" ]
        seq.Members += [plotter]
        
        # MC Performance checking ?
        if self.getProp("MCChecks") :

            from Configurables import ParticleEffPurMoni
            mcPerf = ParticleEffPurMoni(JPsiMuMuName+"MCPerf")
            mcPerf.addTool( PhysDesktop )
            mcPerf.PhysDesktop.InputLocations = ["Phys/"+JPsiMuMuName]
            seq.Members += [mcPerf]

        # Ntuple ?
            if self.getProp("MakeNTuple") :
                from Configurables import ( DecayTreeTuple,
                            TupleToolDecay,
                            LoKi__Hybrid__FilterCriterion,
                            LoKi__Hybrid__TupleTool,
                            TupleToolMCBackgroundInfo,
                            BackgroundCategory,
                            TupleToolTrigger,
                            TupleToolMCTruth,
                            TupleToolVtxIsoln,
                            TupleToolP2VV
                            )
                
                JPsiMuMuTree = DecayTreeTuple( JPsiMuMuName + 'Tuple')
                JPsiMuMuTree.addTool( PhysDesktop )
                JPsiMuMuTree.PhysDesktop.InputLocations = [ 'Phys/'+JPsiMuMuName]
                JPsiMuMuTree.Decay = 'J/psi(1S) ->  ^mu+ ^mu- '
                
                # set some names for ntuple branchs
                MyBranch_jpsi  = "jpsi"
                MyBranch_mup   = "mup"
                MyBranch_mum   = "mum"
                
                # label the branches for the particle tools
                JPsiMuMuTree.Branches = {
                    MyBranch_jpsi  : "J/psi(1S) : J/psi(1S) ->  mu+ mu- ",
                    MyBranch_mup   : "J/psi(1S) ->  ^mu+ mu- ",
                    MyBranch_mum   : "J/psi(1S) ->  mu+ ^mu- ",
                    }
                
                JPsiMuMuTree.ToolList = [
                    "TupleToolEventInfo"
                    , "TupleToolGeneration"
                    , "TupleToolMCTruth"
                    , "TupleToolMCBackgroundInfo"
                    , "TupleToolMCHierarchy"
                    , "TupleToolPrimaries"
                    , "TupleToolVtxIsoln"      
                    , "TupleToolTrackInfo"
                    , "TupleToolPid"
                    , "TupleToolGeometry"
                    , "TupleToolKinematic"
                    , "TupleToolPropertime"
                    , "TupleToolPid"
                    , "TupleToolPrimaries"
                    , "TupleToolTrigger"
                    ]
                
                JPsiMuMuTree.addTool(BackgroundCategory())
                JPsiMuMuTree.BackgroundCategory.SoftPhotonCut = 2000
                JPsiMuMuTree.OutputLevel = INFO
                JPsiMuMuTree.addTool(TupleToolTrigger())
                JPsiMuMuTree.TupleToolTrigger.VerboseL0 = True
                JPsiMuMuTree.addTool(TupleToolMCTruth())
                JPsiMuMuTree.TupleToolMCTruth.StoreAssociationNumbers = True
                JPsiMuMuTree.TupleToolMCTruth.StoreKineticInfo = True
                JPsiMuMuTree.TupleToolMCTruth.StoreVertexInfo = True
                JPsiMuMuTree.TupleToolMCTruth.FillAngles = True
                JPsiMuMuTree.addTool(TupleToolVtxIsoln( OutputLevel = 6 ))
                JPsiMuMuTree.TupleToolVtxIsoln.IP = 2.0
                JPsiMuMuTree.TupleToolVtxIsoln.InputParticles = [ "Phys/StdLooseMuons"]

                seq.Members += [JPsiMuMuTree]

                JPsiMuMuTree.NTupleLUN = "JPSIMUMU"

                from Configurables import NTupleSvc
                NTupleSvc().Output = ["JPSIMUMU DATAFILE='JpsiMuMu_Presel.root'  TYP='ROOT'  OPT='NEW'"]
                

            
