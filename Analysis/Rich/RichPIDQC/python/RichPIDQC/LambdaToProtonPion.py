
## @package RichRecQC
#  RICH PID Calibration and Monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   10/02/2009

__version__ = "$Id: LambdaToProtonPion.py,v 1.3 2009-05-05 15:27:17 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from LHCbKernel.Configuration import *
        
## @class RichRecSysConf
#  Configurable for RICH Lambda -> proton pi PID monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @author Andrew Powell
#  @date   03/05/2009
class LambdaToProtonPionConf(LHCbConfigurableUser) :

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
                                    CombineParticles, FilterDesktop )
        
        if not self.isPropertySet("Sequencer") :
            raise RuntimeError("ERROR : Sequence not set")
        seq = self.getProp("Sequencer")
      
        # Filter Pi Tracks
        pionfilterName = "RichLambdaSelPiFilter"
        pionfilter = FilterDesktop(pionfilterName)
        pionfilter.addTool( PhysDesktop )
        pionfilter.PhysDesktop.InputLocations = [ "StdNoPIDsPions" ]
        pionfilter.Code = "(PT > 0.1*GeV) & (MIPCHI2DV(PRIMARY) > 9) & (TRCHI2DOF < 3) & (ISLONG)"

        # Filter Proton Tracks
        protonfilterName = "RichLambdaSelPrFilter"
        protonfilter = FilterDesktop(protonfilterName)
        protonfilter.addTool( PhysDesktop )
        protonfilter.PhysDesktop.InputLocations = [ "StdNoPIDsProtons" ]
        protonfilter.Code = "(PT > 0.4*GeV) & (MIPCHI2DV(PRIMARY) > 9) & (TRCHI2DOF < 3) & (ISLONG)"

        # Make the Lambda
        lambda2ppiName = "RichLambdaToPrPiSel"
        lambda2ppi = CombineParticles(lambda2ppiName)
        lambda2ppi.addTool(PhysDesktop)
        lambda2ppi.PhysDesktop.InputLocations = [ protonfilterName, pionfilterName ]
        lambda2ppi.DecayDescriptor = "[ Lambda0 -> p+ pi- ]cc"
        lambda2ppi.CombinationCut = "(ADAMASS('Lambda0') < 100*MeV) & (AMAXDOCA('') < 0.2*mm)"
        lambda2ppi.MotherCut = "(ADMASS('Lambda0') < 50.0*MeV) & (PT > 0.5*GeV) & (VFASPF(VCHI2/VDOF) < 6.0) & (MIPDV(PRIMARY) < 0.5) & (BPVVDCHI2 > 750) & (MIPCHI2DV(PRIMARY) < 200) & ( ADWM( 'KS0' , WM( 'pi+' , 'pi-') ) > 15*MeV ) & (LV01 < 0.98) & (LV02 < 0.98) & (LV01 > -0.98) & (LV02 > -0.98)"

        # Add selection algs to the sequence
        seq.Members += [pionfilter,protonfilter,lambda2ppi]

        # Particle Monitoring plots
        from Configurables import ParticleMonitor
        plotter = ParticleMonitor(lambda2ppiName+"Plots")
        plotter.addTool(PhysDesktop)
        plotter.PhysDesktop.InputLocations = [ "Phys/"+lambda2ppiName ]
        plotter.PeakCut     = "(ADMASS('Lambda0')<2*MeV)"
        plotter.SideBandCut = "(ADMASS('Lambda0')>2*MeV)"
        plotter.PlotTools = [ "MassPlotTool","MomentumPlotTool",
                              "CombinedPidPlotTool",
                              "RichPlotTool","CaloPlotTool","MuonPlotTool" ]
        seq.Members += [ plotter ]
        
        # MC Performance checking ?
        if self.getProp("MCChecks") :

            from Configurables import ParticleEffPurMoni
            mcPerf = ParticleEffPurMoni(lambda2ppiName+"MCPerf")
            mcPerf.addTool( PhysDesktop )
            mcPerf.PhysDesktop.InputLocations = ["Phys/"+lambda2ppiName]
            seq.Members += [mcPerf]
                    
        # Ntuple ?
        if self.getProp("MakeNTuple") : pass
        
