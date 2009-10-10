#!/usr/bin/env
# =============================================================================
## @file  CaloReco/CaloPacking.py
#  Helper module to define DST packing/unpacking rules for Calo Hypo objects
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date  2009-10-10
# =============================================================================
"""
Helper module to define DST packing/unpakcing rules for Calo Hypo objects
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================
__all__ = (
    'caloHypoDstPack'     ,  ## the function whcoh defines Dst-packing rules 
    'caloHypoDstUnPack'   ,  ## the function whcih defines Dst-unpacking rules   
    )
# =============================================================================
from Gaudi.Configuration import *

# ==============================================================================
## define Dst packing rules 
def caloHypoDstPack (
    sequence                      , 
    enable                        ,
    hypos    = [ 'Electrons'    ,
                 'Photons'      ,
                 'MergedPi0s'   ,
                 'SplitPhotons' ] ,
    level    = INFO  
    ) :
    """
    Define the Dst-packing rules

    """
    if not enable :
        log.debug('CaloHypoDstPack: the packing is disabled')
        return None
    
    from Configurables import PackCaloHypo
    
    for hypo in hypos :
        _name   = 'Pack'       + hypo
        _input  =  'Rec/Calo/' + hypo  
        _output = 'pRec/Calo/' + hypo
        _alg    = PackCaloHypo(
            name        = _name   ,
            InputName   = _input  ,
            OutputName  = _output ,
            OutputLevel = level   )
        sequence.Members .append ( _alg )
        log.debug ('CaloHypoDstPack: add %s ' % _alg.getFullName() )

# ==============================================================================
## define Dst-unpacking rules 
def caloHypoDstUnPack (
    enable                        ,
    hypos    = [ 'Electrons'    ,
                 'Photons'      ,
                 'MergedPi0s'   ,
                 'SplitPhotons' ] ,
    level    = INFO  
    ) :
    """
    Define Dst-unpacking rules 
    """

    if not enable :
        log.debug('CaloHypoDstUnPack: the unpacking is disabled')
        return None
    
    from Configurables        import UnpackCaloHypo
    from CaloKernel.ConfUtils import onDemand

    for hypo in hypos :
        _name    = 'Unpack'     + hypo 
        _input   = 'pRec/Calo/' + hypo
        _output  =  'Rec/Calo/' + hypo  
        _alg    = UnpackCaloHypo(
            name        = _name   ,
            InputName   = _input  ,
            OutputName  = _output ,
            OutputLevel = level   )
        onDemand ( _alg.OutputName , _alg ) 
        log.debug ('CaloHypoDstUnPack: add %s ' % _alg.getFullName() )

# =============================================================================
if '__main__' == __name__ :
    print __doc__
    print __author__
    print __version__
    
# =============================================================================
# The END 
# =============================================================================
