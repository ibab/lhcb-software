#!/usr/bin/env python
# =============================================================================
# $Id: AP.py,v 1.1 2009-09-13 13:51:37 ibelyaev Exp $ 
# =============================================================================
## @file BenderExample/AP.py
# The simple Bender-based example:
# show " L0 <-> K0S " reflection
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
#  @date 2009-09-01
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
"""
The simple Bender-based example: show ' L0 <-> K0S ' reflection
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
from   LoKiPhys.Phys             import *
# =============================================================================
## Simple class for access MC-truth 
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2006-10-13
class AP(AlgoMC) :
    """
    Simple class for access MC-truth 
    """
    ## standard constructor
    def __init__ ( self , name = 'AP' , **args ) :
        """
        Standard constructor
        """ 
        AlgoMC.__init__ ( self , name )
        for key in args :
            setattr ( self , key , args[key] ) 
            
            
    ## standard method for analyses
    def analyse( self ) :
        """
        Standard method for analyses
        """
        
        # K0s -> pi+ pi0 
        ks   = Trees.MCExclusive  ( Nodes.Pid('KS0') )
        ks  += 'pi+'
        ks  += 'pi-'
        st   = ks.validate  ( self.ppSvc() )
        if st.isFailure()  : return st
        
        ## Lambda0
        l0   = Trees.MCExclusive ( Nodes.Pid('Lambda0')  )
        l0  += 'pi-'
        l0  += 'p+'
        st   = l0.validate  ( self.ppSvc() )
        if st.isFailure()  : return st

        ## Lambda0~
        l0b  = Trees.MCExclusive ( Nodes.Pid('Lambda~0')  )
        l0b += 'p~-'
        l0b += 'pi+'
        st   = l0b.validate ( self.ppSvc() )
        if st.isFailure()  : return st


        cutks  = MCDECTREE( ks  ) 
        mcks   = self.mcselect ( 'ks'  , cutks )
        
        cutl1  = MCDECTREE( l0 ) 
        mcl01  = self.mcselect ( 'l01' , cutl1 )
        
        cutl2  = MCDECTREE( l0b ) 
        mcl02  = self.mcselect ( 'l02' , cutl2 )
                
        if mcks.empty() and mcl01.empty()  and mcl02.empty() : 
            return self.Warning ( 'No mc-trees are found' , SUCCESS )        

        if mcks.empty() :
            mcKS  = NONE
        else :
            mcKS  = MCTRUTH ( self.mcTruth() , mcks  )

        if mcl01.empty () :
            mcL01 = NONE
        else:
            mcL01 = MCTRUTH ( self.mcTruth() , mcl01 )
            
        if mcl02.empty () :
            mcL02 = NONE
        else:
            mcL02 = MCTRUTH ( self.mcTruth() , mcl02 )
        
        pions = self.select ( 'pi' ,
                              ( 'pi+' == ABSID   ) &
                              ( mcKS | mcL01 | mcL02 ) )
        
        self.select ( 'pi+' , pions , Q > 0 )
        self.select ( 'pi-' , pions , Q < 0 )

        tup = self.nTuple ( 'my N-tuple' ) 
        
        ## construct dipions 
        dipion = self.loop ( 'pi+ pi-' , 'KS0' )
        for pi2 in dipion :
            
            m12 = pi2.mass(1,2) / Units.GeV
            if m12 > 1 : continue
            
            chi2 = VCHI2 ( pi2 )
            if not 0.0 <= chi2 < 25 : continue
            
            mc1 = mcKS  ( pi2 )
            mc2 = mcL01 ( pi2 )
            mc3 = mcL02 ( pi2 )
            if mc1 or mc2 or mc3 :  
                self.plot ( M(pi2) / Units.GeV , 'MASS pi pi ' , 0.3 , 0.8 , 500 )
            else: continue

            tup.column ( 'm'     , M(pi2) / Units.GeV )
            tup.column ( 'mc1'   , mc1       )
            tup.column ( 'mc2'   , mc2       )
            tup.column ( 'mc3'   , mc3       )
            tup.column ( 'vchi2' , chi2      )
            tup.column ( 'tr1'   , TRTYPE ( pi2(1) ) ) 
            tup.column ( 'tr2'   , TRTYPE ( pi2(2) ) ) 
            tup.column ( 'lv02'  , LV02(pi2) )

            tup.write()


        return SUCCESS


# =============================================================================
## configure the job
def configure ( **args ) :
    """
    Configure the job
    """
    
    from Configurables import DaVinci, HistogramPersistencySvc,EventSelector, NTupleSvc
    
    daVinci = DaVinci (
        DataType   = 'DC06' , 
        Simulation = True   ,
        HltType    = ''     ) 
    
    HistogramPersistencySvc ( OutputFile = 'AP_Histos.root' ) 
    
    EventSelector (
        PrintFreq = 100 , 
        Input = [
        "DATAFILE='PFN:/afs/cern.ch/lhcb/group/calo/ecal/vol10/DATA/Bs2DsK_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        #"DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsK_1.dst'    TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsK_2.dst'    TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsK_3.dst'    TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsK_4.dst'    TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsK_5.dst'    TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:/afs/cern.ch/lhcb/group/calo/ecal/vol10/DATA/Bs2DsK_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        #"DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsPi_1.dst'    TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsPi_2.dst'    TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsPi_3.dst'    TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsPi_4.dst'    TYP='POOL_ROOTTREE' OPT='READ'",
        ]
        )
    NTupleSvc ( Output = [ "AP DATAFILE='AP_Tuples.root' TYPE='ROOT' OPT='NEW'"] )
    
    gaudi = appMgr() 
    
    ## create local algorithm:
    alg = AP (
        'AP'  
        , PP2MCs = [ 'Relations/Rec/ProtoP/Charged' ]
        , HistoPrint = True
        , NTupleLUN = 'AP'
        , InputLocations = [ 'StdNoPIDsPions' ]
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
    run(500)
    

# =============================================================================
# The END 
# =============================================================================
