#!/usr/bin/env python
# ==========================================================================================
## @file BenderExample/RealXi.py
#
#  A bit naive attempt to recontructx Omega- -> Lambda0 K- 
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>"Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis"</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  "C++ ToolKit for Smart and Friendly Physics Analysis"
#
#  By usage of this code one clearly states the disagreement 
#  with the campain of Dr.O.Callot et al.: 
#  "No Vanya's lines are allowed in LHCb/Gaudi software."
#
#  @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
#  @date 2010-01-09
# ===========================================================================================
"""

A bit naive attempt to recontructx Omega- -> Lambda0 K- 

"""
# ===========================================================================================
__author__   = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__version__  = "CVS Tag $Name: not supported by cvs2svn $, verison $Release:$"
# ===========================================================================================

## import ROOT                           ## needed to produce/visualize the histograms
from   Bender.Main         import *   ## import all bender goodies 
import LHCbMath.Types                 ## easy access to various geometry routines 
from   Gaudi.Configuration import *   ## needed for job configuration

from   GaudiKernel.SystemOfUnits     import GeV, MeV, mm 
from   GaudiKernel.PhysicalConstants import c_light

# =============================================================================
## @class Omega
#  A bit naive attempt to recontruct Omega- -> Lambda0 K- 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2010-01-09
class Omega(Algo) :
    """
    A bit naive attempt to recontruct Omega- -> Lambda0 K-
    """
    
    ## the standard initialization
    def initialize ( self ) :
        """
        Initialization
        """
        sc = Algo.initialize ( self )
        if sc.isFailure() : return sc
        
        self.npv  = self.counter ( '#nPV'  )
        
        self.ltfitter  = self.lifetimeFitter() 
        
        return SUCCESS
    
    ## the only one esential method:
    def analyse  (self ) :
        """
        The only one essential method
        """
        
        ## get Lambda form TES 
        lams    = self.select ( 'lam0' , 'Lambda0' == ABSID )
        if lams.empty() : return SUCCESS
        
        ## get primary vertices form TES 
        primaries = self.vselect( 'PVs' , ISPRIMARY )
        if primaries.empty() :
            return self.Warning('No primary vertices are found', SUCCESS )
        
        self.npv  += 1
        
        kaons   = self.select( 'kaon'    , 'K+' == ABSID )
        if kaons.empty() : return SUCCESS
        
        tup  = self.nTuple ( 'OmegaMinus' )        
        
        mips = MIPCHI2( primaries, self.geo() ) 
        
        omegas = self.loop ( 'lam0 kaon' , 'Omega-' )

        dml = DMASS( 'Lambda0' )
        
        for omega in omegas :
            
            m12 = omega.mass ( 1 , 2 ) / GeV
            if m12 > 2.5 : continue
            
            self.plot ( m12 , 'lam0 K mass 1', 1.5 , 2.5 , 500 ) 
            
            lam = omega(1)
            K   = omega(2)
            
            p   = lam.child(1)
            qP  = Q ( p )
            qK  = Q ( K ) 
            
            if not -0.25 < ( qP + qK ) < 0.25 : continue
            
            self.plot ( m12 , 'lam0 K mass 2', 1.5 , 2.0 , 250 ) 
            
            if 0 < qP : omega.setPID( 'Omega-'  )
            else      : omega.setPID( 'Omega~+' ) 
            
            chi2 = VCHI2 ( omega )
            
            if not 0 <= chi2 < 100 : continue
            
            self.plot ( m12 , 'lam0 K mass 3', 1.5 , 2.0 , 250 ) 
            
            m = M ( omega ) / GeV
            if m > 2.0 : continue
            
            z = VZ( omega ) / mm 
         
            self.plot ( m , 'lam0 K mass 4', 1.5 , 2.5 , 500) 
            
            mips1 = mips ( lam )
            mips2 = mips ( K   )
            
            tup.column ( "mips0" , mips ( omega ) )
            tup.column ( "mips1" , mips1 )
            tup.column ( "mips2" , mips2 ) 
            
            tup.column ( 'pidp'  , PIDp ( p ) )
            
            tup.column ( "lv01"  , LV01( lam )  )
            tup.column ( "z"     , z     )
            tup.column ( "chi2"  , chi2  )
            
            long1 = True if ISLONG ( p ) else False  
            long2 = True if ISLONG ( K ) else False
            
            tup.column ( 'long1'  , long1 )
            tup.column ( 'long2'  , long2 )
            
            down1 = True if ISDOWN ( p ) else False  
            down2 = True if ISDOWN ( K ) else False  
            
            tup.column ( 'down1'  , down1 )
            tup.column ( 'down2'  , down2 )
            
            bpv = self.bestPV ( primaries, omega.particle() )
            if not not bpv :
                _ltime  = LTIME        ( self.ltfitter , bpv )
                _ltchi2 = LTIMEFITCHI2 ( self.ltfitter , bpv )
                
                ltime  = _ltime  ( omega ) * c_light
                ltchi2 = _ltchi2 ( omega )
                pvz    = VZ ( bpv ) 
            else :
                ltime  = -1.e+100
                ltchi2 = -1.e+100
                pvz    = -1.e+100
                
            tup.column ( 'ctau'  , ltime  )
            tup.column ( 'tchi2' , ltchi2 )
            tup.column ( 'pvz'   , pvz    )
            
            ## lifetime for lamda
            _ltl     = LTIME        ( self.ltfitter , omega.vertex() )
            _ltlchi2 = LTIMEFITCHI2 ( self.ltfitter , omega.vertex() )
            
            ltimel   =  _ltl     ( lam )
            ltlchi2  =  _ltlchi2 ( lam )
            
            tup.column ( 'ctaul'  , ltimel  )
            tup.column ( 'tchi2l' , ltlchi2 )
            
            tup.column ( "m"  , m  ) 
            tup.column ( "qP" , qP )

            tup.column ( "dml" , dml ( lam ) )

            tup.write  ( )
            
            self.plot ( m , 'lam0 K mass - 2', 1.4 , 2.0 ) 
            if  chi2 < 50 and 0 <= ltchi2 < 100 and 2<= ltime <60 :
                self.plot ( m , 'lam0 K mass Omega ', 1.5 , 2.0 , 250 )
                
                
        return SUCCESS 
          
    ## Find 'the best associated; primary vertex'
    def bestPV( self , pvs , p ) :
        """
        Find 'the best associated' primary vertex'
        
        """
        pv   = None
        chi2 = 1.e+100
        fun  = VIPCHI2 ( p , self.geo() ) 
        for v in pvs :
            val = fun ( v )
            if 0 <= val < chi2 :
                chi2 = val
                pv   = v
        return pv

        
