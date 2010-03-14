#!/usr/bin/env python
# =============================================================================
## @file BenderExample/RealLam0_V0.py
#
#  The script to analyse the Lambda0 from the V0-stripping
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>``Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis''</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  ``C++ ToolKit for Smart and Friendly Physics Analysis''
#
#  By usage of this code one clearly states the disagreement 
#  with the campain of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2010-02-10
# ===========================================================================================
"""

The script to analyse the Lambda0 from the V0-stripping

This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the campain of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

"""
# ===========================================================================================
__author__   = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__     = "2010-02-10"
__version__  = "CVS Tag $Name: not supported by cvs2svn $, verison $Release:$"
# ===========================================================================================
import ROOT                           ## needed to produce/visualize the histograms
import LHCbMath.Types                 ## easy access to various geometry routines 
from   Bender.Main         import *   ## import all bender goodies 
from   Gaudi.Configuration import *   ## needed for job configuration

from   GaudiKernel.SystemOfUnits     import GeV, MeV, mm, cm 
from   GaudiKernel.PhysicalConstants import c_light
from   GaudiPython.GaudiAlgs         import Vector

# =============================================================================
## @class Lam0
#  Simple algorithm to study Lambda0 -> p pi-
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2010-02-11
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
        
        self.nlam1    = self.counter ( '#Lam_1'  )
        self.nlam2    = self.counter ( '#Lam_2'  )
        self.ltfitter = self.lifetimeFitter()
         
        return SUCCESS
    
    ## the only one esential method: 
    def analyse  (self ) :
        """
        The only one essential method
        """
        
        ## get lambdas from TES:
        
        lams = self.select ( 'lam' , 'Lambda0' == ABSID )        
        self.nlam1 += lams.size()
        
        if lams.empty() :
            return self.Warning ( 'No Lambdas are found' , SUCCESS )
        
        self.nlam2 += lams.size()
        
        primaries = self.vselect ( 'pv' , ISPRIMARY  )
        if primaries.empty() :
            return self.Warning ( 'No primary vertices are found in algo' , SUCCESS )
        
        mips   = MIPCHI2 ( primaries , self.geo() )
        
        odin = self.get('DAQ/ODIN')
        runNum = odin.runNumber   ()
        evtNum = odin.eventNumber () 
        evt1   = int ( evtNum/2**32  ) 
        evt2   = int ( evtNum - 2**32*evt1 ) 
        
        tup  = self.nTuple ( 'Lambda' )        
        
        for lam in lams :
            
            m = M ( lam ) / GeV 
            self.plot ( m , 'Lambda0 - mass 1', 1.05 , 1.25 ) 
            
            p   = lam.child(1) 
            pi  = lam.child(2)
            
            long1 = True if ISLONG ( p   ) else False  
            long2 = True if ISLONG ( pi  ) else False  
            
            down1 = True if ISDOWN ( p   ) else False  
            down2 = True if ISDOWN ( pi  ) else False  
            
            if   long1 and long2 :
                self.plot ( m , 'Lambda0 - mass, LL-1', 1.05 , 1.25 ) 
            elif down1 and down2 :
                self.plot ( m , 'Lambda0 - mass, DD-1', 1.05 , 1.25 )
            else :        
                self.plot ( m , 'Lambda0 - mass, LD-1', 1.05 , 1.25 )
                
            qP  = Q ( p )
            qPi = Q ( pi )
            
            lVertex = lam.endVertex()
            
            chi2 = VCHI2 ( lVertex )
            if not 0 <= chi2 < 25 : continue
            
            self.plot ( M ( lam ) / GeV  , 'Lambda  mass, chi2(vx)<25', 1.05 , 1.25 ) 
            
            z = VZ ( lVertex ) 
            
            if z > 220 * cm : continue

            tup.column ( 'run'     , runNum         )
            tup.column ( 'evt1'    , evt1           )
            tup.column ( 'evt2'    , evt2           )
            tup.column ( 'bxid'    , odin.bunchId() )
            tup.column ( 'bxtype'  , odin.bunchCrossingType() )
            tup.column ( 'trgtype' , odin.triggerType() )
            
            ## get the related primary vertex 
            bpv  = self.bestPV  ( lam )
            if not bpv :
                self.Warning( 'No best-PV is found!', SUCCESS )
                continue
            
            _ltime  = LTIME        ( self.ltfitter , bpv )
            _ltchi2 = LTIMEFITCHI2 ( self.ltfitter , bpv )
            
            ltime  = _ltime  ( lam ) * c_light
            ltchi2 = _ltchi2 ( lam )
            pvz    = VZ ( bpv )
            
            mips1 = mips ( p  ) 
            mips2 = mips ( pi )
            
            if   long1 and long2 :
                self.plot ( m , 'Lambda0 - mass, LL-2', 1.05 , 1.25 ) 
            elif down1 and down2 :
                self.plot ( m , 'Lambda0 - mass, DD-2', 1.05 , 1.25 )
            else :        
                self.plot ( m , 'Lambda0 - mass, LD-2', 1.05 , 1.25 )
                
            tup.column ( "mips0" , mips ( lam ) )
            tup.column ( "mips1" , mips1 )
            tup.column ( "mips2" , mips2 ) 

            self.tupPV ( tup , bpv )
            
            if   down1 and down2 and ltime < 10 * mm : continue
            elif                     ltime <  1 * mm : continue 

            tup.column ( "p0"    , P  ( lam ) / GeV ) 
            tup.column ( "p1"    , P  ( p   ) / GeV ) 
            tup.column ( "p2"    , P  ( pi  ) / GeV ) 
            tup.column ( "pt0"   , PT ( lam ) / GeV ) 
            tup.column ( "pt1"   , PT ( p   ) / GeV ) 
            tup.column ( "pt2"   , PT ( pi  ) / GeV )
            
            tup.column ( "pLam"  , lam . momentum ( ) / GeV ) 
            tup.column ( "pP"    , p   . momentum ( ) / GeV ) 
            tup.column ( "pPi"   , pi  . momentum ( ) / GeV ) 
            
            tup.column ( 'pidp'  , PIDp ( p ) )
            
            tup.column ( "lv01"  , LV01( lam )  )
            tup.column ( "z"     , z     )
            tup.column ( "chi2"  , chi2  )
            
            tup.column ( 'yLam'   , Y   ( lam ) )
            tup.column ( 'yP'     , Y   ( p   ) )
            tup.column ( 'yPi'    , Y   ( pi  ) )
            
            tup.column ( 'y0Lam'  , Y0  ( lam ) )
            tup.column ( 'y0P'    , Y0  ( p   ) )
            tup.column ( 'y0Pi'   , Y0  ( pi  ) )
            
            tup.column ( 'etaLam' , ETA ( lam ) )
            tup.column ( 'etaP'   , ETA ( p   ) )
            tup.column ( 'etaPi'  , ETA ( pi  ) )
            
            tup.column ( 'long1'  , long1 )
            tup.column ( 'long2'  , long2 )
            
            tup.column ( 'down1'  , down1 )
            tup.column ( 'down2'  , down2 )
            
            tup.column ( 'trgh1'  , TRGHP( p   ) )
            tup.column ( 'trgh2'  , TRGHP( pi  ) )
            
            tup.column ( 'trchi2dof1' , TRCHI2DOF ( p   ) )
            tup.column ( 'trchi2dof2' , TRCHI2DOF ( pi  ) )
            
            tup.column ( 'ctau'  , ltime  )
            tup.column ( 'tchi2' , ltchi2 )
            tup.column ( 'pvz'   , pvz    )
            
            tup.column ( "m"  , m  )
            tup.column ( "qP" , qP )
            
            tup.write  ( )

            self.plot ( m , 'p pi mass 5', 1.05 , 1.25 ) 
            
            if  min ( mips1 , mips2 ) < 25  : continue
            if  ltchi2 > 49                 : continue
            
            if   long1 and long2 :
                self.plot ( m , 'Lambda0 mass, LL' , 1.05 , 1.25 , 200 )    
            elif down1 and down2 :
                self.plot ( m , 'Lambda0 mass, DD' , 1.05 , 1.25 , 200 )
            else                 :
                self.plot ( m , 'Lambda0 mass, LD' , 1.05 , 1.25 , 200 )
                    
        return SUCCESS 

    
    def tupPV ( self , tup , pv ) :
        """
        Add information about PV:
        """
        chi2   = -100000
        pts    = Vector()
        typ    = Vector()
        ptmin  =  1.e+6 
        ptmax  = -1.e+6
        
        nLong  = int(0)
        
        if not not pv :
            chi2   = VCHI2 ( pv )
            tracks = pv.tracks()
            for t in tracks :
                if not t : continue 
                pts.push_back ( t.pt   () )
                typ.push_back ( t.type () )
                if t.pt () < ptmin : ptmin = t.pt()
                if t.pt () > ptmax : ptmax = t.pt()
                ty = t.type()
                if ty in ( LHCb.Track.Velo     ,
                           LHCb.Track.VeloR    ,
                           LHCb.Track.Backward ,
                           LHCb.Track.Upstream ,
                           LHCb.Track.Long     ) : nLong += 1 
                
        tup.column ( 'pvchi2'  , chi2  )
        tup.column ( 'trptmin' , ptmin ) 
        tup.column ( 'trptmax' , ptmax ) 
        tup.column ( 'nLong'   , nLong ) 
        tup.farray ( 'trpt'    , pts   , 'nTrk' , 100 ) 
        tup.farray ( 'trtyp'   , typ   , 'nTrk' , 100 ) 
                     
        return SUCCESS 
    
    ## finalize & print histos 
    def finalize ( self ) :
        """
        Finalize & print histos         
        """
        histos = self.Histos()
        for key in histos :
            h = histos[key]
            if hasattr ( h , 'dump' ) : print h.dump(50,30,True)
        return Algo.finalize ( self )


