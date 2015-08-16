#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$ 
# =============================================================================
## @file Tuples2.py 
#
#  Enhanced functionality for n-tuples 
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
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""Enhanced functionality for n-tuples 

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
__version__ = " Version $Revision$ "
# ============================================================================= 
## import everything from bender 
from   Bender.Main import *
# =============================================================================
## optional logging
# =============================================================================
from Bender.Logger import getLogger 
if '__main__' == __name__ : logger = getLogger ( 'BenderTutor.Tuples2' )
else                      : logger = getLogger ( __name__ )
# =============================================================================
## @class Tuples2
#  Enhanced functionality for n-tuples 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
class Tuples2(Algo):
    """
    Enhanced functionality for n-tuples 
    """
    ## the main 'analysis' method 
    def analyse( self ) :   ## IMPORTANT! 
        """
        The main 'analysis' method
        """
        
        ## get particles from the input locations 
        particles = self.select ( 'bmesons', 'Beauty -> J/psi(1S) K+ K-'  )
        if particles.empty() : return self.Warning ( 'No input particles', SUCCESS )
        
        tup = self.nTuple('MyTuple')
        for p in particles :
            
            psi = p(1) ## the first daughter: J/psi 
            
            ## fill few kinematic variables for particles,
            self.treatKine   ( tup , p   , '_b'   )  ## use suffix to mark variables 
            self.treatKine   ( tup , psi , '_psi' )  ## use suffix to mark variables 
            
            self.treatKaons  ( tup , p ) ## fill some basic information for all kaons
            self.treatMuons  ( tup , p ) ## fill some basic information for all muons
            self.treatTracks ( tup , p ) ## fill some basic information for all charged tracks

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
                   TupleFile     = 'Tuples2.root'   ## IMPORTANT 
                   )
    
    ## add the name of Bender algorithm into User sequence sequence 
    alg_name = 'Tuples2'                                  ## mind the name [1]
    dv.UserAlgorithms += [ alg_name ]                     ## mind the name [2] 
    
    ## define the input data
    setData  ( inputdata , catalogs , castor )
    
    ## get/create application manager
    gaudi = appMgr() 
    
    ## (1) create the algorithm with given name 
    alg   = Tuples2 (
        alg_name ,                                        ## mind the name [3]
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
    alg   = gaudi.algorithm('Tuples2')
    alg.NTuplePrint = True 
                          
    
# =============================================================================
# The END
# =============================================================================


