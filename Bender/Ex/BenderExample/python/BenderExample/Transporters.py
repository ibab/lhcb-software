#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id:$ 
# =============================================================================
## @file TestTransporters.py
#
#  Compare CPU performance for particle transporters 
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
#                    $Revision: 193776 $
#  Last modification $Date: 2015-08-16 16:25:37 +0200 (Sun, 16 Aug 2015) $
#                 by $Author: ibelyaev $
# =============================================================================
"""Compare CPU performance for particle transporters 

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

                  $Revision: 193776 $
Last modification $Date: 2015-08-16 16:25:37 +0200 (Sun, 16 Aug 2015) $
               by $Author: ibelyaev $
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@itep.ru "
__date__    = " 2016-01-15 " 
__version__ = "$Revision: 193776 $ "
# ============================================================================= 
## optional logging
# =============================================================================
from Bender.Logger import getLogger
if '__main__' == __name__ : logger = getLogger ( 'TestTransporters' )
else                      : logger = getLogger ( __name__ )
# =============================================================================
## import everything from bender 
from Bender.Main               import *
from GaudiKernel.SystemOfUnits import cm ,  GeV
# =============================================================================
## @class TransportersTest
#  Compare CPU performance for particle transporters 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2016-01-14
class TransportersTest(Algo):
    """
    Compare CPU performance for particle transporters 
    """
    def initialize ( self ) :
        
        sc = Algo.initialize ( self )
        if sc.isFailure() : return sc
        
        IPT = cpp.IParticleTransporter
        
        self.tr1 = self.tool( IPT ,'ParticleTransporter/PT1'          , parent=self )
        if not self.tr1 : return FAILURE
        
        self.tr2 = self.tool( IPT ,'DaVinci::ParticleTransporter/PT2' , parent=self )
        if not self.tr2 : return FAILURE
        
        self.tr3 = self.tool( IPT ,'DaVinci::ParticleTransporter/PT3' , parent=self )
        if not self.tr3 : return FAILURE 

        self.zpos  = [  5 * i * cm for i in range ( -10 , 10 ) ]
        ## self.zpos += [ 20 * i * cm for i in range (   3 , 10 ) ]
        
        for t in self.tr1,self.tr2,self.tr3 :
            sc = cpp.Gaudi.Utils.setProperty( t , 'MeasureCPUPerformance' , 'true' )
            if sc.isFailure() :
                self.Error('CANNOT set property !!' )
        self.nevt  = 0 
        return sc
    
    def finalize ( self ) :
        
        del self.tr1
        del self.tr2
        del self.tr3

        return Algo.finalize ( self ) 

    def cpu ( self , particles, transporter , tag ) :

        csvc     = self.chronoSvc().get()  
        particle = LHCb.Particle() 
        
        chrono   = cpp.Chrono( csvc , tag )
        for z in self.zpos :
            if -30 * cm < z < 100 * cm :  
                for p in particles:
                    sc = transporter.transport ( p , z , particle )

        del chrono
        
    
    ## the main 'analysis' method 
    def analyse( self ) :   ## IMPORTANT! 
        """
        The main 'analysis' method
        """

        self.nevt += 1
        
        particles = self.select('all', PALL )
        if not particles : 
            return self.Warning( "No particles are found", SUCCESS )
        
                    
        print '#particles: %d  %s/%s, %s/%s, %s/%s' % (
            len(particles)  ,
            self.tr1.type() , self.tr1.name() , 
            self.tr2.type() , self.tr2.name() ,
            self.tr3.type() , self.tr3.name() )
        
        if self.nevt < 10 :

            if 1<= len(particles) :
            
                part0 = particles[0]
                part_ = LHCb.Particle()
                for z in self.zpos :
                    if abs ( z ) < 10 * cm : 
                        self.tr3.transport ( part0 , z , part_ )
                    
            self.cpu ( particles , self.tr1 , 'tr1-0' )
            self.cpu ( particles , self.tr2 , 'tr2-0' )
            
        else :
            
            #self.cpu ( particles , self.tr1 , 'tr1-1' )
            #self.cpu ( particles , self.tr2 , 'tr2-1' )
            
            self.cpu ( particles , self.tr1 , 'tr1-2' )
            self.cpu ( particles , self.tr2 , 'tr2-2' )
        
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
    dv = DaVinci ( DataType        = '2012' ,
                   InputType       = 'DST'  )
    
    ## add the name of Bender algorithm into User sequence sequence 
    alg_name = 'Transporters'
    dv.UserAlgorithms += [ alg_name ]
         
    from StandardParticles import StdLooseKaons, StdLoosePions
    kaons = StdLooseKaons.outputLocation()
    pions = StdLoosePions.outputLocation()
   
    ## define the input data
    setData  ( inputdata , catalogs , castor )
    
    ## get/create application manager
    gaudi = appMgr() 
    
    ## (1) create the algorithm with given name 
    alg   = TransportersTest  (
        alg_name ,
        Inputs  = [ kaons, pions ]
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
    run(110)

# =============================================================================
# The END
# =============================================================================


