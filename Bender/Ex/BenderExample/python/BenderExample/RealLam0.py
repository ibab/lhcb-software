#! /usr/bin/env python
# ==========================================================================================
## @file BenderExample/RealLam0.py
#
#  A little bit modified version of original script by Alexander KOZLINSKY  and
#  Thomas BAUER to look for Lambda0 -> p pi- peak on real data
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

#  @author Alexander KOZLINSKY  akozlins@gmail.com
#  @author Thomas    BAUER       thomas@nikhef.nl
#  @author Vanya     BELYAEV    vanya@nikhef.nl
# ===========================================================================================
"""

A little bit modified version of original script by Alexander KOZLINSKY and
Thomas BAUER to look for Lambda0 -> p pi- peak on real data

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
__author__   = " Alexander KOZLINSKY, Thomas BAUER & Vanya BELYAEV "
__date__     = " 2009-12-?? "
__version__  = " CVS Tag $Name: not supported by cvs2svn $, verison $Release:$"
# ===========================================================================================
import ROOT                           ## needed to produce/visualize the histograms
import LHCbMath.Types                 ## easy access to various geometry routines 
from   Bender.MainMC       import *   ## import all bender goodies 
from   Gaudi.Configuration import *   ## needed for job configuration

from   GaudiKernel.SystemOfUnits     import GeV, MeV, mm, cm 
from   GaudiKernel.PhysicalConstants import c_light
from   GaudiPython.GaudiAlgs         import Vector

# =============================================================================
## @class Lam0
#  Simple algorithm to study Lambda0 -> p pi-
#  @author Alexander KOZLINSKY  akozlins@gmail.com
#  @author Thomas    BAUER       thomas@nikhef.nl
#  @author Vanya     BELYAEV    vanya@nikhef.nl
#  @date 2009-12-21
class Lam0(AlgoMC) :
    """
    Simple algorithm to study Lam0 -> p pi-
    """
    ## the standard initialization
    def initialize ( self ) :
        """
        Initialization
        """
        sc = AlgoMC.initialize ( self )
        if sc.isFailure() : return sc

        self.npv           = self.counter ( '#nPV'  )
        self.npi           = self.counter ( '#nPi'  )
        self.np            = self.counter ( '#nP'   )
        self.ltfitter      = self.lifetimeFitter()

        from LoKiPhys.decorators import ALL
        
        self.trueLam0 = ALL
        self.truePi   = ALL
        self.trueP    = ALL

        self.nevent   = 0 
        self.l0eff    = self.counter ( '#L0eff'  )
        
        self.diffr    = 0
        
        self.runs     = {}
        self.runspv   = {}
        
        self.tistos = self.tool( cpp.ITriggerTisTos ,
                                 'TriggerTisTos'    )

        self.tistos.setTriggerInput("Hlt1L0.*Decision")

        self.DATA     = True
        
        return SUCCESS

    ## the only one esential method: 
    def analyse  (self ) :
        """
        The only one essential method
        """

        if  1 == self.nevent % 1000 :
            hdr = self.get('Rec/Header')
            print 'HEADER ', self.nevent , '\n' , hdr 

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
        ## goodTrack = goodTrack & self.trueLam0
        
        pions   = self.select ( 'pion' , ( 'pi+' == ABSID ) & goodTrack & self.truePi )        
        if pions  .empty() : return SUCCESS

        protons = self.select ( 'p'    , ( 'p+'  == ABSID ) & goodTrack & self.trueP  )
        if protons.empty() : return SUCCESS

        self.npv  += 1
        self.npi  += pions.size()
        self.np   += protons.size()

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
            
            long1 = True if ISLONG ( p   ) else False  
            long2 = True if ISLONG ( pi  ) else False  
            
            down1 = True if ISDOWN ( p   ) else False  
            down2 = True if ISDOWN ( pi  ) else False  
        
            qP  = Q ( p )
            qPi = Q ( pi )
            
            if 0 < qP : lam.setPID ( 'Lambda0'  )
            else      : lam.setPID ( 'Lambda~0' )
            
            # check the charge:
            if not -0.5 < ( qP + qPi ) < 0.5 : continue
            
            self.plot ( m12 , 'p pi mass, correct sign', 1.05 , 1.25 ) 
          
            p   = lam(1)
            chi2 = VCHI2(lam)
            if not 0 <= chi2 < 100 : continue
            
            self.plot ( m12 , 'p pi mass, chi2(vx)<100', 1.05 , 1.25 ) 
          
            m = M(lam) / GeV;
            if m > 1.180               : continue
          
            self.plot ( m , 'p pi mass, correct mass', 1.05 , 1.25 ) 

            z = VZ(lam) / mm 

            if z > 220 * cm : continue
            
            mips1 = mips ( p  ) 
            mips2 = mips ( pi )
            
            tup.column ( "mips0" , mips ( lam ) )
            tup.column ( "mips1" , mips1 )
            tup.column ( "mips2" , mips2 ) 

            
            bpv = self.bestPV ( primaries, lam.particle() )

            if not bpv : continue
          
            _ltime  = LTIME        ( self.ltfitter , bpv )
            _ltchi2 = LTIMEFITCHI2 ( self.ltfitter , bpv )
            
            ltime  = _ltime  ( lam ) * c_light
            ltchi2 = _ltchi2 ( lam )
            
            if ltchi2 < 0 : continue
            
            pvz    = VZ ( bpv )
            pvx    = VX ( bpv )
            pvy    = VY ( bpv )

            ok     = False 
            if not self.DATA :
                ok = self.trueLam0 ( lam )
                
            if ok :
                self.plot ( m , 'p pi mass MC (1)'    , 1.05 , 1.25 ) 
                self.plot ( ltime                     , 'ltime   (MC)' , 0 , 200 , 200 )
                self.plot ( min(ltchi2,199)           , 'ltchi2  (MC)' , 0 , 200 , 200 )
                self.plot ( min(chi2,199)             , 'chi2    (MC)' , 0 , 200 , 200 )
                self.plot ( min(min(mips1,mips2),199) , 'mips    (MC)' , 0 , 200 , 200 )
                    
            if   down1 and down2 and ltime < 10 * mm : continue
            elif                     ltime <  1 * mm : continue 

            if ok : self.plot ( m , 'p pi mass MC (2)', 1.05 , 1.25 ) 

            if ltchi2 > 100                          : continue 

            if ok : self.plot ( m , 'p pi mass MC (3)', 1.05 , 1.25 ) 

            ## "cuts"
            if ltchi2 > 49                           : continue

            if ok : self.plot ( m , 'p pi mass MC (4)', 1.05 , 1.25 ) 
            
            if chi2   > 25                           : continue
            
            if ok : self.plot ( m , 'p pi mass MC (5)', 1.05 , 1.25 ) 
            
            if min ( mips1 , mips2 )  < 25           : continue

            if ok : self.plot ( m , 'p pi mass MC (6)', 1.05 , 1.25 ) 
            
            if   down1 and down2 and ltime < 20 * mm : continue
            elif                     ltime <  5 * mm : continue 

            if ok : self.plot ( m , 'p pi mass MC (7)', 1.05 , 1.25 ) 

            ## continue

            self.tupPV   ( tup , bpv )
            self.tupODIN ( tup       )
            self.tupL0   ( tup       )
            self.tisTos  ( tup , lam )             
                 
            tup.column ( "p0"    , P  ( lam ) / GeV ) 
            tup.column ( "p1"    , P  ( p   ) / GeV ) 
            tup.column ( "p2"    , P  ( pi  ) / GeV ) 
            tup.column ( "pt0"   , PT ( lam ) / GeV ) 
            tup.column ( "pt1"   , PT ( p   ) / GeV ) 
            tup.column ( "pt2"   , PT ( pi  ) / GeV )
            
            tup.column ( "pLam"  , lam . momentum ( 0 ) / GeV ) 
            tup.column ( "pP"    , p   . momentum (   ) / GeV ) 
            tup.column ( "pPi"   , pi  . momentum (   ) / GeV ) 
            
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
            tup.column ( 'pvx'   , pvx    )
            tup.column ( 'pvy'   , pvy    )
            
            tup.column ( "m"  , m  )
            tup.column ( "qP" , qP )

            tup.column ( 'trueP'   , self.trueP     ( p   ) )
            tup.column ( 'truePi'  , self.truePi    ( pi  ) )

            _OK = int ( self.trueLam0 ( lam ) )
            
            tup.column ( 'trueLam' , _OK )
            #
            tup.column ( 'diffr' , self.diffr )
            
            tup.column ( 'data' , int ( self.DATA ) )
            
            ## convert momentum into CMS system:
            np = self.toCMS ( lam.momentum ( 0 )  ) / GeV 
            tup.column ( 'yCMS'  , np.Rapidity () )
            tup.column ( 'ptCMS' , np.Pt       () )
           
            tup.write  ( )

            if   long1 and long2 :
                self.plot ( m , 'Lambda0 mass, LL' , 1.08 , 1.18 , 200 )    
            elif down1 and down2 :
                self.plot ( m , 'Lambda0 mass, DD' , 1.08 , 1.18 , 200 )
            else                 :
                self.plot ( m , 'Lambda0 mass, LD' , 1.08 , 1.18 , 200 )

            if not self.DATA and _OK :
                self.plot ( m , 'Lambda0 mass, MC' , 1.08 , 1.18 , 200 )
                
            lam.save('lam0')


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
    
    ## add ODIN information into n-tuple 
    def tupODIN  ( self , tup ) :
        """
        add ODIN information into n-tuple 
        """
        odin = self.get('DAQ/ODIN')
        runNum = odin.runNumber   ()
        evtNum = odin.eventNumber () 
        evt1   = int ( evtNum/2**32  ) 
        evt2   = int ( evtNum - 2**32*evt1 )
        
        tup.column ( 'run'     , runNum         )
        tup.column ( 'evt1'    , evt1           )
        tup.column ( 'evt2'    , evt2           )
        tup.column ( 'bxid'    , odin.bunchId() )
        tup.column ( 'bxtype'  , odin.bunchCrossingType() )
        tup.column ( 'trgtype' , odin.triggerType() )

        goodrun = runNum in [
            ##
            ## from Michael Schmelling's mail 25.02.2k+10, 12:21:32
            ##
            ## RUN   Triggers    sPV    L    D      remarks
            ## ---------------------------------------------------------
            63949    ## 63110     0.544  2.9  4.2  SPD Threshold at 1 Mip
            , 63815  ## 11215     0.595  2.9  4.2
            , 63814  ##  4467     0.595  2.9  4.2
            , 63813  ## 69238     0.627  2.9  4.1
            , 63809  ## 21938     0.428  3.0  4.2
            , 63807  ## 70965     0.462  3.0  4.2  VDM-scan (JP)
            , 63801  ## 88949     0.489  2.9  4.1
            , 63713  ## 14295     0.482  3.0  4.2  no R1&R2
            , 63691  ##  2074     0.421  2.9  4.2
            , 63690  ## 20855     0.201  2.9  4.2
            , 63688  ##  2169     0.487  3.0  4.3  No R1&R2
            , 63687  ## 15642     0.288  2.9  4.2
            , 63686  ## 24391     0.307  2.9  4.2 ---------------------------------------------------------
            ## sum    409308     
            ]
        
        ## for MC-data all runs are "good"
        if not self.DATA : goodrun = 1 
        
        tup.column ( 'goodrun' , int ( goodrun ) )
        
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
        nlong  = 0 
        nback  = 0 
        if not not pv :
            chi2   = VCHI2 ( pv )
            tracks = pv.tracks()
            for t in tracks :
                pts.push_back ( t.pt   () )
                typ.push_back ( t.type () )
                if t.pt () < ptmin : ptmin = t.pt()
                if t.pt () > ptmax : ptmax = t.pt()
                if t.checkFlag ( LHCb.Track.Backward ) : nback += 1
                if LHCb.Track.Long == t.type()         : nlong += 1 
                
        tup.column ( 'pvchi2'  , chi2  )
        tup.column ( 'trptmin' , ptmin ) 
        tup.column ( 'trptmax' , ptmax ) 
        tup.column ( 'ntrback' , nback ) 
        tup.column ( 'ntrlong' , nlong ) 
        tup.farray ( 'trpt'    , pts   , 'nTrk' , 100 ) 
        tup.farray ( 'trtyp'   , typ   , 'nTrk' , 100 ) 
                     
        return SUCCESS

    def tupL0 ( self , tup ) :
        """
        Ann L0 infomration to the tuple 
        """
        l0 = self.get('/Event/Trig/L0/L0DUReport')
        tup.column( 'l0'     , l0.decision() )
        tup.column( 'l0calo' , l0.channelDecisionByName('CALO') )
        return SUCCESS

    def tisTos ( self , tup , v0 ) :

        if hasattr ( v0 , 'particle' ) :
            return self.tisTos ( tup , v0.particle() )
        
        self.tistos.setOfflineInput ( v0 ) 
        
        names = self.tistos.triggerSelectionNames('Hlt1L0CALODecision')

        ## print 'NAMES: ' , [ n for n in names ]
        
        trg   = self.tistos.selectionTisTos ( names )
        
        l0dec = 0 != self.tistos.hltObjectSummaries (
            'Hlt1L0CALODecision'
            ).size()
        
        tis = trg.tis() 
        tos = trg.tos() 
        
        tup.column ( 'tis'    , int ( tis   ) )
        tup.column ( 'tos'    , int ( tos   ) )
        tup.column ( 'l0dec'  , int ( l0dec ) )
        tup.column ( 'lennam' , len ( names ) )

        num1 = 0
        spd  = self.get('Raw/Spd/Digits')
        for d in spd :
            if d.e()  > 0 : num1 += 1
        
        tup.column ( 'spd'            , spd.size() )
        tup.column ( 'spd1'           , num1       )

        return SUCCESS

    ## convert 4-vector to center of mass-system
    def toCMS    ( self , vct ) :
        """
        Return 4-momentum converetd into center-of-mass systems
        """
        if   hasattr ( vct , 'momentum' ) :
            return self.toCMS ( vct.momentum() )

        newE = vct.E  () - 0.0021 * vct.Pz()   
        newX = vct.Px () - 0.0021 * vct.E ()
        
        newM = Gaudi.LorentzVector ( vct )
        newM .SetE  ( newE )
        newM .SetPx ( newX )
        
        return newM 

    
    ## finalize & print histos 
    def finalize ( self ) :
        """
        Finalize & print histos         
        """
        histos = self.Histos()
        for key in histos :
            h = histos[key]
            if hasattr ( h , 'dump' ) : print h.dump(50,30,True)
        return AlgoMC.finalize ( self )
  
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
        HistogramFile = 'RealLam0_Histos.root' 
        )
    
    from GaudiConf.Configuration import NTupleSvc
    NTupleSvc (
        Output = [ "LAM0 DATAFILE='RealLambda_GR.root' TYPE='ROOT' OPT='NEW'" ]
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
    alg = Lam0(
        'Lam0'             ,   ## Algorithm name
        NTupleLUN = 'LAM0' ,   ## Logical unit for output file with N-tuples 
        InputLocations = [ 'StdNoPIDsPions'        ,
                           'StdNoPIDsDownPions'    ,
                           'StdNoPIDsProtons'      ,
                           'StdNoPIDsDownProtons'  ] ## input particles 
        )
    
    gaudi.setAlgorithms ( [ alg ] ) 
    ## gaudi.addAlgorithm ( alg  ) 
    
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
