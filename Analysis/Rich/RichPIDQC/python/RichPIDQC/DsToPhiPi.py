
## @package RichPIDQC
#  RICH PID Calibration and Monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   10/02/2009

__version__ = "$Id: DsToPhiPi.py,v 1.9 2009-09-29 16:38:27 nmangiaf Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *
from Configurables import SelDSTWriter

## @class RichRecSysConf
#  Configurable for RICH Ds -> phi(KK) pi PID monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @author Conor Fitzpatrick  (Conor.Fitzpatrick@cern.ch)
#  @date   15/08/2008
class DsToPhiPiConf(RichConfigurableUser) :

    ## Selection Name
    __sel_name__ = "RichDs2PiPhi"

    ## Possible used Configurables
    __used_configurables__ = [ (SelDSTWriter, __sel_name__+'DST') ]
    
    ## Steering options
    __slots__ = {
         "Context"      : "Offline"  # The context within which to run
        ,"OutputLevel"  : INFO  # The output level to set all algorithms and tools to use
        ,"Sequencer"    : None    # The sequencer to add the calibration algorithms too
        ,"RunSelection" : True
        ,"RunMonitors"  : True
        ,"MCChecks"     : False
        ,"MakeNTuple"   : False
        ,"MakeSelDST"   : False
        ,"DSTPreScaleFraction" : 1.0
        ,"PlotTools"    : [ ]
        ,"Candidates"   : [ ]
        }

    ## Set general job options
    def setOptions(self,alg):
        if self.isPropertySet("OutputLevel") : alg.OutputLevel = self.getProp("OutputLevel")

    ## Configure Ds -> Phi Pi selection
    def __apply_configuration__(self) :

        from Configurables import ( GaudiSequencer,
                                    CombineParticles, OfflineVertexFitter )
        from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand
        
        seq = self.getProp("Sequencer")
        if seq == None : raise RuntimeError("ERROR : Sequence not set")

        if self.getProp("RunSelection") : 

            # STD particles
            from StandardParticles import StdNoPIDsPions, StdNoPIDsKaons
        
            # phi -> K+ K-
            Phi2KKName = self.__sel_name__+"_Phi2KK"
            Phi2KK = CombineParticles(Phi2KKName)
            Phi2KK.DecayDescriptor = "phi(1020) -> K+ K-"
            Phi2KK.CombinationCut = "(ADAMASS('phi(1020)')<75*MeV)"
            Phi2KK.MotherCut = "(ADMASS('phi(1020)')<50*MeV) & (BPVVDCHI2>60) & (MIPDV(PRIMARY)<0.5) & (VFASPF(VCHI2) < 20)"
            Phi2KK.DaughtersCuts = {"K+"     :    "(PT>300*MeV) & (P>2*GeV) & (MIPDV(PRIMARY) < 0.5) &  (BPVIPCHI2() > 20)",
                                    "K-"     :    "(PT>300*MeV) & (P>2*GeV) & (MIPDV(PRIMARY) < 0.5) &  (BPVIPCHI2() > 20)"}
            self.setOptions(Phi2KK)
            Phi2KKSel = Selection( Phi2KKName+'Sel',
                                   Algorithm = Phi2KK,
                                   RequiredSelections = [StdNoPIDsKaons] )
        
            # Bs -> J/psi phi
            Ds2piPhiName = self.__sel_name__
            Ds2piPhi = CombineParticles(Ds2piPhiName)
            Ds2piPhi.DecayDescriptor = "[D_s+ -> pi+ phi(1020)]cc"
            Ds2piPhi.addTool( OfflineVertexFitter )
            Ds2piPhi.ParticleCombiners.update( { "" : "OfflineVertexFitter" } )
            Ds2piPhi.OfflineVertexFitter.useResonanceVertex = True
            Ds2piPhi.CombinationCut = "(ADAMASS('D_s+')<75*MeV)"
            Ds2piPhi.MotherCut = "(ADMASS('D_s+')<50*MeV) & (BPVDIRA>0.9999) & (BPVVDCHI2>85) & (MIPDV(PRIMARY)<0.1) &  (VFASPF(VCHI2) < 10)"
            Ds2piPhi.DaughtersCuts = {"pi+" : "(PT>300*MeV) & (P>2*GeV) & (MIPDV(PRIMARY) >0.1) & (BPVIPCHI2() > 20)"}
            self.setOptions(Ds2piPhi)
            Ds2piPhiSel = Selection( Ds2piPhiName+'Sel',
                                     Algorithm = Ds2piPhi,
                                     RequiredSelections = [Phi2KKSel,StdNoPIDsPions] )

            # Selection Sequence
            selSeq = SelectionSequence( self.__sel_name__+'Seq', TopSelection = Ds2piPhiSel )

            # Run the selection sequence.
            seq.Members += [selSeq.sequence()]

        # Particle Monitoring plots
        if self.getProp("RunMonitors") :
            
            from Configurables import ParticleMonitor
            plotter =  ParticleMonitor(self.__sel_name__+"Plots")
            if self.getProp("RunSelection") : 
                plotter.Inputs = [ 'Phys/'+self.__sel_name__+'Sel' ]
            else:
                plotter.Inputs = self.getProp("Candidates")
            plotter.PeakCut     = "(ADMASS('D_s+')<100*MeV)"
            plotter.SideBandCut = "(ADMASS('D_s+')>100*MeV)"
            plotter.PlotTools   = self.getProp("PlotTools") 
            self.setOptions(plotter)
            seq.Members += [ plotter ]

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
            mcPerf = ParticleEffPurMoni(Ds2piPhiName+"MCPerf")
            mcPerf.Inputs = ['Phys/'+self.__sel_name__+'Sel']
            self.setOptions(mcPerf)
            seq.Members += [mcPerf]
                    
        # Ntuple ?
        if self.getProp("MakeNTuple") :

            outputLevel = INFO

            from Configurables import ( DecayTreeTuple, TupleToolDecay, TupleToolMCTruth,
                                        TupleToolMCBackgroundInfo, TupleToolGeometry,
                                        TupleToolKinematic, TupleToolPrimaries,
                                        TupleToolEventInfo, MCTupleToolHierarchy,
                                        MCTupleToolKinematic,
                                        TupleToolPid, TupleToolTrackInfo,
                                        TupleToolVtxIsoln, LoKi__Hybrid__TupleTool )

            Tuple = DecayTreeTuple(Ds2piPhiName+"Tuple")
            Tuple.Inputs = ["Phys/"+Ds2piPhiName]
            Tuple.Decay = "[D_s+ -> ^pi+ (^phi(1020) => ^K+ ^K-)]cc";
            Tuple.Branches = {
                "pion"        : "[D_s+ -> ^pi+ (phi(1020) => K+ K-)]cc"
                ,"kaonplus"       : "[D_s+ -> pi+ (phi(1020) => ^K+ K-)]cc"
                ,"kaonminus"       : "[D_s+ -> pi+ (phi(1020) => K+ ^K-)]cc"
                ,"phi"        : "[D_s+ -> pi+ (^phi(1020) => K+ K-)]cc"
                ,"D_s"       : "[D_s+]cc :[D_s+ -> pi+ (phi(1020) => K+ K-)]cc"
                }

            Tuple.addTool(TupleToolDecay, name = 'pion')
            Tuple.addTool(TupleToolDecay, name = 'phi')
            Tuple.addTool(TupleToolDecay, name = 'kaonplus')
            Tuple.addTool(TupleToolDecay, name = 'kaonminus')
            Tuple.addTool(TupleToolDecay, name = 'D_s')
            
            # k+ specific
            kaonplusLoKiTool = LoKi__Hybrid__TupleTool( 'kaonplusLoKiTool')
            kaonplusLoKiTool.Variables = {
                
		"LOKI_PIDK" : "PIDK",
		"LOKI_PIDe" : "PIDe",
		"LOKI_PIDpi" : "PIDpi",
		"LOKI_PIDp" : "PIDp",
		"LOKI_PIDmu" : "PIDmu",
                "LOKI_PIDK_PIDpi" : "PIDK-PIDpi",
                "LOKI_PIDK_PIDe" : "PIDK-PIDe",
                "LOKI_PIDK_PIDmu" : "PIDK-PIDmu",
                "LOKI_PIDK_PIDp" : "PIDK-PIDp"
                }
            
            Tuple.kaonplus.addTool(kaonplusLoKiTool, name='kaonplusLoKiTool')
            Tuple.kaonplus.ToolList = [ 'LoKi::Hybrid::TupleTool/kaonplusLoKiTool']
            
            
            # k- specific
            kaonminusLoKiTool = LoKi__Hybrid__TupleTool( 'kaonminusLoKiTool')
            kaonminusLoKiTool.Variables = {
		"LOKI_PIDK" : "PIDK",
		"LOKI_PIDe" : "PIDe",
		"LOKI_PIDpi" : "PIDpi",
		"LOKI_PIDp" : "PIDp",
		"LOKI_PIDmu" : "PIDmu",
                "LOKI_PIDK_PIDpi" : "PIDK-PIDpi",
                "LOKI_PIDK_PIDe" : "PIDK-PIDe",
                "LOKI_PIDK_PIDmu" : "PIDK-PIDmu",
                "LOKI_PIDK_PIDp" : "PIDK-PIDp"
                }

            Tuple.kaonminus.addTool(kaonminusLoKiTool, name='kaonminusLoKiTool')
            Tuple.kaonminus.ToolList = [ 'LoKi::Hybrid::TupleTool/kaonminusLoKiTool']

            # pi+ specific
            pionLoKiTool = LoKi__Hybrid__TupleTool( 'pionLoKiTool')
            pionLoKiTool.Variables = {
		"LOKI_PIDK" : "PIDK",
		"LOKI_PIDe" : "PIDe",
		"LOKI_PIDpi" : "PIDpi",
		"LOKI_PIDp" : "PIDp",
		"LOKI_PIDmu" : "PIDmu",
                "LOKI_PIDpi_PIDK" : "PIDpi-PIDK",
                "LOKI_PIDpi_PIDe" : "PIDpi-PIDe",
                "LOKI_PIDpi_PIDmu" : "PIDpi-PIDmu",
                "LOKI_PIDpi_PIDp" : "PIDpi-PIDp"
                }
            
            Tuple.pion.addTool(pionLoKiTool, name='pionLoKiTool')
            Tuple.pion.ToolList = [ 'LoKi::Hybrid::TupleTool/pionLoKiTool']
            
            # phi specific
            phiLoKiTool =  LoKi__Hybrid__TupleTool( 'phiLoKiTool')
            phiLoKiTool.Variables = {
                }
            
            Tuple.phi.addTool(phiLoKiTool, name='phiLoKiTool')
            Tuple.phi.ToolList = [ 'LoKi::Hybrid::TupleTool/phiLoKiTool'
                                   ]
            
            
            # D_s specific
            DsLoKiTool =  LoKi__Hybrid__TupleTool( 'DsLoKiTool')
            DsLoKiTool.Variables = {
                }
            
            Tuple.D_s.addTool(DsLoKiTool, name='DsLoKiTool')
            Tuple.D_s.ToolList = [
                "LoKi::Hybrid::TupleTool/DsLoKiTool"
                ]
            
            # Common to all particles
            LoKiTool = LoKi__Hybrid__TupleTool( 'LoKiTool')
            LoKiTool.Variables = {
                
                "LOKI_ABSID" : "ABSID",
                "LOKI_BPVIPCHI2" : "BPVIPCHI2()",
                "LOKI_BPVDIRA" :  "BPVDIRA",
                "LOKI_BPVLTFITCHI2" : "BPVLTFITCHI2('PropertimeFitter/properTime:PUBLIC')",
                "LOKI_BPVLTCHI2" : "BPVLTCHI2('PropertimeFitter/properTime:PUBLIC')",
                "LOKI_BPVLTIME" : "BPVLTIME('PropertimeFitter/properTime:PUBLIC')",
                "LOKI_BPVVDCHI2" : "BPVVDCHI2",
                "LOKI_ID" : "ID",
                "LOKI_MIPDV_PRIMARY" : "MIPDV(PRIMARY)",
                "LOKI_MIPCHI2DV_PRIMARY" : "MIPCHI2DV(PRIMARY)",
                "LOKI_MM" : "MM",
                "LOKI_M" : "M",
                "LOKI_P" : "P",
                "LOKI_PT" : "PT",
                "LOKI_TRCHI2" : "TRCHI2",
                "LOKI_TRCHI2DOF" : "TRCHI2DOF",
                "LOKI_VFASPF_VCHI2" : "VFASPF(VCHI2)",
                "LOKI_VFASPF_VDOF": "VFASPF(VDOF)" 
                }
            
            Tuple.addTool(LoKiTool, name = 'LoKiTool')
            Tuple.ToolList = [
                "LoKi::Hybrid::TupleTool/LoKiTool"
                ,"TupleToolEventInfo"
                ,"TupleToolGeometry"
                ,"TupleToolKinematic"
                ,"TupleToolMCBackgroundInfo"
                ,"TupleToolPid"
                ,"TupleToolPrimaries"
                ,"TupleToolTrackInfo"
                ,"TupleToolVtxIsoln"
                ,"TupleToolMCTruth"
                ]
            
            Tuple.addTool(TupleToolEventInfo)
            Tuple.TupleToolEventInfo.OutputLevel = outputLevel
            
            Tuple.addTool(TupleToolGeometry)
            Tuple.TupleToolGeometry.OutputLevel = outputLevel
            
            Tuple.addTool(TupleToolKinematic)
            Tuple.TupleToolKinematic.OutputLevel = outputLevel
            
            Tuple.addTool(TupleToolMCBackgroundInfo)
            Tuple.TupleToolMCBackgroundInfo.OutputLevel = outputLevel
            
            Tuple.addTool(MCTupleToolHierarchy)
            Tuple.MCTupleToolHierarchy.OutputLevel = outputLevel
            
            Tuple.addTool(TupleToolMCTruth)
            Tuple.TupleToolMCTruth.OutputLevel = outputLevel
            Tuple.TupleToolMCTruth.addTool(MCTupleToolKinematic())
            Tuple.TupleToolMCTruth.addTool(MCTupleToolHierarchy())
            Tuple.TupleToolMCTruth.ToolList = [
                                               "MCTupleToolKinematic" , 
                                               "MCTupleToolHierarchy" 
                                              ]
            Tuple.TupleToolMCTruth.MCTupleToolKinematic.StoreKineticInfo = True
            Tuple.TupleToolMCTruth.MCTupleToolKinematic.StoreVertexInfo = True
            Tuple.TupleToolMCTruth.MCTupleToolKinematic.StorePropertimeInfo = True
            Tuple.TupleToolMCTruth.MCTupleToolKinematic.OutputLevel = outputLevel
            
            Tuple.addTool(TupleToolPid)
            Tuple.TupleToolPid.OutputLevel = outputLevel
            
            Tuple.addTool(TupleToolPrimaries)
            Tuple.TupleToolPrimaries.OutputLevel = outputLevel
            
            Tuple.addTool(TupleToolTrackInfo)
            Tuple.TupleToolTrackInfo.OutputLevel = outputLevel
            
            Tuple.addTool(TupleToolVtxIsoln)
            Tuple.TupleToolVtxIsoln.OutputLevel = outputLevel

            seq.Members += [Tuple]

            Tuple.NTupleLUN = "DSPHIPI"

            from Configurables import NTupleSvc
            NTupleSvc().Output = ["DSPHIPI DATAFILE='DsToPhiPi.root'  TYP='ROOT'  OPT='NEW'"]

