#!/usr/bin/env python
# =============================================================================
# $Id: Bs2DsK.py,v 1.4 2009-11-16 16:00:36 ibelyaev Exp $ 
# =============================================================================
## @file BenderExample/Bs2DsK.py
#  The simple Bender-based example: find recontructed Bs -> BsK candidates 
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>"Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis"</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  "C++ ToolKit for Smart and Friendly Physics Analysis"
#
#  By usage of this code one clearly states the disagreement 
#  with the campain of Dr.O.Callot et al.: 
#  "No Vanya's lines are allowed in LHCb/Gaudi software."
#
#  @date 2006-10-12
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
"""
The simple Bender-based example: find recontructed Bs -> BsK candidates 
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $ "
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
    ## standard constructor
    def __init__ ( self , name = 'Bs2DsK' , **kwargs ) :
        """
        Standard constructor
        """ 
        return AlgoMC.__init__ ( self , name , **kwargs )
    
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
        
        ## Xb -> ^Ds+/Ds- K+/K-
        bs = Trees.MCExclusive ( Nodes.Beauty )
        bs += mark ( ds )     # mark Ds!
        bs += Nodes.CC("K+") 
        
        st =  bs.validate ( self.ppSvc() )
        if st.isFailure()  : return st
        
        cut = MCDECTREE(bs) 
        mcbs = self.mcselect ('mcbs' , ( 'B_s0' == MCABSID )  & cut  )
            
        if mcbs.empty() or 1 < mcbs.size() :
            return self.Warning ( 'No mc-trees are found' , SUCCESS )        

        # collect "marked" paricles
        mcds = std.vector('const LHCb::MCParticle*')()

        bs.reset() 
        for b in mcbs :
            if bs ( b ) :
                bs.collect ( mcds )
                bs.reset()
                
        mcBs = MCTRUTH ( self.mcTruth() , mcbs )        
        mcDs = MCTRUTH ( self.mcTruth() , mcds )
        
        kaons = self.select ( 'kaons' ,
                              ( 'K+' == ABSID ) &
                              ( PIDK > 2      ) & mcDs )
        pions = self.select ( 'pion'   ,
                              ( 'pi+' == ABSID )  &
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

# =============================================================================
## configure the job
def configure ( **args ) :
    """
    Configure the job
    """
        
    from Configurables import DaVinci, HistogramPersistencySvc , EventSelector 
    
    daVinci = DaVinci (
        DataType   = 'DC06'  , 
        Simulation = True    )
    
    HistogramPersistencySvc ( OutputFile = 'Bs2DsK_Histos.root' ) 
    
    EventSelector (
        Input = [
        # Bs -> Ds K 
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsK_1.dst' TYP='POOL_ROOTTREE' OPT='READ'"
        ,"DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsK_2.dst' TYP='POOL_ROOTTREE' OPT='READ'"
        ,"DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsK_3.dst' TYP='POOL_ROOTTREE' OPT='READ'"
        ,"DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsK_4.dst' TYP='POOL_ROOTTREE' OPT='READ'"
##         # Bs -> Ds pi 
##         ,"DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsK_1.dst' TYP='POOL_ROOTTREE' OPT='READ'"
##         ,"DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsK_2.dst' TYP='POOL_ROOTTREE' OPT='READ'"
##         ,"DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsK_3.dst' TYP='POOL_ROOTTREE' OPT='READ'"
##         ,"DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsK_4.dst' TYP='POOL_ROOTTREE' OPT='READ'"
        ]
    )
    
    gaudi = appMgr() 
    
    ## create the local algorithm:
    alg = Bs2DsK(
        'Bs2DsK'   , 
        ## PP -> MC relation tables 
        PP2MCs         = [ 'Relations/Rec/ProtoP/Charged' ] ,
        ## print histos 
        HistoPrint     = True , 
        ## define the input particles:
        InputLocations = [ 'StdTightKaons' ,
                           'StdTightPions' ,
                           'StdNoPIDsKaons']
        )
    
    ## gaudi.addAlgorithm ( alg ) 
    gaudi.setAlgorithms( [alg] )
    
    evtSel = gaudi.evtSel()    
    evtSel.PrintFreq = 50
    
    return SUCCESS 
    
# =============================================================================
## job steering 
if __name__ == '__main__' :

    ## make printout of the own documentations 
    print __doc__
    print __author__
    print __version__

    import GaudiPython.HistoUtils
    
    ## configure the job:
    configure()

    ## run the job
    run(1000)

    gaudi=appMgr()

# =============================================================================
# The END 
# =============================================================================
