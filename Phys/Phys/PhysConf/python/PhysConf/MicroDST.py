#!/usr/bin/env python
# =============================================================================
# @file Bender/MicroDST.py
#
# Helper module to deal with uDST, mainly TisTos'sing for uDst 
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>``Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis''</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  ``C++ ToolKit for Smart and Friendly Physics Analysis''
#
#  By usage of this code one clearly states the disagreement 
#  with the smear campaign of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
# @author VanyaBELYAEV Ivan.Belyaev@nikhef.nl
# @date   2011-10-05
#
#                    $Revision: 129470 $
#  Last modification $Date: 2011-09-26 12:40:16 +0200 (Mon, 26 Sep 2011) $
#                 by $Author: ibelyaev $ 
# =============================================================================
"""

Helper module to deal with uDST, mainly TisTos'sing for uDst 

This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-10-05"
__version__ = "$Revision: 129470 $"
__all__     = ( 'trunkConf' , )
# =============================================================================

# =============================================================================
## configure variopsi uDst settings from RootInTES 
def uDstConf ( rootInTes ) :
    """
    Various settings for mDst
    
    """
    #
    ## pick up all goodies from Juan
    #
    import MicroDSTConf.TriggerConfUtils as TCU

    #
    ## 'rootify'
    #
    if 0 != rootInTes.find ( '/Event/') : rootInTes = '/Event/' + rootInTes 

    rootInTes = TCU.fixTrunk ( rootInTes )

    #
    ## configure L0,Hlt,e tc using Juan's functions:
    #
    TCU.configureL0AndHltDecoding ( rootInTes )

    from Configurables import DataOnDemandSvc
    dod = DataOnDemandSvc( Dump = True )
    #
    ## extra configuration:
    #   it is more relevant for Stripping-17 data,
    #   however it is backard compatible with other uDsts 
    #

    #
    ## 0. Kill Event/DAQ
    #
    def killDAQ ()  : 
        from Configurables import EventNodeKiller
        killer = EventNodeKiller ( "KillDAQ" )
        killer.Nodes += [ '/Event/DAQ' ]
        try : 
            from Gaudi.Configuration import getConfigurable
            conf   = getConfigurable ( 'DaVinciEventInitSeq' ) 
            conf.Members.insert ( 0 , killer ) 
        except:
            print "Failed to add killer agent for", killer.Nodes
            
    from Gaudi.Configuration import appendPostConfigAction
    appendPostConfigAction ( killDAQ )
 
    #
    ## 1. Copy/Link ODIN, RawEvent and RecSummary
    #
    from Configurables import Gaudi__DataLink as Link
    rawEvt  = Link ( 'LinkRawEvent'                      ,
                     What   =  '/Event/Trigger/RawEvent' ,
                     Target = rootInTes + 'DAQ/RawEvent' )
    odin    = Link ( 'LinkODIN'                          ,
                     What   =          '/Event/DAQ/ODIN' ,
                     Target = rootInTes +     'DAQ/ODIN' )
    summary = Link ( 'LinkSummary'                       ,
                     What   =      '/Event/Rec/Summary'  ,
                     Target = rootInTes + 'Rec/Summary'  )    
    header  = Link ( 'LinkHeader'                        ,
                     What   =       '/Event/Rec/Header'  ,
                     Target = rootInTes +  'Rec/Header'  )
    
    dod.AlgMap [ rawEvt  . Target ] = rawEvt 
    dod.AlgMap [ odin    . Target ] = odin
    dod.AlgMap [ summary . Target ] = summary
    dod.AlgMap [ header  . Target ] = header

    #
    ##
    #
    from Configurables import OdinTimeDecoder, ODINDecodeTool
    odinDec = OdinTimeDecoder ( )
    odinDec.addTool( ODINDecodeTool, 'ODINDecodeTool' )
    odinDec.ODINDecodeTool.RawEventLocations = [
        rootInTes + 'DAQ/RawEvent' ] + odinDec.ODINDecodeTool.RawEventLocations
    
    from Configurables import DecodePileUpData
    pileupDec = DecodePileUpData()
    pileupDec.RawEventLocation = rootInTes + 'DAQ/RawEvent' 
    
    
# =============================================================================
if __name__ == '__main__' :
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120


# =============================================================================
# The END 
# =============================================================================
