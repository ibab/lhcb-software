# =============================================================================
## @file
#  Configuration of Hadron Lines
#  @author V. Gligorov for J. Albrecht (vladimir.gligorov@cern.ch) 
#  @date 2009-09-15
# =============================================================================
"""
 script to configure Hadron via T trigger lines, used for commisioning
"""
# =============================================================================
__author__  = "Gerhard Raven Gerhard.Raven@nikhef.nl"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.22 $"
# =============================================================================

from Gaudi.Configuration import * 

from HltLine.HltLinesConfigurableUser import *

class Hlt1HadronViaTLinesConf(HltLinesConfigurableUser) :
    #--------------------------------
    #
    # (V. Gligorov 15/09/09)

    # steering variables
    #------------------------
    #   Single Hadron Line
    #     SingleHadronViaT_PtCut
    #   DiHadron Line
    #     HadViaTMain_IPCut
    #     HadViaTMain_PtCut
    #     HadViaTCompanion_IPCut
    #     HadViaTCompanion_PtCut
    #
    __slots__ = { 'L0Channel'                   : "Hadron" 
                , 'HadViaTSingle_IPCut'         : 0.1
                , 'HadViaTDi_IPCut'             : 0.1
                , 'HadViaT_ETCut'               : 2500.
                , 'HadViaTMain_PTCut'           : 2500.
                , 'VeloTMatchCut'               : 80
                , 'HadViaTCompanion_DOCACut'    : 0.2
                , 'HadViaTCompanion_DZCut'      : 0.
                , 'HadViaTCompanion_PTCut'      : 1000.
                , 'HadViaTCompanion_PointingCut': 0.4
                , 'SingleHadronViaT_PTCut'      : 5000.
                }
    
    def __apply_configuration__(self) : 
        pass
