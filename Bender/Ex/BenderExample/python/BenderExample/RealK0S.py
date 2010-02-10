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

import ROOT                           ## needed to produce/visualize the histograms
from   Bender.Main         import *   ## import all bender goodies 
import LHCbMath.Types                 ## easy access to various geometry routines 
from   Gaudi.Configuration import *   ## needed for job configuration

from   GaudiKernel.SystemOfUnits     import GeV, MeV, mm, cm 
from   GaudiKernel.PhysicalConstants import c_light
from   GaudiPython.GaudiAlgs         import Vector


from   BenderExample.RealLam0        import Lam0 
# ============================================================================================
## @class Ks
#  Simple algorithm to study K0s -> pi+ pi-
#  @author Alexander KOZLINSKY  akozlins@gmail.com
#  @author Thomas    BAUER       thomas@nikhef.nl
#  @author Vanya     BELYAEV    vanya@nikhef.nl
#  @date 2009-12-21
class Ks(Lam0) :
    """
    Simple algorithm to study K0s -> pi+ pi-
    """    
    def initialize ( self ) :
        """
        Initialization
        """
        sc = Lam0.initialize ( self )
        if sc.isFailure() : return sc

        self.npip = self.counter ( '#nPi+' )
        self.npim = self.counter ( '#nPi-' )

        return SUCCESS
    
    ## the only one esential method: 
    def analyse  (self ) :
        """
        The only one essential method
        """
        
        primaries = self.vselect( 'PVs' , ISPRIMARY )
        if primaries.empty() :
            return self.Warning('No primary vertices are found', SUCCESS )

        
        goodTrack = ( P > 1.5 * GeV ) & ( TRCHI2DOF < 10 )
        
        pips = self.select( 'pi+' , ( 'pi+' == ID ) & goodTrack )
        pims = self.select( 'pi-' , ( 'pi-' == ID ) & goodTrack ) 
        
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
          if not 0 <= chi2 < 25 : continue
          
          self.plot ( m12 , 'pi pi mass, chi2(vx)<25', 0.4 , 0.6 ) 

          m = M(ks) / GeV;
          if not 0.4 < m < 0.6 : continue
          
          self.plot ( m , 'pi pi mass, correct mass', 0.4 , 0.6 ) 

          z = VZ(ks) / mm 

          if z > 220 * cm : continue
          
          pi1  = ks(1)
          pi2  = ks(2)

          mips1 = mips ( pi1 ) 
          mips2 = mips ( pi2 )

          if min ( mips1 , mips2 ) < 25  : continue
          
          long1 = True if ISLONG ( pi1 ) else False  
          long2 = True if ISLONG ( pi2 ) else False  

          down1 = True if ISDOWN ( pi1 ) else False  
          down2 = True if ISDOWN ( pi2 ) else False  


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

          
          self.tupPV ( tup , bpv )
 
          if  down1 and down2 and ltime < 10 * mm : continue
          elif                    ltime <  1 * mm : continue 

          if ltchi2 > 49                         : continue

          tup.column ( "mips0" , mips ( ks  ) )
          tup.column ( "mips1" , mips1  )
          tup.column ( "mips2" , mips2  ) 

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
          
          tup.column ( 'yKs'    , Y   ( ks  ) )
          tup.column ( 'yPi1'   , Y   ( pi1 ) )
          tup.column ( 'yPi2'   , Y   ( pi2 ) )
          
          tup.column ( 'y0Ks'   , Y0  ( ks  ) )
          tup.column ( 'y0Pi1'  , Y0  ( pi1 ) )
          tup.column ( 'y0Pi2'  , Y0  ( pi2 ) )
                    
          tup.column ( 'etaKs'  , ETA ( ks  ) )
          tup.column ( 'etaPi1' , ETA ( pi1 ) )
          tup.column ( 'etaPi2' , ETA ( pi2 ) )
   

          tup.column ( 'long1'  , long1 )
          tup.column ( 'long2'  , long2 )

          tup.column ( 'down1'  , down1 )
          tup.column ( 'down2'  , down2 )

          tup.column ( 'trghp1'     , TRGHP ( pi1 ) )
          tup.column ( 'trghp2'     , TRGHP ( pi2 ) )

          tup.column ( 'trchi2dof1' , TRCHI2DOF ( pi1 ) )
          tup.column ( 'trchi2dof2' , TRCHI2DOF ( pi2 ) )

          
          tup.column ( 'ctau'  , ltime  )
          tup.column ( 'tchi2' , ltchi2 )
          tup.column ( 'pvz'   , pvz    )

          goodTT1 = self.goodTT ( pi1 )
          goodTT2 = self.goodTT ( pi2 )

          tup.column ( "tt1" , goodTT1 )
          tup.column ( "tt2" , goodTT2 )
          
          tup.column ( "m" , m )
          
          tup.write  ( )

          if   long1 and long2 :
              if chi2 < 25 and 1  <= ltime and 25 <= min(mips1,mips2) :  
                  self.plot ( m , 'pi pi mass, LL', 0.4 , 0.6 , 200 )
          elif down1 and down2 :
              if chi2 < 25 and 10 <= ltime and 25 <= min(mips1,mips2) :  
                  self.plot ( m , 'pi pi mass, DD', 0.4 , 0.6 , 200 )
          elif down1 and down2 :
              if chi2 < 25 and  1 <= ltime and 25 <= min(mips1,mips2) :  
                  self.plot ( m , 'pi pi mass, LD', 0.4 , 0.6 , 200 )
              
        return SUCCESS 

        
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
    
    from Configurables import NoPIDsParticleMaker    
    for nam in ( 'StdNoPIDsPions'       ,
                 'StdNoPIDsDownPions'   ) :
        alg = NoPIDsParticleMaker ( nam )
        alg.InputPrimaryVertices = 'Strip/Rec/Vertex/Primary' 
        
    gaudi = appMgr()
        
    alg = Ks (
        'Ks'              ,   ## Algorithm name
        NTupleLUN = 'K0S' ,   ## Logical unit for output file with N-tuples        
        ## primary vertices for stripping: 
        InputPrimaryVertices = 'Strip/Rec/Vertex/Primary' ,
        InputLocations = [ 'StdNoPIDsPions'     ,
                           'StdNoPIDsDownPions' ] ## input particles 
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
    
    pfn = '/castor/cern.ch/grid/lhcb/data/2009/DST/00005848/0000/00005848_0000000%d_1.V0.dst'
    files = [ pfn % i for i in range(1,7) ]
    print files
    
    ##from BenderExample.JuanFiles2009 import files 
    
    evtsel.open( files ) 
    
    run ( -1 )
    
    myalg = gaudi.algorithm ( 'Ks' )
    
    import GaudiPython.HistoUtils
    histos = myalg.Histos()
    for (k,h) in histos.items() :
        if hasattr ( h , 'dump' ) :
            print h.dump ( 50 , 30 , False )
        
# =============================================================================
# The END 
# =============================================================================
