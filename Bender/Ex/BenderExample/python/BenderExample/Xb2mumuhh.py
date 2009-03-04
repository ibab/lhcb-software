#!/usr/bin/env python
# =============================================================================
# $Id: Xb2mumuhh.py,v 1.1 2009-03-04 11:53:39 ibelyaev Exp $
# =============================================================================
## @file BenderExample/Bs2DsK.py
#  The simple Bender-based example: find recontructed Xv -> mu mu h+ h- candidates 
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
#  @date 2009-01-16
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
# =============================================================================
"""
The simple Bender-based example: find recontructed Xv -> mu mu h+ h- candidates 
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $ "
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
class Xb2mumuhh(AlgoMC) :
    """
    find recontructed Xb -> mu+ mu- h+ h- candidates 
    """
    ## standard constructor
    def __init__ ( self , name = 'Xb2mumuhh' ) :
        """
        Standard constructor
        """ 
        return AlgoMC.__init__ ( self , name )
    
    ## standard method for analyses
    def analyse( self ) :
        """
        Standard method for analyses
        """
        
        # Xb -> mu+ mu- h+ h- 
        xb = Trees.MCExclusive  ( Nodes.Beauty )
        
        dimu  = Trees.MCExclusive  ( Nodes.Hadron )
        dimu += 'mu+'
        dimu += 'mu-'
        dimu  = Trees.Photos ( dimu ) 
        
        h1    = Nodes.Pid('pi+') | Nodes.Pid ('K+') | Nodes.Pid( 'p+'  ) 
        h2    = Nodes.Pid('pi-') | Nodes.Pid ('K-') | Nodes.Pid( 'p~-' )

        diha  = Trees.MCExclusive  ( Nodes.Hadron )
        diha += h1 
        diha += h2 
        diha  = Trees.Photos ( diha ) 
        
        x1 = Trees.MCExclusive  ( Nodes.Beauty )
        x1 += 'mu+'
        x1 += 'mu-'
        x1 += h1
        x1 += h2
        x1 = Trees.Photos ( x1 ) 
        
        x2 = Trees.MCExclusive  ( Nodes.Beauty )
        x2 += 'mu+'
        x2 += 'mu-'
        x2 += diha
        x2 = Trees.Photos ( x2 ) 

        x3 = Trees.MCExclusive  ( Nodes.Beauty )
        x3 += dimu
        x3 += h1
        x3 += h2 
        x3 = Trees.Photos ( x3 ) 
        
        x4 = Trees.MCExclusive  ( Nodes.Beauty )
        x4 += dimu
        x4 += diha
        x4 = Trees.Photos ( x4 ) 
        
        xb = x1 | x2 | x3 | x4 
        
        st =  xb.validate ( self.ppSvc() )
        if st.isFailure()  : 
            return self.Error ( 'Invalid decay descriptor' , st )        
        
        cut = MCDECTREE( xb ) 
        mcxb = self.mcselect ('mcxb' , cut )
        if mcxb.empty() :
            return self.Warning ( 'No mc-trees are found' , SUCCESS )        
        
        mcXb = MCTRUTH ( self.mcTruth() , mcxb )        
        
        muons   = self.select ( 'mu' , ( 'mu+' == ABSID )   & mcXb )
        
        if muons.size() < 2 : return SUCCESS
        
        hadrons = self.select ( 'h'  ,
                                ( ( 'pi+' == ABSID ) |
                                  ( 'K+'  == ABSID ) |
                                  ( 'p'   == ABSID ) ) & mcXb )
        
        if hadrons.size() < 2 : return SUCCESS
        

        m1 = self.select ( 'mu+' , muons   , Q > 0 ) 
        m2 = self.select ( 'mu-' , muons   , Q < 0 ) 
        h1 = self.select ( 'h+'  , hadrons , Q > 0 ) 
        h2 = self.select ( 'h-'  , hadrons , Q < 0 ) 
                
        xb = self.loop ( 'mu+ mu- h+ h-' , 'B0' )
        for b in xb :
            m1234 = b.mass(1,2,3,4) / Units.GeV
            if not 3 < m1234 < 7.0  : continue
            chi2 = VCHI2 ( b )
            if not 0 <=  chi2 < 25  : continue
            if  not mcXb ( b )      : continue 
            self.plot ( M( b ) / Units.GeV , 'mass mu+ mu- h+ h-' , 3.0 , 7.0 , 200 )
            b.save ('Xb') 
            
        Xb = self.selected('Xb')
        self.setFilterPassed ( not Xb.empty()  )
            
        return SUCCESS

# =============================================================================
## configure the job
def configure ( **args ) :
    """
    Configure the job
    """
        
    from Configurables import DaVinci, HistogramPersistencySvc , EventSelector 
    
    daVinci = DaVinci (
        DataType   = 'DC06' , # default  
        Simulation = True   ,
        HltType    = '' ) 
    
    HistogramPersistencySvc ( OutputFile = 'Xb2mumuhh_Histos.root' ) 
    
    EventSelector (
        PrintFreq = 100 ,
        Input = [
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Xb2mumuhh_1.dst' TYP='POOL_ROOTTREE' OPT='READ'" ,
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Xb2mumuhh_2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ,
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Xb2mumuhh_3.dst' TYP='POOL_ROOTTREE' OPT='READ'" ,
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2PsiPhi.dst'   TYP='POOL_ROOTTREE' OPT='READ'" ,
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2PsiPhi_1.dst' TYP='POOL_ROOTTREE' OPT='READ'" ,
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2PsiPhi_2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ,
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2PsiPhi_4.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
        ]
        )
    
    gaudi = appMgr() 
    
    ## create local algorithm:
    alg = Xb2mumuhh()
    
    ##gaudi.addAlgorithm ( alg ) 
    gaudi.setAlgorithms( [alg] )
    
    alg.PP2MCs = [ 'Relations/Rec/ProtoP/Charged' ]
    ## print histos 
    alg.HistoPrint = True
    
    ## configure the desktop
    desktop = gaudi.tool ( 'Xb2mumuhh.PhysDesktop' )
    desktop.InputLocations = [
        '/Event/Phys/StdLooseKaons'   ,
        '/Event/Phys/StdLoosePions'   ,
        '/Event/Phys/StdLooseProtons' ,
        '/Event/Phys/StdLooseMuons' 
        ]
    desktop.PropertiesPrint = True
        
    return SUCCESS 
    
# =============================================================================
## job steering 
if __name__ == '__main__' :

    ## make printout of the own documentations 
    print __doc__

    ## configure the job:
    configure()

    ## run the job
    run(1000)
    

# =============================================================================
# The END 
# =============================================================================
