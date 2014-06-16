#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$ 
# =============================================================================
## @file HistosAndTuples.py 
#
#  Inspect particles from certain TES location and fill histograms and tuple 
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
"""Inspect particles from certain TES location and fill histograms and tuple 

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
import ROOT 
from GaudiKernel.SystemOfUnits import GeV 
## import everything from bender 
from Bender.Main import *
# =============================================================================
## optional logging
# =============================================================================
from Bender.Logger import getLogger 
if '__main__' == __name__ : logger = getLogger ( 'BenderTutor.Histos&Tuples' )
else                      : logger = getLogger ( __name__ )
# =============================================================================
## @class HistosAndTuples
#  Inspect particles from the certain TES location and fill histos and tuple
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
class HistosAndTuples(Algo):
    """
    Inspect particles from the certain TES location and fill histos and tuple
    """
    ## the main 'analysis' method 
    def analyse( self ) :   ## IMPORTANT! 
        """
        The main 'analysis' method
        """

        ## get *ALL* particles from the input locations 
        particles = self.select ( 'allinputs', ALL  )
        if particles.empty() : return self.Warning('No input particles', SUCCESS )

        ## loop over particles and fill few histograms: 
        for p in particles :
            #          what        histo-ID    low-edge  high-edge  #bins 
            self.plot( PT (p)/GeV , 'pt(B)'  , 0        , 20       , 50  ) 
            self.plot( M  (p)/GeV , 'm(B)'   , 5.2      , 5.4      , 40  ) 
            self.plot( M1 (p)/GeV , 'm(psi)' , 3.0      , 3.2      , 20  )
            self.plot( M23(p)/GeV , 'm(KK)'  , 1.0      , 1.040    , 20  )

        tup = self.nTuple('MyTuple')
        for p in particles :

            tup.column_float( 'mB'   , M   ( p ) / GeV )
            tup.column_float( 'ptB'  , PT  ( p ) / GeV ) 
            tup.column_float( 'mPsi' , M1  ( p ) / GeV )
            tup.column_float( 'mKK'  , M23 ( p ) / GeV )

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
                   HistogramFile = 'Histos.root' , 
                   TupleFile     = 'Tuples.root'   ## IMPORTANT 
                   )
    
    ## add the name of Bender algorithm into User sequence sequence 
    alg_name = 'HistosAndTuples'
    dv.UserAlgorithms += [ alg_name ]
    
    ## define the input data
    setData  ( inputdata , catalogs , castor )
    
    ## get/create application manager
    gaudi = appMgr() 
    
    ## (1) create the algorithm with given name 
    alg = HistosAndTuples (
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

    ## get the applictaion manager 
    gaudi = appMgr()

    alg = gaudi.algorithm('HistosAndTuples')
    for key in alg.Histos() :
        histo = alg.Histos(key) 
        print histo.dump(20,10)

    ## event loop 
    run(5000)

    ## alg.PrintHistos()
    
# =============================================================================
# The END
# =============================================================================


