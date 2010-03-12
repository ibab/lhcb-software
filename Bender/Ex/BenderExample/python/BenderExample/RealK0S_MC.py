#!/usr/bin/env python
# ==========================================================================================
## @file BenderExample/RealK0S_MC.py
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
from   Bender.MainMC       import *   ## import all bender goodies 
import LHCbMath.Types                 ## easy access to various geometry routines 
from   Gaudi.Configuration import *   ## needed for job configuration

from   GaudiKernel.SystemOfUnits     import GeV, MeV, mm, cm 
from   GaudiKernel.PhysicalConstants import c_light
from   GaudiPython.GaudiAlgs         import Vector

import LoKiMC.trees              as Trees  
import PartProp.Nodes            as Nodes

from   BenderExample.RealK0S     import Ks
from   BenderExample.RealLam0_MC import newP

# =============================================================================
## @class KsMC
#  Simple algorithm to study K0S -> pi+ pi-
#  @author Vanya     BELYAEV    vanya@nikhef.nl
#  @date 2009-12-21
class KsMC ( Ks ) :
    """
    Simple algorithm to study K0S -> pi+ pi-
    """    
    ## the standard initialization
    def initialize ( self ) :
        """
        Initialization
        """
        sc = Ks.initialize ( self )
        if sc.isFailure() : return sc

        self.nmc           = self.counter ( '#nMC'  )

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
        
        ##
        ##if not l0decision : return SUCCESS 
        ##
        
        from LoKiPhys.decorators import FALSE 
        
        self.trueKs = FALSE 
        self.DATA   = False 
        
        # K0s -> p pi 
        ks  = Trees.MCExclusive  ( Nodes.Pid('KS0') )        
        ks += Nodes.Pid ('pi+' ) 
        ks += Nodes.Pid ('pi-' ) 
        st  = ks.validate ( self.ppSvc() )
        if st.isFailure()  : return st
        
        ## try to find the true Ks:
        mcks = self.mcselect  (
            'mcks' ,
            ( 'KS0' == MCID    ) &
            ( 0         < MCPZ ) &
            ( 200 * MeV < MCPT ) &
            (   2 * GeV > MCPT ) & 
            ( 2.5       < MCY  ) &
            ( 4.0       > MCY  ) &
            MCOVALID       & 
            ( abs ( MCVFASPF( MCVZ ) ) < 300 ) &
            ( abs ( MCVFASPF( MCVX ) ) <   4 ) &
            ( abs ( MCVFASPF( MCVY ) ) <   4 ) 
            )
        
        self.nmc += mcks.size()

        if mcks.empty() : return self.Warning('No MC-K0S (1)', SUCCESS )

        tMC = self.nTuple('MCK0S')
        
        fromB     = MCINANCESTORS ( BEAUTY  )
        fromC     = MCINANCESTORS ( CHARM   )
        fromS     = MCINANCESTORS ( STRANGE )
        fromXi    = MCINANCESTORS ( 'Xi-'    == MCABSID )
        fromOmega = MCINANCESTORS ( 'Omega-' == MCABSID )
        fromLong  = MCINANCESTORS ( MCDECNODE ( self.llongnode ) )

        fromXs     = MCFROMXS
        fromDecays = MCFROMDECAYS
        
        decay     = MCDECTREE ( ks )

        collisions = self.get('Gen/Collisions')
        self.diffr =  int ( 1 )   
        if collisions.empty() : self.diffr = 0
        for c in collisions : 
            ptype   = c.processType()    
            if not ptype in [91,92,93,94] : self.diffr = int ( 0 ) 

        l0 = self.get('/Event/Trig/L0/L0DUReport')
        l0dec   = int ( l0.decision()  ) 
        l0calo  = int ( l0.channelDecisionByName('CALO')  ) 

        for p in mcks :

            np = newP ( p.momentum() ) / GeV 
            
            tMC.column ( 'pt'     , MCPT   ( p )  / GeV )
            tMC.column ( 'p'      , MCP    ( p )  / GeV )
            tMC.column ( 'Y'      , MCY    ( p )        )
            tMC.column ( 'eta'    , MCETA  ( p )        )
            tMC.column ( 'phi'    , MCPHI  ( p )        )
            tMC.column ( 'ctau'   , MCCTAU ( p )        )
            tMC.column ( 'p'      , p.momentum () / GeV )
            #
            tMC.column ( 'decay'  , decay ( p ) )
            #
            tMC.column ( 'fromB'  , fromB     ( p ) ) 
            tMC.column ( 'fromC'  , fromC     ( p ) ) 
            tMC.column ( 'fromS'  , fromS     ( p ) ) 
            tMC.column ( 'fromXi' , fromXi    ( p ) ) 
            tMC.column ( 'fromOm' , fromOmega ( p ) ) 
            tMC.column ( 'fromLo' , fromLong  ( p ) )
            ##
            tMC.column ( 'fromXs'   , fromXs     ( p ) )
            tMC.column ( 'fromDecs' , fromDecays ( p ) )
            ##
            tMC.column ( 'l0dec'  , l0dec      ) 
            tMC.column ( 'l0calo' , l0calo     ) 
            tMC.column ( 'diffr'  , self.diffr ) 
            # new PT
            tMC.column ( 'npt'    , np.Pt() )

            
            tMC.write()
            
            self.plot ( MCPT ( p ) / GeV , 'MCpt for Ks ' , 0 , 5 , 500 )

        ## subselect for true decays only 
        mcks = self.mcselect (
            'mclksec' ,
            mcks      ,
            decay & fromDecays & ~fromLong & ~fromXs )

        if mcks.empty() : return self.Warning('No MC-K0S (2)', SUCCESS ) 
        
        self.trueKs = MCTRUTH ( self.mcTruth() , mcks ) 
        
        return Ks.analyse ( self ) 


