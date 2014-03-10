#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id: PySelector.py 165594 2013-12-03 13:20:35Z albarano $
# =============================================================================
# @file Selectors.py
# 
# Helper module to fix a problems in communication of
# <c>TTree/TChain::Process</c> and <c>TPySelector</c>.
# In PyROOT some of original C++ methods are disable.
# The module provides the "recovery" for missing methods
#
# @see TTree::Process
# @see TChain::Process
# @see TPySelector
#
# @code
#
# from PyPAW.Selectors import Selector
#
# class MySelector ( Selector ) :
#
#      def __init__ ( self ) :
#
#           return Selector ( self )
#
#      def Process  ( self , entry ) :
#           # == getting the next entry from the tree
#           if self.GetEntry ( entry ) <= 0 : return 0             ## RETURN 
#        
#           # == for more convenience
#           tree=self.fChain
#
#           # apply trivial "acceptance" cuts 
#           if not 2 <= tree.y   <=  4.5   : return 0               ## RETURN
#           if not 1 <= tree.pt  <=  10.0  : return 0               ## RETURN
#
#           return 1
#
# 
# selector = MySelector()
#
# chain = ...
# chain.process ( selector )  ## NB: note lowercase "process" here !!!
#
# @endcode
#
# The module has been developed and used with great success in
# ``Kali, framework for fine calibration of LHCb Electormagnetic Calorimeter''
#
# @author Vanya BELYAEV Ivan.Belyaev@cern.ch
# @date   2010-04-30
# 
#                    $Revision: 165594 $
#  Last Modification $Date: 2013-12-03 14:20:35 +0100 (Tue, 03 Dec 2013) $
#                 by $Author: albarano $
# =============================================================================
"""
Helper module to fix a problems in communication of
TTree/TChain::Process< and TPySelector.
In PyROOT some of original C++ methods are disable.
The module provides the 'recovery' for missing methods

# from PyPAW.Selectors import Selector
#
# class MySelector ( Selector ) :
#
#    def __init__ ( self ) :
#       return Selector ( self )
#    def Process  ( self , entry ) :
#        # == getting the next entry from the tree
#        if self.GetEntry ( entry ) <= 0 : return 0             ## RETURN 
#    
#        # == for more convenience
#        tree=self.fChain
#    
#        # apply trivial 'acceptance' cuts 
#        if not 2 <= tree.y   <=  4.5   : return 0               ## RETURN
#        if not 1 <= tree.pt  <=  10.0  : return 0               ## RETURN
#    
#        return 1
#
#
# selector = MySelector()
# 
# chain = ...
#
# chain.process ( selector )  ## NB: note lowercase ``process'' here !!!
#

The module has been developed and used with great success in
``Kali, framework for fine calibration of LHCb Electormagnetic Calorimeter''

More complicated (and more useful) cases are covered by
SelectorWithCuts and SelectorWithVars classes 

"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@cern.ch"
__date__    = "2010-04-30"
__version__ = "$Revision: 165594 $" 
# =============================================================================
__all__ = (
'Selector'         ,     ## The ``fixed'' TPySelector
'Selector2'        ,     ## The ``fixed'' TPySelector
'SelectorWithCuts' ,     ## The ``fixed'' TPySelector with TTree-formula 
'SelectorWithVars'       ## Generic slector to fill RooDataSet form TTree/TChain
)
# =============================================================================
import ROOT
##
try:
    import cppyy
except ImportError:
    # FIXME: backward compatibility
    print "# WARNING: using PyCintex as cppyy implementation"
    import PyCintex as cppyy
    import sys
    sys.modules['cppyy'] = cppyy
    
# construct the global C++ namespace 
cpp       = cppyy.makeNamespace('')
Analysis  = cpp.Analysis

## C++ Selector 
Selector  = Analysis.Selector
# =============================================================================
## @class Selector2
#  Useful intermediate class for implementation of (py)selectors 
#  @see Analysis::Selector
#  @see TPySelector
#  @see TSelector
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-05-09
class Selector2 ( Analysis.Selector) :
    """
    Useful intermediate class for implementation of (py)selectors 
    
    """
    ## constructor 
    def __init__ ( self ) :
        """
        Standart constructor
        """
        ## initialize the base 
        Analysis.Selector.__init__ ( self , None , self )
        
    def Notify         ( self               ) : return True
    def Terminate      ( self               ) : pass 
    def SlaveTerminate ( self               ) : pass 
    def Begin          ( self , tree = None ) : pass
    def SlaveBegin     ( self , tree        ) : pass 
    def Init           ( self , tree        ) : pass
    ## the major method
    def Process        ( self , entry       ) :
        """
        The major method 
        """
        # load data 
        if self.GetEntry ( entry ) < 0 : return 0

        #
        ## put your code here 
        #
        
        return 1 
# =============================================================================
## @class SelectorWithCuts
#  Efficient selector that runs only for ``good''-events  
#  @see Analysis::SelectorWithCuts
#  @see Analysis::Selector
#  @see TPySelector
#  @see TSelector
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-05-09
class SelectorWithCuts (Analysis.SelectorWithCuts) :
    """
    Efficient selector that runs only for ``good''-events  
    """
    ## constructor 
    def __init__ ( self , selection ) :
        """
        Standart constructor
        """
        ## initialize the base 
        Analysis.SelectorWithCuts.__init__ ( self , selection , None , self )
        if not self.cuts() :
            raise RuntimeError ("__init__:  Invalid Formula %s " % self.cuts() )
        
    def Notify         ( self               ) : return True
    def Terminate      ( self               ) : pass 
    def SlaveTerminate ( self               ) : pass 
    def Begin          ( self , tree = None ) : pass
    def SlaveBegin     ( self , tree        ) :
        if not self.ok() or not self.formula() :
            raise RuntimeError ("SlaveBegin:Invalid Formula %s " % self.cuts() )        
    def Init           ( self , tree        ) :
        if not self.ok() or not self.formula() :
            raise RuntimeError ("Init:      Invalid Formula %s " % self.cuts() )
        

    ## the major method
    def Process        ( self , entry       ) :
        """
        The major method 
        """
        # load data 
        if self.GetEntry ( entry ) < 0 : return 0

        #
        ## put your code here 
        #
        
        return 1
    
# =============================================================================
## define the helper function for proper decoration of ROOT.TTree/TChain
#
# @code
#
# from PyPAW.PySelector import Selector
#
# class MySelector ( Selector ) :
#
#      def __init__ ( self ) :
#
#           return Selector ( self )
#
#      def Process  ( self , entry ) :
#          # == getting the next entry from the tree
#           if self.GetEntry ( entry ) <= 0 : return 0             ## RETURN 
#        
#           # == for more convenience
#           tree=self.fChain
#
#           # apply trivial "acceptance" cuts 
#           if not 2 <= tree.y   <=  4.5   : return 0               ## RETURN
#           if not 1 <= tree.pt  <=  10.0  : return 0               ## RETURN
#
#           return 1
#
# 
# selector = MySelector()
#
# chain = ...
# chain.process ( selector )  ## NB: note lowercase "process" here !!!
#
# @endcode
#
# The module has been developed and used with great success in
# ``Kali, framework for fine cailbration of LHCb Electormagnetic Calorimeter''
#
# @see Analysis::Selector 
# @see Analysis::Process 
# @author Vanya BELYAEV Ivan.Belyaev@cern.ch
# @date   2010-04-30
#
def _process_ ( self , selector , *args ) :
    """
    ``Process'' the tree/chain with proper TPySelector :
    
    from PyPAW.Selectors import Selector
    
    class MySelector ( Selector ) : ... 
    
    selector = MySelector()
    
    chain = ...
    chain.process ( selector )  ## NB: note lowercase ``process'' here !!!
    
    """
    return Analysis.Process.process ( self , selector , *args )

_process_. __doc__ += '\n' + Analysis.Process.process.__doc__


# =============================================================================
## finally: decorate TTree/TChain
for t in ( ROOT.TTree , ROOT.TChain ) : t.process  = _process_ 


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
    #             selection = ' chi2vx<30 && pt>2*GeV ' ,  ## filtering
    #            )
    #  chain = ...
    #  chain.process ( selector )
    #  dataset = selector.dataset
    # 
    """
    ## constructor 
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
            from PyPAW.progress_bar import ProgressBar
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
        self._logger.info (
            'Events Processed/Total/Skept %d/%d/%d\nCUTS: "%s"' % (
            self._events ,
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
if '__main__' == __name__ :

    print 100*'*'
    print __doc__
    print 100*'*'
    print ' Author  : ' , __author__
    print ' Date    : ' , __date__
    print ' Version : ' , __version__
    print 100*'*'
    
# =============================================================================
# The END 
# =============================================================================

