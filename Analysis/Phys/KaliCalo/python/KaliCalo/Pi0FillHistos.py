#!/usr/bin/env python
# =============================================================================
"""
A module for creating and filling the histograms
"""
# =============================================================================
from ROOT import TPySelector, TH1F
from math import sqrt

from GaudiKernel.SystemOfUnits import MeV 

import copy 
import ROOT 
import KaliCalo.Kali  as Kali
CellID = Kali.CellID 
from GaudiPython.Bindings import gbl as cpp


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





## use Wim Lavrijsen's trick: 
##selector = '$KALICALOROOT/root/TPySelectorFix.C' 
##if 0 > ROOT.gROOT.LoadMacro ( selector ) : 
##    raise RunTimeError, "Unable to LoadMacro '%s'" % selector
Kali.TPySelectorFix = cpp.Kali_TPySelectorFix

# =============================================================================
## ROOT 'selector' for filling the histograms
class FillPi0( Kali.TPySelectorFix  ):
    """
    ROOT selector for filling the histograms
    """

    ## constructor form histogram map and map of coefficients
    #  @param histos   the map of histograms
    #  @param lambdas  the map of calibration coefficients
    #  @param cellFunc the function used for grouping the cells
    def __init__ ( self                           ,
                   histos   = Kali.HistoMap  ()   ,
                   lambdas  = Kali.LambdaMap ()   ,
                   cellFunc = Kali.SameCell  ()   ,
                   betas    = [ 8.3 , 8.8 , 9.5 ] ,
                   Unit     = MeV                 ) :  
        
        Kali.TPySelectorFix.__init__ ( self , None , self ) ## initialize the base 
        self._histos  = histos  ## the histogram map
        self._lambdas = lambdas ## the map of coefficients

        self._global    = self._histos [ Kali.EcalZone   ]
        self._inner     = self._histos [ Kali.InnerZone  ]
        self._middle    = self._histos [ Kali.MiddleZone ]
        self._outer     = self._histos [ Kali.OuterZone  ]
        self._cellFunc  = cellFunc
        self._betas     = copy.deepcopy ( betas )
        self._Unit      = Unit
        self._frequency = 0 
        self._print('__init__')
        
        self._gRow      = Kali.GlobalRow
        self._gCol      = Kali.GlobalCol
        
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
    def Process( self, entry ):
        """
        Fills the histograms from a tree
        """
        # == getting the next entry from the tree
        if self.GetEntry ( entry ) <= 0 : return 0
        
        # == for more convenience
        bamboo=self.fChain
        
        if not self._frequency :
            entries  = bamboo.GetEntries()
            entries  = int ( entries /  50.0 / 10000.0 ) * 10000
            entries  = max ( entries , 100000 ) 
            self._frequency = entries
            
        # == printout
        if 0 == entry % self._frequency:
            self._print ( 'Process %d' % entry )
            
        ## 3x3 spd ?? 
        ## if bamboo.spd1 and bamboo.spd2 : return 1
        if bamboo.spd1 or bamboo.spd2 : return 1
        
        # == cell (category) number
        
        ic1 = CellID ( bamboo.ind1 ) 
        ic2 = CellID ( bamboo.ind2 )
        
        ic1.setCalo ( 2 )
        ic2.setCalo ( 2 )

        area1 = ic1.area()
        area2 = ic2.area()
        
        #
        # "massage" cell-ID, e.g. group them
        # 
        ic1 = CellID ( self._cellFunc ( ic1 ) ) 
        ic2 = CellID ( self._cellFunc ( ic2 ) ) 

        ## get the correction coefficients:
        lam1 = self._lambdas [ ic1 ] [ -1 ] ## the last one is relevant here
        lam2 = self._lambdas [ ic2 ] [ -1 ] ## the last one is relevant here 

        # == caculating the corrected mass
        corrMass = bamboo.m12 * sqrt( lam1 * lam2 ) * self._Unit
        if not 0 < corrMass < 350 : return 1                ## RETURN

        ## get the histograms for the first cell 
        hs1  = self._histos  [ ic1 ].histos()        
        ## get the histograms and for the first cell 
        hs2  = self._histos  [ ic2 ].histos()  
        
        hc1  = self._histos  [ ic1 ].counters()        
        hc2  = self._histos  [ ic2 ].counters()        

        ## get 'per-area' histos
        iz1 = CellID ( ic1.calo() , area1 , self._gRow , self._gCol )
        iz2 = CellID ( ic2.calo() , area2 , self._gRow , self._gCol )
        hz1  = self._histos  [ iz1 ].histos   () ## global 'per-zone' histos
        hz2  = self._histos  [ iz2 ].histos   () ## global 'per-zone' histos
        cz1  = self._histos  [ iz1 ].counters () ## global 'per-zone' histos
        cz2  = self._histos  [ iz2 ].counters () ## global 'per-zone' histos
        ## everything 
        hgl  = self._global.histos   ()          ## global histos 
        cgl  = self._global.counters ()          ## global counters
        
        ## 

        ok1 = False 
        ok2 = False 
        ok3 = False
        
        if   max ( bamboo.prs1 , bamboo.prs2 ) * self._Unit < 10 : ok1 = True 
        elif min ( bamboo.prs1 , bamboo.prs2 ) * self._Unit > 10 : ok3 = True
        else:
            ok2 = True 

        background = 1 == bamboo.bkg
        
        if ok2 :
            corrMass  = bamboo.m12 * self._Unit
            factor2   = self._betas[area2] * bamboo.prs2/bamboo.g2E
            corrMass *= sqrt ( lam1 )
            corrMass *= sqrt ( lam2 + ( 1 - lam2 ) * factor2 )
            if abs ( corrMass - 135. ) < 20 :
                hc2[0]   += factor2
                cz2[0]   += factor2
                cgl[0]   += factor2
                
        elif ok3 :
            corrMass  = bamboo.m12 * self._Unit
            factor1   = self._betas[area1] * bamboo.prs1/bamboo.g1E
            factor2   = self._betas[area2] * bamboo.prs2/bamboo.g2E
            corrMass *= sqrt ( lam1 + ( 1 - lam1 ) * factor1 )
            corrMass *= sqrt ( lam2 + ( 1 - lam2 ) * factor2 ) 
            if abs ( corrMass - 135. ) < 20 :
                hc1[1]   += factor1 
                cz1[1]   += factor1
                cgl[1]   += factor1                
                hc2[1]   += factor2
                cz2[1]   += factor2
                cgl[1]   += factor2                

        if corrMass > 250  : return 1 

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


