#!/usr/bin/env python
# =============================================================================
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
#
#  @author Alexander KOZLINSKY  akozlins@gmail.com
#  @author Thomas    BAUER       thomas@nikhef.nl
#  @author Vanya     BELYAEV    vanya@nikhef.nl
# =============================================================================
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

import LoKiMC.trees              as Trees  
import PartProp.Nodes            as Nodes
import PartProp.decorators

from   BenderExample.RealLam0 import Lam0

    
# =============================================================================
## @class Lam0MC
#  Simple algorithm to study Lambda0 -> p pi-
#  @author Vanya     BELYAEV    vanya@nikhef.nl
#  @date 2009-12-21
class Lam0MC ( Lam0 ) :
    """
    Simple algorithm to study Lambda0 -> p pi-
    """    
    ## the standard initialization
    def initialize ( self ) :
        """
        Initialization
        """
        sc = Lam0.initialize ( self )
        if sc.isFailure() : return sc
        
        self.nmc           = self.counter ( '#nMC'   )
        
        self.llongnode     = Nodes.LongLived
        self.llongnode.validate ( self.ppSvc() )
        self.llongtree     = MCDECNODE ( self.llongnode )
        self.llongtree.validate ( self.ppSvc() )
        
        self.i_ievent  = 0
        
        return SUCCESS

    
    def analyse  ( self ) :
        

        l0 = self.get('/Event/Trig/L0/L0DUReport')
        l0decision  = l0.decision() 
        self.l0eff +=  l0decision 

        from LoKiPhys.decorators import FALSE 
        
        self.trueLam0 = FALSE 
        self.DATA     = False 
        
        # Lambda0 -> p pi 
        lam  = Trees.MCExclusive  ( Nodes.CC('Lambda0') )        
        lam += Nodes.CC ('p+'  ) 
        lam += Nodes.CC ('pi-' ) 
        st   = lam.validate ( self.ppSvc() )
        if st.isFailure()  : return st

        ## try to find the true lambdas:
        mclams = self.mcselect  (
            'mclam' ,
            ( 'Lambda0'  == MCABSID ) &
            ( 0    < MCPZ ) &
            ## ( 200 * MeV < MCPT ) &
            ## (   2 * GeV > MCPT ) & 
            ## ( 2.5  < MCY  ) &
            ## ( 4.0  > MCY  ) &
            MCOVALID        & 
            ( abs ( MCVFASPF ( MCVZ ) ) < 300 ) &
            ( abs ( MCVFASPF ( MCVX ) ) <   4 ) &
            ( abs ( MCVFASPF ( MCVY ) ) <   4 ) 
            ) 
        
        if mclams.empty() :
            return self.Warning('No MC-lambdas (1)', SUCCESS ) 
        
        self.nmc += mclams.size()
        
        tMC = self.nTuple('MCLam0')

        lam0    = ( 'Lambda0'  == MCID ) 
        lam0bar = ( 'Lambda~0' == MCID ) 
        
        fromB     = MCINANCESTORS ( BEAUTY  )
        fromC     = MCINANCESTORS ( CHARM   )
        fromS     = MCINANCESTORS ( STRANGE )
        fromXi    = MCINANCESTORS ( 'Xi-'    == MCABSID )
        fromOmega = MCINANCESTORS ( 'Omega-' == MCABSID )
        fromLong  = MCINANCESTORS ( MCDECNODE ( self.llongnode ) ) 
        decay     = MCDECTREE ( lam )  ## decay pattern 
        
        fromXs     = MCFROMXS
        fromDecays = MCFROMDECAYS
        
        collisions = self.get('Gen/Collisions')
        self.diffr = 1
        if collisions.empty() : self.diffr = 0
        for c in collisions : 
            ptype   = c.processType()    
            if not ptype in [91,92,93,94] : self.diffr = 0 
            
        l0 = self.get('/Event/Trig/L0/L0DUReport')
        l0dec   = int ( l0.decision()  ) 
        l0calo  = int ( l0.channelDecisionByName('CALO')  ) 

        for p in mclams : 

            tMC.column ( 'pid1'    , MCID    ( p )         )
            tMC.column ( 'pid2'    , p.particleID().pid()  ) 
            tMC.column ( 'lam0'    , lam0    ( p )         )
            tMC.column ( 'lam0bar' , lam0bar ( p )         )
            tMC.column ( 'pt'      , MCPT    ( p )  / GeV  )
            tMC.column ( 'p'       , MCP     ( p )  / GeV  )
            tMC.column ( 'Y'       , MCY     ( p )         )
            tMC.column ( 'eta'     , MCETA   ( p )         )
            tMC.column ( 'phi'     , MCPHI   ( p )         )
            tMC.column ( 'ctau'    , MCCTAU  ( p )         )
            tMC.column ( 'p'       , p.momentum () / GeV   )
            #
            tMC.column ( 'decay'   , decay     ( p ) )
            #
            tMC.column ( 'fromB'   , fromB     ( p ) ) 
            tMC.column ( 'fromC'   , fromC     ( p ) ) 
            tMC.column ( 'fromS'   , fromS     ( p ) ) 
            tMC.column ( 'fromXi'  , fromXi    ( p ) ) 
            tMC.column ( 'fromOm'  , fromOmega ( p ) ) 
            tMC.column ( 'fromLo'  , fromLong  ( p ) )
            #
            tMC.column ( 'fromXs'   , fromXs     ( p ) )
            tMC.column ( 'fromDecs' , fromDecays ( p ) )
            #
            tMC.column ( 'l0dec'   , l0dec      ) 
            tMC.column ( 'l0calo'  , l0calo     ) 
            tMC.column ( 'diffr'   , self.diffr ) 
            #
            ## 4-vector in center-of-mass system:
            np = self.toCMS ( p ) / GeV 
            tMC.column ( 'yCMS'    , np.Rapidity () )
            tMC.column ( 'ptCMS'   , np.Pt       () )
            ##
            tMC.write()
            
            self.plot  ( MCPT ( p ) / GeV , 'MCpt for Lambda ' , 0 , 5 , 500 )

        ## subselect for true decays only 
        mclams = self.mcselect (
            'mclamdec' ,
            mclams     ,
            decay & fromDecays & ~fromLong & ~fromXs ) 

        if mclams.empty() : return self.Warning('No MC-lambdas (2)', SUCCESS ) 

        mclams_ = std.vector('const LHCb::MCParticle*')()
        for l in mclams : 
            
            ## 4-vector in center-of-mass system:
            np = self.toCMS ( p )
            
            y  = np.Rapidity ()
            if not 2.5        <= y  <= 4.0       : continue 
            pt = np.Pt       ()
            if not 200 * MeV  <= pt <= 2.0 * GeV : continue 
            mclams_.push_back ( l )
            
        if mclams_.empty() : return self.Warning('No MC-lambdas (3)', SUCCESS ) 

        self.trueLam0 = MCTRUTH ( self.mcTruth() , mclams_ ) 

        return Lam0.analyse ( self )


