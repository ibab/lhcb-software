#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id:$ 
# =============================================================================
## @file Jascha.py 
#
#  Try to understand vertex fits 
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
#                    $Revision: 172885 $
#  Last modification $Date: 2014-05-16 19:48:30 +0200 (Fri, 16 May 2014) $
#                 by $Author: ibelyaev $
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

                  $Revision: 172885 $
Last modification $Date: 2014-05-16 19:48:30 +0200 (Fri, 16 May 2014) $
               by $Author: ibelyaev $
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@itep.ru "
__date__    = " 2006-10-12 " 
__version__ = " $Revision: 172885 $ "
# ============================================================================= 
## import everything from bender 
from Bender.Logger import setLogging
setLogging ( 3 )
## import everything from bender 
from   Bender.MainMC import *
# =============================================================================
## optional logging
# =============================================================================
from Bender.Logger import getLogger 
if '__main__' == __name__ : logger = getLogger ( 'Xuhao'  )
else                      : logger = getLogger ( __name__ )
# =============================================================================
## @class Jascha
#  Get list of trigger lines, efficient for D0 -> K- mu+ nu decays
#  @date 2016-01-14
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
class Jascha(AlgoMC):
    """
    Enhanced functionality for n-tuples 
    """

    ## the main 'analysis' method 
    def analyse( self ) :   ## IMPORTANT! 
        """
        The main 'analysis' method
        """

        psi = self.select ( 'psi' , 'J/psi(1S) -> mu+ mu-'    )
        kst = self.select ( 'kst' , '[K*(892)+ ->  K+ pi0]CC' )
        k   = self.select ( 'k'   , 'K+'  == ABSID            )
        pi0 = self.select ( 'pi0' , 'pi0' == ID               )

        if 1 != len(psi) or 1 != len(k) or 1 != len(kst) or pi0.empty() :
            return self.Warning('No RC components ', SUCCESS )

        vf = self.vertexFitter()

        vrtx1  = LHCb.Vertex()
        daugs1 = LHCb.Particle.ConstVector()
        daugs1.push_back ( psi[0] )
        daugs1.push_back ( k  [0] )
        
        vrtx2  = LHCb.Vertex()
        daugs2 = LHCb.Particle.ConstVector()
        daugs2.push_back ( psi[0] )
        daugs2.push_back ( kst[0] )

        vrtx3  = LHCb.Vertex()
        daugs3 = LHCb.Particle.ConstVector()
        daugs3.push_back ( psi[0] )
        daugs3.push_back ( k  [0] )
        daugs3.push_back ( pi0[0] )
        
        sc1 = vf.fit ( vrtx1 , daugs1 )
        sc2 = vf.fit ( vrtx2 , daugs2 )
        sc3 = vf.fit ( vrtx3 , daugs3 )

        if sc1.isFailure() :
            self.Error("Failure fit1", sc1 )
            return SUCCESS
        if sc2.isFailure() :
            self.Error("Failure fit2", sc2 )
            return SUCCESS
        if sc3.isFailure() :
            self.Error("Failure fit3", sc3 )
            return SUCCESS
            
        cnt1_12 = self.counter('delta(chi2)/1-2')
        cntx_12 = self.counter('delta(x)/1-2')
        cnty_12 = self.counter('delta(y)/1-2')
        cntz_12 = self.counter('delta(z)/1-2')

        cnt1_13 = self.counter('delta(chi2)/1-3')
        cntx_13 = self.counter('delta(x)/1-3')
        cnty_13 = self.counter('delta(y)/1-3')
        cntz_13 = self.counter('delta(z)/1-3')
        
        cnt2_12 = self.counter('vdchi2/1-2')
        cnt2_13 = self.counter('vdchi2/1-3')
        

        ## delta chi2 
        cnt1_12 += ( VCHI2 ( vrtx1 ) - VCHI2 ( vrtx2 ) )
        cnt1_13 += ( VCHI2 ( vrtx1 ) - VCHI2 ( vrtx3 ) )

        p1 = vrtx1.position()
        p2 = vrtx2.position()
        p3 = vrtx3.position()

        ## delta-x,y,z 
        cntx_12 += p1.X() - p2.X()
        cnty_12 += p1.Y() - p2.Y()
        cntz_12 += p1.Z() - p2.Z()

        ## delta-x,y,z 
        cntx_13 += p1.X() - p3.X()
        cnty_13 += p1.Y() - p3.Y()
        cntz_13 += p1.Z() - p3.Z()

        ## chi2 of vertex-vertex distance 
        vd = VVDCHI2 ( vrtx1 )

        cnt2_12 += vd ( vrtx2 )
        cnt2_13 += vd ( vrtx3 )

        cnt_dof_1 = self.counter('DOF1')
        cnt_dof_2 = self.counter('DOF2')
        cnt_dof_3 = self.counter('DOF3')

        cnt_dof_1 += VDOF( vrtx1 )
        cnt_dof_2 += VDOF( vrtx2 )
        cnt_dof_3 += VDOF( vrtx3 )
        
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
    ## delegate the actual configuration to DaVinci
    dv = DaVinci ( DataType        = '2012' ,
                   InputType       = 'DST'  ,
                   Simulation      = True   ,
                   TupleFile       = 'Jascha.root'      , 
                   DDDBtag         = 'dddb-20130929-1'  , 
                   CondDBtag       = 'sim-20130522-1-vc-md100'
                   )
    
    from StandardParticles import StdAllLooseKaons     as kaons 
    from StandardParticles import StdAllLooseMuons     as muons
    from StandardParticles import StdLooseResolvedPi0  as pi0  
    
    decay       = '[ B+ -> (J/psi(1S) =>  mu+  mu- )  ( K*(892)+ ->  K+  pi0 ) ]CC'
    decay_K     = '[ B+ -> (J/psi(1S) =>  mu+  mu- )  ( K*(892)+ -> ^K+  pi0 ) ]CC'
    decay_Kst   = '[ B+ -> (J/psi(1S) =>  mu+  mu- ) ^( K*(892)+ ->  K+  pi0 ) ]CC'
    decay_mu    = '[ B+ -> (J/psi(1S) => ^mu+ ^mu- )  ( K*(892)+ -> ^K+  pi0 ) ]CC'
    decay_pi0   = '[ B+ -> (J/psi(1S) => ^mu+ ^mu- )  ( K*(892)+ ->  K+ ^pi0 ) ]CC'
    decay_gamma = '[ B+ -> (J/psi(1S) => ^mu+ ^mu- )  ( K*(892)+ ->  K+ (pi0 -> ^gamma ^gamma ) ) ]CC'
    
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
    
    true_pi0 = SimpleSelection (
        'MCTruePi0'    , ## selection name 
        FilterDesktop   , ## algorithm type
        [ pi0      ] , ## input/required selection
        ## algorithm propperties 
        Preambulo = [ "from LoKiPhysMC.decorators import *" ] ,
        Code      = """
        mcMatch ('%s')
        & CHILDCUT ( mcMatch ('%s') , 1 )
        & CHILDCUT ( mcMatch ('%s') , 2 )
        """ % ( decay_pi0 , decay_gamma , decay_gamma )
        )
    
    from GaudiConfUtils.ConfigurableGenerators import CombineParticles
    sel_Kst = SimpleSelection (
        'SelKst'          , ## name 
        CombineParticles  , ## algorithm type 
        [ true_pi0 , true_kaons ]    , ## input/required selections 
        ## algorithm propperties 
        Preambulo = [ "from LoKiPhysMC.decorators import *" ] ,
        ## the decays to be reconstructed
        DecayDescriptor = '[K*(892)+ -> K+ pi0]cc' ,
        ## combination cut : mass window 
        CombinationCut = """
        in_range ( 0.8 *  GeV ,  AM , 1.0 * GeV )
        """ ,
        ##      mother cut : require good vertex
        MotherCut = "mcMatch ('%s' )" % decay_Kst
        )

    from GaudiConfUtils.ConfigurableGenerators import CombineParticles
    sel_Jpsi = SimpleSelection (
        'SelJpsi'         , ## name 
        CombineParticles  , ## algorithm type 
        [ true_muons ]    , ## input/required selections 
        ## the decays to be reconstructed
        DecayDescriptor = 'J/psi(1S) -> mu+ mu-' ,
        ## combination cut : mass window 
        CombinationCut = """
        in_range ( 3.0 *  GeV ,  AM , 3.2 * GeV )
        """ ,
        ##      mother cut : require good vertex
        MotherCut      = "  VFASPF( VCHI2 ) <  10 "
        )

    from PhysSelPython.Wrappers import SelectionSequence
    SEQ_Kst  = SelectionSequence ( 'TRUEKSSTAR'  , sel_Kst    )
    SEQ_Jpsi = SelectionSequence ( 'TRUEPSI'     , sel_Jpsi   )
    SEQ_K    = SelectionSequence ( 'TRUEK'       , true_kaons ) 
    
    alg_name = 'Jascha'
    ## add the name of Bender algorithm into User sequence sequence 
    from Configurables import GaudiSequencer
    seq_lines = GaudiSequencer (
        'SEQ_lines' ,
        Members = [
        GaudiSequencer( 'K'   , Members = [ SEQ_K   .sequence() ] ) ,
        GaudiSequencer( 'PSI' , Members = [ SEQ_Jpsi.sequence() ] ) ,
        GaudiSequencer( 'KST' , Members = [ SEQ_Kst .sequence() ] ) , alg_name ]
        )
    
    dv.UserAlgorithms = [ seq_lines ] 
        
    ## define the input data
    setData  ( inputdata , catalogs , castor )
    
    ## get/create application manager
    gaudi = appMgr() 
    
    alg   = Jascha (
        alg_name ,
        Inputs    = [ SEQ_K   .outputLocation()            ,
                      SEQ_Kst .outputLocation()            ,
                      SEQ_Jpsi.outputLocation()            ,
                      'Phys/StdLooseResolvedPi0/Particles' ] ,
        ParticleCombiners = { '' : 'LoKi::VertexFitter/TESTFIT:PUBLIC'} , 
        VertexFitters     = { '' : 'LoKi::VertexFitter/TESTFIT:PUBLIC'}
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
    
    ## ('/MC/2012/Beam4000GeV-2012-MagDown-Nu2.5-Pythia8/Sim08g/Digi13/Trig0x409f0045/Reco14a/Stripping21NoPrescalingFlagged/12143401/ALLSTREAMS.DST', 'dddb-20130929-1', 'sim-20130522-1-vc-md100', 25, 519292)  

    inputdata = [
        ##'/lhcb/MC/2012/ALLSTREAMS.DST/00043279/0000/00043279_00000025_2.AllStreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00043279/0000/00043279_00000028_2.AllStreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00043279/0000/00043279_00000017_2.AllStreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00043279/0000/00043279_00000018_2.AllStreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00043279/0000/00043279_00000009_2.AllStreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00043279/0000/00043279_00000032_2.AllStreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00043279/0000/00043279_00000008_2.AllStreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00043279/0000/00043279_00000007_2.AllStreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00043279/0000/00043279_00000024_2.AllStreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00043279/0000/00043279_00000033_2.AllStreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00043279/0000/00043279_00000015_2.AllStreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00043279/0000/00043279_00000022_2.AllStreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00043279/0000/00043279_00000034_2.AllStreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00043279/0000/00043279_00000016_2.AllStreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00043279/0000/00043279_00000012_2.AllStreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00043279/0000/00043279_00000004_2.AllStreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00043279/0000/00043279_00000020_2.AllStreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00043279/0000/00043279_00000029_2.AllStreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00043279/0000/00043279_00000019_2.AllStreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00043279/0000/00043279_00000014_2.AllStreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00043279/0000/00043279_00000013_2.AllStreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00043279/0000/00043279_00000001_2.AllStreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00043279/0000/00043279_00000010_2.AllStreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00043279/0000/00043279_00000006_2.AllStreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00043279/0000/00043279_00000026_2.AllStreams.dst',
        ]
    
    configure( inputdata , castor = True )
    
    ## event loop 
    run(1000)

    gaudi = appMgr()
    alg   = gaudi.algorithm('Lines')
    
# =============================================================================
# The END
# =============================================================================