## ============================================================================
## fill the histograms from the tree 
def fillHistos ( tree                          ,
                 histomap  = Kali.HistoMap  () ,
                 lambdamap = Kali.LambdaMap () ,
                 cellFunc  = lambda s : s      ,
                 Unit      = MeV               ) :  
    """
    Fill the histograms from the tree 
    """
    
    selector = FillPi0 ( histomap    ,
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


# =============================================================================
## fill data base with histos and lambdas 
def fillDataBase (
    lambdas                           , 
    file_names                        ,
    tree_name   = "KaliPi0/Pi0-Tuple" ,
    dbase_name  = 'kali_db.gz'        ,
    cellFunc    = lambda s : s        ,
    Unit        = MeV                 ) :

    """
    Fill data base with histos and lambdas
    """

    if issubclass ( type ( file_names ) , str ) :
        file_names = [ file_names ]

    histos   = Kali.HistoMap ()

    import sets
    badfiles = sets.Set() 

    for file_name in file_names :
        
        from  KaliCalo.Utils import RootFile 
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
        histos,lambdas = fillHistos ( tree      ,
                                      histos    ,
                                      lambdas   ,
                                      cellFunc  ,
                                      Unit      )
        
        del f 
        
    ## update data base
    if dbase_name:
        print 'Save Histos to DB:', dbase_name 
        histos.save ( dbase_name ) 
    
    return (histos,lambdas,badfiles) 
    
             
# =============================================================================
# The END 
# =============================================================================
