#!/usr/bin/env python
# ==========================================================================================
## @file BenderExample/RealDPlus.py
#
#  Very naive attempt to search for D+ -> K- pi+ pi+ decays with the first collisison.
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

Very naive attempt to search for D+ -> K- pi+ pi+ decays with the first collisison.

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
## @class DPlus
#  Simple algorithm to study D+ -> K- pi+ pi+
#  @author Vanya     BELYAEV    vanya@nikhef.nl
#  @date 2009-12-21
class DPlus(Algo) :
    """
    Simple algorithm to study D+ -> K- pi+ pi+
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
        
        tup   = self.nTuple ( 'DPlus' )        
        
        mips = MIPCHI2( primaries, self.geo() ) 
        
        ds= self.loop ('K pi pi' , 'D+')
        
        for D in ds :
            
            m123 = D.mass(1, 2, 3 ) / GeV
            if not 1.5 < m123 < 2.2 : continue

            kaon  = D(1)
            pi1   = D(2)
            pi2   = D(3)

            qK   = Q ( kaon )
            qPi1 = Q ( pi1  )
            qPi2 = Q ( pi2  )
            
            # check the charge:
            if not -0.5 < ( qK + qPi1 ) < 0.5 : continue
            if not -0.5 < ( qK + qPi2 ) < 0.5 : continue


            if qK < 0 : D.setPID( 'D+'  )
            else      : D.setPID( 'D-' ) 
            
            chi2 = VCHI2(D)
            if not 0 <= chi2 < 100 : continue
            
            m = M(D) / GeV;
            if not 1.5 < m   < 2.2 : continue
                        
            tup.column ( "mipsD"   , mips ( D     ) )
            tup.column ( "mipsK"   , mips ( kaon  ) )
            tup.column ( "mipspi1" , mips ( pi1   ) ) 
            tup.column ( "mipspi2" , mips ( pi2   ) ) 

            tup.column ( 'pidK'    , PIDK ( kaon ) )
            
            z = VZ  ( D ) / mm 
            tup.column ( "lv01"  , LV01( D )  )
            tup.column ( "z"     , z     )
            tup.column ( "chi2"  , chi2  )
            
            bpv = self.bestPV ( primaries, D.particle() )
            
            ltime  = -1.e+100
            ltchi2 = -1.e+100
            pvz    = -1.e+100
            
            if not not bpv :
                _ltime  = LTIME        ( self.ltfitter , bpv )
                _ltchi2 = LTIMEFITCHI2 ( self.ltfitter , bpv )
                
                ltime  = _ltime  ( D ) * c_light
                ltchi2 = _ltchi2 ( D )
                pvz    = VZ ( bpv ) 
                
            tup.column ( 'ctau'  , ltime  )
            tup.column ( 'tchi2' , ltchi2 )
            tup.column ( 'pvz'   , pvz    )
                
            tup.column ( "m" , m )
            
            tup.write  ( )
            
            if 0 <= ltchi2 < 100 and  -1 < ltime :
                self.plot ( m , 'K pi pi mass' , 1.6 , 2.0 , 200 )
                D.save( "Dplus" )

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
        HistogramFile = 'RealDPlus_Histos.root' 
        )
    
    if datafiles : 
        EventSelector (
            Input     = datafiles
            )
        
        NTupleSvc (
            Output = [ "DPLUS DATAFILE='RealDPlus.root' TYPE='ROOT' OPT='NEW'" ]
            )
    
    ## tuning of 'loose' PV recontruction by Mariusz: 
    importOptions("$PATPVROOT/options/PVLoose.py")
    
    
    gaudi = appMgr()
    
    alg = DPlus(
        'DPlus'                 ,   ## Algorithm name
        NTupleLUN = 'DPLUS'     , ## Logical unit for output file with N-tuples 
        InputLocations = [ 'StdNoPIDsPions', 'StdNoPIDsKaons' ] ## input particles 
        )
    
    gaudi.setAlgorithms ( [ 'PatPVOffline' , alg ] ) 
    
    return SUCCESS 



# =============================================================================
# The actual job steering
if '__main__' == __name__ :


    from Configurables  import EventSelector
    
    EventSelector( PrintFreq = 1000 )
    
    configure ([])

    gaudi = appMgr()
    evtsel = gaudi.evtSel()
    
    from BenderExample.JuanFiles2009 import files
    
    evtsel.open( files ) 
    
    run ( -1 )
    
    
    
# =============================================================================
# The END 
# =============================================================================
