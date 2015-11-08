#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$ 
# =============================================================================
## @file FakeParticle.py 
#
#  Demonstration how to use "Fake-H" utility 
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
#  @date 2015-10-31
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""Demonstration how to use ``Fake-H''-utility

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
__date__    = " 2014-10-31" 
__version__ = " $Revision$"
# ============================================================================= 
## import everything from bender 
from Bender.Main               import *
from GaudiKernel.SystemOfUnits import GeV 
from BenderTools.FakeH         import FakeKaon 
# =============================================================================
## optional logging
# =============================================================================
from Bender.Logger import getLogger 
if '__main__' == __name__ : logger = getLogger ( 'BenderTutor.TupleTools' )
else                      : logger = getLogger ( __name__ )
# =============================================================================
## @class FakeParticle
#  Demonstration how to use ``Fake-H''-utility
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2015-10-31
class FakeParticle(Algo):
    """
    Demonstration how to use ``Fake-H''-utility 
    """
    ## the main 'analysis' method 
    def analyse( self ) :   ## IMPORTANT! 
        """
        The main 'analysis' method
        """

        ## get particles from the input locations 
        particles = self.select ( 'bmesons', '[Beauty -> J/psi(1S) K- pi+]CC'  )
        if particles.empty() : return self.Warning('No input particles', SUCCESS )

        fun_m   = DTF_FUN( M   , True , 'J/psi(1S)' )
        fun_m23 = DTF_FUN( M23 , True , 'J/psi(1S)' )
        
        for b in particles :

            pion = b(3)
            
            mass_0 = fun_m   ( b ) / GeV 
            m23_0  = fun_m23 ( b ) / GeV 

            print 'ORIGINAL      :  %s M=%s[GeV] M23=%s[GeV] ' % ( b.decay() , mass_0 , m23_0 )
            print 'WITH FAKE KAON:  %s M=%s[GeV] M23=%s[GeV] ' % ( b.decay() , mass_0 , m23_0 )
            
            with FakeKaon ( pion ) :

                mass_1 = fun_m   ( b ) / GeV 
                m23_1  = fun_m23 ( b ) / GeV 
                
                print 'WITH FAKE KAON:  %s M=%s[GeV] M23=%s[GeV] ' % ( b.decay() , mass_1 , m23_1 )

            print 'CONTEXT-OFF   :  %s ' % b.decay()
                
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
    the_line  = 'Phys/SelPsi2KPiForPsiX/Particles'
    
    from PhysConf.Filters import LoKi_Filters
    fltrs = LoKi_Filters (
        VOID_Code = """
        0 < CONTAINS('%s/%s')
        """ % ( rootInTES , the_line ) 
        )
    dv = DaVinci ( DataType        = '2012'              ,
                   InputType       = 'MDST'              ,
                   RootInTES       = rootInTES           ,
                   EventPreFilters = fltrs.filters('WG') ,   
                   TupleFile       = 'TisTosTuples.root' ## IMPORTANT 
                   )
    
    ## add the name of Bender algorithm into User sequence sequence 
    alg_name = 'Fake'
    dv.UserAlgorithms += [ alg_name ]
    
    ## define the input data
    setData  ( inputdata , catalogs , castor )
    
    ## get/create application manager
    gaudi = appMgr() 
    
    ## (1) create the algorithm with given name 
    alg   = FakeParticle(
        alg_name ,
        RootInTES = rootInTES , ## we are reading uDST
        Inputs    = [ the_line ]
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
    run(500)
    
# =============================================================================
# The END
# =============================================================================


