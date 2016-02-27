#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$ 
# =============================================================================
## @file Xuhao.py 
#
#  Get list of trigger lines, efficient for D0 -> K- mu+ nu decays 
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
#  @date 2016-01-14
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""Get list of trigger lines, efficient for D0 -> K- mu+ nu decays

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

                  $Revision$
Last modification $Date$
               by $Author$
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@itep.ru "
__date__    = " 2006-10-12 " 
__version__ = " $Revision$ "
# ============================================================================= 
## logging
# =============================================================================
from Bender.Logger import getLogger 
if '__main__' == __name__ : logger = getLogger ( 'Xuhao'  )
else                      : logger = getLogger ( __name__ )
# =============================================================================
## import everything from bender 
from   Bender.Main import *
# =============================================================================
## @class Lines
#  Get list of trigger lines, efficient for D0 -> K- mu+ nu decays
#  @date 2016-01-14
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
class Lines(Algo):
    """
    Enhanced functionality for n-tuples 
    """
    def initialize ( self ) :

        sc = Algo.initialize ( self ) ## initialize the base class
        if sc.isFailure() : return sc

        #
        ## container to collect trigger information, e.g. list of fired lines 
        #
        triggers = {}
        triggers ['D'    ] = {} ## slot to keep information for D 
        triggers ['Kaon' ] = {} ## slot to keep information for Kaon
        triggers ['Muon' ] = {} ## slot to keep information for Muon
        
        sc = self.tisTos_initialize ( triggers , {} )
        if sc.isFailure() : return sc
        
        return SUCCESS
    
    ## the main 'analysis' method 
    def analyse( self ) :   ## IMPORTANT! 
        """
        The main 'analysis' method
        """
        
        ## get particles from the input locations 
        particles = self.select ( 'dmesons', '[D0 -> K- mu+]CC'  )
        if particles.empty() :
            return self.Warning('No input particles', SUCCESS )
        
        for D in particles :

            K  = D(0)
            Mu = D(1)
            
            #
            ## collect trigger information for D0 and its daughters 
            #
            self.decisions   ( D  , self.triggers['D'   ] )
            self.decisions   ( K  , self.triggers['Kaon'] )
            self.decisions   ( Mu , self.triggers['Muon'] )
            
        return SUCCESS      ## IMPORTANT!!! 
# =============================================================================
## @class Tistos
#  TisTos'sing for trigger lines, efficient for D0 -> K- mu+ nu decay
#  @date 2016-01-14
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
class Tistos(Algo):
    """
    TisTos'sing for trigger lines, efficient for D0 -> K- mu+ nu decay
    """
    def initialize ( self ) :
        
        sc = Algo.initialize ( self ) ## initialize the base class
        if sc.isFailure() : return sc

        #
        ## container to collect trigger information, e.g. list of fired lines 
        #
        triggers = {}
        triggers ['D'    ] = {} ## slot to keep information for D 
        triggers ['Kaon' ] = {} ## slot to keep information for Kaon
        triggers ['Muon' ] = {} ## slot to keep information for Muon

        #
        ## the lines to be investigated in details
        #
        lines    = {}
        lines    [ "D" ] = {} ## trigger lines for D 
        #
        ## six mandatory keys:
        #
        lines    [ "D" ][   'L0TOS' ] = 'L0(DiMuon|Muon)Decision'
        lines    [ "D" ][   'L0TIS' ] = 'L0(Hadron|DiMuon|Muon|Electron|Photon)Decision'
        
        lines    [ "D" ][ 'Hlt1TOS' ] = 'Hlt1(SingleMuon|TrackMuon|TrackAllL0).*Decision'
        lines    [ "D" ][ 'Hlt1TIS' ] = 'Hlt1(SingleMuon|TrackAllL0).*Decision'
        
        lines    [ "D" ][ 'Hlt2TOS' ] = 'Hlt2(CharmSemilepD02HMuNu|TopoMu|SingleMuon).*Decision'
        lines    [ "D" ][ 'Hlt2TIS' ] = 'Hlt2(Charm|Topo|Dst).*Decision'
        
        sc = self.tisTos_initialize ( triggers , lines )
        if sc.isFailure() : return sc
     
        return SUCCESS
    
    ## the main 'analysis' method 
    def analyse( self ) :   ## IMPORTANT! 
        """
        The main 'analysis' method
        """
        
        ## get particles from the input locations 
        particles = self.select ( 'dmesons', '[D0 -> K- mu+]CC'  )
        if particles.empty() :
            return self.Warning('No input particles', SUCCESS )

        tup = self.nTuple('TisTos')
        for D in particles :
            
            K  = D(0)
            Mu = D(1)            
            #
            self.tisTos      (
                D                 , ## particle 
                tup               , ## n-tuple
                'D0_'             , ## prefix for variable name in n-tuple
                self.lines['D']   , ## trigger lines to be inspected
                verbose = True      ## good ooption for those who hates bit-wise operations 
                )
            
        return SUCCESS      ## IMPORTANT!!! 
# =============================================================================

