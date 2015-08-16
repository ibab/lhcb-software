#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id: TisTosTuple.py 172885 2014-05-16 17:48:30Z ibelyaev $ 
# =============================================================================
## @file TisTosTuple.py 
#
#  Simple demonstration how to fill n-tuple with TisTos information
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
#                    $Revision: 172885 $
#  Last modification $Date: 2014-05-16 19:48:30 +0200 (Fri, 16 May 2014) $
#                 by $Author: ibelyaev $
# =============================================================================
"""Simple demonstration how to fill n-tuple with TisTos information

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
from   Bender.Main import *
# =============================================================================
## optional logging
# =============================================================================
from Bender.Logger import getLogger 
if '__main__' == __name__ : logger = getLogger ( 'BenderTutor.Tis&Tos' )
else                      : logger = getLogger ( __name__ )
# =============================================================================
## @class TisTosTuple
#  Enhanced functionality for n-tuples 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
class TisTosTuple(Algo):
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
        triggers ['psi'] = {} ## slot to keep information for J/psi 
        
        #
        ## the lines to be investigated in details
        #
        lines    = {}
        lines    [ "psi" ] = {} ## trigger lines for J/psi

        #
        ## six mandatory keys:
        #
        lines    [ "psi" ][   'L0TOS' ] = 'L0(DiMuon|Muon)Decision'
        lines    [ "psi" ][   'L0TIS' ] = 'L0(Hadron|DiMuon|Muon|Electron|Photon)Decision'
        lines    [ "psi" ][ 'Hlt1TOS' ] = 'Hlt1(DiMuon|TrackMuon).*Decision'
        lines    [ "psi" ][ 'Hlt1TIS' ] = 'Hlt1(DiMuon|SingleMuon|Track).*Decision'
        lines    [ "psi" ][ 'Hlt2TOS' ] = 'Hlt2DiMuon.*Decision'
        lines    [ "psi" ][ 'Hlt2TIS' ] = 'Hlt2(Charm|Topo|DiMuon|Single).*Decision'

        sc = self.tisTos_initialize ( triggers , lines )
        if sc.isFailure() : return sc
     
        return SUCCESS
    
    ## the main 'analysis' method 
    def analyse( self ) :   ## IMPORTANT! 
        """
        The main 'analysis' method
        """
        
        ## get particles from the input locations 
        particles = self.select ( 'bmesons', 'Beauty -> J/psi(1S) K+ K-'  )
        if particles.empty() : return self.Warning('No input particles', SUCCESS )
        
        ## prepare useful functors:
        mfit  = DTF_FUN  ( M , True , strings('J/psi(1S)') ) ## use PV and J/psi-mass constraint
        c2dtf = DTF_CHI2NDOF ( True , strings('J/psi(1S)') ) ## ditto
        ctau  = DTF_CTAU ( 0 , True , strings('J/psi(1S)') ) ## ditto 
        
        tup = self.nTuple('MyTuple')
        for b in particles :
            
            psi =b(1) ## the first daughter: J/psi 
            
            c2_dtf = c2dtf ( b )  
            if not -1.0 <= c2_dtf <=   5 : continue
            m_fit  = mfit  ( b ) / GeV
            if not  5.2 <= m_fit  <= 5.4 : continue 
            c_tau  = ctau  ( b )
            if not  0.1 <= c_tau  <=  10 : continue

            #
            ## fill few kinematic variables for particles,
            #
            self.treatKine   ( tup , b   , '_b'   )
            self.treatKine   ( tup , psi , '_psi' )

            #
            ## collect trigger information for J/psi (list of fired lines)
            #
            self.decisions   ( psi , self.triggers['psi'] )

            #
            ## fill n-tuple with TISTOS information for J/psi
            #
            self.tisTos      (
                psi               , ## particle 
                tup               , ## n-tuple
                'psi_'            , ## prefix for variable name in n-tuple
                self.lines['psi'] , ## trigger lines to be inspected
                verbose = True      ## good ooption for those who hates bit-wise operations 
                )
            
            tup.write() 
            
        ## 
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
    rootInTES = '/Event/PSIX'
    dv = DaVinci ( DataType      = '2012'        ,
                   InputType     = 'MDST'        ,
                   RootInTES     = rootInTES     ,
                   TupleFile     = 'TisTosTuples.root' ## IMPORTANT 
                   )
    
    ## add the name of Bender algorithm into User sequence sequence 
    alg_name = 'TisTos'
    dv.UserAlgorithms += [ alg_name ]
    
    ## define the input data
    setData  ( inputdata , catalogs , castor )
    
    ## get/create application manager
    gaudi = appMgr() 
    
    ## (1) create the algorithm with given name 
    alg   = TisTosTuple (
        alg_name ,
        RootInTES = rootInTES , ## we are reading uDST
        Inputs    = [ 'Phys/SelPsi2KForPsiX/Particles' ]
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
    ## BKQuery ( '/LHCb/Collision12/Beam4000GeV-VeloClosed-MagDown/Real Data/Reco14/Stripping20/WGBandQSelection7/90000000/PSIX.MDST'   )
    inputdata = [
        '/lhcb/LHCb/Collision12/PSIX.MDST/00035290/0000/00035290_00000221_1.psix.mdst',
        '/lhcb/LHCb/Collision12/PSIX.MDST/00035290/0000/00035290_00000282_1.psix.mdst',
        '/lhcb/LHCb/Collision12/PSIX.MDST/00035290/0000/00035290_00000238_1.psix.mdst',
        '/lhcb/LHCb/Collision12/PSIX.MDST/00035290/0000/00035290_00000185_1.psix.mdst',
        '/lhcb/LHCb/Collision12/PSIX.MDST/00035290/0000/00035290_00000278_1.psix.mdst',
        '/lhcb/LHCb/Collision12/PSIX.MDST/00035290/0000/00035290_00000190_1.psix.mdst',
        '/lhcb/LHCb/Collision12/PSIX.MDST/00035290/0000/00035290_00000192_1.psix.mdst',
        '/lhcb/LHCb/Collision12/PSIX.MDST/00035290/0000/00035290_00000247_1.psix.mdst',
        '/lhcb/LHCb/Collision12/PSIX.MDST/00035290/0000/00035290_00000225_1.psix.mdst',
        '/lhcb/LHCb/Collision12/PSIX.MDST/00035290/0000/00035290_00000289_1.psix.mdst',
        ]
    
    configure( inputdata , castor = True )
    
    ## event loop 
    run(10000)

    gaudi = appMgr()
    alg   = gaudi.algorithm('TisTos')
    alg.NTuplePrint = True 
                          
    
# =============================================================================
# The END
# =============================================================================


