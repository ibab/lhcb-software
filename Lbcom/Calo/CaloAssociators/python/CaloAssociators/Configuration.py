#!/usr/bin/env gaudirun.py
# =============================================================================
## Confurable for Calorimeter MC-truth associations 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhe.nl
#  @date 2008-07-17
# =============================================================================
"""
Confiurable for Calorimeter MC-truth associations 
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================
__all__ = (
    'CaloAssociatorsConf', 
    )
# =============================================================================
from LHCbKernel.Configuration import *

from Configurables            import CaloDigitConf

from CaloKernel.ConfUtils     import ( printOnDemand  ,
                                       setTheProperty )

import logging
_log = logging.getLogger ('CaloAssociators')

# =============================================================================
## @class CaloAssociatorsConf
#  Configurable for Calorimeter MC-truth associators
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-07-17
class CaloAssociatorsConf(LHCbConfigurableUser):
    """
    Configurable for Calorimeter MC-truth associators
    """
    __slots__ = {
        ##
        "Context"            : "Offline"   # The context to run
        , "MeasureTime"      : True        # Measure the time for sequencers
        , "OutputLevel"      : INFO        # The global output level
        ##
        , 'Sequence'         : None        # The sequencer to add the CALO Associations
        , 'EnableMCOnDemand' : True        # Enable Digits-On-Demand
        , 'Digits'        : ['Ecal']
        ##
        }
      ## documentation lines 
    _propertyDocDct = {
        ##
        "Context"            : """ The context to run """ 
        , "MeasureTime"      : """ Measure the time for sequencers """ 
        , "OutputLevel"      : """ The global output level """ 
        ##
        , 'Sequence'         : """ The sequencer to add the CALO Associations """ 
        , 'EnableMCOnDemand' : """ Enable MC-On-Demand """ 
        , 'Digits'        : """ List of allowed sub-detectors"""
        ##
        }
    ## used Configurables
    __used_configurables__ = (
        CaloDigitConf , 
        )

    # the basic definition of MC-truth 
    def mcTruth ( self ) :
        """
        The basic definition of MC-truth
        """
        from CaloAssociators.MCTruthOnDemand import caloMCTruth
        main = caloMCTruth (
            self.getProp ( 'Context'          ) ,
            self.getProp ( 'EnableMCOnDemand' ) ,
            self.getProp ( 'Digits'           )
            )
        setTheProperty ( main , 'OutputLevel' , self.getProp('OutputLevel') )
        setTheProperty ( main , 'MeasureTime' , self.getProp('MeasureTime') )
        
        if self.isPropertySet ( 'Sequence' ) : 
            CaloDigitsConf (
                Context              = self.getProp('Context'         ) ,
                MeasureTime          = self.getProp('MeasureTime'     ) ,
                OutputLevel          = self.getProp('OutputLevel'     ) ,
                EnableDigitsOnDemand = self.getProp('EnableMCOnDemand') ,
                Sequence             = self.getProp('Sequence'        ) , 
                Digits               = self.getProp('Digits'          )
                )
            sequence.Members += [ main ]

        return main 
        
    # actualy apply the configuration 
    def applyConf( self ) :
        """
        Apply the configuration 
        """
        _log.info('Apply CaloAssociators Configuration')
        _log.info( self )

        self.mcTruth()
        
        if self.getProp( 'EnableMCOnDemand' )  :
            _log.info ( printOnDemand () ) 
            
 
        
        
# =============================================================================
if '__main__' == __name__ :
    print __doc__
    print __author__
    print __version__
    
# =============================================================================
# The END
# =============================================================================
