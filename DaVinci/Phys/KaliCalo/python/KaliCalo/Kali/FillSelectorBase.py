#!/usr/bin/env python
# =============================================================================
# @file FillSelectorBase.py
# @author Albert Puig Navarro (albert.puig@cern.ch)
# @based on Vanya Belyaev's Pi0FillHistos.py
# @date 2010-11-03
# =============================================================================
"""
"""

__author__ = "Albert Puig (albert.puig@cern.ch)"
__date__ = "$Date$"
__version__ = "$Revision$"

import copy

from GaudiKernel.SystemOfUnits import MeV 
from GaudiPython.Bindings import gbl as cpp

from KaliCalo.Kali.HistoBase import HistoMapBase
from  KaliCalo.Utils         import RootFile 

import KaliCalo.Cells     as Cells
import KaliCalo.FakeCells as FakeCells

TPySelectorFix = cpp.Kali_TPySelectorFix

# =============================================================================
## ROOT 'selector' for filling the histograms
class FillPi0Base( TPySelectorFix  ):
    """
    ROOT selector for filling the histograms
    """

    ## constructor form histogram map and map of coefficients
    #  @param histos   the map of histograms
    #  @param lambdas  the map of calibration coefficients
    #  @param cellFunc the function used for grouping the cells
    def __init__ ( self                             ,
                   histos                           ,
                   lambdas                          ,
                   cellFunc = FakeCells.SameCell () ,
                   betas    = [ 8.3 , 8.8 , 9.5 ]   ,
                   Unit     = MeV                   ) :  
        
        TPySelectorFix.__init__ ( self , None , self ) ## initialize the base 
        self._histos  = histos  ## the histogram map
        self._lambdas = lambdas ## the map of coefficients

        self._global     = self._histos [ Cells.EcalZone   ]
        self._inner      = self._histos [ Cells.InnerZone  ]
        self._middle     = self._histos [ Cells.MiddleZone ]
        self._outer      = self._histos [ Cells.OuterZone  ]
        self._cellFunc   = cellFunc
        self._betas      = copy.deepcopy ( betas )
        self._Unit       = Unit
        self._frequency  = 0
        self._background = histos._background 
        self._print('__init__')
        
        self._gRow      = Cells.GlobalRow
        self._gCol      = Cells.GlobalCol
        
    def _print ( self , phase ) :
        print 'py: Phase(%-16s) : %9d %9d %9d %s' % (
            phase                          ,
            len ( self._histos           ) ,
            len ( self._lambdas          ) ,
            self._histos.entries()         ,
            self._global.entries()
            )

    ## get all coefficients 
    def lambdas  ( self ) :
        return self._lambdas 

    ## get all histograms 
    def histos  ( self ) :
        return self._histos  

    ## the major method: processing of the tree
    def Process        ( self, entry ) : raise NotImplementedError ( "Kali.FillPi0 cannot be used directly" )
    def Begin          ( self        ) : self._print ( 'Begin'          ) 
    def SlaveBegin     ( self, tree  ) : self._print ( 'SlaveBegin'     ) 
    def SlaveTerminate ( self        ) : self._print ( 'SlaveTerminate' )
    def Terminate      ( self        ) : self._print ( 'Terminate'      )

class FillDatabase(object):
    def __init__(self, FillSelector, HistoMapClass):
        if not issubclass( FillSelector , FillPi0Base ):
            raise Exception ( "Specified Fill Selector is not a subclass of FillPi0" )
        self._FillSelector  = FillSelector
        if not issubclass( HistoMapClass , HistoMapBase ):
            raise Exception ( "Specified HistoMap class is not a subclass of Kali.HistoMapBase" )
        self._HistoMapClass = HistoMapClass

    ## ============================================================================
    ## fill data base with histos and lambdas   
    def __call__( self,
                  lambdas                             , 
                  file_names                          ,
                  tree_name   = "KaliPi0/Pi0-Tuple"   ,
                  dbase_name  = 'kali_db.gz'          ,
                  cellFunc    = FakeCells.SameCell () ,
                  Unit        = MeV                   ) :

        """
        Fill data base with histos and lambdas
        """
        FillSelector  = self._FillSelector
        HistoMapClass = self._HistoMapClass

        ## check files
        if issubclass ( type ( file_names ) , str ) :
            file_names = [ file_names ]

        histos   = HistoMapClass ()

        badfiles = set()

        for file_name in file_names :
  
            f = RootFile ( file_name , safe = False )

            if not f.isOK () :
                badfiles.add  ( file_name )
                continue 

            ## get the tree 
            tree = f.Get( tree_name )
            if not tree       :
                raise NameError("Unable to get  ROOT TTree('%s')" % tree_name )

            print 'Tree has %10d entries, %s/%s ' %  ( long ( tree.GetEntries () ) ,
                                                       tree.GetName    () ,
                                                       f.GetName       () )

            ## fill the histograms 
            histos,lambdas = self.fillHistos ( tree         ,
                                               histos       ,
                                               lambdas      ,
                                               cellFunc     ,
                                               Unit         )

            del f 

        ## update data base
        if dbase_name:
            print 'Save Histos to DB:', dbase_name 
            histos.save ( dbase_name ) 

        return (histos,lambdas,badfiles)
      
    ## ============================================================================
    ## fill the histograms from the tree 
    def fillHistos ( self                              ,
                     tree                              ,
                     histomap                          ,
                     lambdamap                         ,
                     cellFunc  = FakeCells.SameCell () ,
                     Unit      = MeV                   ) :  
        """
        Fill the histograms from the tree 
        """
        FillSelector = self._FillSelector
        ## instantiate the selector
        selector = FillSelector ( histomap    ,
                                  lambdamap   ,
                                  cellFunc    ,
                                  Unit = Unit )  

        print '#entries in tree: %10d ' % tree.GetEntries() 
        ##tree.Process ( selector , '' , 20000 )
        tree.Process ( selector )

        lambdas = selector.lambdas ()
        histos  = selector.histos  ()

        print ' histos  : ', len ( histos  ) , histos.entries() 
        print ' lambdas : ', len ( lambdas )

        return ( histos , lambdas )  

# EOF
