#!/usr/bin/env python
# ==========================================================================================
## @file BenderExample/RealK0S.py
#
#  A little bit modified version of original script by Alexander KOZLINSKY  and
#  Thomas BAUER to look for K0S -> pi+ pi- peak on real data
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
#  @author Alexander KOZLINSKY  akozlins@gmail.com
#  @author Thomas    BAUER       thomas@nikhef.nl
#  @author Vanya     BELYAEV    vanya@nikhef.nl
# ===========================================================================================
"""

A little bit modified version of original script by Alexander KOZLINSKY and
Thomas BAUER to look for K0s -> pi+ pi- peak on real data

"""
# ===========================================================================================
__author__   = "Alexander KOZLINSKY, Thomas BAUER & Vanya BELYAEV "
__version__  = "CVS Tag $Name: not supported by cvs2svn $, verison $Release:$"
# ===========================================================================================

## import ROOT                           ## needed to produce/visualize the histograms
from   Bender.Main         import *   ## import all bender goodies 
import LHCbMath.Types                 ## easy access to various geometry routines 
from   Gaudi.Configuration import *   ## needed for job configuration

from   GaudiKernel.SystemOfUnits     import GeV, MeV, mm 
from   GaudiKernel.PhysicalConstants import c_light

# ============================================================================================
## @class Ks
#  Simple algorithm to study K0s -> pi+ pi-
#  @author Alexander KOZLINSKY  akozlins@gmail.com
#  @author Thomas    BAUER       thomas@nikhef.nl
#  @author Vanya     BELYAEV    vanya@nikhef.nl
#  @date 2009-12-21
class Ks(Algo) :
    """
    Simple algorithm to study K0s -> pi+ pi-
    """
    
    def initialize ( self ) :
        """
        Initialization
        """
        sc = Algo.initialize ( self )
        if sc.isFailure() : return sc

        self.npv  = self.counter ( '#nPV'  )
        self.npip = self.counter ( '#nPi+' )
        self.npim = self.counter ( '#nPi-' )

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

        pips = self.select( 'pi+' , 'pi+' == ID)
        pims = self.select( 'pi-' , 'pi-' == ID)
        
        if pips.empty() or pims.empty() : return SUCCESS
        
        self.npv  += 1
        self.npip += pips.size()
        self.npim += pims.size()        

        tup  = self.nTuple ( 'Ks' )        

        mips = MIPCHI2( primaries, self.geo() ) 

        kss = self.loop ('pi+ pi-' , 'KS0')
        
        for ks in kss :
            
          m12 = ks.mass(1, 2) / GeV
          if not 0.0 < m12 < 1.0 : continue

          self.plot ( m12 , 'pi pi mass 1', 0.4 , 0.6 ) 
          
          chi2 = VCHI2(ks)
          if not 0 <= chi2 < 100 : continue
          
          self.plot ( m12 , 'pi pi mass 2', 0.4 , 0.6 ) 

          m = M(ks) / GeV;
          if not 0.0 < m   < 0.6 : continue
          
          self.plot ( m , 'pi pi mass 3', 0.4 , 0.6 ) 

          z = VZ(ks) / mm 

          pi1  = ks(1)
          pi2  = ks(2)
          
          tup.column ( "mips0" , mips ( ks  ) )
          tup.column ( "mips1" , mips ( pi1 ) )
          tup.column ( "mips2" , mips ( pi2 ) ) 

          tup.column ( "p0"    , P  ( ks  ) / GeV ) 
          tup.column ( "p1"    , P  ( pi1 ) / GeV ) 
          tup.column ( "p2"    , P  ( pi2 ) / GeV ) 
          tup.column ( "pt0"   , PT ( ks  ) / GeV ) 
          tup.column ( "pt1"   , PT ( pi1 ) / GeV ) 
          tup.column ( "pt2"   , PT ( pi2 ) / GeV )
          #
          tup.column ( "pKs"   , ks  . momentum ( 0 ) ) 
          tup.column ( "pPi1"  , pi1 . momentum (   ) ) 
          tup.column ( "pPi2"  , pi2 . momentum (   ) ) 

          tup.column ( "lv01"  , LV01 ( ks )  )
          tup.column ( "z"     , z     )
          tup.column ( "chi2"  , chi2  )

          tup.column ( 'lmu1'  , ISLOOSEMUON ( pi1 ) )
          tup.column ( 'lmu2'  , ISLOOSEMUON ( pi2 ) )

          tup.column ( 'mu1'   , ISMUON ( pi1 ) )
          tup.column ( 'mu2'   , ISMUON ( pi2 ) )

          tup.column ( 'inmu1' , INMUON ( pi1 ) )
          tup.column ( 'inmu2' , INMUON ( pi2 ) )

          long1 = True if ISLONG ( pi1 ) else False  
          long2 = True if ISLONG ( pi2 ) else False  

          tup.column ( 'long1'  , long1 )
          tup.column ( 'long2'  , long2 )

          down1 = True if ISDOWN ( pi1 ) else False  
          down2 = True if ISDOWN ( pi2 ) else False  

          tup.column ( 'down1'  , down1 )
          tup.column ( 'down2'  , down2 )

          tup.column ( 'trghp1'     , TRGHP ( pi1 ) )
          tup.column ( 'trghp2'     , TRGHP ( pi2 ) )

          tup.column ( 'trchi2dof1' , TRCHI2DOF ( pi1 ) )
          tup.column ( 'trchi2dof2' , TRCHI2DOF ( pi2 ) )

          bpv = self.bestPV ( primaries, ks.particle() )
          if not not bpv :
              _ltime  = LTIME        ( self.ltfitter , bpv )
              _ltchi2 = LTIMEFITCHI2 ( self.ltfitter , bpv )

              ltime  = _ltime  ( ks ) * c_light
              ltchi2 = _ltchi2 ( ks )
              pvz    = VZ ( bpv ) 
          else :
              ltime  = -1.e+100
              ltchi2 = -1.e+100
              pvz    = -1.e+100

          tup.column ( 'ctau'  , ltime  )
          tup.column ( 'tchi2' , ltchi2 )
          tup.column ( 'pvz'   , pvz    )
          
          tup.column ( "m" , m )
          
          tup.write  ( )

          ## good vertex & good pointing to primary vertex 
          if chi2 < 9 and ltchi2 < 16 :  
              self.plot ( m , 'pi pi mass 4', 0.4 , 0.6 , 200 )
              ## LL-case and lifetime cut 
              if    long1 and long2 and  5 < ltime < 80 :
                  self.plot ( m , 'pi pi mass, LL, 5<ctau<80 ', 0.4 , 0.6 , 200 ) 
              ## DD-case and lifetime cut 
              elif  down1 and down2 and  5 < ltime < 80 :
                  self.plot ( m , 'pi pi mass, DD, 5<ctau<80 ', 0.4 , 0.6 , 200 ) 
              
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
        HistogramFile = 'RealK0S_Histos.root' 
        )
    
    if datafiles : 
        EventSelector (
            Input     = datafiles
            )

    NTupleSvc (
        Output = [ "K0S DATAFILE='RealK0S.root' TYPE='ROOT' OPT='NEW'" ]
        )
    
    ## tuning of 'loose' PV recontruction by Mariusz: 
    importOptions("$PATPVROOT/options/PVLoose.py")
    
    
    gaudi = appMgr()
    
    alg = Ks(
        'Ks'              ,   ## Algorithm name
        NTupleLUN = 'K0S' ,   ## Logical unit for output file with N-tuples 
        InputLocations = [ 'StdNoPIDsPions', 'StdNoPIDsDownPions' ] ## input particles 
        )
    
    #gaudi.setAlgorithms ( [ 'PatPVOffline' , alg ] ) 
    gaudi.setAlgorithms ( [ alg ] ) 
    
    return SUCCESS 


# =============================================================================
# The actual job steering
if '__main__' == __name__ :

    from Configurables  import EventSelector    
    EventSelector( PrintFreq = 1000 )
    
    ## configure with the empty list of input data 
    configure ( [] )
    
    gaudi = appMgr()
    
    evtsel = gaudi.evtSel()

    from BenderExample.JuanFiles2009 import files 
    
    evtsel.open( files ) 
          
    run ( -1 )
    
    myalg = gaudi.algorithm ( 'Ks' )

    import GaudiPython.HistoUtils
    histos = myalg.Histos()
    for (k,h) in histos.items() :
        if hasattr ( h , 'dump' ) :
            print h.dump ( 50 , 30 , True )
        
# =============================================================================
# The END 
# =============================================================================
