#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id:$ 
# =============================================================================
## @file MCmatch.py 
#
#  Very simple manipulations with MC-truth matched events 
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
#  with the smear campaign of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2006-10-12
#
#                    $Revision:$
#  Last modification $Date: 2010-09-13 13:24:04 $
#                 by $Author: ibelyaev $
# =============================================================================
"""Very simple manipulations with MC-truth matched events 

oooooooooo.                              .o8                     
`888'   `Y8b                            \"888                     
 888     888  .ooooo.  ooo. .oo.    .oooo888   .ooooo.  oooo d8b 
 888oooo888' d88' `88b `888P\"Y88b  d88' `888  d88' `88b `888\"\"8P 
 888    `88b 888ooo888  888   888  888   888  888ooo888  888     
 888    .88P 888    .o  888   888  888   888  888    .o  888     
o888bood8P'  `Y8bod8P' o888o o888o `Y8bod88P\" `Y8bod8P' d888b    

This file is a part of BENDER project:
   ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
   ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
    ``No Vanya's lines are allowed in LHCb/Gaudi software.''

                  $Revision:$
Last modification $Date: 2010-09-13 13:24:04 $
               by $Author: ibelyaev $
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@itep.ru "
__date__    = " 2006-10-12 " 
__version__ = " Version $Revision: 1.15 $ "
# =============================================================================
import ROOT 
from   GaudiKernel.SystemOfUnits import GeV 
## import everything from bender 
from   Bender.MainMC import * 
# =============================================================================
## optional logging
# =============================================================================
from Bender.Logger import getLogger 
if '__main__' == __name__ : logger = getLogger ( 'BenderTutor.MCmatch' )
else                      : logger = getLogger ( __name__ )
# =============================================================================
## @class MCmatch 
#  Very simple manipulations with MC-truth matched events 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
class MCmatch(AlgoMC):                        ## <--- Note the base class here
    """
    Make combinatorics and composed particle creation in Bender 
    """
    ## the main 'analysis' method 
    def analyse( self ) :   ## IMPORTANT! 
        """
        The main 'analysis' method
        """

        ## get "all" decays 
        mybs = self.mcselect ( 'mybs' , '[ B_s0 ==> (J/psi(1S)=> mu+ mu-) K+ K- pi+ pi-]CC' )

        if mybs.empty() :
            self.Warning('No true Bs-decays are found!')
            allb = self.mcselect( 'allmcb' , BEAUTY )
            print allb
            return SUCCESS

        ## get psis from MC-decays 
        mypsi = self.mcselect ( 'mypsi' , '[ B_s0 ==> ^(J/psi(1S)=>mu+ mu-)  K+ K- pi+ pi-]CC' )
        ## get pions from MC-decays : note carets 
        mypi  = self.mcselect ( 'mypi' , '[ B_s0 ==> (J/psi(1S)=>mu+ mu-)  K+ K- ^pi+ ^pi-]CC' )
        ## get psis from MC-decays  : note carets 
        myk   = self.mcselect ( 'myk'  , '[ B_s0 ==> (J/psi(1S)=>mu+ mu-) ^K+ ^K- pi+ pi-]CC' )
        
        ## create mc-truth functors 
        trueB   = MCTRUTH ( mybs  , self.mcTruth () ) ## IMPORTANT 
        truePsi = MCTRUTH ( mypsi , self.mcTruth () ) ## IMPORTANT 
        trueK   = MCTRUTH ( myk   , self.mcTruth () ) ## IMPORTANT 
        truePi  = MCTRUTH ( mypi  , self.mcTruth () ) ## IMPORTANT 

        ## jump to reco-world
        reco_psi  = self.select( 'psi', 'J/psi(1S)' ==    ID )
        if reco_psi.empty() : return SUCCESS
        ## selct only MC-truth matched pions:
        reco_pi   = self.select( 'pi' , ( 'pi+'     == ABSID ) & truePi ) ## NB!
        if reco_pi .empty() : return SUCCESS
        reco_k    = self.select( 'k'  ,   'K+'      == ABSID            ) 
        if reco_pi .empty() : return SUCCESS
        
        ## prepare useful functors:
        mfit  = DTF_FUN  ( M , True , strings('J/psi(1S)') ) ## use PV and J/psi-mass constraint
        c2dtf = DTF_CHI2NDOF ( True , strings('J/psi(1S)') ) ## ditto
        ctau  = DTF_CTAU ( 0 , True , strings('J/psi(1S)') ) ## ditto 
      
        tup = self.nTuple('MyTuple')
        ##                 1  2 3 4  5 
        Bs = self.loop ( 'psi k k pi pi ' , 'B_s0' )
        for b in Bs :

            jpsi = b(1)
            k1   = b(2)
            k2   = b(3)
            pi1  = b(4)
            pi2  = b(5)

            if 0 < Q( k1)*Q( k2) : continue ## skip wrong charge combinations 
            if 0 < Q(pi1)*Q(pi2) : continue ## skip wrong charge combinations  

            m = b.mass() / GeV 
            if not  4.9 <= m     <= 5.6 : continue
            
            vchi2 = VCHI2 ( b )
            if not -1.0 <= vchi2 <= 100 : continue

            m_fit  = mfit  ( b ) / GeV
            if not  5.0 <= m_fit  <= 5.5 : continue 
            c_tau  = ctau  ( b )
            if not -1.0 <= c_tau  <= 100 : continue
            c2_dtf = c2dtf ( b )  
            if not -1.0 <= c2_dtf <=  10 : continue
            
            tup.column  (  'mBdtf' , m_fit  )
            tup.column  (  'c2dtf' , c2_dtf )
            tup.column  (  'ctau'  , c_tau  )
 
            tup.column_float ( 'm'       , M ( b ) / GeV )
            tup.column_float ( 'mpsi'    , M(jpsi) / GeV )
            
            tup.column       ( 'psi'     , jpsi.momentum() / GeV )
            
            tup.column_bool  ( 'trueB'   , trueB   ( b    )   )
            tup.column_bool  ( 'truePsi' , truePsi ( jpsi )   )
            tup.column_bool  ( 'trueK1'  , trueK   (  k1  )   )
            tup.column_bool  ( 'trueK2'  , trueK   (  k2  )   )
            tup.column_bool  ( 'truePi1' , truePi  ( pi1  )   )
            tup.column_bool  ( 'truePi2' , truePi  ( pi2  )   )

            tup.write ()

        ## 
        return SUCCESS      ## IMPORTANT!!! 
# =============================================================================

# =============================================================================
## The configuration of the job
def configure ( inputdata        ,    ## the list of input files  
                catalogs = []    ,    ## xml-catalogs (filled by GRID)
                castor   = False ,    ## use the direct access to castor/EOS ? 
                params   = {}    ) :
    
    ## read only events with Detached J/psi line fired
    Jpsi_location  = '/Event/AllStreams/Phys/FullDSTDiMuonJpsi2MuMuDetachedLine/Particles'
    from PhysConf.Filters import LoKi_Filters
    fltrs = LoKi_Filters ( STRIP_Code = "HLT_PASS_RE('Stripping.*DiMuonJpsi2MuMuDeta.*')"  )
    
    ## import DaVinci    
    from Configurables import DaVinci
    ## delegate the actual configuration to DaVinci
    dv = DaVinci ( EventPreFilters =  fltrs.filters ('Filters') , ## IMPORTANT
                   Simulation      = True                       , ## IMPORTANT  
                   DDDBtag         = 'dddb-20130929-1'          , ## IMPORTANT 
                   CondDBtag       = 'sim-20130522-1-vc-mu100'  , ## IMPORTANT 
                   DataType        = '2012'       ,
                   InputType       = 'DST'        ,
                   PrintFreq       =  10          , 
                   TupleFile       = 'MCmatch.root' )
    
    ## add the name of Bender algorithm into User sequence sequence 
    alg_name = 'MCmatch'
    dv.UserAlgorithms += [ alg_name ]
    
    from StandardParticles import StdLooseKaons 
    kaons = StdLooseKaons.outputLocation()
    from StandardParticles import StdLoosePions 
    pions = StdLoosePions.outputLocation()
    
    ## define the input data
    setData  ( inputdata , catalogs , castor )
    
    ## get/create application manager
    gaudi = appMgr() 
    
    ## (1) create the algorithm with given name 
    alg   = MCmatch (
        alg_name  ,
        Inputs            = [ Jpsi_location , kaons , pions ] , 
        ParticleCombiners = { '' : 'LoKi::VertexFitter' } , 
        PPMCs             = [ "Relations/Rec/ProtoP/Charged"] ## to speedup a bit 
        )
             
    return SUCCESS 
# =============================================================================

# =============================================================================
## Job steering 
if __name__ == '__main__' :

    logger.info ( 80*'*'  ) 
    logger.info ( __doc__ ) 
    logger.info ( ' Author  : %s ' %  __author__  ) 
    logger.info ( ' Version : %s ' %  __version__ ) 
    logger.info ( ' Date    : %s ' %  __date__    ) 
    logger.info ( 80*'*'  ) 
    
    ## job configuration
    ## BKQuery('/MC/2012/Beam4000GeV-2012-MagUp-Nu2.5-Pythia8/Sim08d/Digi13/Trig0x409f0045/Reco14a/Stripping20NoPrescalingFlagged/13246002/ALLSTREAMS.DST')
    inputdata = [
        '/lhcb/MC/2012/ALLSTREAMS.DST/00033494/0000/00033494_00000013_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00033494/0000/00033494_00000040_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00033494/0000/00033494_00000022_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00033494/0000/00033494_00000004_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/000`33494/0000/00033494_00000023_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00033494/0000/00033494_00000058_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00033494/0000/00033494_00000016_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00033494/0000/00033494_00000026_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00033494/0000/00033494_00000051_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00033494/0000/00033494_00000018_1.allstreams.dst',
        ]
    
    configure( inputdata , castor = True )
    
    ## event loop 
    run(1000)
    
# =============================================================================
# The END
# =============================================================================


