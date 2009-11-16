#!/usr/bin/env python
# =============================================================================
# $Id: D02hh.py,v 1.4 2009-11-16 16:00:36 ibelyaev Exp $
# =============================================================================
## @file BenderExample/D02hh.py
#  The simple Bender-based example: find recontructed D0 -> hh candidates 
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
The simple Bender-based example: find recontructed D0 -> hh candidates 
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $ "
# =============================================================================
## import everything from bender
import GaudiKernel.SystemOfUnits as Units 
from   Bender.Awesome            import *
import LoKiMC.trees              as Trees  
import PartProp.Nodes            as Nodes
from   LoKiCore.functions        import *
# =============================================================================
## Simple class for access MC-truth 
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2006-10-13
class D02hh(AlgoMC) :
    """
    find recontructed D0 -> hh  candidates 
    """
    ## standard constructor
    def __init__ ( self , name = 'D02hh' , **kwargs ) :
        """
        Standard constructor
        """ 
        return AlgoMC.__init__ ( self , name , **kwargs )
    
    ## standard method for analyses
    def analyse( self ) :
        """
        Standard method for analyses
        """
        
        # D0/D~0 -> hh 
        d0 = Trees.MCExclusive  ( Nodes.CC('D0') )        
        d0 += Nodes.Pid('pi-') | Nodes.Pid('K-')
        d0 += Nodes.Pid('pi+') | Nodes.Pid('K+')
        
        d0 = Trees.MCPhotos ( d0 )
        
        st =  d0.validate ( self.ppSvc() )
        if st.isFailure()  : return st
        
        cut = MCDECTREE( d0 ) 
        mcd0 = self.mcselect ('mcd0' , cut )
        if mcd0.empty() or 1 < mcd0.size() :
            return self.Warning ( 'No mc-trees are found' , SUCCESS )        

        
        mcD0 = MCTRUTH ( self.mcTruth() , mcd0 )

        
        hadrons = self.select ( 'h' ,
                                ( ( 'pi+' == ABSID ) |
                                  ( 'K+'  == ABSID ) ) & mcD0 )  
        
        self.select ( 'h+' , hadrons , 0 < Q )
        self.select ( 'h-' , hadrons , 0 > Q )
        
        d0 = self.loop ( 'h+ h-' , 'D0' )
        
        for D in d0 :
            m12 = D.mass(1,2) / Units.GeV
            if m12 > 4.0  : continue
            chi2 = VCHI2 ( D )
            if not 0 <=  chi2 < 25  : continue
            if  not mcD0 ( D )      : continue 
            self.plot ( M(D) / Units.GeV , 'mass h+ h-' , 0.6 , 3.0 , 240 )            
            D.save('D0')
            

        D0 = self.selected('D0')
        self.setFilterPassed ( not D0.empty()  )
            
        return SUCCESS

# =============================================================================
## configure the job
def configure ( **args ) :
    """
    Configure the job
    """
        
    from Configurables import DaVinci, HistogramPersistencySvc , EventSelector 
    
    daVinci = DaVinci (
        DataType   = 'DC06', # default  
        Simulation = True  )
    
    HistogramPersistencySvc ( OutputFile = 'D02hh_Histos.root' ) 
    
    EventSelector (
        PrintFreq = 100 ,
        Input = [
        "DATAFILE='PFN:/afs/cern.ch/lhcb/group/calo/ecal/vol10/DATA/D02hh_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        #"DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/D02hh_1.dst' TYP='POOL_ROOTTREE' OPT='READ'", 
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/D02hh_2.dst' TYP='POOL_ROOTTREE' OPT='READ'", 
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/D02hh_3.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
        ]
        )
    
    gaudi = appMgr() 
    
    ## create local algorithm:
    alg = D02hh(
        'D02hh' ,
        ## MC-relations
        PP2MCs = [ 'Relations/Rec/ProtoP/Charged' ] ,
        ## print histos 
        HistoPrint = True ,
        ## input particles:
        InputLocations = [ 'StdNoPIDsPions' , 'StdNoPIDsKaons' ]
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
