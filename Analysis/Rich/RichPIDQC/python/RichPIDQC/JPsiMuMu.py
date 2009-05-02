
## @package RichRecQC
#  RICH PID Calibration and Monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   10/02/2009

__version__ = "$Id: JPsiMuMu.py,v 1.2 2009-05-02 08:34:52 ukerzel Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from LHCbKernel.Configuration import *
                
## @class JPsiMuMu
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
                                    CombineParticles, OfflineVertexFitter )
        
        seq = self.getProp("Sequencer")
        if seq == None : raise RuntimeError("ERROR : Sequence not set")

        ## J/psi -> mu mu
        JPsiMuMuName                        = "JPsiMuMu"
        JPsiMuMu                            = CombineParticles(JPsiMuMuName)
        JPsiMuMu.DecayDescriptor            = "J/psi(1S) -> mu+ mu- "
        JPsiMuMu.addTool( PhysDesktop() )
        JPsiMuMu.PhysDesktop.InputLocations = ["Phys/StdLooseMuons"]
        JPsiMuMu.CombinationCut             = "(ADAMASS('J/psi(1S)') < 100*MeV)"
        JPsiMuMu.MotherCut                  = "(ADMASS('J/psi(1S)') < 10*MeV) & (VFASPF(VCHI2/VDOF)<6)"
        JPsiMuMu.DaughtersCuts              = {"mu+"     :    "(PT>1200*MeV) & (TRCHI2DOF<1.6)",
                                               "mu-"     :    "(PT>1200*MeV) & (TRCHI2DOF<1.6) "}
                                  
        # Particle Monitoring plots
        from Configurables import ( ParticleMonitor )
        plotter =  ParticleMonitor(name+"Plots")
        plotter.addTool(PhysDesktop())
        plotter.PhysDesktop.InputLocations = [ "Phys/"+name ]
        plotter.PeakCut     = "(ADMASS('J/psi(1S)')<30*MeV)" # CRJ : Guess
        plotter.SideBandCut = "(ADMASS('J/psi(1S)')>30*MeV)" # CRJ : Guess
        plotter.PlotTools = [ "MassPlotTool","MomentumPlotTool",
                              "CombinedPidPlotTool",
                              "RichPlotTool","CaloPlotTool","MuonPlotTool" ]
        seq.Members += [ plotter ]
        
        # MC Performance checking ?
        if self.getProp("MCChecks") :

            from Configurables import ParticleEffPurMoni
            mcPerf = ParticleEffPurMoni(name+"MCPerf")
            mcPerf.addTool( PhysDesktop() )
            mcPerf.PhysDesktop.InputLocations = ["Phys/"+name]
            seq.Members += [mcPerf]
    
        # Ntuple ?
        if self.getProp("MakeNTuple") : pass