
## @package RichPIDQC
#  RICH PID Calibration and Monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   10/02/2009

__version__ = "$Id: JPsiMuMu.py,v 1.14 2009-10-02 11:54:18 pkoppenb Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *
from Configurables import SelDSTWriter

## @class JPsiMuMuConf
#  Configurable for RICH J/psi -> mu mu PID monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @author Nicola Mangiafave
#  @date   15/08/2008
class JPsiMuMuConf(RichConfigurableUser) :

    ## Selection Name
    __sel_name__ = "RichJPsiMuMu"

    ## Possible used Configurables
    __used_configurables__ = [ (SelDSTWriter, __sel_name__+'DST') ]

    ## Steering options
    __slots__ = {
         "Context"         : "Offline"  # The context within which to run
        ,"OutputLevel"     : INFO  # The output level to set all algorithms and tools to use
        ,"Sequencer"    : None      # The sequencer to add the calibration algorithms too
        ,"RunSelection" : True
        ,"RunMonitors"  : False
        ,"MCChecks"   : False
        ,"MakeNTuple" : False
        ,"MakeSelDST" : False
        ,"DSTPreScaleFraction" : 1.0
        ,"PlotTools"  : [ ]
        ,"Candidates" : [ ]
        }

    ## Set general job options
    def setOptions(self,alg):
        if self.isPropertySet("OutputLevel") : alg.OutputLevel = self.getProp("OutputLevel")

    ## Configure Jpsi -> Mu Mu selection
    def __apply_configuration__(self) :

        from Configurables import ( GaudiSequencer, CombineParticles )
        from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

        if not self.isPropertySet("Sequencer") :
            raise RuntimeError("ERROR : Sequence not set")
        seq = self.getProp("Sequencer")

        if self.getProp("RunSelection") :

            # STD particles
            from StandardParticles import StdLooseMuons

            # J/psi -> mu mu
            JPsiMuMuName               = self.__sel_name__
            JPsiMuMu                   = CombineParticles(JPsiMuMuName)
            JPsiMuMu.DecayDescriptor   = "J/psi(1S) -> mu+ mu- "
            JPsiMuMu.CombinationCut    = "(ADAMASS('J/psi(1S)') < 150*MeV)"
            JPsiMuMu.MotherCut         = "(ADMASS('J/psi(1S)') < 130*MeV) & (VFASPF(VCHI2/VDOF)<6) & (PT > 2500*MeV)"
            JPsiMuMu.DaughtersCuts     = {"mu+" : "(PT>1400*MeV)"\
                                          "& (P>5*GeV)"\
                                          "& (TRCHI2DOF<2.0)"\
                                          "& (PPINFO(LHCb.ProtoParticle.MuonBkgLL,-10000)<-2.5)"\
                                          "& (PPINFO(LHCb.ProtoParticle.MuonMuLL,-10000)>-10)"}
            self.setOptions(JPsiMuMu)
            JPsiMuMuSel = Selection( JPsiMuMuName+'Sel',
                                     Algorithm = JPsiMuMu,
                                     RequiredSelections = [StdLooseMuons] )

            # Selection Sequence
            selSeq = SelectionSequence( self.__sel_name__+'Seq', TopSelection = JPsiMuMuSel )

            # Run the selection sequence.
            seq.Members += [selSeq.sequence()]

        # Particle Monitoring plots
        if self.getProp("RunMonitors") :

            from Configurables import ( ParticleMonitor )
            plotter =  ParticleMonitor(self.__sel_name__+"Plots")
            if self.getProp("RunSelection") : 
                plotter.Inputs      = [ 'Phys/'+self.__sel_name__+'Sel' ]
            else:
                plotter.Inputs = self.getProp("Candidates")
            plotter.PeakCut     = "(ADMASS('J/psi(1S)')<20*MeV)" # Considering sigma = 13
            plotter.SideBandCut = "(ADMASS('J/psi(1S)')>20*MeV)" # Considering sigma = 13
            plotter.PlotTools   = self.getProp("PlotTools") 
            self.setOptions(plotter)
            seq.Members += [plotter]

        # Make a DST ?
        if self.getProp("MakeSelDST"):

            # Prescale
            from Configurables import DeterministicPrescaler
            scaler = DeterministicPrescaler( self.__sel_name__+'PreScale',
                                             AcceptFraction = self.getProp("DSTPreScaleFraction") )
            seq.Members += [scaler]
            # Write the DST
            MyDSTWriter = SelDSTWriter( self.__sel_name__+"DST",
                                        SelectionSequences = [ selSeq ],
                                        OutputPrefix = self.__sel_name__ )
            seq.Members += [MyDSTWriter.sequence()]

        # MC Performance checking ?
        if self.getProp("MCChecks") :

            from Configurables import ParticleEffPurMoni
            mcPerf = ParticleEffPurMoni(JPsiMuMuName+"MCPerf")
            mcPerf.Inputs = ['Phys/'+self.__sel_name__+'Sel']
            self.setOptions(mcPerf)
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
                                        MCTupleToolKinematic,
                                        MCTupleToolHierarchy,
                                        TupleToolVtxIsoln,
                                        TupleToolP2VV
                                        )
            
            JPsiMuMuTree = DecayTreeTuple( JPsiMuMuName + 'Tuple')
            JPsiMuMuTree.Inputs = [ 'Phys/'+JPsiMuMuName]
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
                , "MCTupleToolKinematic"
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
            JPsiMuMuTree.TupleToolMCTruth.addTool(MCTupleToolKinematic())
            JPsiMuMuTree.TupleToolMCTruth.addTool(MCTupleToolHierarchy())
            JPsiMuMuTree.TupleToolMCTruth.ToolList = [
                "MCTupleToolKinematic" ,
                "MCTupleToolHierarchy"
                ]
            JPsiMuMuTree.TupleToolMCTruth.MCTupleToolKinematic.StoreKineticInfo = True
            JPsiMuMuTree.TupleToolMCTruth.MCTupleToolKinematic.StoreVertexInfo = True
            JPsiMuMuTree.TupleToolMCTruth.MCTupleToolKinematic.StorePropertimeInfo = False
            JPsiMuMuTree.addTool(TupleToolVtxIsoln( OutputLevel = 6 ))
            JPsiMuMuTree.TupleToolVtxIsoln.IP = 2.0
            JPsiMuMuTree.TupleToolVtxIsoln.InputParticles = [ "Phys/StdLooseMuons"]
            
            seq.Members += [JPsiMuMuTree]
            
            JPsiMuMuTree.NTupleLUN = "JPSIMUMU"
            
            from Configurables import NTupleSvc
            NTupleSvc().Output = ["JPSIMUMU DATAFILE='JpsiMuMu_Presel.root'  TYP='ROOT'  OPT='NEW'"]



