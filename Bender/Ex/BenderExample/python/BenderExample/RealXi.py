#!/usr/bin/env python
# ==========================================================================================
## @file BenderExample/RealXi.py
#
#  A bit naive attempt to recontructx Xi- -> Lambda0 pi- 
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

A bit naive attempt to recontruct Xi- -> Lambda0 pi- 

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
## @class Xi
#  A bit naive attempt to recontruct Xi- -> Lambda0 pi- 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2010-01-09
class Xi(Algo) :
    """
    A bit naive attempt to recontruct Xi- -> Lambda0 pi-
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
        
        pions   = self.select( 'pion'    , 'pi+' == ABSID )
        if pions.empty() : return SUCCESS
        
        tup  = self.nTuple ( 'XiMinus' )        
        
        mips = MIPCHI2( primaries, self.geo() ) 
        
        xis = self.loop ( 'lam0 pion' , 'Xi-' )

        dml = DMASS( 'Lambda0' )
        
        for xi in xis :
            
            m12 = xi.mass ( 1 , 2 ) / GeV
            if m12 > 2.0 : continue
            
            self.plot ( m12 , 'lam0 pi mass 1', 1.1 , 2.1 , 500 ) 
            
            lam = xi(1)
            pi  = xi(2)
            
            p   = lam.child(1)
            qP  = Q ( p  )
            qPi = Q ( pi ) 
            
            if not -0.25 < ( qP + qPi ) < 0.25 : continue
            
            self.plot ( m12 , 'lam0 pi mass 2', 1.1 , 2.1 , 500 ) 
            
            if 0 < qP : xi.setPID( 'Xi-'  )
            else      : xi.setPID( 'Xi~+' ) 
            
            chi2 = VCHI2 ( xi )
            
            if not 0 <= chi2 < 100 : continue
            
            self.plot ( m12 , 'lam0 pi mass 3', 1.1 , 2.1 , 500 ) 
            
            m = M ( xi ) / GeV
            if m > 1.6 : continue
            
            z = VZ(xi) / mm 
         
            self.plot ( m , 'lam0 pi mass 4', 1.1 , 1.6 , 250 ) 
            
            mips1 = mips ( lam )
            mips2 = mips ( pi  )
            
            tup.column ( "mips0" , mips ( xi ) )
            tup.column ( "mips1" , mips1 )
            tup.column ( "mips2" , mips2 ) 
            
            tup.column ( 'pidp'  , PIDp ( p ) )
            
            tup.column ( "lv01"  , LV01( lam )  )
            tup.column ( "z"     , z     )
            tup.column ( "chi2"  , chi2  )
            
            long1 = True if ISLONG ( p   ) else False  
            long2 = True if ISLONG ( pi  ) else False
            
            tup.column ( 'long1'  , long1 )
            tup.column ( 'long2'  , long2 )
            
            down1 = True if ISDOWN ( p   ) else False  
            down2 = True if ISDOWN ( pi  ) else False  
            
            tup.column ( 'down1'  , down1 )
            tup.column ( 'down2'  , down2 )
            
            bpv = self.bestPV ( primaries, xi.particle() )
            if not not bpv :
                _ltime  = LTIME        ( self.ltfitter , bpv )
                _ltchi2 = LTIMEFITCHI2 ( self.ltfitter , bpv )
                
                ltime  = _ltime  ( xi ) * c_light
                ltchi2 = _ltchi2 ( xi )
                pvz    = VZ ( bpv ) 
            else :
                ltime  = -1.e+100
                ltchi2 = -1.e+100
                pvz    = -1.e+100
                
            tup.column ( 'ctau'  , ltime  )
            tup.column ( 'tchi2' , ltchi2 )
            tup.column ( 'pvz'   , pvz    )
            
            ## lifetime for lamda
            _ltl     = LTIME        ( self.ltfitter , xi.vertex() )
            _ltlchi2 = LTIMEFITCHI2 ( self.ltfitter , xi.vertex() )
            
            ltimel   =  _ltl     ( lam )
            ltlchi2  =  _ltlchi2 ( lam )
            
            tup.column ( 'ctaul'  , ltimel  )
            tup.column ( 'tchi2l' , ltlchi2 )
            
            tup.column ( "m"  , m  ) 
            tup.column ( "qP" , qP )

            tup.column ( "dml" , dml ( lam ) )

            tup.write  ( )
            
            self.plot ( m , 'lam0 pi mass - 2', 1.1 , 1.6 ) 
            if  chi2 < 50 and 0 <= ltchi2 < 100 and 5<= ltime <150 :
                self.plot ( m , 'lam0 pi mass Xi ', 1.1 , 1.6 , 250 )
                
                
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
    
    configLam0 ( datafiles )
    
    xi = Xi (
        'Xi',
        NTupleLUN = 'LAM0' ,   ## Logical unit for output file with N-tuples 
        InputLocations = [ 'Lam0'               ,
                           'StdNoPIDsPions'     ,
                           'StdNoPIDsDownPions' ] ## input particles 
        )
    
    gaudi = appMgr()

    gaudi.addAlgorithm ( xi )

    
# =============================================================================
# The actual job steering
if '__main__' == __name__ :
        
    from Configurables  import EventSelector
    
    EventSelector( PrintFreq = 1000 )
    
    configure ( [] )

    gaudi = appMgr()
    
    evtsel = gaudi.evtSel()

    from BenderExample.JuanFiles2009 import files 
    
    evtsel.open( fiels ) 
          
    run ( -1 )

    import GaudiPython.HistoUtils
    
    myalg  = gaudi.algorithm ( 'Xi' )
    histos = myalg.Histos()
    for (k,h) in histos.items() :
        if hasattr ( h , 'dump' ) :
            print h.dump ( 50 , 25 , False  )
        
# =============================================================================
# The END 
# =============================================================================
