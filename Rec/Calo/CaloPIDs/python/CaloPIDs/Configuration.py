#!/usr/bin/env gaudirun.py
# =============================================================================
## Configurable for Calorimeter PID 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhe.nl
#  @date 2008-07-17
# =============================================================================
"""
Configurable for Calorimeter PID
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================
__all__   = (
    'HltCaloPIDsConf'     ,
    'OffLineCaloPIDsConf' 
    )
# =============================================================================

from LHCbKernel.Configuration import *
from Configurables            import HistogramSvc

from CaloKernel.ConfUtils     import ( addAlgs        ,
                                       printOnDemand  ,
                                       prntCmp        ,
                                       hltContext     ,
                                       setTheProperty ) 
from CaloPIDs.PIDs            import caloPIDs

# =============================================================================
## @class CaloPIDsConf
#  Configurable for Calorimeter PID
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-07-17
class CaloPIDsConf(LHCbConfigurableUser):
    """
    Class/Configurable to define the calorimeter PID
    """
    ## define the slots
    __slots__ = {
        ##
        "Context"              : "Offline"   # The context within which to run
        , "MeasureTime"        : True        # Measure the time for sequencers
        , "OutputLevel"        : INFO        # The global output level
        ##
        , 'Sequence'           : None        # The sequencer to add the CALO reconstruction algorithms to
        , 'EnablePIDsOnDemand' : False       # enable Reco-On-Demand
        ##
        , 'DataType'           : None        # Data type  
        }
    
    ## Configure recontruction of Calo Charged  PIDs 
    def caloPIDs ( self ) :
        """
        Configure recontruction of Calo Charged  PIDs 
        """
        cmp = caloPIDs ( self.Context            ,
                         self.EnablePIDsOnDemand ) 
        log.info ('Configured Calo PIDs      Reco : %s ' % cmp.name()  )
        ##
        return cmp 
    
    ## Check the configuration
    def checkConfiguration ( self ) :
        """
        Check the configuration
        """
        log.warning('CaloPIDsConf: Configuration is not checked!')
    
   ## Calorimeter PID Configuration
    def applyConf ( self ) :
        """
        Calorimeter PID Configuration
        """
        log.info ( 'Apply Calo PIDs Configuration ')

        self.checkConfiguration()
        
        pids = self.caloPIDs()

        setTheProperty ( pids , 'Context'     , self.Context )
        setTheProperty ( pids , 'OutputLevel' , self.getProp('OutputLevel') )
        setTheProperty ( pids , 'MeasureTime' , self.getProp('MeasureTime') )
        
        if self.getProp('Sequence') :
            addAlgs  ( self.Sequence , pids ) 
            log.info ('Configure main Calo PIDs Sequence  : %s '% self.Sequence.name() )
            log.info ( prntCmp ( self.Sequence ) ) 
        else :
            log.info ('Configure Calorimeter PIDs blocks ' )            
            log.info ( prntCmp ( pids ) )
            
        ##print ' CONFIGURATIONL' , prntCmp ( pids )
        
        # configure the histogram input: 
        hsvc = HistogramSvc ( 'HistogramDataSvc' )
        inputs = hsvc.Input
        found  = False  
        for line in inputs :
            if 0 == line.find ( 'CaloPIDs') : found = True
            
        if found :
            log.info ("CaloPIDsConf: LUN 'CaloPIDs' has been defined already") 
        elif 'DC06' == self.DataType :
            hsvc.Input += [ "CaloPIDs DATAFILE='$PARAMFILESROOT/data/CaloPIDs_DC06_v2.root' TYP='ROOT'" ]
        else:
            hsvc.Input += [ "CaloPIDs DATAFILE='$PARAMFILESROOT/data/CaloPIDs_DC09_v1.root' TYP='ROOT'" ]

        if hltContext ( self.Context ) : 
            importOptions("$CALOPIDSROOT/options/HltPhotonPDF.opts")
        else :
            importOptions("$CALOPIDSROOT/options/PhotonPDF.opts")
        
            
        if self.EnablePIDsOnDemand :
            log.info ( printOnDemand () ) 

        ##print ' ON-DEMAND ' , printOnDemand () 

# =============================================================================
## @class HltCaloPIDsConf
#  Configurable for Calorimeter PID
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-07-17
class HltCaloPIDsConf(CaloPIDsConf):
    """
    Class/Configurable to define the calorimeter PID for Hlt 
    """
    __slots__ = {}
    
    ## Check the configuration
    def checkConfiguration ( self ) :
        """
        Check the configuration
        """
        if not hltContext ( self.Context ) :
            raise AttributeError, 'Invalid context for HltCaloPIDsConf'
        
# =============================================================================
## @class OffLineCaloPIDsConf
#  Configurable for Calorimeter PID
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-07-17
class OffLineCaloPIDsConf(CaloPIDsConf):
    """
    Class/Configurable to define the calorimeter PID for Off-Line 
    """
    __slots__ = {}
    
    ## Check the configuration
    def checkConfiguration ( self ) :
        """
        Check the configuration
        """
        if hltContext ( self.Context ) :
            raise AttributeError, 'Invalid context for OffLineCaloPIDsConf'
        


        
# =============================================================================
# The END
# =============================================================================
