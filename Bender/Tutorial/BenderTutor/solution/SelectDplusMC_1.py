#!/usr/bin/env python
# =============================================================================
# $Id: SelectDplusMC_1.py,v 1.1 2010-09-13 13:24:04 ibelyaev Exp $ 
# =============================================================================
## @file solution/SelectDplusMC_1.py
#
#  Simple example that illustrates the MC-selection of D+/D_s+ 
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
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2010-09-10
#
#  Last modification $Date: 2010-09-13 13:24:04 $
#                 by $Author: ibelyaev $
# =============================================================================
"""
Simple example that illustrates the MC-selection of D+/D_s+ 

This file is a part of BENDER project:
    ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
    ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
    ``No Vanya's lines are allowed in LHCb/Gaudi software.''

Last modification $Date: 2010-09-13 13:24:04 $
               by $Author: ibelyaev $
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__date__    = " 2010-10-10 " 
__version__ = " Version $Revision: 1.1 $ "
# =============================================================================
## import everything from BENDER
from Bender.MainMC import *
# =============================================================================
## @class SelectDplus
#  Simple algoritm to MC-select D+/D_s+ from stripped DSTs 
class SelectDplusMC(AlgoMC):
    
    # =========================================================================
    ## the main method: analyse the event
    def analyse( self ) :
        """
        the main method: analyse the event
        """
        
        ## check MC-truth

        # get the list of true D        
        mcD  = self.mcselect ( 'd_or_ds'   , '[ ( D+ | D_s+ )  -->  K-  K+  pi+]CC' )
        
        for p in mcD : print p.decay() 

        ## require only one true MC-decay 
        if  1 != len(mcD)  : return SUCCESS
        
        Dmc    = mcD(0)
        mcCTau = MCCTAU ( Dmc )

        
        # true kaons from D-decays
        mcK  = self.mcselect ( 'mcK'  , mcD , '[ ( D+ | D_s+ ) --> ^K- ^K+  pi+]CC' )
        # true pions from D-decays
        mcPi = self.mcselect ( 'mcPi' , mcD , '[ ( D+ | D_s+ ) -->  K-  K+ ^pi+]CC' )


        # get mc-truth matcher:
        match = self.mcTruth('MC-Match')

        ## functors for MC-truth match:
        trueD  = MCTRUTH ( match , mcD  )
        trueK  = MCTRUTH ( match , mcK  )
        truePi = MCTRUTH ( match , mcPi )
        
        
        ## get the recontructed partiles :

        kaons  = self.select ( 'K'  , ( 'K+'  == ABSID ) & trueK  ) 
        pions  = self.select ( 'pi' , ( 'pi+' == ABSID ) & truePi ) 

        print len(kaons), len(pions)
        
        kplus  = self.select ( 'K+' , kaons , Q > 0 )
        kminus = self.select ( 'K-' , kaons , Q < 0 )

        D = self.loop ( 'K- K+ pi' , 'D+' )

        for d in D :
            
            m123 = d.mass(1,2,3) / GeV 
            if not 1.7 <  m123   <  2.0 : continue

            vchi2 = VCHI2 ( d )
            if not 0   <= vchi2  <  100 : continue

            m = M( d ) / GeV
            if not 1.8 <  m      < 1.95 : continue

            self.plot (  m , ' Mass KK#pi - All   ' , 1.80 , 1.95 , 150 ) 

            kaon1 = d(1)
            kaon2 = d(2)
            pion  = d(3)

            if not trueK  ( kaon1 ) : continue 
            if not trueK  ( kaon2 ) : continue 
            if not truePi ( pion  ) : continue
            if not trueD  ( d     ) : continue
            
            self.plot (  m , ' Mass KK#pi - True ' , 1.80 , 1.95 , 150 )
            
            pv = self.bestVertex( d.particle() )
            if not pv : contine
            
            ctauFun = LTIME ( self.lifetimeFitter() , pv ) * c_light 

            ctau = ctauFun ( d )

            if not -0.1 < ctau < 1.9 : continue
            self.plot ( ctau  , 'c*#tau for D^{+} ' , -0.1 , 1.9 , 150 )

            ## 
            dCTau = ctau  - mcCTau 
            self.plot ( dCTau  , ' #delta(c*#tau) for D^{+} ' , -0.2 , 0.2 )
            
            
            
        return SUCCESS


    ## finalize & print histos 
    def finalize ( self ) :
        """
        Finalize & print histos         
        """
        #
        self.dumpHistos ( 50 , 30 , True )
        #
        return AlgoMC.finalize ( self )

# =============================================================================
## The configuration of the job
def configure( inputdata , catalogs = [] ) :
    """
    The configuration of the job
    """
    from Configurables import DaVinci , GaudiSequencer 
    DaVinci ( DataType        = '2010' ,
              Simulation      = True   ,     ## ATTENTION!!!
              Lumi            = False  ,
              HistogramFile   = 'DplusMC_Histos.root' )
    
    from Configurables import CondDB
    CondDB  ( IgnoreHeartBeat = True )
    
    setData ( inputdata , catalogs )
    
    ## get/create Application Manager
    gaudi = appMgr()
    
    # modify/update the configuration:
    
    ## create the algorithm
    alg = SelectDplusMC (
        'DplusMC'                               , 
        # 
        Inputs = [
        'Phys/StdLooseKaons/Particles' ,
        'Phys/StdLoosePions/Particles'         
        ] ,
        # Use LoKi-fitters :
        ParticleCombiners = { '' : "LoKi::VertexFitter" } , 
        # be efficient: use MC-truth only for charged particles:
        PP2MCs            = [ 'Relations/Rec/ProtoP/Charged'] 
        )
    
    ## add algorithm into main DaVinci sequence
    dvUser = gaudi.algorithm('GaudiSequencer/DaVinciUserSequence' , True )
    dvUser.Members += [ alg.name() ]
    
    return SUCCESS 
# =============================================================================


# =============================================================================
## Job steering 
if __name__ == '__main__' :
    
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120
    
    ## job configuration
    inputdata = [
        ## dbpath: /LHCb/MC/2010/Beam3500GeV-VeloClosed-MagUp-Nu1/2010-sim03Reco03-withTruth/21263000/DST
        '/castor/cern.ch/grid' + '/lhcb/MC/2010/DST/00006767/0000/00006767_000000%02d_1.dst' % i for i in range ( 1 , 30 )
        ]
    
    configure( inputdata )
    
    ## event loop 
    run(10)
    
# =============================================================================
# The END
# =============================================================================
