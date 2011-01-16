#!/usr/bin/env python
# ==========================================================================================
# $Id$ 
# ==========================================================================================
## @file BenderExample/RealK0S.py
#
#  A little bit modified version of original script by Alexander KOZLINSKY  and
#  Thomas BAUER to look for K0S -> pi+ pi- peak on real data
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
#  @author Alexander KOZLINSKY  akozlins@gmail.com
#  @author Thomas    BAUER       thomas@nikhef.nl
#  @author Vanya     BELYAEV    vanya@nikhef.nl
# ===========================================================================================
"""

A little bit modified version of original script by Alexander KOZLINSKY and
Thomas BAUER to look for K0s -> pi+ pi- peak on real data

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
__author__   = "Alexander KOZLINSKY, Thomas BAUER & Vanya BELYAEV "
__date__     = " 2009-12-?? "
__version__  = "CVS Tag $Name: not supported by cvs2svn $, verison $Release:$"
# ===========================================================================================
import ROOT                           ## needed to produce/visualize the histograms
from   Bender.MainMC       import *   ## import all bender goodies 
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
        
        from LoKiPhys.decorators import ALL
        
        self.trueKs  = ALL
        self.truePi1 = ALL
        self.truePi2 = ALL
        
        return SUCCESS
    
    ## the only one esential method: 
    def analyse  (self ) :
        """
        The only one essential method
        """
        
        if  1 == self.nevent % 1000 :
            hdr = self.get('Rec/Header')
            print 'HEADER ', self.nevent , '\n' , hdr         
        self.nevent += 1

        odin = self.get('DAQ/ODIN')
        runNum = odin.runNumber   ()

        if self.DATA :
            rcnt  = self.counter('#run_%d'    % runNum )
            rcnt += 1 
        
        primaries = self.vselect( 'PVs' , ISPRIMARY )
        if primaries.empty() :
            return self.Warning('No primary vertices are found', SUCCESS )
        
        if self.DATA :
            rcnt  = self.counter('#run_%d_pv' % runNum )
            rcnt += 1 
        
        goodTrack = ( P > 2.0 * GeV ) & ( TRCHI2DOF < 25 ) 
        
        pips = self.select ( 'pi+' , ( 'pi+' == ID ) & goodTrack & self.truePi1 )
        if pips.empty() : return SUCCESS
        
        pims = self.select ( 'pi-' , ( 'pi-' == ID ) & goodTrack & self.truePi2 )
        if pims.empty() : return SUCCESS
                
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

            self.plot ( m12 , 'pi pi mass, chi2(vx)<100', 0.4 , 0.6 ) 
            
            m = M(ks) / GeV;
            if not 0.4 < m < 0.6 : continue
            
            self.plot ( m , 'pi pi mass, correct mass', 0.4 , 0.6 ) 
            
            z = VZ(ks) / mm 
            
            if z > 220 * cm : continue
            
            pi1  = ks(1)
            pi2  = ks(2)
            
            mips1 = mips ( pi1 ) 
            mips2 = mips ( pi2 )
            
            long1 = True if ISLONG ( pi1 ) else False  
            long2 = True if ISLONG ( pi2 ) else False  
            
            down1 = True if ISDOWN ( pi1 ) else False  
            down2 = True if ISDOWN ( pi2 ) else False  
            
            bpv = self.bestPV ( primaries, ks.particle() )
            
            if not bpv : continue
            
            _ltime  = LTIME        ( self.ltfitter , bpv )
            _ltchi2 = LTIMEFITCHI2 ( self.ltfitter , bpv )
            
            ltime  = _ltime  ( ks ) * c_light
            ltchi2 = _ltchi2 ( ks )
            pvz    = VZ ( bpv ) 
            pvx    = VX ( bpv ) 
            pvy    = VY ( bpv ) 
            
            ok    = False 
            if not self.DATA :
                ok = self.trueKs ( ks )
            
            if ok :
                self.plot ( m , 'pi pi mass MC (1)', 0.4 , 0.6 ) 
                self.plot ( ltime                     , 'ltime   (MC)' , 0 , 200 , 200 )
                self.plot ( min(ltchi2,199)           , 'ltchi2  (MC)' , 0 , 200 , 200 )
                self.plot ( min(chi2,199)             , 'chi2    (MC)' , 0 , 200 , 200 )
                self.plot ( min(min(mips1,mips2),199) , 'mips    (MC)' , 0 , 200 , 200 )
          
            if  down1 and down2 and ltime < 10 * mm : continue
            elif                    ltime <  1 * mm : continue 

            if ok : self.plot ( m , 'pi pi mass MC (2)', 0.4 , 0.6 ) 
            
            if ltchi2 > 100                         : continue

            if ok : self.plot ( m , 'pi pi mass MC (3)', 0.4 , 0.6 ) 

            ## "cuts"
            if ltchi2 > 49                           : continue

            if ok : self.plot ( m , 'pi pi mass MC (4)', 0.4 , 0.6 ) 
            
            if chi2   > 25                           : continue
            
            if ok : self.plot ( m , 'pi pi mass MC (5)', 0.4 , 0.6 ) 

            if min ( mips1 , mips2 )  < 25           : continue

            if ok : self.plot ( m , 'pi pi mass MC (6)', 0.4 , 0.6 ) 
            
            if   down1 and down2 and ltime < 10 * mm : continue
            elif                     ltime <  1 * mm : continue
            
            if ok : self.plot ( m , 'pi pi mass MC (7)', 0.4 , 0.6 ) 

            self.tupPV   ( tup , bpv )
            self.tupODIN ( tup )
            self.tupL0   ( tup )
            self.tisTos  ( tup , ks  )             
 
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
            tup.column ( 'pvx'   , pvx    )
            tup.column ( 'pvy'   , pvy    )
            
            tup.column ( "m" , m )
          
            tup.column ( 'truePi1' , self.truePi1   ( pi1 ) )
            tup.column ( 'truePi2' , self.truePi    ( pi2 ) )


            _OK = int ( self.trueKs ( ks ) ) 
            tup.column ( 'trueKs'  , _OK )

            tup.column ( 'diffr' , self.diffr )
            
            tup.column ( 'data' , int ( self.DATA ) )
            
            ## convert momentum into CMS system:
            np = self.toCMS ( ks.momentum ( 0 )  ) / GeV 
            tup.column ( 'yCMS'  , np.Rapidity () )
            tup.column ( 'ptCMS' , np.Pt       () )
         
            tup.write  ( )
          
            if   long1 and long2 :
                self.plot ( m , 'Ks  mass, LL' , 0.4  , 0.6 , 200 )
            elif down1 and down2 :
                self.plot ( m , 'Ks  mass, DD' , 0.4  , 0.6 , 200 )
            else                 :
                self.plot ( m , 'Ks  mass, LD' , 0.4  , 0.6 , 200 )

            if not self.DATA and _OK :
                self.plot ( m , 'Ks  mass, MC' , 0.4  , 0.6 , 200 )
                
            ks.save('ks')


        k0s = self.selected('ks')

        self.setFilterPassed ( not k0s.empty() )        
          
        return SUCCESS 

        
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
        EvtMax        = -1     , 
        HistogramFile = 'RealK0S_Histos.root' 
        )
    
    from GaudiConf.Configuration import NTupleSvc
    NTupleSvc (
        Output = [ "K0S DATAFILE='RealK0S_GR.root' TYPE='ROOT' OPT='NEW'" ]
        )
    
    ## trigger-related stuff, many thanks to Thomas Ruf
    
    from Configurables import  ( DataOnDemandSvc         ,
                                 HltDecReportsDecoder    ,
                                 HltSelReportsDecoder    ,
                                 HltVertexReportsDecoder )
    
    dod = DataOnDemandSvc ( Dump = True )
    
    dod.AlgMap[ "Trig/L0/Calo"     ] = "L0CaloCandidatesFromRaw/L0CaloFromRaw" 
    dod.AlgMap[ "Trig/L0/FullCalo" ] = "L0CaloCandidatesFromRaw/L0CaloFromRaw" 
    dod.AlgMap[ "Hlt/DecReports"   ] = HltDecReportsDecoder(OutputLevel = 4)
    dod.AlgMap[ "Hlt/SelReports"   ] = HltSelReportsDecoder(OutputLevel = 4)
    dod.AlgMap[ "Hlt/VertexReports"] = HltVertexReportsDecoder( OutputLevel = 4)
    
    from Configurables import TriggerTisTos
    TriggerTisTos  ( TriggerInputWarnings = True ) 

    ## define the input data:
    setData ( datafiles , catalogs )
    
    ##
    ## 2. Jump into the wonderful world of the actual Gaudi components!
    ## 
    
    ## get the actual application manager (create if needed)
    gaudi = appMgr() 
    
    ## create local algorithm:
    alg = Ks (
        'Ks'              ,   ## Algorithm name
        NTupleLUN = 'K0S' ,   ## Logical unit for output file with N-tuples        
        InputLocations = [ 'StdNoPIDsPions'     ,
                           'StdNoPIDsDownPions' ] ## input particles 
        )
    
    gaudi.setAlgorithms ( [ alg ] ) 
    
    return SUCCESS


# =============================================================================
# The actual job steering
if '__main__' == __name__ :
    
    ## make printout of the own documentations 
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print ' dir(%s) : %s ' % ( __name__    , dir() )
    print '*'*120  

    import BenderExample.Data2009Reco07

    ##
    ## "regular data"
    ##
    from Gaudi.Configuration import EventSelector 
    files  = EventSelector().Input
    files.reverse() 
    configure ( files )

    ##
    ##   few "special" runs
    ##
    
##     prefix = '/castor/cern.ch/grid/lhcb/data/2009/DST/'
##     _files = [ '00005845/0000/00005845_00000057_1.dst' ,
##                '00005845/0000/00005845_00000058_1.dst' ,
##                '00005844/0000/00005844_00000003_1.dst' ,
##                '00005845/0000/00005845_00000049_1.dst' ]
##     files = [ prefix + f for f in _files ]
##     configure  (files ) 
    
    run ( 1000 )
    
# =============================================================================
# The END 
# =============================================================================
