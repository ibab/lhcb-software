#!/usr/bin/env python
# =============================================================================
"""
A module for creating and filling the histograms
"""
# =============================================================================
from ROOT import TPySelector, TH1F
from math import sqrt

# ========================== some global variables =========================
#== histograms and coefficients
FilledHistos={}                        # array of the pi0mass histograms
alam = {}                              # array of the calibration coefficients

# ========================== starting of the selector =========================
class MyPySelector( TPySelector ):
    """
    ROOT selector for filling the histograms
    """
    def Begin( self ):
        print 'py: beginning'

    def SlaveBegin( self, tree ):
        print 'py: slave beginning'

    #== a function for getting the values of calibration coefficients
    def CoefGet(self, coef, FilledHistos):
        """
        Gets the current coefficient values and deletes the histograms
        created during the previous iteration
        """
        for i in FilledHistos.keys():
            del FilledHistos[i]
        alam=coef

# ============================== Processing tree ==============================
    def Process( self, entry ):
        """
        Fills the histograms from a tree
        """
        #== for more convenience
        bamboo=self.fChain

        #== getting the next entry from the tree
        bamboo.GetEntry( entry )

        #== printout
        if((entry%1000)==0):
            print 'py: processing', entry

        #== cell (category) number
        ic1=bamboo.ind1
        ic2=bamboo.ind2

        #== creating the histograms and correction coefficients for the cell
        alam[ic1],FilledHistos[ic1] = HiCreate(alam,FilledHistos,ic1)
        alam[ic2],FilledHistos[ic2] = HiCreate(alam,FilledHistos,ic2)

        #== caculating the corrected mass
        corrMass = bamboo.m12*sqrt(alam[ic1]*alam[ic2])

        #== cuts
        MassCut=(corrMass>0.05)and(corrMass<0.2) # pi0 mass limits
        CellCut = (ic1 != ic2)                   # the two photons should not
                                                 # hit the same cell

        #== loop over all the hit cells
        for k in alam.keys():
            #== checking if at least one of the photons hits the current
            #== cell category and applying pt and prs cuts to it
            cut1=(ic1==k)
            cut2=(ic2==k)

            #== filling the histograms
            if (CellCut and MassCut and (cut1 or cut2)):
                FilledHistos[k].Fill(corrMass)   # for the corresponding cell

        #== return true
        return 1


    def SlaveTerminate( self ):
        print 'py: slave terminating'

    def Terminate( self ):
        print 'py: Terminate'

# =========== Return the histograms and coefficients (called from Pi0.py) =====
    # return the filled histograms
    def HiReturn( self ):
        """
        Returnes the filled histograms and calibration coefficients to Pi0.py
        """
        return FilledHistos,alam

# ==== Function for adding a histogram and a coefficient to the dictionary ====
def HiCreate(alam, FilledHistos, index):
    """
    Fills the arrays of calibration coefficients and histograms
    in case of the first iteration
    """
    #== histogram parameters
    nbins = 100                            # number of bins
    left  = 0.05                           # left mass limit
    right = 0.2                            # right mass limit

    #== filling the arrays of calibration coefficients and histograms
    #== in case of the first iteration
    if not alam.has_key(index):
        alam[index]=1.

    if not FilledHistos.has_key(index):
        FilledHistos[index] = TH1F("FH%i" % index,"pi0mass, cell%i" % index,nbins,left,right)
    
    return alam[index],FilledHistos[index]
### EOF


import copy 
import Kali
import ROOT 
CellID = Kali.CellID 

## use Wim Lavrijsen's trick: 
selector = '$KALICALOROOT/root/TPySelectorFix.C+' 
if 0 > ROOT.gROOT.LoadMacro ( selector ) : 
    raise RunTimeError, "Unable to LoadMacro '%s'" % selector  


def fakeNum ( i , j ) :
    """
    Define 'fake' zone 
    """
    z   = sqrt ( (i-31.5)**2 + (j-31.5)**2 )
    z  -= 5
    z   = abs ( z )
    return int ( z / 10 )