# =============================================================================
## Reset TOPALG sequence. IMPORTANT: It saves a lot of CPU time!!!
def  post_action ( ) :
    """
    Reset DV-init sequence. IMPORTANT: It saves a lot of CPU time!!!
    """
    
    from Configurables import GaudiSequencer, ApplicationMgr 
    hlt = GaudiSequencer('Hlt')

    ApplicationMgr().TopAlg = [ hlt ] 
        
    print 'RealK0S_MC: TOPALG is cleared!'

# ============================================================================================

## configure the job 
def configure ( datafiles                    ,
                catalogs = []                ,
                tFile    = 'RealK0S_MC.root' ) :
    """
    Job configuration 
    """
    from Configurables           import DaVinci       ## needed for job configuration
    from Configurables           import EventSelector ## needed for job configuration 
    from GaudiConf.Configuration import FileCatalog   ## needed for job configuration 
    from GaudiConf.Configuration import NTupleSvc     ## needed for job configuration 
    from Configurables           import GaudiSequencer 
    davinci = DaVinci (
        DataType      = '2009' ,
        Simulation    = True   ,
        RedoMCLinks   = True   , 
        ## from Thomas 
        DDDBtag       = "head-20100119" ,
        CondDBtag     = "MC-20100205-vc15mm-md100" , 
        ## 
        PrintFreq     = 1000   ,
        EvtMax        = -1     , 
        HistogramFile = 'RealK0S_MC_Histos.root' ,
        ##
        UserAlgorithms = [ GaudiSequencer('Hlt') ] 
        )
    
    if datafiles : 
        EventSelector (
            Input     = datafiles
            )

    if catalogs :
        from GaudiConf.Configuration import FileCatalog  
        FileCatalog ( Catalogs = catalogs )
        
    NTupleSvc (
        Output = [ "K0S DATAFILE='%s' TYPE='ROOT' OPT='NEW'" % tFile ]
        )
    
    ## 
    ## trigger-related stuff, many thanks to Thomas Ruf 
    ##
        
    from Configurables import TriggerTisTos
    TriggerTisTos  ( TriggerInputWarnings = True ) 

    from Configurables import HltConf,L0Conf
    hltconf       = HltConf()
    hltconf.L0TCK = '0x1309'
    hltconf.ThresholdSettings = 'Physics_MinBiasL0_PassThroughHlt_09Dec09'
    L0Conf ( TCK = '0x1309' ) 
    
    # =============================================================================
    ## Important: use Post Config action! 
    appendPostConfigAction ( post_action )
    
    gaudi = appMgr()
    
    from   BenderExample.RealK0S import Ks
    
    alg = KsMC(
        'Ks'                   ,   ## Algorithm name
        NTupleLUN      = 'K0S' ,   ## Logical unit for output file with N-tuples
        PP2MCs         = [ 'Relations/Rec/ProtoP/Charged' ] , 
        InputLocations = [ 'StdNoPIDsPions',
                           'StdNoPIDsDownPions' ] ## input particles 
        )
    
    gaudi.addAlgorithm ( alg ) 
    
    return SUCCESS 


# =============================================================================
# The actual job steering
if '__main__' == __name__ :
    
    import sys
    
    iL    = int ( sys.argv[1] )
    iR    = int ( sys.argv[2] )    
    files = []
    
    from Configurables  import EventSelector
    EventSelector( PrintFreq = 1000 )

##     import BenderExample.MC2009_9
##     files = EventSelector().Input 
    
##     print  80*'*'
##     print  ' ARGV  : %s '    %  sys.argv
##     print  ' RANGE : %d/%d ' % ( iL , iR )
##     print  ' FILES : %d '    %  len(files)
##     iR = min ( iR , len ( files ) )
##     files = files [ iL : iR ]
##     print  ' FILES : %d '    % len(files)
##     for f in files : print f 
##     print  80*'*'
    
    tupfile = 'RealK0S_MC_MAS_MN%d.root' % iL
    
    ## configure with the empty list of input data 
    configure (
        files   ,
        catalogs = [
        'xmlcatalog_file:local.xml'            ,
        'xmlcatalog_file:Sim04Reco03-XDST.xml' ,
        'xmlcatalog_file:Sim04Reco03-DST.xml'
        ] ,
        tFile = tupfile
        )
    
    gaudi = appMgr()
    

    ## 
    ## Manuel 
    ## 
    evtSel = gaudi.evtSel()
    pattern = '/castor/cern.ch/user/m/mschille/%d/%d/outputdata/Brunel.dst'
    files = []
    
    ##for job in ( 179 , ) :  ## STD 
    ##for job in ( 180 , ) :  ## STD - 1*RMS       
    for job in ( 181 , ) :    ## STD + 1*RMS 
        for subjob in range ( iL , min ( iR , 75 ) ) :
            
            if subjob in ( 47 , 48 , 49 ) : continue 
            
            f = pattern % ( job , subjob )
            files.append ( f )

    
    print ' INPUT FILES: '
    for f in files : print f        
    evtSel.open ( files ) 
            
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
