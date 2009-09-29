
## @package DstCalibSel
#  RICH PID Calibration and Monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   10/02/2009

__version__ = "$Id: Configuration.py,v 1.6 2009-09-29 14:49:39 nmangiaf Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from LHCbKernel.Configuration import *
        
## @class RichDstarToD0PiSelConf
#  Configurable for RICH D* -> D0(Kpi) pi selection
#  @author Andrew Powell
#  @date   15/08/2008
class RichDstarToD0PiSelConf(LHCbConfigurableUser) :

    ## Possible used Configurables
    __used_configurables__ = [ ]
    
    ## Steering options
    __slots__ = {
        "Sequencer"   : None    # The sequencer to add the calibration algorithms too
        }

    ## Selection name
    def selName(self) : return "RichDstarToD0Pi"
    
    ## Configure the selection
    def __apply_configuration__(self) :

        from Configurables import GaudiSequencer

        ###############################################################
        #
        # 1) Define a sequence
        #
        if not self.isPropertySet("Sequencer") :
            raise RuntimeError("ERROR : Sequence not set")
        testseq = self.getProp("Sequencer")
 
        ###############################################################
        #
        # 3) Filter K/Pi Tracks
        #
        from Configurables import FilterDesktop
        trackfilterName = self.selName() + "TrackFilter"
        trackfilter = FilterDesktop(trackfilterName)
        trackfilter.InputLocations = [ "StdNoPIDsPions", "StdNoPIDsKaons" ]
        trackfilter.Code = "(PT > 0.3*GeV) & (P > 2*GeV) & (MIPCHI2DV(PRIMARY) > 4)"
        testseq.Members += [ trackfilter ]
        ###############################################################
        pionfilterName = self.selName() + "PionFilter"
        pionfilter = FilterDesktop(pionfilterName)
        pionfilter.InputLocations = [ "StdNoPIDsPions" ]
        pionfilter.Code = "(MIPCHI2DV(PRIMARY) > 9)"
        testseq.Members += [ pionfilter ]
        ###############################################################
        #
        # 3) Add the D0 -> KPi Algorithm
        #
        from Configurables import D02KPiNoPID
        d02kpiName = self.selName()+"_D0ToKPiNoPID"
        d02kpi = D02KPiNoPID(d02kpiName)
        d02kpi.InputLocations = [ trackfilterName ]
        from GaudiKernel.SystemOfUnits import MeV
        d02kpi.D0MassWindow = 25.0*MeV
        d02kpi.D0VertexChi2Cut = 10
        d02kpi.D0MinPtCut = 1250.0*MeV
        d02kpi.D0MinIPChi2Cut = 99999
        testseq.Members += [ d02kpi ]
        #d02kpi.OutputLevel = 6
        ###############################################################
        #
        # 4) Make the D*
        #
        from Configurables import Dstar2D0Pi
        dstar2d0pi = Dstar2D0Pi(self.selName())
        dstar2d0pi.InputLocations = [pionfilterName,d02kpiName]
        dstar2d0pi.DstarMassWindow = 20.0*MeV
        dstar2d0pi.DstarVertexChi2Cut = 10
        dstar2d0pi.DstarMinPtCut = 1250.0*MeV
        dstar2d0pi.DstarD0MassDiff = 0.5*MeV
        testseq.Members += [ dstar2d0pi ]
        #dstar2d0pi.OutputLevel = 6
        ###############################################################
