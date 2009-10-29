#!/usr/bin/env python
# =============================================================================
# @file CaloDAQ/CaloDigits.py
#  The configuration for CaloDigits  
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-26
# =============================================================================
"""
The configuration for CaloDigits  
"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@nikhef.nl'
__version__ = 'CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $'
# =============================================================================
from Gaudi.Configuration import *

from CaloKernel.ConfUtils import getAlgo


import logging
_log = logging.getLogger('CaloDAQ')

# =============================================================================
## the digits 
def caloDigits ( context        ,
                 enableOnDemand ,
                 createADCs     ) :
    """
    Decoding of Calo-Digits
    """
    _cntx = 'Offline'
    if context != _cntx :
        _log.warning('CaloDigit: Context is redefined to be Offline')
        
    from Configurables import ( CaloZSupAlg       ,
                                CaloDigitsFromRaw ,
                                GaudiSequencer    ) 
    
    spd  = getAlgo ( CaloDigitsFromRaw       , 
                     "SpdFromRaw"            ,
                     _cntx                   ,
                     "Raw/Spd/Digits"        ,
                     enableOnDemand          )
    
    
    prs  = getAlgo ( CaloDigitsFromRaw       , 
                     "PrsFromRaw"            ,
                     _cntx                   ,
                     "Raw/Prs/Digits"        ,
                     enableOnDemand          )
    
    ecal = getAlgo ( CaloZSupAlg             , 
                     "EcalZSup"              ,
                     _cntx                   ,
                     "Raw/Ecal/Digits"       ,
                     enableOnDemand          )
    
    hcal = getAlgo ( CaloZSupAlg             , 
                     "HcalZSup"              ,
                     _cntx                   ,
                     "Raw/Hcal/Digits"       ,
                     enableOnDemand          )
    
    if createADCs :
        for a in ( spd , prs , ecal , hcal ) :
            t = a.OutputType.upper()
            if   t in [ 'DIGIT', 'DIGITS' , 'CALODIGIT' , 'CALODIGITS' ] :
                t.OutputType = 'Both'
                _log.warning( 'Change OutputType for %s from %s to %'
                              % ( t.getFullName() , t , t.OutputType ) )
            elif t in [ 'ADC'  , 'ADCS'   , 'CALOADC'   , 'CALOADCS'   ] :
                pass
            elif t is 'BOTH' :
                pass
            else :
                t.OutputType = 'Adc'
                _log.warning( 'Change OutputType for %s from %s to %'
                              % ( t.getFullName() , t , t.OutputType ) )
            
            
    ## combine them into sequence  
    alg = GaudiSequencer (
        'CaloDigits'      ,
        Context = context ,
        Members = [ spd  ,
                    prs  ,
                    ecal ,
                    hcal ] ,
        IgnoreFilterPassed = True  
        )
    
    return alg 
    
    
# =============================================================================
if '__main__' == __name__ :
    print __doc__
    print __author__
    print __version__
    

# =============================================================================
# The END 
# =============================================================================
