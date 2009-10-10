#!/usr/bin/env python 
# =============================================================================
## @file  CaudiConf/CaloPackingConf.py
#  Helper module to define DST (un)packing rules for Calo Hypo objects
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date  2009-10-10
# =============================================================================
"""
Helper module to define DST (un)packing rule for Calo Hypo objects
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================
__all__ = (
    'CaloDstPackConf'     ,  ## the configurable, responsible for Dst packing   
    'CaloDstUnPackConf'      ## the configurable, responsible for Dst unpacking   
    )
# =============================================================================
from      Gaudi.Configuration import *
from LHCbKernel.Configuration import *

# =============================================================================
## @class CaloDstPackConf
#  the configurable for packing of Calorimeter objects on Dst 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date  2009-10-10
class CaloDstPackConf ( LHCbConfigurableUser ) :
    """
    Class/Configurable to define Dst-packing rules for Calorimeter objects 
    """
    ##define slots
    __slots__ = {
        'Sequence'    : None      , ## The sequence to be appended
        'Hypos'       : [ 'Photons'      ,
                          'Electrons'    ,
                          'MergedPi0s'   ,
                          'SplitPhotons'
                          ]       , ## The list of hypos to be packed 
        'Enable'      : False     , ## Enable/disable the packing 
        'OutputLevel' : INFO        ## The global output level
       }
    ## documentation lines
    _propetyDocDct = {
        'Sequence'    : """ The sequence to be appended    """ , 
        'Hypos'       : """ the list of hypos to be packed """ , 
        'Enable'      : """ Enable/disable the packing     """ , 
        "OutputLevel" : """ The global output level        """
        }
    
    ## Check the configuration
    def checkConfiguration ( self ) :
        """
        Check the configuration
        """
        if not self.isPropertySet ('Sequence') and self.getProp('Enable') :
            log.error('CaloDstPackConf: Sequence is not set!')
            raise AttributeError, 'CaloDstPackConf: Sequence is not set!'
        
        if not self.getProp('Sequence')        and self.getProp('Enable') : 
            log.error('CaloDstPackConf: Sequence is invalid')
            raise AttributeError, 'CaloDstPackConf: Sequence is invalid'
        
    ## apply the configuration
    def applyConf ( self ) :
        """
        Apply the configuration 
        """
        self.checkConfiguration()
        
        log.info ('CaloDstPackConf: Apply Calo Dst-Packing Configuration ')
        log.info ( self )

        if not self.getProp('Enable') : return
        
        from CaloReco.CaloPacking import caloHypoDstPack
        
        caloHypoDstPack (
            self.getProp ('Sequence'    ) ,
            self.getProp ('Enable'      ) ,
            self.getProp ('Hypos'       ) ,
            self.getProp ('OutputLevel' ) 
            )

# =============================================================================
## @class CaloDstUnPackConf
#  The configurable for unpacking of Calorimeter objects from Dst 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date  2009-10-10
class CaloDstUnPackConf ( LHCbConfigurableUser ) :
    """
    Class/Configurable to define Dst-unpacking rules for Calorimeter objects 
    """
    ##define slots
    __slots__ = {
        'Hypos'       : [ 'Photons'      ,
                          'Electrons'    ,
                          'MergedPi0s'   ,
                          'SplitPhotons'
                          ]       , ## The list of hypos to be packed 
        'Enable'      : False     , ## Enable/disable the packing 
        'OutputLevel' : INFO      , ## The global output level
        }
    ## documentation lines
    _propetyDocDct = {
        'Hypos'       : """ the list of hypos to be packed """  , 
        'Enable'      : """ Enable/disable the packing     """  , 
        "OutputLevel" : """ The global output level        """
        }

   ## Check the configuration
    def checkConfiguration ( self ) :
        """
        Check the configuration
        """
        pass 

    ## apply the configuration
    def applyConf ( self ) :
        """
        Apply the configuration 
        """
        self.checkConfiguration()
        
        log.info ('CaloDstUnPackConf: Apply Calo Dst-UnPacking Configuration ')
        log.info ( self )

        if not self.getProp('Enable') : return

        from CaloReco.CaloPacking import caloHypoDstUnPack
        
        caloHypoDstUnPack (
            self.getProp ('Enable'      ) ,
            self.getProp ('Hypos'       ) ,
            self.getProp ('OutputLevel' ) 
            )

## ============================================================================
if '__main__' == __name__ :
    print __doc__
    print __author__
    print __version__

    
# =============================================================================
# The END 
# =============================================================================