# =============================================================================
## The configuration of the job
def configure ( inputdata        ,    ## the list of input files  
                catalogs = []    ,    ## xml-catalogs (filled by GRID)
                castor   = False ,    ## use the direct access to castor/EOS ? 
                params   = {}    ) :
    
    ## import DaVinci 
    from Configurables import DaVinci
    
    decay    = '[(D0 =>  K-  mu+ nu_mu) && ~(D0 --> K- (pi0|eta) mu+ nu_mu ... )]CC'
    decay_K  = '[(D0 => ^K-  mu+ nu_mu) && ~(D0 --> K- (pi0|eta) mu+ nu_mu ... )]CC'
    decay_mu = '[(D0 =>  K- ^mu+ nu_mu) && ~(D0 --> K- (pi0|eta) mu+ nu_mu ... )]CC'
    
    from PhysConf.Filters import LoKi_Filters
    fltrs = LoKi_Filters (
        MC_Code      =   "has ( MCDECTREE('%s') )" % decay  ,
        MC_Preambulo = [ "from LoKiCore.functions import has" ]
        )
    
    ## delegate the actual configuration to DaVinci
    dv = DaVinci ( DataType        = '2012' ,
                   EventPreFilters = fltrs.filters('MC-filter') ,  
                   InputType       = 'DST'  ,
                   Simulation      = True   ,
                   TupleFile       = 'Xuhao.root'      , 
                   DDDBtag         = 'Sim08-20130503-1',
                   CondDBtag       = 'Sim08-20130503-1-vc-md100'
                   )
    
    ## 
    ## reconstruct D0 -> K mu candidates
    ##
    from StandardParticles import StdAllLooseKaons as kaons 
    from StandardParticles import StdAllLooseMuons as muons 
    
    from PhysSelPython.Wrappers                import SimpleSelection
    from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
    #
    true_kaons = SimpleSelection (
        'MCTrueKaon'  , ## selection name 
        FilterDesktop , ## algorithm type
        [ kaons ]     , ## input/required selection
        ## algorithm propperties 
        Preambulo = [ "from LoKiPhysMC.decorators import *" ] ,
        Code      = "mcMatch ('%s', 2 )" % decay_K
        )
    #
    true_muons = SimpleSelection (
        'MCTrueMuon'    , ## selection name 
        FilterDesktop   , ## algorithm type
        [ muons       ] , ## input/required selection
        ## algorithm propperties 
        Preambulo = [ "from LoKiPhysMC.decorators import *" ] ,
        Code      = "mcMatch ('%s', 2 )" % decay_mu 
        )
    
    from GaudiConfUtils.ConfigurableGenerators import CombineParticles
    sel_D0 = SimpleSelection (
        'SelD0'             , ## name 
        CombineParticles    , ## algorithm type 
        [ true_muons , true_kaons   ] , ## input/required selections 
        ## the decays to be reconstructed
        DecayDescriptor = '[D0 -> K- mu+]cc' ,
        ## combination cut : mass window 
        CombinationCut = """
        in_range ( 1 *  GeV ,  AM , 2 * GeV )
        """ ,
        ##      mother cut : require good vertex & some separation 
        MotherCut      = """
        ( VFASPF( VCHI2 ) <  10 ) &
        ( BPVVDCHI2       >   9 )  
        """ ,
        )
    
    from PhysSelPython.Wrappers import SelectionSequence
    SEQ_D0 = SelectionSequence ( 'TRUED0'  , sel_D0 )

    alg1_name = 'Lines'
    ## add the name of Bender algorithm into User sequence sequence 
    from Configurables import GaudiSequencer
    seq_lines = GaudiSequencer (
        'SEQ_lines' ,
        Members = [ SEQ_D0. sequence () , alg1_name  ]
        )
    
    alg2_name = 'TisTosAlg'
    ## add the name of Bender algorithm into User sequence sequence 
    from Configurables import GaudiSequencer
    seq_tistos = GaudiSequencer (
        'SEQ_tistos' ,
        Members = [ SEQ_D0. sequence () , alg2_name  ]
        )
    
    dv.UserAlgorithms = [ seq_lines , seq_tistos ] 
        
    ## define the input data
    setData  ( inputdata , catalogs , castor )

    from BenderTools.Utils import silence, totalSilence 
    silence      ()
    totalSilence ()
    
    ## get/create application manager
    gaudi = appMgr() 
    
    alg1   = Lines (
        alg1_name ,
        Inputs    = [ SEQ_D0.outputLocation() ]
        )
    alg2   = Tistos (
        alg2_name ,
        Inputs    = [ SEQ_D0.outputLocation() ]
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
    
    ## /MC/2012/Beam4000GeV-2012-MagDown-Nu2.5-Pythia6/Sim08a/Digi13/Trig0x409f0045/Reco14/Stripping20NoPrescalingFlagged/27163008/ALLSTREAMS.DST
    params = {
        'DDDB'   : 'Sim08-20130503-1',
        'CONDDB' : 'Sim08-20130503-1-vc-md100'
        }
    inputdata = [
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000035_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000029_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000034_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000061_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000013_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000007_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000099_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000093_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000023_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000005_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000112_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000063_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000012_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000074_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000072_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000098_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000059_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000094_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000042_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000014_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000049_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000062_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000071_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000031_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000055_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000110_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000060_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000027_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000085_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000001_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000017_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000088_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000076_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000052_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000078_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000024_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000033_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000102_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000045_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000090_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000025_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000048_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000026_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000008_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000057_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000054_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00024156/0000/00024156_00000028_1.allstreams.dst'
        ]
    
    configure( inputdata , castor = True )
    
    ## event loop 
    run(5000)

    gaudi = appMgr()
    alg   = gaudi.algorithm('Lines')
    
# =============================================================================
# The END
# =============================================================================


