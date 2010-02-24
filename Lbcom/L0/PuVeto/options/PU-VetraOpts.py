"""
###############################################################################
#                                                                             #
#  Options to run the PU trigger algo emulator in Vetra	on real data          #
#                                                                             #
#  Example usage:                                                             #
#    gaudirun.py PU-VetraOpts.py 		                              #
#                                                                             #
#  @package Tell1/Vetra                                                       #
#  @author  Serena Oggero                                                     #
#  @date    04/12/2009                                                        #
#                                                                             #
###############################################################################
"""

from Gaudi.Configuration import EventSelector
from Configurables import LHCbApp, GaudiSequencer
from GaudiKernel.Configurable import appendPostConfigAction

from Configurables import Vetra
vetra = Vetra()
vetra.DataBanks = 'NONE' #excludes any other algo from Vetra
vetra.EvtMax = 1000
vetra.HistogramFile = "/PUVetoAlg_vetra63466.root" 
# default settings
LHCbApp().DDDBtag   = 'head-20080905'
LHCbApp().CondDBtag = 'head-20080905'

from Configurables import PuVetoAlg
l0PuVeto = PuVetoAlg() 
l0PuVeto.OutputLevel = 3 
# if in TAE mode...
#l0PuVeto.RawEventLocation = 'Prev1/DAQ/RawEvent'
#l0PuVeto.OutputFileName = "/calib/trg/l0pus/BeamData/PUVetoAlg_l0PuVeto63466.root"
#l0PuVeto.MakePlots = True

moniL0Pu = GaudiSequencer ( 'Moni_L0PileUp' )
moniL0Pu.Members = [l0PuVeto]

def myPU():
  GaudiSequencer( 'MoniVELOSeq' ).Members = [ moniL0Pu ] 
appendPostConfigAction( myPU )

EventSelector().FirstEvent = 1
EventSelector().PrintFreq = 1
EventSelector().Input = [
    "DATAFILE='/daqarea/lhcb/data/2009/RAW/FULL/LHCb/COLLISION09/63466/063466_0000000001.raw' SVC='LHCb::MDFSelector'" 
    ]


###############################################################################
