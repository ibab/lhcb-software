
## @package RichRecQC
#  RICH PID Calibration and Monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   10/02/2009

__version__ = "$Id: DstarToDzeroPi.py,v 1.1 2009-05-03 14:49:49 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from LHCbKernel.Configuration import *
#from Configurables import RichDstarToD0PiSelConf
from DstCalibSel.Configuration import RichDstarToD0PiSelConf
                
## @class DstarToDzeroPiConf
#  Configurable for RICH D* -> D0(Kpi) pi PID monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @author Andrew Powell
#  @date   15/08/2008
class DstarToDzeroPiConf(LHCbConfigurableUser) :

    ## Possible used Configurables
    __used_configurables__ = [ RichDstarToD0PiSelConf ]
    
    ## Steering options
    __slots__ = {
        "Sequencer"   : None    # The sequencer to add the calibration algorithms too
        ,"MCChecks"   : False
        ,"MakeNTuple" : False
        }

    ## Configure Ds -> Phi Pi selection
    def __apply_configuration__(self) :

        if not self.isPropertySet("Sequencer") :
            raise RuntimeError("ERROR : Sequence not set")
        seq = self.getProp("Sequencer")
  
        # The selection algs
        from Configurables import GaudiSequencer
        name = RichDstarToD0PiSelConf().selName()
        selAlgs = GaudiSequencer(name+"SelSeq")
        selAlgs.MeasureTime = True
        RichDstarToD0PiSelConf().setProp("Sequencer",selAlgs)
        seq.Members += [ selAlgs ]

        # Particle Monitoring plots
        from Configurables import ( ParticleMonitor, PhysDesktop )
        plotter =  ParticleMonitor(name+"Plots")
        plotter.addTool(PhysDesktop())
        plotter.PhysDesktop.InputLocations = [ "Phys/"+name ]
        plotter.PeakCut     = "(ADMASS('D_s+')<100*MeV)" # CRJ : Guess
        plotter.SideBandCut = "(ADMASS('D_s+')>100*MeV)" # CRJ : Guess
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
