
## @package RichRecQC
#  RICH PID Calibration and Monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   10/02/2009

__version__ = "$Id: DsToPhiPi.py,v 1.6 2009-05-03 18:23:56 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from LHCbKernel.Configuration import *
        
## @class RichRecSysConf
#  Configurable for RICH Ds -> phi(KK) pi PID monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @author Conor Fitzpatrick  (Conor.Fitzpatrick@cern.ch)
#  @date   15/08/2008
class DsToPhiPiConf(LHCbConfigurableUser) :

    ## Possible used Configurables
    __used_configurables__ = [ ]
    
    ## Steering options
    __slots__ = {
        "Sequencer"   : None    # The sequencer to add the calibration algorithms too
        ,"MCChecks"   : False
        ,"MakeNTuple" : False
        }

    ## Configure Ds -> Phi Pi selection
    def __apply_configuration__(self) :

        from Configurables import ( GaudiSequencer, PhysDesktop,
                                    CombineParticles, OfflineVertexFitter )

        seq = self.getProp("Sequencer")
        if seq == None : raise RuntimeError("ERROR : Sequence not set")
          
        ## phi -> K+ K-
        Phi2KKName = "Phi2KK"
        Phi2KK = CombineParticles(Phi2KKName)
        Phi2KK.DecayDescriptor = "phi(1020) -> K+ K-"
        Phi2KK.addTool( PhysDesktop )
        Phi2KK.PhysDesktop.InputLocations = ["Phys/StdNoPIDsKaons"]
        Phi2KK.CombinationCut = "(ADAMASS('phi(1020)')<75*MeV)"
        Phi2KK.MotherCut = "(ADMASS('phi(1020)')<50*MeV) & (BPVVDCHI2>60) & (MIPDV(PRIMARY)<0.5) & (VFASPF(VCHI2) < 20)"
        Phi2KK.DaughtersCuts = {"K+"     :    "(PT>300*MeV) & (P>2*GeV) & (MIPDV(PRIMARY) < 0.5) &  (BPVIPCHI2() > 20)",
                                "K-"     :    "(PT>300*MeV) & (P>2*GeV) & (MIPDV(PRIMARY) < 0.5) &  (BPVIPCHI2() > 20)"}
        
        ## Bs -> J/psi phi
        Ds2piPhiName = "Ds2PiPhi"
        Ds2piPhi = CombineParticles(Ds2piPhiName)
        Ds2piPhi.DecayDescriptor = "[D_s+ -> pi+ phi(1020)]cc"
        Ds2piPhi.addTool( PhysDesktop )
        Ds2piPhi.addTool( OfflineVertexFitter() )
        Ds2piPhi.VertexFitters.update( { "" : "OfflineVertexFitter"} )
        Ds2piPhi.OfflineVertexFitter.useResonanceVertex = True
        Ds2piPhi.PhysDesktop.InputLocations = ["Phys/"+Phi2KKName,"Phys/StdNoPIDsPions"]
        Ds2piPhi.CombinationCut = "(ADAMASS('D_s+')<75*MeV)"
        Ds2piPhi.MotherCut = "(ADMASS('D_s+')<50*MeV) & (BPVDIRA>0.9999) & (BPVVDCHI2>85) & (MIPDV(PRIMARY)<0.1) &  (VFASPF(VCHI2) < 10)"
        Ds2piPhi.DaughtersCuts = {"pi+"        :       "(PT>300*MeV) & (P>2*GeV) & (MIPDV(PRIMARY) >0.1) & (BPVIPCHI2() > 20)"}

        # Add selection algs to the sequence
        seq.Members += [Phi2KK,Ds2piPhi]

        # Particle Monitoring plots
        from Configurables import ParticleMonitor
        plotter =  ParticleMonitor(Ds2piPhiName+"Plots")
        plotter.addTool(PhysDesktop)
        plotter.PhysDesktop.InputLocations = [ "Phys/"+Ds2piPhiName ]
        plotter.PeakCut     = "(ADMASS('D_s+')<100*MeV)"
        plotter.SideBandCut = "(ADMASS('D_s+')>100*MeV)"
        plotter.PlotTools = [ "MassPlotTool","MomentumPlotTool",
                              "CombinedPidPlotTool",
                              "RichPlotTool","CaloPlotTool","MuonPlotTool" ]
        seq.Members += [ plotter ]
        
        # MC Performance checking ?
        if self.getProp("MCChecks") :

            from Configurables import ParticleEffPurMoni

            mcPerf = ParticleEffPurMoni(Ds2piPhiName+"MCPerf")
            mcPerf.addTool( PhysDesktop )
            mcPerf.PhysDesktop.InputLocations = ["Phys/"+Ds2piPhiName]
            seq.Members += [mcPerf]
                    
        # Ntuple ?
        if self.getProp("MakeNTuple") :

            outputLevel = INFO

            from Configurables import ( DecayTreeTuple, TupleToolDecay, TupleToolMCTruth,
                                        TupleToolMCBackgroundInfo, TupleToolGeometry,
                                        TupleToolKinematic, TupleToolPrimaries,
                                        TupleToolEventInfo, TupleToolMCHierarchy,
                                        TupleToolPid, TupleToolTrackInfo,
                                        TupleToolVtxIsoln, LoKi__Hybrid__TupleTool )

            Tuple = DecayTreeTuple(Ds2piPhiName+"Tuple")
            Tuple.addTool( PhysDesktop )
            Tuple.PhysDesktop.InputLocations = ["Phys/"+Ds2piPhiName]
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
                ,"TupleToolMCHierarchy"
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
            
            Tuple.addTool(TupleToolMCHierarchy)
            Tuple.TupleToolMCHierarchy.OutputLevel = outputLevel
            
            Tuple.addTool(TupleToolMCTruth)
            Tuple.TupleToolMCTruth.FillAngles = False
            Tuple.TupleToolMCTruth.StoreKineticInfo = True
            Tuple.TupleToolMCTruth.StoreVertexInfo = True
            Tuple.TupleToolMCTruth.StorePropertimeInfo = True
            Tuple.TupleToolMCTruth.OutputLevel = outputLevel
            
            Tuple.addTool(TupleToolPid)
            Tuple.TupleToolPid.OutputLevel = outputLevel
            
            Tuple.addTool(TupleToolPrimaries)
            Tuple.TupleToolPrimaries.OutputLevel = outputLevel
            
            Tuple.addTool(TupleToolTrackInfo)
            Tuple.TupleToolTrackInfo.OutputLevel = outputLevel
            
            Tuple.addTool(TupleToolVtxIsoln)
            Tuple.TupleToolVtxIsoln.OutputLevel = outputLevel

            seq.Members += [Tuple]
