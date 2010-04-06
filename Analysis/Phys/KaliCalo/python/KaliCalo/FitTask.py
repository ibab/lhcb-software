#!/usr/bin/env python
# =============================================================================
# $Id: FitTask.py,v 1.4 2010-04-06 19:11:46 ibelyaev Exp $
# =============================================================================
## @file KaliKalo/FitTask.py
#  The helper class for parallel fitting using GaudiPython.Parallel
#  @see GaudiPyhon.Parallel
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2010-03-27
# =============================================================================
"""
The helper class for parallel fitting using GaudiPython.Parallel
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@itep.ru "
__date__    = " 2010-03-27 "
__version__ = " CVS Tag $Name: not supported by cvs2svn $ , version $Revision: 1.4 $ "
__all__     = (
    "FitTask"   ,
    "fitHistos"
    )
# =============================================================================
import GaudiPython.Parallel as Parallel
import KaliCalo.Pi0HistoFit as Fit
import KaliCalo.Kali        as Kali 
# =============================================================================
## @class FitTask 
#  The helper class for parallel fitting using GaudiPython.Parallel
#  @see GaudiPyhon.Parallel
#  @see GaudiPyhon.Parallel.Task
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2010-03-27
class FitTask ( Parallel.Task ) :
    """
    The helper class for parallel fitting using GaudiPython.Parallel

    
    @author Vanya BELYAEV Ivan.Belyaev@itep.ru
    @date 2010-03-27
    """
    
    ## constructor from histo-map
    def __init__ ( self   , histos ) :

        ## initializae the base class 
        Parallel.Task.__init__ ( self )
        
        ## get all references 
        self.__all    = histos [ Kali.EcalZone   ]
        self.__inner  = histos [ Kali.InnerZone  ]
        self.__middle = histos [ Kali.MiddleZone ]
        self.__outer  = histos [ Kali.OuterZone  ]
        
        ## ensure than references are (pre)fit properly: 
        Fit.preFitHistoSet ( self.__all )
        
        Fit.fitHistoSet ( self.__inner  , self.__all , True )
        Fit.fitHistoSet ( self.__middle , self.__all , True )
        Fit.fitHistoSet ( self.__outer  , self.__all , True )

    ## local initialization 
    def initializeLocal ( self ) :
        """
        Local initialization
        """
        print 'FitTask: Start parallel processing (local)'
        self.output = {}
        
    ## local initialization 
    def initializeRemote ( self ) :
        """
        Remote initialization
        """
        import os
        host = os.environ['HOSTNAME']
        #
        print 'FitTask: Start parallel processing (%s)' % host 
        return Parallel.Task.initializeRemote ( self )
    
    def _resetOutput ( self ) :
        self.output ={}
        
    ## process the list of histo sets 
    def process  ( self , histogroup ) :

        """
        Process the list of histo-sets 
        """
        histos, igroup = histogroup
        
        import os
        host = os.environ['HOSTNAME']
        #
        print 'FitTask: Start of processing at ', host , igroup , len ( histos ) 

        for histoset in histos :
            
            key = histoset.cellID()
            
            if key in ( Kali.EcalZone   ,
                        Kali.InnerZone  ,
                        Kali.MiddleZone ,
                        Kali.OuterZone  ) : continue 
            
            if   key.area() == Kali.InnerZone.area() : 
                result = Fit.fitHistoSet ( histoset , self.__inner  )
            elif key.area() == Kali.MiddleZone.area() : 
                result = Fit.fitHistoSet ( histoset , self.__middle )
            elif key.area() == Kali.OuterZone.area() : 
                result = Fit.fitHistoSet ( histoset , self.__outer  )
            else :
                result = Fit.fitHistoSet ( histoset , self.__all    )

            self.output [ key ] = histoset
            
        print 'FitTask: End   of processing at ', host , igroup , len(self.output) 

    ## finalization of the task 
    def finalize ( self ) :
        """
        Finalization of the task
        """
        print 'FitTask: finalize TOTAL: ' , len(self.output)

    ## merge the result form the different processes 
    def _mergeResults ( self, result ) :
        """
        merge the result form the different processes
        """ 
        self.output.update ( result ) 

# ==============================================================================
## perform the fit for the histograms 
def __fit_p_Histos ( histomap       ,
                     manager        ,
                     nHistos   = 60 ) :
    """
    Perform the ``parallel'' fit for the histograms 
    """
    task = FitTask ( histomap ) 

    data = histomap.split ( nHistos )
    
    status = manager.process ( task , data ) 
    
    for key in task.output :
        histomap.insert ( task.output[key] )
        
    return histomap

# ==============================================================================
## perform the regular (sequential) fit for the histograms 
def __fit_s_Histos ( histomap ) :
    """
    Perform the regular (sequential) fit for the histograms 
    """
    
    ## get 'All-Ecal' histograms 
    hA = histomap [ Kali.EcalZone   ]
    
    ## (pre) fit them! 
    Fit.preFitHistoSet ( hA )
    
    ## inner area 
    hI = histomap [ Kali.InnerZone  ]
    ## middle area 
    hM = histomap [ Kali.MiddleZone ]
    ## outer area 
    hO = histomap [ Kali.OuterZone  ]
    
    ## fit them!!
    print 'FitInner  : ' , Fit.fitHistoSet ( hI , hA , True )
    print 'FitMiddle : ' , Fit.fitHistoSet ( hM , hA , True )
    print 'FitOuter  : ' , Fit.fitHistoSet ( hO , hA , True )
    
    keys = histomap.keys()    
    keys.sort()
    for key in Kali.Zones  :
        keys.remove ( key     )
        keys.insert ( 0 , key ) 
        
    for key in keys :
        
        if key in Kali.Zones : continue
        
        if   key.area() == Kali.InnerZone.area() : 
            result = Fit.fitHistoSet ( hs , hI )
        elif key.area() == Kali.MiddleZone.area() : 
            result = Fit.fitHistoSet ( hs , hM )
        elif key.area() == Kali.OuterZone.area() : 
            result = Fit.fitHistoSet ( hs , hO )
        else :
            result = Fit.fitHistoSet ( hs , hA )
            
    return histomap 

# ==============================================================================
## perform the fit for the histograms 
def fitHistos ( histomap          ,
                manager   = None  ,
                nHistos   = 60    ) :
    """
    perform the paralell fit for the histograms
    """

    if manager :
        histomap = __fit_p_Histos ( histomap , manager , nHistos )
    else :
        histomap = __fit_s_Histos ( histomap )

    print ' #fitHistos: ', len(histomap)
    
    return histomap 

    
# =============================================================================
if '__main__' == __name__ :
    
    ## make printout of the own documentations 
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print ' dir(%s) : %s ' % ( __name__    , dir() )
    print ' __all__ : '    %  __all__ 
    print '*'*120  

# =============================================================================
# The END 
# =============================================================================


