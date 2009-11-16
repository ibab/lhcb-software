#!/usr/bin/env python
# =============================================================================
# $Id: muDst.py,v 1.2 2009-11-16 16:00:37 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
# =============================================================================
## The simple Bender-script to read Micro DST
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
#  @date 2009-05-18
#  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
# =============================================================================
"""
The simple Bender-script to read MicroDST
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $ "
# =============================================================================
## import everything from bender
import ROOT 
from   Bender.All                    import *
from   GaudiKernel.SystemOfUnits     import GeV
from   GaudiKernel.PhysicalConstants import c_light
from   LoKiCore.functions            import * 
import LoKiPhys.Phys
import LoKiMC.MC

# =============================================================================
## Simple class to perform simple action on muDST
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-05-15
class muDst(AlgoMC) :
    """
    Simple class to process MicorDST data 
    """
    
    ## standard constructor
    def __init__ ( self , name = 'muDst' , **args ) :
        """
        Standard constructor from algorithm name and properties/attributes
        """ 
        AlgoMC.__init__ ( self , name , **args )
        self.ievent = 0

        
    def mcRelated ( self , rc , mclist ) :

        rcFun = RCTRUTH ( rc , self.mcTruth() )
        for mc in mclist :
            if rcFun ( mc ) : return mc
            mcFun = MCTRUTH ( mc , self.mcTruth() )
            if mcFun ( rc ) : return mc 
        return None 

                    
    ## standard method for analyses
    def analyse ( self ) :
        """
        Standard method for analyses
        """
        
        # get all particles
        all = self.select('all' , ALL )

        # loop on them and print the decay structure for first 20 events 
        self.ievent += 1 
        if self.ievent < 20 :
            for p in all : print p.decay()
        
        # get all reconstructed Bs 
        recBs = self.select( 'Bs' , 'B_s0' == ABSID )

        # get all MC-Bs particles
        mcBs  = self.mcselect ( 'Bs' , 'B_s0' == MCABSID ) 

        # get all reconstructed primary vertices
        primaries = self.vselect ( 'primaries' , PRIMARY )

        # lifetiem fitter
        ltfit = self.lifetimeFitter() 
        
        # loop over reconstructed Bs-candidates:
        for B in recBs :

            mass = M ( B ) / GeV 
            self.plot  ( mass , ' mass of Bs ' , 4 , 6 )            
            
            # get the related primary veretx from DVAlgorithm
            pv = self.getRelatedPV ( B )
            if not pv : continue
            
            # create the functor for evaluation of proper time
            ctauFun = LTIME ( pv , ltfit ) * c_light 
            
            ctau = ctauFun ( B ) 
            self.plot ( ctau , ' c*tau for RC ' , -1 , 3  )
            
            # choose right mc:
            mcB = self.mcRelated ( B , mcBs ) 
            if not mcB : continue
            
            mcmass = MCM ( mcB ) / GeV
            self.plot  ( mass - mcmass , ' mass resolution ' , -0.100 , 0.100 )            
            
            mcctau = MCCTAU ( mcB )
            self.plot  ( mcctau  , ' c*tau for MC ' , -1 , 3  )
            self.plot  ( ctau - mcctau , ' ctau resolution ' , -0.050 , 0.050 )            
            
                        
        self.setFilterPassed ( not recBs.empty() )
        
        return SUCCESS                                           # RETURN 
    
    
# =============================================================================
## configure the job
def configure ( **args ) :
    """
    Configure the job
    """

    from Configurables import NTupleSvc, HistogramPersistencySvc, DaVinci, EventSelector

    TYPE = args.get( 'TYPE','DC06' )
    
    daVinci = DaVinci (
        DataType   = TYPE , 
        Simulation = True 
        ) 
    
    HistogramPersistencySvc ( OutputFile = 'muDst_Histos_.root'                         ) 
    NTupleSvc ( Output = [ "MUDST DATAFILE='muDst_Tuples_%s.root' TYPE='ROOT' OPT='NEW'" ] )
    
    
    FILES = args['FILES']
    EventSelector ( 
        PrintFreq = 100    ,
        Input     = FILES  
        )
    
    from Configurables import EventClockSvc
    EventClockSvc( EventTimeDecoder = "FakeEventTime" )  
    
    ## get the actual application manager (create if needed)
    gaudi = appMgr()

    evtClock = gaudi.service('EventClockSvc')
    evtClock.EventTimeDecoder = 'FakeEventTime'
    
    ## create local algorithm:
    SELECTION = args['SELECTION']
    ROOTINTES = args.get( 'RootInTES' , '/Event/microDST/')
    
    alg = muDst (
        PropertiesPrint = True      , 
        HistoPrint      = True      ,
        NTupleLUN       = "MUDST"   ,
        P2MCs           = ['Phys/' + SELECTION + '/P2MCPRelations' ] ,
        RootInTES       =            ROOTINTES , 
        InputLocations  =          [ SELECTION ] ,
        PP2MCs          = [] 
        )
    
    ## get the application manager (create if needed)
    gaudi = appMgr()
    
    ## finally inform Application Manager about our algorithm
    gaudi.setAlgorithms( [alg] )
    
    return SUCCESS 
    
# =============================================================================
## job steering 
if __name__ == '__main__' :

    ## make printout of the own documentations 
    print __doc__
    
    ## configure the job:
    configure (
        FILES = [
        "DATAFILE='PFN:/afs/cern.ch/lhcb/group/calo/ecal/vol10/DATA/SeqDC06selBs2JpsiPhiWithMC_50.0_Kevt_NoPVReFit.dst'   TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:/afs/cern.ch/lhcb/group/calo/ecal/vol10/DATA/SeqDC06selBs2JpsiPhiWithMC_50.0_Kevt_NewReFitPVs.dst' TYP='POOL_ROOTTREE' OPT='READ'"
        ] ,
        SELECTION = 'DC06selBs2JpsiPhi_unbiased' 
        )
    
    ## run the job
    run(-1)

# =============================================================================
# The END 
# =============================================================================

