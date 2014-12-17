#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$ 
# =============================================================================
## @file MakeB.py 
#
#  Make combinatorics and composed particle creation in Bender 
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
"""Make combinatorics and composed particle creation in Bender 

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
from   GaudiKernel.SystemOfUnits import GeV 
## import everything from bender 
from   Bender.Main import * 
# =============================================================================
## optional logging
# =============================================================================
from Bender.Logger import getLogger 
if '__main__' == __name__ : logger = getLogger ( 'BenderTutor.MakeB' )
else                      : logger = getLogger ( __name__ )
# =============================================================================
## @class MakeB 
#  Make combinatorics and composed particle creation in Bender 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
class MakeB(Algo):                  
    """
    Make combinatorics and composed particle creation in Bender 
    """
    ## the main 'analysis' method 
    def analyse( self ) :   ## IMPORTANT! 
        """
        The main 'analysis' method
        """
        ## get J/psi mesons from the input
        psis  = self.select ( 'psi' , 'J/psi(1S) -> mu+ mu-' )
        if psis.empty() : return self.Warning('No J/psi candidates are found!', SUCCESS )
        
        ## get energetic kaons from the input: note we select both K+ and K-
        kaons = self.select ( 'k'  , ( 'K+' == ABSID ) & ( PT > 1 * GeV ) &  ( PROBNNk > 0.2 ) )
        
        ## prepare useful functors:
        mfit  = DTF_FUN  ( M , True , strings('J/psi(1S)') ) ## use PV and J/psi-mass constraint
        c2dtf = DTF_CHI2NDOF ( True , strings('J/psi(1S)') ) ## ditto
        ctau  = DTF_CTAU ( 0 , True , strings('J/psi(1S)') ) ## ditto 
                                
        tup = self.nTuple('MyTuple')

        ## make a loop over J/psi K combinations :
        for b in self.loop( 'psi k' , 'B+' ) :
            ## fast evaluation of mass
            m12 = b.mass(1,2) / GeV 
            if not 4.9 < m12  < 5.6 : continue

            ## check the common vertex
            vchi2 = VCHI2 ( b ) 
            if not 0<= vchi2 < 20   : continue   ## skip large chi2 and fit failures  
            
            ## get the mass of B
            mass = M ( b ) / GeV 
            if not 4.9 < mass < 5.6 : continue 

            psi   = b(1) ## the first daughter 
            kaon  = b(2) ## the second daughter
            if not psi  : continue
            if not kaon : continue
            
            if   Q ( kaon ) < 0 : b.setPID ( 'B-' )
            else                : b.setPID ( 'B+' ) 
            
            m_fit  = mfit  ( b ) / GeV
            if not  5.0 <= m_fit  <= 5.5 : continue 
            c_tau  = ctau  ( b )
            if not -1.0 <= c_tau  <= 100 : continue
            c2_dtf = c2dtf ( b )  
            if not -1.0 <= c2_dtf <=  10 : continue
                        
            tup.column  (  'mB'    , mass   )
            tup.column  (  'mBdtf' , m_fit  )
            tup.column  (  'c2dtf' , c2_dtf )
            tup.column  (  'ctau'  , c_tau  )
            
            tup.write()
            
            ## finally, save good candiated! 
            b.save ( 'MyB' )                        


        ## get all saved B-candidates 
        savedb = self.selected('MyB')
        if 0 < len ( savedb ) : self.setFilterPassed( True ) 
            
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
    Jpsi_location  = '/Event/Dimuon/Phys/FullDSTDiMuonJpsi2MuMuDetachedLine/Particles'
    from PhysConf.Filters import LoKi_Filters
    fltrs = LoKi_Filters ( STRIP_Code = "HLT_PASS_RE('Stripping.*DiMuonJpsi2MuMuDeta.*')"  )
    
    ## import DaVinci    
    from Configurables import DaVinci
    ## delegate the actual configuration to DaVinci
    dv = DaVinci ( EventPreFilters =  fltrs.filters ('Filters') , ## IMPORTANT
                   DataType        = '2012'       ,
                   InputType       = 'DST'        ,
                   TupleFile       = 'MakeB.root' )
    
    ## add the name of Bender algorithm into User sequence sequence 
    alg_name = 'MakeB'
    dv.UserAlgorithms += [ alg_name ]
    
    from StandardParticles import StdLooseKaons 
    kaons = StdLooseKaons.outputLocation()
    
    ## define the input data
    setData  ( inputdata , catalogs , castor )
    
    ## get/create application manager
    gaudi = appMgr() 
    
    ## (1) create the algorithm with given name 
    alg   = MakeB (
        alg_name  ,
        Inputs            = [ Jpsi_location , kaons ] , 
        ParticleCombiners = { '' : 'LoKi::VertexFitter' }
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
    ## BKQuery ( '/LHCb/Collision12/Beam4000GeV-VeloClosed-MagDown/Real Data/Reco14/Stripping20/90000000/DIMUON.DST' )
    inputdata = [
        '/lhcb/LHCb/Collision12/DIMUON.DST/00020198/0001/00020198_00012742_1.dimuon.dst',
        '/lhcb/LHCb/Collision12/DIMUON.DST/00020198/0001/00020198_00015767_1.dimuon.dst',
        '/lhcb/LHCb/Collision12/DIMUON.DST/00020198/0000/00020198_00007306_1.dimuon.dst',
        '/lhcb/LHCb/Collision12/DIMUON.DST/00020198/0001/00020198_00016402_1.dimuon.dst',
        '/lhcb/LHCb/Collision12/DIMUON.DST/00020198/0000/00020198_00002692_1.dimuon.dst',
        '/lhcb/LHCb/Collision12/DIMUON.DST/00020738/0000/00020738_00005943_1.dimuon.dst',
        '/lhcb/LHCb/Collision12/DIMUON.DST/00020350/0000/00020350_00008129_1.dimuon.dst',
        '/lhcb/LHCb/Collision12/DIMUON.DST/00021211/0000/00021211_00000461_1.dimuon.dst',
        '/lhcb/LHCb/Collision12/DIMUON.DST/00021211/0000/00021211_00001009_1.dimuon.dst',
        '/lhcb/LHCb/Collision12/DIMUON.DST/00020350/0000/00020350_00001011_1.dimuon.dst',
        ]
    configure( inputdata , castor = True )
    
    ## event loop 
    run(15000)

    gaudi = appMgr()
    alg   = gaudi.algorithm('MakeB')
    
    alg.NTuplePrint = True 
                          
    
# =============================================================================
# The END
# =============================================================================


