#!/usr/bin/env python
# =============================================================================
## @file
#  Configuration file to run Calo-MC-associations on-demand
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-08-08
# =============================================================================
"""

Configuration file to run Calorimeter Monte Carlo Truth Associations on-demand

"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $"
# =============================================================================
# nothing to be imported 
__all__     = (
    'caloMCTruth', 
    ) 
# =============================================================================
from Gaudi.Configuration   import *

from Configurables         import ( CaloDigit2MCLinks2Table ,
                                    CaloClusterMCTruth      )

from CaloKernel.ConfUtils  import ( onDemand       ,
                                    getAlgo        ) 

import logging
_log = logging.getLogger ( 'CaloAssociators' )

# =============================================================================
## define configuration 
def caloMCTruth ( context           = 'Offline' ,
                  enableMCOnDemand  = True      ,
                  Digits            = [ 'Ecal'] ) :
    """
    Define the configuration for on-demand association for Calo objects 
    """

    from CaloDAQ.CaloDigits import caloDigits

    ## configure 'digits'
    caloDigits( 'Offline'        , 
                enableMCOnDemand ,
                False,
                Digits)

    from Configurables import GaudiSequencer 
    main = getAlgo (
        GaudiSequencer ,
        'CaloMCTruth'  ,
        context        ,
        ''             ,
        False          ,
        )
    main.Members = []
    
    mc2digits = getAlgo (
        GaudiSequencer               ,
        "CaloDigitMCTruth"           ,
        'Offline'                    , 
        'Relations/Raw/Calo/Digits'  ,
        enableMCOnDemand             , 
        )
    mc2digits.Members = []

    from Configurables import CaloDigit2MCLinks2Table
    links2table = getAlgo (
        CaloDigit2MCLinks2Table      ,
        'CaloDigitMCLinks2Table'     , 
        'Offline'                    ,
        'Relations/Raw/Calo/Digits'  ,
        False 
        )
    links2table.Inputs = [] 
    
    for dig in Digits :
        
        location = 'Raw/' + dig + '/Digits'
        algName  = dig + 'ZSup' 
        if  dig in ( 'Spd' , 'Prs' ) :
            algName = dig + 'FromRaw'
        alg      = getConfigurable( algName )

        mc2digits   .Members += [ alg      ]
        links2table .Inputs  += [ location ] 

    mc2digits.Members += [ links2table ]

    main.Members += [ mc2digits ]
    
    from Configurables import CaloClusterMCTruth
    
    clusters = getAlgo ( CaloClusterMCTruth            ,   
                         'CaloClusterMCTruth'          ,
                         context                       , 
                         'Relations/Rec/Calo/Clusters' ,
                         enableMCOnDemand              )

    main.Members += [ clusters ]

    return main

# =============================================================================
if '__main__' == __name__ :
    
    print __doc__
    print __author__
    print __version__
    print printOnDemand() 
    
# =============================================================================
# The END
# =============================================================================
