
## @package DstCalibSel
#  RICH PID Calibration and Monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   10/02/2009

__version__ = "$Id: Configuration.py,v 1.1.1.1 2009-02-16 14:11:47 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from LHCbKernel.Configuration import *
        
## @class DstarToD0PiSelConf
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
    def selName(self) : return "Dstar2D0Pi"
    
    ## Configure the selection
    def __apply_configuration__(self) :

        from Configurables import GaudiSequencer

        ###############################################################
        #
        # 1) Define a sequence
        #
        testseq = self.getProp("Sequencer")
        if testseq == None : raise RuntimeError("ERROR : Sequence not set")

        ###############################################################
        #
        # 3) Filter K/Pi Tracks
        #
        from Configurables import FilterDesktop, PhysDesktop
        trackfilter = FilterDesktop("TrackFilter")
        trackfilter.addTool( PhysDesktop() )
        trackfilter.PhysDesktop.InputLocations = [ "StdNoPIDsPions", "StdNoPIDsKaons" ]
        trackfilter.Code = "(PT > 0.3*GeV) & (P > 2*GeV) & (MIPCHI2DV(PRIMARY) > 4)"
        testseq.Members += [ trackfilter ]
        pionfilter = FilterDesktop("PionFilter")
        pionfilter.addTool( PhysDesktop() )
        pionfilter.PhysDesktop.InputLocations = [ "StdNoPIDsPions" ]
        pionfilter.Code = "(MIPCHI2DV(PRIMARY) > 9)"
        testseq.Members += [ pionfilter ]
        ###############################################################
        #
        # 3) Add the D0 -> KPi Algorithm
        #
        from Configurables import D02KPiNoPID
        d02kpi = D02KPiNoPID("D02KPi")
        d02kpi.addTool( PhysDesktop() )
        d02kpi.PhysDesktop.InputLocations = [ "TrackFilter" ]
        from GaudiKernel.SystemOfUnits import MeV
        d02kpi.D0MassWindow = 25.0*MeV
        d02kpi.D0VertexChi2Cut = 10
        d02kpi.D0MinPtCut = 1250.0*MeV
        d02kpi.D0MinIPChi2Cut = 99999
        testseq.Members += [ d02kpi ]
        d02kpi.OutputLevel = 6
        ###############################################################
        #
        # 4) Make the D*
        #
        from Configurables import Dstar2D0Pi
        dstar2d0pi = Dstar2D0Pi(self.selName())
        dstar2d0pi.addTool( PhysDesktop() )
        dstar2d0pi.PhysDesktop.InputLocations = [ "PionFilter", "D02KPi"]
        dstar2d0pi.DstarMassWindow = 20.0*MeV
        dstar2d0pi.DstarVertexChi2Cut = 10
        dstar2d0pi.DstarMinPtCut = 1250.0*MeV
        dstar2d0pi.DstarD0MassDiff = 0.5*MeV
        testseq.Members += [ dstar2d0pi ]
        dstar2d0pi.OutputLevel = 6
        ###############################################################