## ROOT 'selector' for filling the histograms
class FillPi0( ROOT.TPySelectorFix  ):
    """
    ROOT selector for filling the histograms
    """

    ## constructor form histogram map and map of coefficients
    #  @param histos   the map of histograms
    #  @param lambdas  the map of calibration coefficients
    #  @param cellFunc the function used for grouping the cells
    def __init__ ( self                         ,
                   histos   = Kali.HistoMap  () ,
                   lambdas  = Kali.LambdaMap () ,
                   cellFunc = lambda s : s      ) :
        
        ROOT.TPySelectorFix.__init__ ( self , None , self ) ## initialize the base 
        self._histos  = copy.deepcopy ( histos  ) ## the histogram map
        self._lambdas = copy.deepcopy ( lambdas ) ## the map of coefficients

        self._histos.reset()
        
        self._global   = self._histos [ Kali.EcalZone   ]
        self._inner    = self._histos [ Kali.InnerZone  ]
        self._middle   = self._histos [ Kali.MiddleZone ]
        self._outer    = self._histos [ Kali.OuterZone  ]
        self._cellFunc = cellFunc 
        self._print('__init__')
        
    def _print ( self , phase ) :
        print 'py: Phase(%-19s) : %9d %9d %9d %s' % (
            phase                          ,
            len ( self._histos           ) ,
            len ( self._lambdas          ) ,
            len ( self._histos.entries() ) ,
            self._global.entries()
            )
        
    ## get all coefficients 
    def lambdas ( self ) :
        return copy.deepcopy ( self._lambdas ) 

    ## get all histograms 
    def histos  ( self ) :
        return copy.deepcopy ( self._histos ) 

    ## the major method: processing of the tree
    def Process( self, entry ):
        """
        Fills the histograms from a tree
        """
        # == getting the next entry from the tree
        if self.GetEntry ( entry ) <= 0 : return 0
        
        # == for more convenience
        bamboo=self.fChain

        # == printout
        if 0 == entry % 10000 : self._print ( 'Process %d ' % entry )

        
        if bamboo.spd1 or bamboo.spd2 : return 1
        
        # == cell (category) number
        
        ic1 = CellID ( bamboo.ind1 ) 
        ic2 = CellID ( bamboo.ind2 )
        
        ic1.setCalo ( 2 )
        ic2.setCalo ( 2 )

        #
        # "massage" cell-ID, e.g. group them
        # 
        ic1 = CellID ( self._cellFunc ( ic1 ) ) 
        ic2 = CellID ( self._cellFunc ( ic2 ) ) 

        ## get the correction coefficients:
        lam1 = self._lambdas [ ic1 ] [ -1 ] ## the last one is relevant here
        lam2 = self._lambdas [ ic2 ] [ -1 ] ## the last one is relevant here 

        # == caculating the corrected mass
        corrMass = bamboo.m12 * sqrt( lam1 * lam2 )
        if not 0 < corrMass < 250 : return 1                ## RETURN

        ## get the histograms for the first cell 
        hs1  = self._histos  [ ic1 ].histos()        
        ## get the histograms and for the first cell 
        hs2  = self._histos  [ ic2 ].histos()  

        ## get 'per-area' histos
        iz1 = CellID ( ic1.calo() , ic1.area() , 0 , 0 )
        iz2 = CellID ( ic2.calo() , ic2.area() , 0 , 0 )
        hz1  = self._histos  [ iz1 ].histos() ## global 'per-zone' histos
        hz2  = self._histos  [ iz2 ].histos() ## global 'per-zone' histos
        ## everything 
        hgl  = self._global.histos()          ## global histos 
        
        ## 
        
        ok1 = max ( bamboo.prs1 , bamboo.prs2 ) < 10
        ok2 = bamboo.prs1 < 10 and bamboo.prs2  > 10 
        ok3 = min ( bamboo.prs1 , bamboo.prs2 ) > 10

        background = 1 == bamboo.bkg
        
        if not background :

            if   ok1 : 
                hs1 [ 0 ].Fill ( corrMass )
                hs2 [ 0 ].Fill ( corrMass )
                hz1 [ 0 ].Fill ( corrMass )
                hz2 [ 0 ].Fill ( corrMass )
                hgl [ 0 ].Fill ( corrMass )
            elif ok2 : 
                hs1 [ 1 ].Fill ( corrMass )
                hz1 [ 1 ].Fill ( corrMass )
                hgl [ 1 ].Fill ( corrMass )
            elif ok3 : 
                hs1 [ 2 ].Fill ( corrMass )
                hs2 [ 2 ].Fill ( corrMass )
                hz1 [ 2 ].Fill ( corrMass )
                hz2 [ 2 ].Fill ( corrMass )
                hgl [ 2 ].Fill ( corrMass )
                
        else :

            if   ok1 : 
                hs1 [ 3 ].Fill ( corrMass )
                hs2 [ 3 ].Fill ( corrMass )
                hz1 [ 3 ].Fill ( corrMass )
                hz2 [ 3 ].Fill ( corrMass )
                hgl [ 3 ].Fill ( corrMass )
            elif ok2 : 
                hs1 [ 4 ].Fill ( corrMass )
                hz1 [ 4 ].Fill ( corrMass )
                hgl [ 4 ].Fill ( corrMass )
            elif ok3 : 
                hs1 [ 5 ].Fill ( corrMass )
                hs2 [ 5 ].Fill ( corrMass )
                hz1 [ 5 ].Fill ( corrMass )
                hz2 [ 5 ].Fill ( corrMass )
                hgl [ 5 ].Fill ( corrMass )
        
        return 1

    def Begin          ( self       ) : self._print ( 'Begin'          ) 
    def SlaveBegin     ( self, tree ) : self._print ( 'SlaveBegin'     ) 
    def SlaveTerminate ( self       ) : self._print ( 'SlaveTerminate' )
    def Terminate      ( self       ) : self._print ( 'Terminate'      )


    