# =============================================================================
## configure the job 
def configure ( datafiles        ,
                catalogs = []  ) :
    """
    Job configuration 
    """
    from GaudiConf.Configuration import NTupleSvc     ## needed for job configuration 
    from Configurables import GaudiSequencer
    
    from Configurables           import DaVinci       ## needed for job configuration
    davinci = DaVinci (
        DataType      = '2009' ,
        Simulation    = True   ,
        RedoMCLinks   = True   ,
        ## from Thomas 
        DDDBtag       = "head-20100119" ,
        CondDBtag     = "MC-20100429-vc15mm-md100" ,     
        ## 
        PrintFreq     = 1000   ,
        EvtMax        = -1     , 
        HistogramFile = 'RealLam0_MC_Histos.root' ,
        ##
        UserAlgorithms = [ GaudiSequencer('Hlt') ] 
        )
    
    NTupleSvc ( Output = [
        "LAM0 DATAFILE='RealLam0_MC.root' TYPE='ROOT' OPT='NEW'" 
        ] )
    
    ## trigger-related stuff, many thanks to Thomas Ruf 

        
    from Configurables import TriggerTisTos
    TriggerTisTos  ( TriggerInputWarnings = True ) 

    from Configurables import DataOnDemandSvc    
    dod = DataOnDemandSvc( Dump = True )
    
    from Configurables import HltConf,L0Conf
    hltconf       = HltConf()
    hltconf.L0TCK = '0x1309'
    hltconf.ThresholdSettings = 'Physics_MinBiasL0_PassThroughHlt_09Dec09'
    L0Conf ( TCK = '0x1309' ) 
    
    ## end of trigger configuration

    ## define the input data:
    setData ( datafiles , catalogs )
    
    ##
    ## 2. Jump into the wonderful world of the actual Gaudi components!
    ## 
    
    ## get the actual application manager (create if needed)
    gaudi = appMgr() 

    
    ## create local algorithm:
    alg = Lam0MC(
        'Lam0'                   ,   ## Algorithm name
        NTupleLUN      = 'LAM0'  ,   ## Logical unit for output file with N-tuples
        ## PP2MCs         = [ 'Relations/Rec/ProtoP/Charged' ] ,
        InputLocations = [ 'StdNoPIDsPions'        ,
                           'StdNoPIDsDownPions'    ,
                           'StdNoPIDsProtons'      ,
                           'StdNoPIDsDownProtons'  ] ## input particles 
        )
    
    gaudi.addAlgorithm ( alg ) 

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


    ## 
    ## ``Hit-Efficiency-Massaged'' MC by Manuel
    ## 
    prefix = '/castor/cern.ch/grid'
    files  = [
        '/lhcb/user/m/mschille/2010_05/8044/8044272/Brunel.dst' ,
        '/lhcb/user/m/mschille/2010_05/8044/8044274/Brunel.dst' ,
        '/lhcb/user/m/mschille/2010_05/8044/8044276/Brunel.dst' , 
        '/lhcb/user/m/mschille/2010_05/8044/8044278/Brunel.dst' , 
        '/lhcb/user/m/mschille/2010_05/8044/8044282/Brunel.dst'
        ]
    
    ## configure 
    configure (
        ## input data files :
        [ prefix + f for f in files ]
        )
            
    run ( 10000 )
    
    
# =============================================================================
# The END 
# =============================================================================
