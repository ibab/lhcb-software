#!/usr/bin/env python
# ==========================================================================================
## @file BenderExample/RealD0.py
#
#  Very naive attempt to search for D0-> K-pi_ decays with the first collisison.
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
#  @author Vanya     BELYAEV    vanya@nikhef.nl
# ===========================================================================================
"""

Very naive attempt to search for D0 -> K- pi+ decays with the first collisison.

"""
# ===========================================================================================
__author__   = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__version__  = "CVS Tag $Name: not supported by cvs2svn $, verison $Release:$  "
# ===========================================================================================

## import ROOT                           ## needed to produce/visualize the histograms
from   Bender.Main         import *   ## import all bender goodies 
import LHCbMath.Types                 ## easy access to various geometry routines 
from   Gaudi.Configuration import *   ## needed for job configuration

from   GaudiKernel.SystemOfUnits     import GeV, MeV, mm 
from   GaudiKernel.PhysicalConstants import c_light

# =============================================================================
## @class D0
#  Simple algorithm to study D0-> K- pi+
#  @author Vanya     BELYAEV    vanya@nikhef.nl
#  @date 2009-12-21
class D0(Algo) :
    """
    Simple algorithm to study D0 -> K- pi+
    """
    
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
        
        primaries = self.vselect( 'PVs' , ISPRIMARY )
        if primaries.empty() :
            return self.Warning('No primary vertices are found', SUCCESS )
        
        pions = self.select ( 'pi' , 'pi+' == ABSID )
        kaons = self.select ( 'K'  , 'K+'  == ABSID )
        
        if pions.empty() or kaons.empty() : return SUCCESS
        
        self.npv  += 1
        
        tup   = self.nTuple ( 'D0' )        
        
        mips = MIPCHI2( primaries, self.geo() ) 
        
        d0s= self.loop ('K pi' , 'D0')
        
        for d0 in d0s :
            
            m12 = d0.mass(1, 2) / GeV
            if not 1.4 < m12 < 2.1 : continue

            kaon  = d0(1)
            pion  = d0(2)

            qK  = Q(kaon)
            qPi = Q(pion)

            # check the charge:
            if not -0.5 < ( qK + qPi ) < 0.5 : continue

            if qK < 0 : d0.setPID( 'D0'  )
            else      : d0.setPID( 'D~0' ) 
            
            chi2 = VCHI2(d0)
            if not 0 <= chi2 < 100 : continue
            
            m = M(d0) / GeV;
            if not 1.4 < m   < 2.1 : continue
                        
            tup.column ( "mipsD0" , mips ( d0    ) )
            tup.column ( "mipsK"  , mips ( kaon  ) )
            tup.column ( "mipspi" , mips ( pion  ) ) 

            tup.column ( 'pidK' , PIDK ( kaon ) )
            
            z = VZ  ( d0 ) / mm 
            tup.column ( "lv01" , LV01( d0 )  )
            tup.column ( "z"     , z     )
            tup.column ( "chi2"  , chi2  )

            
            bpv = self.bestPV ( primaries, d0.particle() )
            
            ltime  = -1.e+100
            ltchi2 = -1.e+100
            pvz    = -1.e+100
            
            if not not bpv :
                _ltime  = LTIME        ( self.ltfitter , bpv )
                _ltchi2 = LTIMEFITCHI2 ( self.ltfitter , bpv )
                
                ltime  = _ltime  ( d0 ) * c_light
                ltchi2 = _ltchi2 ( d0 )
                pvz    = VZ ( bpv ) 
                
            tup.column ( 'ctau'  , ltime  )
            tup.column ( 'tchi2' , ltchi2 )
            tup.column ( 'pvz'   , pvz    )
                
            tup.column ( "m" , m )
            
            tup.write  ( )
            
            if 0 <= ltchi2 < 100 and  -1 < ltime :
                self.plot ( m , 'K pi mass' , 1.6 , 2.0 , 200 )
                d0.save( "myD0" )
                

        tup2  = self.nTuple ( 'Dstar' )                        
        dstars = self.loop  ("myD0 pi", "D*(2010)+" )
            
        isD0 = 'D0' == ID
        
        for dstar in dstars :
            
            d0    = dstar(1)
            pion2 = dstar(2)
            
            ok1 = isD0 ( d0 )
            
            if     ok1 and  Q(pion2) < 0 : continue 
            if not ok1 and  Q(pion2) > 0 : continue

            dm =  ( dstar.mass(1,2) - M(d0) ) / GeV
            
            if dm > 0.170 : continue
            
            self.plot ( dm , 'M(D0pi)-M(d0)' , 0.130 , 0.170 , 100 )
            
            chi2 = VCHI2 ( dstar )
            if not 0 <= chi2 < 100 : continue

            if ok1 : dstar.setPID( 'D*(2010)+' )
            else   : dstar.setPID( 'D*(2010)-' )
                            
            kaon = child ( d0 , 1)
            pion = child ( d0 , 2 )
            
            tup2.column ( "dm"      , dm  )
            tup2.column ( "md0"     , M ( d0 ) / GeV )
            tup2.column ( "mipsD0"  , mips ( d0    ) )
            tup2.column ( "mipsD0"  , mips ( d0    ) )
            tup2.column ( "mipsK"   , mips ( kaon  ) )
            tup2.column ( "mipspi"  , mips ( pion  ) )
            tup2.column ( "mipspi2" , mips ( pion2 ) ) 

            tup2.column ( "lv01"  , LV01 ( d0 )  )
            tup2.column ( "chi2"  , chi2  )

            bpv = self.bestPV ( primaries, d0 )
            
            ltime  = -1.e+100
            ltchi2 = -1.e+100
            pvz    = -1.e+100
            
            if not not bpv :
                _ltime  = LTIME        ( self.ltfitter , bpv )
                _ltchi2 = LTIMEFITCHI2 ( self.ltfitter , bpv )
                
                ltime  = _ltime  ( d0 ) * c_light
                ltchi2 = _ltchi2 ( d0 )
                pvz    = VZ ( bpv ) 
                
            tup2.column ( 'ctau'  , ltime  )
            tup2.column ( 'tchi2' , ltchi2 )
            tup2.column ( 'pvz'   , pvz    )
                
            tup2.write()
            
                
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
        
    from Configurables           import DaVinci       ## needed for job configuration
    from Configurables           import EventSelector ## needed for job configuration 
    from GaudiConf.Configuration import FileCatalog   ## needed for job configuration 
    from GaudiConf.Configuration import NTupleSvc     ## needed for job configuration 

    davinci = DaVinci (
        DataType      = '2009' ,
        Simulation    = False  ,
        PrintFreq     = 1000   ,
        EvtMax        = -1     , 
        HistogramFile = 'RealD0_Histos.root' 
        )
    
    if datafiles : 
        EventSelector (
            Input     = datafiles
            )
        
        NTupleSvc (
            Output = [ "D0 DATAFILE='RealD0.root' TYPE='ROOT' OPT='NEW'" ]
            )
    
    ## tuning of 'loose' PV recontruction by Mariusz: 
    importOptions("$PATPVROOT/options/PVLoose.py")
    
    
    gaudi = appMgr()
    
    alg = D0(
        'D0'              ,   ## Algorithm name
        NTupleLUN = 'D0'  ,   ## Logical unit for output file with N-tuples 
        InputLocations = [ 'StdNoPIDsPions', 'StdNoPIDsKaons' ] ## input particles 
        )
    
    gaudi.setAlgorithms ( [ 'PatPVOffline' , alg ] ) 
    
    return SUCCESS 



# =============================================================================
# The actual job steering
if '__main__' == __name__ :
    
    from Configurables  import EventSelector
    
    EventSelector( PrintFreq = 1000 )
    
    configure ( [] )
    
    gaudi = appMgr()
    
    evtsel = gaudi.evtSel()

    from BenderExample.JuanFiles2009 import files
    
    evtsel.open( files ) 
          
    run ( -1 )

# =============================================================================
# The END 
# =============================================================================
