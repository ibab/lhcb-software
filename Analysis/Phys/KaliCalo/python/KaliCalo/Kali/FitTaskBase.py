#!/usr/bin/env python
# =============================================================================
# @file FitTaskBase.py
# @author Albert Puig Navarro (albert.puig@cern.ch)
# @based on Vanya Belyaev's TreeTask.py
# @date 2010-11-13
# =============================================================================
"""
"""

__author__ = "Albert Puig (albert.puig@cern.ch)"
__date__ = "$Date$"
__version__ = "$Revision$"

"""
The helper class for parallel fitting using GaudiPython.Parallel
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@itep.ru "
__date__    = " 2010-03-27 "
__version__ = " CVS Tag $Name: not supported by cvs2svn $ , version $Revision: 1.5 $ "
__all__     = (
    "FitTask"   ,
    "fitHistos"
    )
# =============================================================================
import GaudiPython.Parallel as Parallel
import KaliCalo.Cells       as Cells
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
    def __init__ ( self   , histos , fitFunction , preFitFunction ) :

        ## initializae the base class 
        Parallel.Task.__init__ ( self )
        
        ## store the fit function
        self._fitFunction = fitFunction
        
        ## get all references 
        self.__all    = histos [ Cells.EcalZone   ]
        self.__inner  = histos [ Cells.InnerZone  ]
        self.__middle = histos [ Cells.MiddleZone ]
        self.__outer  = histos [ Cells.OuterZone  ]
        
        ## ensure than references are (pre)fit properly: 
        preFitFunction ( self.__all )
        
        fitFunction ( self.__inner  , self.__all , True )
        fitFunction ( self.__middle , self.__all , True )
        fitFunction ( self.__outer  , self.__all , True )

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
            
            if key in ( Cells.EcalZone   ,
                        Cells.InnerZone  ,
                        Cells.MiddleZone ,
                        Cells.OuterZone  ) : continue 
            
            if   key.area() == Cells.InnerZone.area() : 
                result = self._fitFunction ( histoset , self.__inner  )
            elif key.area() == Cells.MiddleZone.area() : 
                result = self._fitFunction ( histoset , self.__middle )
            elif key.area() == Cells.OuterZone.area() : 
                result = self._fitFunction ( histoset , self.__outer  )
            else :
                result = self._fitFunction ( histoset , self.__all    )

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
        print 'FIT-MERGE: ', len ( self.output ) 

# ==============================================================================
## perform the fit for the histograms 
class FitHistos( object ):
    def __init__ ( self , fitFunction, preFitFunction ):
        self.fitFunction    = fitFunction
        self.preFitFunction = preFitFunction
        
    def __call__ ( self              ,
                   histomap          ,
                   manager   = None  ,
                   nHistos   = 60    ) :

        """
        perform the paralell fit for the histograms
        """

        if manager :
            histomap = self.__fit_p_Histos ( histomap , manager , nHistos )
        else :
            histomap = self.__fit_s_Histos ( histomap )
            
        print ' #fitHistos: ', len(histomap)
        return histomap  

    # ==============================================================================
    ## perform the fit for the histograms 
    def __fit_p_Histos ( self           ,
                         histomap       ,
                         manager        ,
                         nHistos   = 60 ) :
        """
        Perform the ``parallel'' fit for the histograms 
        """
        task = FitTask ( histomap , self.fitFunction , self.preFitFunction ) 

        data = histomap.split ( nHistos )

        status = manager.process ( task , data ) 

        for key in task.output :
            histomap.insert ( task.output[key] )

        return histomap
        
    # ==============================================================================
    ## perform the regular (sequential) fit for the histograms 
    def __fit_s_Histos ( self , histomap ) :
        """
        Perform the regular (sequential) fit for the histograms 
        """

        ## get 'All-Ecal' histograms 
        hA = histomap [ Cells.EcalZone   ]

        ## (pre) fit them! 
        self.preFitFunction ( hA )

        ## inner area 
        hI = histomap [ Cells.InnerZone  ]
        ## middle area 
        hM = histomap [ Cells.MiddleZone ]
        ## outer area 
        hO = histomap [ Cells.OuterZone  ]

        ## fit them!!
        print 'FitInner  : ' , self.fitFunction ( hI , hA , True )
        print 'FitMiddle : ' , self.fitFunction ( hM , hA , True )
        print 'FitOuter  : ' , self.fitFunction ( hO , hA , True )

        keys = histomap.keys()    
        keys.sort()
        for key in Cells.Zones  :
            keys.remove ( key     )
            keys.insert ( 0 , key ) 

        for key in keys :
            hs = histomap[ key ]

            if key in Cells.Zones :
              continue

            if   key.area() == Cells.InnerZone.area() : 
                result = self.fitFunction ( hs , hI )
            elif key.area() == Cells.MiddleZone.area() : 
                result = self.fitFunction ( hs , hM )
            elif key.area() == Cells.OuterZone.area() : 
                result = self.fitFunction ( hs , hO )
            else :
                result = self.fitFunction ( hs , hA )

        return histomap 
        
# EOF