# =============================================================================
## configure the job 
def configure ( datafiles ) :
    """
    Job configuration 
    """
    
    from BenderExample.RealLam0 import configure as configLam0

    import BenderExample.StdNoPIDsDownKaons
    
    configLam0 ( datafiles )
    
    om = Omega (
        'Omega',
        NTupleLUN = 'LAM0' ,   ## Logical unit for output file with N-tuples 
        InputLocations = [ 'Lam0'               ,
                           'StdNoPIDsKaons'     ,
                           'StdNoPIDsDownKaons' ] ## input particles 
        )
    
    gaudi = appMgr()

    gaudi.addAlgorithm ( om )

    
# =============================================================================
# The actual job steering
if '__main__' == __name__ :
        
    from Configurables  import EventSelector
    
    EventSelector( PrintFreq = 1000 )
    
    configure ( [] )

    gaudi = appMgr()

    import PartProp.Service

    evtsel = gaudi.evtSel()

    from BenderExample.JuanFile2009 import files 
    
    evtsel.open( files ) 
          
    run ( -1 )

    import GaudiPython.HistoUtils
    
    myalg  = gaudi.algorithm ( 'Omega' )
    histos = myalg.Histos()
    for (k,h) in histos.items() :
        if hasattr ( h , 'dump' ) :
            print h.dump ( 50 , 25 , False  )
        
# =============================================================================
# The END 
# =============================================================================
