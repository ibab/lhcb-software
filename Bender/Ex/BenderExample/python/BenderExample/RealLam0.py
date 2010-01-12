#!/usr/bin/env python
# ==========================================================================================
## @file BenderExample/RealLam0.py
#
#  A little bit modified version of original script by Alexander KOZLINSKY  and
#  Thomas BAUER to look for Lambda0 -> p pi- peak on real data
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
Thomas BAUER to look for Lambda0 -> p pi- peak on real data

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

# =============================================================================
## @class Lam0
#  Simple algorithm to study Lambda0 -> p pi-
#  @author Alexander KOZLINSKY  akozlins@gmail.com
#  @author Thomas    BAUER       thomas@nikhef.nl
#  @author Vanya     BELYAEV    vanya@nikhef.nl
#  @date 2009-12-21
class Lam0(Algo) :
    """
    Simple algorithm to study Lam0 -> p pi-
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
        
        primaries = self.vselect( 'PVs' , ISPRIMARY )
        if primaries.empty() :
            return self.Warning('No primary vertices are found', SUCCESS )

        pions   = self.select( 'pion'    , 'pi+' == ABSID )
        protons = self.select( 'p'       , 'p+'  == ABSID )
        
        if pions.empty() or protons.empty() : return SUCCESS

        self.npv  += 1
        
        tup  = self.nTuple ( 'Lambda' )        
        
        mips = MIPCHI2( primaries, self.geo() ) 

        lams = self.loop ('p pion' , 'Lambda0')

        dm = ADMASS ( 'Lambda0' ) 
        for lam in lams :
            
          m12 = lam.mass(1, 2) / GeV
          if m12 > 1.5 : continue
          
          self.plot ( m12 , 'p pi mass 1', 1.05 , 1.25 ) 

          p   = lam(1)
          pi  = lam(2)

          qP  = Q ( p )
          qPi = Q ( pi )

          if 0 < qP : lam.setPID ( 'Lambda0'  )
          else      : lam.setPID ( 'Lambda~0' )
          
          # check the charge:
          if not -0.5 < ( qP + qPi ) < 0.5 : continue
          
          self.plot ( m12 , 'p pi mass 2', 1.05 , 1.25 ) 

          p   = lam(1)
          chi2 = VCHI2(lam)
          if not 0 <= chi2 < 100 : continue

          self.plot ( m12 , 'p pi mass 3', 1.05 , 1.25 ) 
          
          m = M(lam) / GeV;
          if m > 1.3 : continue
          
          self.plot ( m , 'p pi mass 4', 1.05 , 1.25 ) 

          z = VZ(lam) / mm 

          mips1 = mips ( p  ) 
          mips2 = mips ( pi )
          
          tup.column ( "mips0" , mips ( lam ) )
          tup.column ( "mips1" , mips1 )
          tup.column ( "mips2" , mips2 ) 

          tup.column ( "p0"    , P  ( lam ) / GeV ) 
          tup.column ( "p1"    , P  ( p   ) / GeV ) 
          tup.column ( "p2"    , P  ( pi  ) / GeV ) 
          tup.column ( "pt0"   , PT ( lam ) / GeV ) 
          tup.column ( "pt1"   , PT ( p   ) / GeV ) 
          tup.column ( "pt2"   , PT ( pi  ) / GeV )
          
          tup.column ( "pLam"  , lam . momentum ( 0 ) / GeV ) 
          tup.column ( "pP"    , p   . momentum (   ) / GeV ) 
          tup.column ( "pPi"   , pi  . momentum (   ) / GeV ) 

          tup.column ( 'pidp'  , PIDp ( p ) )
          
          tup.column ( "lv01"  , LV01( lam )  )
          tup.column ( "z"     , z     )
          tup.column ( "chi2"  , chi2  )

          tup.column ( 'lmu1' , ISLOOSEMUON( p  ) )
          tup.column ( 'lmu2' , ISLOOSEMUON( pi ) )

          tup.column ( 'mu1'  , ISMUON( p  ) )
          tup.column ( 'mu2'  , ISMUON( pi ) )

          long1 = True if ISLONG ( p   ) else False  
          long2 = True if ISLONG ( pi  ) else False  

          tup.column ( 'long1'  , long1 )
          tup.column ( 'long2'  , long2 )

          down1 = True if ISDOWN ( p   ) else False  
          down2 = True if ISDOWN ( pi  ) else False  
          
          tup.column ( 'down1'  , down1 )
          tup.column ( 'down2'  , down2 )

          tup.column ( 'trgh1'    , TRGHP( p   ) )
          tup.column ( 'trgh2'    , TRGHP( pi  ) )

          tup.column ( 'trchi2dof1' , TRCHI2DOF ( p   ) )
          tup.column ( 'trchi2dof2' , TRCHI2DOF ( pi  ) )

          bpv = self.bestPV ( primaries, lam.particle() )
          if not not bpv :
              _ltime  = LTIME        ( self.ltfitter , bpv )
              _ltchi2 = LTIMEFITCHI2 ( self.ltfitter , bpv )

              ltime  = _ltime  ( lam ) * c_light
              ltchi2 = _ltchi2 ( lam )
              pvz    = VZ ( bpv ) 
          else :
              ltime  = -1.e+100
              ltchi2 = -1.e+100
              pvz    = -1.e+100

          tup.column ( 'ctau'  , ltime  )
          tup.column ( 'tchi2' , ltchi2 )
          tup.column ( 'pvz'   , pvz    )
          
          tup.column ( "m"  , m  )
          tup.column ( "qP" , qP )
          
          tup.write  ( )

          self.plot ( m , 'p pi mass 5', 1.05 , 1.25 ) 

          if long1 and long1 :
              if chi2 < 25 and  5 <= ltime and 25 <= min(mips1,mips2) :  
                  self.plot ( m , 'Lambda0 mass, LL' , 1.05 , 1.25 , 200 )
                  if dm ( lam ) < 0.010 : lam.save( 'lam' )
                  if ltchi2 <64 and ltime <100  : 
                      self.plot ( m , 'Lambda0 mass, LL-2' , 1.05 , 1.25 , 200 )
          
          if down1 and down2 :
              if chi2 < 25 and 20 <= ltime and 49<= min(mips1,mips2) :  
                  self.plot ( m , 'Lambda0 mass, DD' , 1.05 , 1.25 , 200 ) 
                  if dm ( lam ) < 0.015 : lam.save( 'lam' )
                  if ltchi2 <49 and ltime <250  : 
                      self.plot ( m , 'Lambda0 mass, DD-2' , 1.05 , 1.25 , 200 )
          
          lambdas = self.selected('lam0')

          self.setFilterPassed ( not lambdas.empty() )
          
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
        HistogramFile = 'RealLam0_Histos.root' 
        )
    
    if datafiles : 
        EventSelector (
            Input     = datafiles
            )

    NTupleSvc (
        Output = [ "LAM0 DATAFILE='RealLambda.root' TYPE='ROOT' OPT='NEW'" ]
        )
    
    ## tuning of 'loose' PV recontruction by Mariusz: 
    importOptions("$PATPVROOT/options/PVLoose.py")
    
    
    gaudi = appMgr()
    
    alg = Lam0(
        'Lam0'             ,   ## Algorithm name
        NTupleLUN = 'LAM0' ,   ## Logical unit for output file with N-tuples 
        InputLocations = [ 'StdNoPIDsPions'        ,
                           'StdNoPIDsDownPions'    ,
                           'StdNoPIDsProtons'      ,
                           'StdNoPIDsDownProtons'  ] ## input particles 
        )
    
    #gaudi.setAlgorithms ( [ 'PatPVOffline' , alg ] ) 
    gaudi.setAlgorithms ( [ alg ] ) 
    
    return SUCCESS 


data =  [
    '/castor/cern.ch/user/j/jpalac/72/0/outputdata/Data2009.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/72/1/outputdata/Data2009.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/72/2/outputdata/Data2009.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/72/3/outputdata/Data2009.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/72/4/outputdata/Data2009.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/72/5/outputdata/Data2009.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/72/6/outputdata/Data2009.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/72/7/outputdata/Data2009.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/72/8/outputdata/Data2009.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/72/9/outputdata/Data2009.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/72/10/outputdata/Data2009.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/72/11/outputdata/Data2009.PhysEvent.dst'
    ]

files_5730 = [
    '/castor/cern.ch/user/j/jpalac/74/0/outputdata/Prod5730.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/74/1/outputdata/Prod5730.PhysEvent.dst'
    ]

files_5731 = [
    '/castor/cern.ch/user/j/jpalac/76/0/outputdata/Prod5731.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/76/1/outputdata/Prod5731.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/76/2/outputdata/Prod5731.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/76/3/outputdata/Prod5731.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/76/4/outputdata/Prod5731.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/76/5/outputdata/Prod5731.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/76/6/outputdata/Prod5731.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/76/7/outputdata/Prod5731.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/76/8/outputdata/Prod5731.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/76/9/outputdata/Prod5731.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/76/10/outputdata/Prod5731.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/76/11/outputdata/Prod5731.PhysEvent.dst'
    ]

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
    
    myalg = gaudi.algorithm ( 'Lam0' )

    import GaudiPython.HistoUtils
    histos = myalg.Histos()
    for (k,h) in histos.items() :
        if hasattr ( h , 'dump' ) :
            print h.dump ( 50 , 25 , True )
        

# =============================================================================
# The END 
# =============================================================================
