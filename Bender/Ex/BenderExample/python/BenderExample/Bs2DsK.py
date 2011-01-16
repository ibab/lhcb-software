#!/usr/bin/env python
# =============================================================================
# $Id$ 
# =============================================================================
# $URL$ 
# =============================================================================
## @file BenderExample/Bs2DsK.py
#  The simple Bender-based example: find recontructed Bs -> BsK candidates 
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
#  with the campain of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
#  @date 2006-10-12
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""
The simple Bender-based example: find recontructed Bs -> BsK candidates

This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the campain of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

Last modification $Date$
               by $Author$
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__date__    = " 2006-10-12 "
__version__ = " Version $Revision$ "
# =============================================================================
## import everything form bender
import GaudiKernel.SystemOfUnits as Units 
from   Bender.Awesome            import *
import LoKiMC.trees              as Trees  
import PartProp.Nodes            as Nodes
from   LoKiCore.functions        import *
# =============================================================================
## Simple class for access MC-truth 
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2006-10-13
class Bs2DsK(AlgoMC) :
    """
    find recontructed Bs -> Ds K candidates 
    """
    ## standard method for analyses
    def analyse( self ) :
        """
        Standard method for analyses
        """

        # Ds+/Ds- -> K+ K- pi+/pi-
        ds = Trees.MCExclusive  ( Nodes.CC('D_s+') , 1 , True )
        ds += 'K+'
        ds += 'K-'
        ds += Nodes.CC('pi+')
        
        ## Xb -> Ds+/Ds- K+/K-
        bs = Trees.MCExclusive ( Nodes.Beauty )
        bs += ds                 # mark Ds!
        bs += Nodes.CC("K+") 
        
        st =  bs.validate ( self.ppSvc() )
        if st.isFailure()  : return st

        ## Xb -> ^Ds+/Ds- K+/K-
        bsm = Trees.MCExclusive ( Nodes.Beauty )
        bsm += mark ( ds )     # mark Ds!
        bsm += Nodes.CC("K+") 
        
        st =  bsm.validate ( self.ppSvc() )
        if st.isFailure()  : return st
        
        cut = MCDECTREE(bs) 
        mcbs = self.mcselect ('mcbs' , ( 'B_s0' == MCABSID ) & cut  )

        if mcbs.empty() or 1 < mcbs.size() :
            return self.Warning ( 'No mc-trees are found' , SUCCESS )        

        # collect "marked" MC particles
        mcds = LHCb.MCParticle.ConstVector ()

        bsm.reset() 
        for b in mcbs :
            if bsm ( b ) :
                bsm.collect ( mcds )
                bsm.reset()


        mcBs = MCTRUTH ( self.mcTruth() , mcbs )        
        mcDs = MCTRUTH ( self.mcTruth() , mcds )
        
        kaons = self.select ( 'kaons' ,
                              ( 'K+' == ABSID   ) &
                              ( PIDK > 2        ) & mcDs )
        pions = self.select ( 'pion'   ,
                              ( 'pi+' == ABSID  ) &
                              ( PIDK  < 1       ) &
                              ( PIDe  < 1       ) &
                              ( PIDmu < 1       ) & mcDs )
        
        bkaons = self.select ( 'bk' ,
                               ( 'K+' == ABSID ) & mcBs & ~mcDs )
        
        k1p = self.select ( 'k+' , kaons , 0 < Q )
        k1m = self.select ( 'k-' , kaons , 0 > Q )
        
        ds = self.loop ( 'k+ k- pion' , 'D_s+' )
        for D in ds :
            m123 = D.mass(1,2,3) / Units.GeV
            if not 1.4 < m123 < 2.0 : continue
            chi2 = VCHI2 ( D )
            if not 0 <=  chi2 < 100 : continue
            if  not mcDs ( D )      : continue 
            self.plot ( M(D) / Units.GeV , 'mass K+ K- pi' , 1.8 , 2.1 , 300 )
            sq = SUMQ( D )
            if sq < 0 : D.setPID ( 'D_s-' )
            D.save('Ds')
            
        bs = self.loop ( 'Ds bk' , 'B_s0')
        for B in bs :
            m12 = B.mass(1,2) / Units.GeV
            if not 3 < m12  < 7   : continue
            chi2 = VCHI2 ( B ) 
            if not 0<= chi2 < 100 : continue 
            if not mcDs ( B(1) )  : continue
            if not mcBs ( B(2) )  : continue
            if not mcBs ( B    )  : continue 
            self.plot ( M(B) / Units.GeV , 'mass Ds K' , 4.0 , 6.0 , 200 )
            B.save ('Bs') 

        Bs = self.selected('Bs')
        self.setFilterPassed ( not Bs.empty()  )
            
        return SUCCESS

    ## finalize & print histos 
    def finalize ( self ) :
        """
        Finalize & print histos         
        """
        histos = self.Histos()
        for key in histos :
            h = histos[key]
            if hasattr ( h , 'dump' ) : print h.dump(50,30,True)
        return AlgoMC.finalize ( self )
    
        
# =============================================================================
## configure the job
def configure ( datafiles  , catalogs = [] ) :
    """
    Configure the job
    """
    
    from Configurables import DaVinci
    daVinci = DaVinci (
        DataType   = 'MC09'  , 
        Simulation = True
        )
    
    from Configurables import HistogramPersistencySvc 
    HistogramPersistencySvc ( OutputFile = 'Bs2DsK_Histos.root' ) 

    ## define the input data 
    setData ( datafiles , catalogs )
    
    gaudi = appMgr() 
    
    ## create the local algorithm:
    alg = Bs2DsK(
        'Bs2DsK'   , 
        ## PP -> MC relation tables 
        PP2MCs         = [ 'Relations/Rec/ProtoP/Charged' ] ,
        ## print histos 
        HistoPrint     = True , 
        ## define the input particles:
        InputLocations = [
        'StdTightKaons'  ,
        'StdTightPions'  ,
        'StdNoPIDsKaons'
        ]
        )
    
    gaudi.addAlgorithm ( alg ) 
    ##gaudi.setAlgorithms( [alg] )
    
    return SUCCESS 

# =============================================================================
## job steering 
if __name__ == '__main__' :
    
    ## make printout of the own documentations 
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print ' dir(%s) : %s ' % ( __name__    , dir() )
    print '*'*120
    
    ## configure the job:
    inputdata = [ 
        # Bs -> Ds K
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005055/0000/00005055_00000103_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005055/0000/00005055_00000104_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005055/0000/00005055_00000105_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005055/0000/00005055_00000106_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005055/0000/00005055_00000107_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005055/0000/00005055_00000108_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005055/0000/00005055_00000109_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005055/0000/00005055_00000110_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005055/0000/00005055_00000111_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005055/0000/00005055_00000112_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'"]

    
    configure( inputdata ) 


    ## run the job
    run ( 201 )
    
# =============================================================================
# The END 
# =============================================================================
