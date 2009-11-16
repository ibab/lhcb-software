#!/usr/bin/env python
# =============================================================================
# $Id: Xb2hhh.py,v 1.4 2009-11-16 16:00:37 ibelyaev Exp $ 
# =============================================================================
## @file BenderExample/Bs2DsK.py
#  The simple Bender-based example: find recontructed Xb -> h h h candidates 
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
The simple Bender-based example: find recontructed Xb -> h h h candidates 
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
class Xb2hhh(AlgoMC) :
    """
    find recontructed Xb -> hhh candidates 
    """
    ## standard constructor
    def __init__ ( self , name = 'Xb2hhh' , **kwargs ) :
        """
        Standard constructor
        """ 
        return AlgoMC.__init__ ( self , name , **kwargs )
    
    ## standard method for analyses
    def analyse( self ) :
        """
        Standard method for analyses
        """
        
        # Xb -> mu+ mu- h+ h- 
        ##xb = Trees.MCExclusive  ( Nodes.Beauty , 1 , True )
        xb = Trees.MCExclusive  ( Nodes.Beauty )
        h1 = Nodes.Pid ('pi+') | Nodes.Pid ('K+') | Nodes.Pid( 'p+'  ) 
        h2 = Nodes.Pid ('pi-') | Nodes.Pid ('K-') | Nodes.Pid( 'p~-' )
        h3 = Nodes.CC  ('pi+') | Nodes.CC  ('K+') 
        xb += h1 
        xb += h2 
        xb += h3 

        xb = Trees.Photos ( xb ) 
        
        st =  xb.validate ( self.ppSvc() )
        if st.isFailure()  : 
            return self.Error ( 'Invalid decay descriptor' , st )        

        cut = MCDECTREE( xb ) 
        mcxb = self.mcselect ('mcxb' , cut )
        if mcxb.empty() :
            return self.Warning ( 'No mc-trees are found' , SUCCESS )        

        primaries = self.vselect( 'PVs' , ISPRIMARY )
        if primaries.empty() :
            return self.Warning('No Primary vertices are found!', SUCCESS)
        
        mcXb = MCTRUTH ( self.mcTruth() , mcxb )        
        
        hadrons = self.select ( 'h'  ,
                                ( ( 'pi+' == ABSID ) |
                                  ( 'K+'  == ABSID ) |
                                  ( 'p+'  == ABSID ) ) & mcXb )
        
        nh  = self.counter('#hadron')
        nh += hadrons.size()
        
        if hadrons.size() < 3 : return SUCCESS
        
        h1  = self.select ( 'h+'  , hadrons , Q > 0 ) 
        h2  = self.select ( 'h-'  , hadrons , Q < 0 ) 
        m   = self.select ( 'pk'  , hadrons ,
                            ( 'pi+' == ABSID ) | ( 'K+' == ABSID ) )
        
        xb = self.loop ( 'h+ h- pk' , 'B+' )

        prot = 'p+' == ABSID 
        for b in xb :
            m123 = b.mass(1,2,3) / Units.GeV
            
            if not 3 < m123 < 7     : continue
            
            p1 = prot ( b(1) )
            p2 = prot ( b(2) )
            
            if p1 and not p2 : continue
            if p2 and not p1 : continue
                
            chi2 = VCHI2 ( b )
            if not 0 <=  chi2 < 36  : continue
            if  not mcXb ( b )      : continue 
            self.plot ( M( b ) / Units.GeV , 'mass h+ h- h' , 3 , 7 , 200 )
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
        Simulation = True   ) 
    
    HistogramPersistencySvc ( OutputFile = 'Xb2hhh_Histos.root' ) 
    
    EventSelector (
        PrintFreq = 100 , 
        Input   = [
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Xb2hhh_3K.dst'    TYP='POOL_ROOTTREE' OPT='READ'", 
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Xb2hhh_3pi.dst'   TYP='POOL_ROOTTREE' OPT='READ'", 
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Xb2hhh_piKK.dst'  TYP='POOL_ROOTTREE' OPT='READ'", 
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Xb2hhh_pipiK.dst' TYP='POOL_ROOTTREE' OPT='READ'", 
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Xb2hhh_ppK.dst'   TYP='POOL_ROOTTREE' OPT='READ'"
        ]
        )
    
    gaudi = appMgr() 
    
    ## create local algorithm:
    alg = Xb2hhh(
        ## particle combiners: 
        ParticleCombiners = { "" :  "LoKi::VertexFitter" } ,
        ## MC-links 
        PP2MCs = [ 'Relations/Rec/ProtoP/Charged' ]  ,
        ## print histos 
        HistoPrint = True  , 
        ## input particles 
        InputLocations = [ 'StdLooseKaons'   ,
                           'StdLoosePions'   ,
                           'StdLooseProtons' ]
    )
    
    ##gaudi.addAlgorithm ( alg ) 
    gaudi.setAlgorithms( [alg] )
    

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