# =============================================================================
## configure the job 
def configure ( datafiles , catalogs = [] ) :
    """
    Job configuration 
    """

    ##
    ## 1. Static configuration using "Configurables"
    ##
    
    from Configurables           import DaVinci    
    davinci = DaVinci (
        DataType      = '2009' ,
        Simulation    = False  ,
        PrintFreq     = 1000   ,
        HistogramFile = 'RealLam0_V0_Histos.root' 
        )
    
    from GaudiConf.Configuration import NTupleSvc   
    NTupleSvc (
        Output = [ "LAM0 DATAFILE='RealLam0_V0.root' TYPE='ROOT' OPT='NEW'" ]
        )
    
    ## define the input data:
    setData ( datafiles , catalogs )
    
    ##
    ## 2. Jump into the wonderful world of the actual Gaudi components!
    ## 
    
    ## get the actual application manager (create if needed)
    gaudi = appMgr() 
    
    ## create local algorithm:
    alg = Lam0(
        'Lam0'             ,   ## Algorithm name
        NTupleLUN = 'LAM0' ,   ## Logical unit for output file with N-tuples
        ## 
        InputLocations = [ '/Event/Strip/Phys/StrippingLambda0' ] ## input particles 
        )
    
    gaudi.setAlgorithms ( [ alg ] ) 
    
    return SUCCESS 


# =============================================================================
# The actual job steering
if '__main__' == __name__ :

    ## make printout of the own documentation
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print ' dir(%s) : %s ' % ( __name__    , dir() )
    print '*'*120  
    
    configure ('/castor/cern.ch/user/p/pkoppenb/DATA2009/000000.V0.dst') 
    
    run ( -1 )
    

# =============================================================================
# The END 
# =============================================================================
