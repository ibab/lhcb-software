#!/usr/bin/env ipython
# ==========================================================================================#
# $Id:$
# ==========================================================================================
## @file  SelectorWithVars.py
#
#  Generic selector to fill RooDataSet from TChain/TTree
#  - the variables can be specified generically 
#  - ther selection is equipped with the progress bar 
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
#  @date   2014-03-02
#  @author Vanya     BELYAEV Ivan.Belyaev@itep.ru
#  @thanks Alexander BARANOV a.baranov@cern.ch
#
#                    $Revision:$
#  Last modification $Date:$
#                 by $Author:$ 
# =============================================================================
"""

Generic selector to fill RooDataSet form TChain/TTree 

- the variables can be specified generically 
- ther selection is equipped with the progress bar 

This file is a part of BENDER project:
    ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
    ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campain of Dr.O.Callot et al.: 
    ``No Vanya's lines are allowed in LHCb/Gaudi software.''

"""
# =============================================================================
__author__  = 'Vanya BELYAEV  Ivan.Belyaev@itep.ru'
__date__    = "2014-03-02"
__version__ = '$Revision:$'
__all__     = (
    "SelectorWithVars"
    )
# =============================================================================
import ROOT
from   AnalysisPython.PySelector   import SelectorWithCuts 
# ==============================================================================
## Define generic selector to fill RooDataSet from TChain
#
#  @code
# 
#  variables = [ ... ]
#
#  ## add a variable 'my_name1' from the tree/chain 
#  variables += [ 
#   #  name       descriptor           min-value , max-value  
#   ( 'my_name1' , 'my_description1' , low       , high     )
#   ]
# 
#  ## get a variable 'my_name' from the tree/chain with accessor function, e.g. rescale it on-fligh
#  variables += [ 
#   #  name       descriptor           min-value , max-value , access function   
#   ( 'my_name2' , 'my_description2' , low       , high      , lambda s : s.my_name2/1000 )
#   ]
#   
#  ## get  less trivial expression
#  variables += [ 
#   #  name       descriptor           min-value , max-value , access function   
#   ( 'my_name3' , 'my_description3' , low       , high      , lambda s : s.var1+s.var2 ) 
#  ]
#
#  ## any function that gets Tchain/Tree and avaluated to double.
#  #  e.g. it coudl be TMVAReader
#  def myvar ( chain ) : ....
#  variables += [ 
#   #  name       descriptor           min-value , max-value , access function   
#   ( 'my_name4' , 'my_description4' , low       , high      , myvar ) 
#  ]
#
#
#  ## add already booked varibales: 
#  v5 = ROOT.RooRealVal( 'my_name5' )
#  variables += [  ( v5 , lambda s : s.var5 ) ]
#
#  
#  ## add already booked varibales: 
#  v6 = ROOT.RooRealVal( 'my_name6' )
#  variables += [  ( v6                     ) ] ## get varibale "my_name6"
#
#
#  #
#  ## finally create selector
#  # 
#  selector = SelectorWithVars (
#             variables                             ,
#             selection = " chi2vx<30 && pt>2*GeV " ,  ## filtering
#            )
#  chain = ...
#  chain.process ( selector )
#  dataset = selector.dataset
# 
#  @endcode
#
#  @date   2014-03-02
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @thanks Alexander BARANOV 
class SelectorWithVars(SelectorWithCuts) :
    """
    Create and fill the basic dataset for RooFit
    """
    def __init__ ( self                           ,
                   variables                      ,  ## list of variables  
                   selection                      ,  ## Tree-selection 
                   cuts         = lambda s : True ,
                   name         = ''              ,
                   fullname     = ''              ) : 

        
        if not     name :
            from   AnalysisPython.PyRoUts      import dsID 
            name = dsID()
            
        if not fullname : fullname = "%s/%s " % ( __name__ , name )

        #
        ## create the logger 
        #
        from   AnalysisPython.Logger           import getLogger
        self._logger = getLogger ( fullname ) 
        #

        #
        ## instantiate the base class
        # 
        SelectorWithCuts.__init__ ( self , selection ) ## initialize the base

        #
        ## keep the cuts
        # 
        self._cuts   = cuts

        #
        ## variables
        # 
        self.varset      = ROOT.RooArgSet()
        self._variables  = []
        
        #
        ## add the variables one by one 
        #
        for v in variables : self.addVariable ( *v )

        #
        ## Book dataset
        # 
        self.data    = ROOT.RooDataSet (
            ##
            name      ,
            fullname  , 
            ##
            self.varset
            )
        
        #
        ## it is still very puzzling for me: shodul this line be here at all??
        ROOT.SetOwnership ( self.data  , False )
        
        self._events   = 0
        self._progress = None 
        self._total    = 1
        self._skip     = 0 

    ## delete the selector, try to clear and delete the dataset 
    def __del__    ( self  )  :
        #
        self.data.Clear()
        self.data.reset()
        #
        del self.data
        
    ## get the dataset 
    def dataset   ( self  ) :
        """ Get the data-set """ 
        return self.data
    
    ## the only one actually important method 
    def Process ( self, entry ):
        """
        Fills data set 
        """
        #
        ## == getting the next entry from the tree
        #
        if self.GetEntry ( entry ) <  0 : return 0             ## RETURN 
        #
        
        if not self._progress :
            self._total =  self.fChain.GetEntries()
            self._logger.info ( 'TChain entries: %d' % self._total  )
            ## decoration:
            from   AnalysisPython.progress_bar import ProgressBar
            self._progress = ProgressBar (
                0                        ,
                self._total              ,
                80                       ,
                mode = 'fixed'           )
            
        if 0 == self._events % 500 :
            self._progress.update_amount ( self.event () )
            print self._progress , '\r',
            
        self._events += 1
        #
        ## == for more convenience
        #
        bamboo = self.fChain

        #
        ## apply cuts (if needed) 
        # 
        if not self . _cuts ( bamboo )  : return 0 

        #
        ## loop over all varibales
        # 
        for v in self._variables :

            var    =  v[0]  ## variable 
            vmin   =  v[2]  ## min-value 
            vmax   =  v[3]  ## max-value 
            vfun   =  v[4]  ## accessor-function 

            value  = vfun ( bamboo )
            if not vmin <= value <= vmax :
                self._skip += 1 
                return 0

            var.setVal ( value ) 


        self.data .add ( self.varset ) 

        return 1 

    ## add declared variable to RooDataSet 
    def addVariable ( self , var , *args ) :
        """
        Add decared variable to RooDataSet 
        """
        if   isinstance ( var , str ) :      ## just the name of variable   
            
            vname = var            ## name 
            vdesc = args[0]        ## description 
            vmin  = args[1]        ## min-value 
            vmax  = args[2]        ## max-value 
            #
            ## accessor function
            #
            if 3 < len ( args ) : vfun = args[3]
            else                : vfun = lambda s : getattr( s , vname )
            # 
            var = ROOT.RooRealVar ( vname , vdesc , vmin , vmax )

        elif isinstance ( var , ROOT.RooRealVar ) : # variable itself 

            vname = var.GetName  () ## name 
            vdesc = var.GetTitle () ## description
            vmin  = var.getMin   () ## min-value 
            vmax  = var.getMax   () ## max-value 
            #
            ## accessor function
            #
            if 0 < len ( args ) : vfun = args[0]
            else                : vfun = lambda s : getattr( s , vname )

        else :

            self._logger.error   ( 'Invalid variable description!' )
            raise AttributeError,  'Invalid variable description!'
            
        self.varset.add ( var ) 
        self._variables += [ ( var , vdesc , vmin , vmax , vfun ) ] 

    #
    def Terminate ( self  ) :
        #
        if self._progress :
            self._progress.update_amount ( self.event () )
            print self._progress , '\r',
        #        
        print '' 
        self._logger.info ( 'Events Processed/Total/Skept %d/%d/%d\nCUTS: "%s"' % (  self._events ,
                                                                                     self._total  ,
                                                                                     self._skip   , 
                                                                                     self.cuts () ) ) 
        self.data.Print('v')
        if not len ( self.data ) : self._logger.warning("Empty dataset!")

    # 
    def Init    ( self, chain ) :
        # 
        if self._progress :
            self._progress.update_amount ( self.event () )
            print self._progress , '\r',
        #
        return SelectorWithCuts.Init ( self , chain ) 

    def Begin          ( self , tree = None ) :
        ## 
        if self._progress :
            self._progress.update_amount ( self.event () )
            print self._progress , '\r',
    #
    def SlaveBegin     ( self , tree        ) :
        # 
        if self._progress :
            self._progress.update_amount ( self.event () )
            print self._progress , '\r',
    #
    def SlaveTerminate ( self               ) :
        # 
        if self._progress :
            self._progress.update_amount ( self.event () )
            print self._progress , '\r',
        #
        
# =============================================================================
# The END 
# =============================================================================
