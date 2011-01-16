#!/usr/bin/env python
# =============================================================================
# $Id$ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $, version $Revision$
# =============================================================================
## The simple Bender-script to read Micro DST
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
#  @date 2009-05-18
#  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
# =============================================================================
"""
The simple Bender-script to read MicroDST

This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the campain of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__date__    = " 2009-05-18 "
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision$ "
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

        if self.ievent < 20 :
            for p in recBs : print p.decay()

        # get all MC-Bs particles
        mcBs  = self.mcselect ( 'Bs' , 'B_s0' == MCABSID ) 
            
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
def configure ( datafiles , catalogs = [] ) :
    """
    Configure the job
    """
    
    from Configurables import DaVinci
    daVinci = DaVinci (
        DataType   = 'MC09', 
        Simulation = True 
        ) 
    
    from Configurables import HistogramPersistencySvc
    HistogramPersistencySvc ( OutputFile = 'muDst_Histos_.root') 
    
    from Configurables import NTupleSvc
    NTupleSvc ( Output = [
        "MUDST DATAFILE='muDst_Tuples.root' TYPE='ROOT' OPT='NEW'" ]
                )
    
    ## define/set the input data 
    setData ( datafiles , catalogs )
    
    ##
    ## jump into the world of the actual Gaudi components!
    ## 
    
    ## get the actual application manager (create if needed)
    gaudi = appMgr()
    
    
    ## create local algorithm:
    
    SELECTION = 'SelBs2JpsiPhiPrescaled'
    alg = muDst (
        PropertiesPrint  = True      , 
        HistoPrint       = True      ,
        NTupleLUN        = "MUDST"   ,
        RootInTES        =  '/Event/MicroDST/' , 
        InputLocations   =          [ SELECTION ] ,
        ## Particle      -> MCParticle relation tables
        P2MCs            = ['Phys/' + SELECTION + '/P2MCPRelations' ] ,
        ## ProtoParticle -> MCParticle relation tables
        PP2MCs           = [] 
        )
    
    ## finally inform Application Manager about our algorithm
    gaudi.setAlgorithms( [
        alg 
        ] )
    
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
    configure (
        '/castor/cern.ch/user/j/jpalac/MicroDST/DaVinci_v25r2/MC_2010_3.5TeV_CPV_20Kevt.SeqBs2JpsiPhiPrescaled.mdst'
        )
    
    ## run the job
    run (1000)

    
# =============================================================================
# The END 
# =============================================================================

