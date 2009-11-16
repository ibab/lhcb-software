#!/usr/bin/env python
# =============================================================================
# $Id: Bs2PhiGamma.py,v 1.5 2009-11-16 16:00:36 ibelyaev Exp $ 
# =============================================================================
## @file BenderExample/Bs2PhiGamma.py
#  Simple module to test/verify MC-association problem for the photons
#  claimed by Victor Yakovenko (LAL)
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-12-19
# =============================================================================
"""
Simple module to test/verify MC-association problem for the photons
claimed by Victor Yakovenko (LAL)
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $"
# =============================================================================
from Bender.MainMC import * 
import GaudiKernel.SystemOfUnits as Units

# =============================================================================
## @class Bs2PhiGamma
#  simple class/algorithm to test MC-associations for the photons
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-12-19
class Bs2PhiGamma(AlgoMC) :
    """
    Simple class to test MC-associations for photons from Bs -> phi gamma 
    """
    ## the only one essential method 
    def analyse ( self ) :
        """
        The only one essential method
        """

        finder = self.mcFinder()
        
        mcbs  = finder.find("[ B_s0 -> (  phi(1020) ->  K+  K- )  gamma]cc")
        
        if mcbs.empty() : return self.Warning('No MC-Bs are found', SUCCESS ) 
        mcphi = finder.find("[ B_s0 -> ( ^phi(1020) ->  K+  K- )  gamma]cc") 
        mck   = finder.find("[ B_s0 -> (  phi(1020) -> ^K+ ^K- )  gamma]cc")
        mcg   = finder.find("[ B_s0 -> (  phi(1020) ->  K+  K- ) ^gamma]cc") 

        if mcphi.empty() or mck.empty() or mcg.empty() : 
            return self.Warning ('No MC are found', SUCCESS ) 

        mc = self.mcTruth() 
        mcGamma = MCTRUTH ( mc , mcg   )
        mcPhi   = MCTRUTH ( mc , mcphi )

        ## get the photons
        gamma = self.select ( 'gamma' ,
                              ( 'gamma' == ID )  & ( 2.5 * Units.GeV < PT ) )
        if gamma.empty() : return self.Warning ('No RC-photons are found', SUCCESS ) 
        
        ## get the reconstructed kaons
        kaons  = self.select ( "kaons" , "K+" == ABSID )
        kplus  = self.select ( "k+"    , kaons , 0 < Q )
        kminus = self.select ( "k-"    , kaons , 0 > Q )
        
        if kplus.empty() or kminus.empty() :
            return self.Warning ('No RC-kaons are found', SUCCESS ) 
        
        phis = self.loop ( "k+ k-" , "phi(1020)" )
        for phi in phis :
            m12  = phi.mass(1,2)
            if m12 > 1050 * Units.MeV : continue
            chi2 = VCHI2 ( phi )
            if chi2 < 0 or chi2 > 100 : continue
            phi.save('phi')
            
        phis = self.selected('phi')
        if phis.empty() : return self.Warning('No RC-phis are found', SUCCESS ) 
        
        Bs = self.loop ( "phi gamma" , "B_s0" )
        for B in Bs  :
            m12 = B.mass(1,2) / Units.GeV 
            if m12 < 4  or m12 > 6 : continue

            self.plot ( m12 , 'mass phi gamma' , 4 , 6 , 100 )

            Gamma = B(2)
            Phi   = B(1)
            
            gamma = mcGamma ( B(2) )
            phi   = mcPhi   ( B(1) )
            
            if gamma :
                self.plot ( m12 , 'mass phi (true)gamma'       , 4 , 6 )
            if phi   :
                self.plot ( m12 , 'mass (true)phi gamma'       , 4 , 6 )
            if phi and gamma :
                self.plot ( m12 , 'mass (true)phi (true)gamma' , 4 , 6 )


        return SUCCESS 

    
# =============================================================================
# make the real configuration 
# =============================================================================
def configure() :
    """
    Perform the real configuration of the job
    """
    
    from Configurables import DaVinci
    
    daVinci = DaVinci (
        DataType   = 'MC09' , # default  
        Simulation = True   
        )
    
    from Configurables import HistogramPersistencySvc, EventSelector
    HistogramPersistencySvc ( OutputFile = 'PhiGamma_Histos.root' ) 
    
    EventSelector (
        Input   = [
        ## Use events, selected by Fatima 
        ## "DATAFILE='PFN:castor:/castor/cern.ch/user/f/fsoomro/MC09Signal/PhiGammaMC09.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        ## use 'original' events 
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005102/0000/00005102_00000001_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005102/0000/00005102_00000002_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005102/0000/00005102_00000003_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005102/0000/00005102_00000004_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005102/0000/00005102_00000005_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005102/0000/00005102_00000006_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005102/0000/00005102_00000007_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'"
        ]
        )
    ## get the actual application manager (create if needed)
    gaudi = appMgr() 
    
    ## create local algorithm:
    alg = Bs2PhiGamma(
        'Bs2PhiGamma'         ,
        # print histos 
        HistoPrint     = True ,
        # input particles 
        InputLocations = [
        'StdLooseKaons'       ,
        'StdLooseAllPhotons'
        ]
        )
        
    gaudi.addAlgorithm ( alg ) 
    
    return SUCCESS 


# =============================================================================
## the actual job steering 
if '__main__' == __name__ :

    configure()
    run(1000) 

# =============================================================================
# The END 
# =============================================================================
