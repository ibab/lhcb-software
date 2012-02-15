
## @file
#  Configuration of Hlt Lines which accept only 1 PV events
#  @author Marcin Kucharczyk
#  @date 2012-01-18
# =============================================================================
__author__  = "Marcin Kucharczyk Marcin.Kucharczyk@@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.0 $"
# =============================================================================

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import *

class Hlt1SinglePVLinesConf(HltLinesConfigurableUser):
    __slots__ = {
         'L0Channel'  : 'HighSumETJet'
        }

    def __apply_configuration__(self):
        from HltLine.HltLine import Hlt1Line
        from Configurables import PatPV3D
        myPV3D = PatPV3D('SinglePVAlg')
        myPV3D.OutputVerticesName = "SinglePVCont"
        from Configurables import LoKi__VoidFilter as VoidFilter
        algCheckVtcs = VoidFilter('SinglePVCont', Code = "CONTAINS('SinglePVCont') == 1" )
        channel = self.getProp('L0Channel')
        Hlt1Line('SinglePV',
                 prescale  = self.prescale,
                 postscale = self.postscale,
                 L0DU = "L0_CHANNEL('%s')" % channel,
                 algos = [myPV3D,algCheckVtcs]
                 )


