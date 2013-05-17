
## @file
#  Configuration of Hlt1 Lines which accept only 1 PV events
#  @author Marcin Kucharczyk
#  @date 2012-01-18
# =============================================================================
__author__  = "Marcin Kucharczyk Marcin.Kucharczyk@@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.0 $"
# =============================================================================

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import *

class Hlt1HighPtJetsSinglePVLinesConf(HltLinesConfigurableUser):
    __slots__ = {
         'L0Channel'  : 'HighSumETJet'
        }

    def __apply_configuration__(self):
        from HltLine.HltLine import Hlt1Line
        from HltTracking.HltPVs import PV3D
        from Configurables import LoKi__VoidFilter as VoidFilter
        algCheckVtcs = VoidFilter('HighPtJetsSinglePVDecision', Code = "CONTAINS('Hlt/Vertex/PV3D') == 1" )
        channel = self.getProp('L0Channel')
        Hlt1Line('HighPtJetsSinglePV',
                 prescale  = self.prescale,
                 postscale = self.postscale,
                 L0DU = "L0_CHANNEL('%s')" % channel,
                 algos = [PV3D(),algCheckVtcs]
                 )


