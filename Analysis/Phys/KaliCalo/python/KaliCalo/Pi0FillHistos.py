#!/usr/bin/env python
# =============================================================================
## @file KaliCalo/Pi0FillHistos.py
#  The helper module (ROOT Selector) which fills the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-05
# =============================================================================
"""
The helper module (ROOT Selector) which fills the histograms 
"""
# =============================================================================
__all__ = (
    'Pi0Histos'    , ## T-Selector to fill the ihstograms 
    'processTree'  , ## process one tree
    'processFile'  , ## process one file 
    'processFiles'   ## process the list of files   
    )
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@nikhef.nl'
__version__ = 'CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $'
# =============================================================================
import ROOT 
import copy 
from   math                      import sqrt 
from   GaudiPython.Bindings      import gbl  as cpp
from   GaudiKernel.SystemOfUnits import GeV

## namesapce LHCb 
LHCb = cpp.LHCb
LHCb.CaloCellID.__str__  = LHCb.CaloCellID.toString 
LHCb.CaloCellID.__repr__ = LHCb.CaloCellID.toString 

CaloCellID = LHCb.CaloCellID 

cell0 = LHCb.CaloCellID( 2 , 0 , 0 , 0 )

# =============================================================================
## use Wim Lavrijsen's trick: 
selector = '$KALICALOROOT/root/TPySelectorFix.C+' 
if 0 > ROOT.gROOT.LoadMacro ( selector ) : 
    raise RunTimeError, "Unable to LoadMacro '%s'" % selector  


# =============================================================================
## @class Pi0Histos
#  The basic class ("TPySelector") to fill the pi0-histograms for Kali
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-05
# =============================================================================
class Pi0Histos ( ROOT.TPySelectorFix ) :
    """
    The basic class (TPySelector) to fill the histograms for Kali
    """
    
    ## constructor
    #  @param calibr  map of calibration/correction coefficients
    #  @param histos  map of histograms
    def __init__ ( self          ,
                   calibr        ,
                   histos        ,
                   debug  = 1000 ) :
        """
        """
        ROOT.TPySelectorFix.__init__ ( self , None , self )
        self.calibr = calibr
        self.histos = histos
        self.debug  = debug 
        print 'I am Pi0FillHistos::__init__', self.checkHistos() 

    def Begin          ( self ) :
        print 'I am Pi0FillHistos::Begin', self.checkHistos() 
        
    def SlaveBegin     ( self , tree ) :
        print 'I am Pi0FillHistos::SlaveBegin', self.checkHistos()
        
    def Terminate      ( self ) :
        print 'I am Pi0FillHistos::Terminate', self.checkHistos() 
        
    def SlaveTerminate ( self ) :
        print 'I am Pi0FillHistos::SlaveTerminate', self.checkHistos() 

    ## the major method 
    def Process ( self , entry ) :
        if self.GetEntry ( entry ) <= 0 : return 0
        
        tree  = self.fChain
        cell1 = CaloCellID(tree.ind1)
        cell2 = CaloCellID(tree.ind2)
        cell1.setCalo ( 2 ) 
        cell2.setCalo ( 2 )

        # get the global histogram (for checks&monitoring
        h0   = self.histos.get ( cell0 , None ) 
        if not h0 : h0 = self.bookHisto ( cell0 )

        # get the histogram for the first photon/cell 
        h1   = self.histos.get ( cell1 , None ) 
        if not h1 : h1 = self.bookHisto ( cell1 ) 
            
        # get the histogram for the second photon/cell 
        h2   = self.histos.get ( cell2 , None )         
        if not h2 : h2 = self.bookHisto ( cell2 ) 

        # calibration coefficient for the first cell 
        lam1 = self.calibr.get ( cell1 , 1.0 )
        
        # calibration coefficient for the second cell 
        lam2 = self.calibr.get ( cell2 , 1.0 )

        # calculate the corrected mass 
        mass = tree.m12 * sqrt ( lam1 * lam2 )

        h0 . Fill ( mass ) 
        h1 . Fill ( mass )
        h2 . Fill ( mass )
        
        if 0 == entry % self.debug  :
            print ' Processing ',   \
                  entry ,           \
                  cell1 ,           \
                  cell2 ,           \
                  h0.GetEntries() , \
                  h1.GetEntries() , \
                  h2.GetEntries() , \
                  self.checkHistos () 
            
        
        return True 

    ## Book the histogram and insert it into the dictionary 
    def bookHisto ( self , cell ) :
        """
        Book the thistogram an dinsert it into the dictionry 
        """
        h = ROOT.TH1D ( 'h'                  + str ( cell.index() ) , 
                        'pi0 mass for cell ' + cell.toString()      ,
                        125 , 0 , .250 )
        
        self.histos [ cell ] = h 
        return h
    
    ## get the histograms 
    def getHistos ( self ) :
        """
        Get the histograms 
        """
        return copy.deepcopy ( self.histos )

    ## get the calibration constants 
    def getCalibr ( self ) :
        """
        Get the calibration constants 
        """
        return copy.deepcopy ( self.calibr )

    def checkHistos ( self , histos = None ) :
        """
        check the histos
        """
        if not histos : histos = self.histos
        mn =  1.e+30
        mx = -1.e+30
        ok = 0 
        for key in histos :
            h = histos [ key ] 
            if not h : continue
            ok += 1
            mn  = min ( mn , h.GetEntries() )
            mx  = max ( mx , h.GetEntries() )
        return ( len(histos) , ok , mn , mx )
    
# =============================================================================
## process the filling of the histograms for one tree/chain
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-31
def processTree ( tree        ,      ## the tree/chain
                  calibr = {} ,      ## the calibration coefficients 
                  histos = {} ) :    ## the histograms 
    """
    Process the filling of histogram for one tree/chain

    >>> tree   =  ... ## get the tree/chain  
    >>> calib  =  ... ## get the calibration/correction coefficients
    >>> histos =  ... ## the histograms

    >>> result = processTree ( tree , calbr , histos )
    
    """

    ## create the TSeelctor
    selector = Pi0Histos ( calibr , histos )
    ## process the tree 
    tree.Process ( selector )
    ## get the results from selector
    calibr = selector.getCalibr()
    histos = selector.getHistos() 
    return ( calibr , histos )

# =============================================================================
## process the file
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-31
def processFile ( file         ,       ## the file name 
                  calibr = {}  ,       ## calibration coefficients 
                  histos = {}  ) :     ## the histograms 

    file = ROOT.TFile ( file )
    tree = file.Get('KaliPi0/Pi0-Tuple')
    return processTree ( tree , calibr , histos )

# =============================================================================
## the dispatcher:   list of files, file, tree
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-31
def process   ( objects ,
                calibr  ,
                histos  ) :
    """
    """
    if list is type ( objects ) :
        for obj in objects :
            result = process ( obj , calibr , histos )
            calibr = copy.deepcopy ( result[0] )
            histos = copy.deepcopy ( result[1] )
        return ( calibr , histos )
    
    if str is type ( objects )  :
        file = objects 
        return processFile ( file , calibr , histos )
    
    tree = objects 
    return processTree ( tree , cialbr , histos )
        
# =============================================================================
if '__main__' == __name__ :
    print __doc__
    print __author__
    print __version__

    
# =============================================================================
# The END 
# =============================================================================
